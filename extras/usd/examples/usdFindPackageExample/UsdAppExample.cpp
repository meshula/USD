
// A minimal, but non-trivial USD application example. It creates a stage
// with a cube, and emits messages for various events.

#ifdef _MSC_VER
#pragma warning(disable:4244) // suppress warnings about implicit conversions
#pragma warning(disable:4305) // suppress warnings about integer truncation
#endif

#include <pxr/base/tf/weakBase.h>
#include <pxr/base/tf/weakPtr.h>
#include <pxr/usd/usd/notice.h>
#include <pxr/usd/usd/prim.h>
#include <pxr/usd/usd/stage.h>
#include <pxr/usd/usdGeom/xformable.h>

#include <cstdlib>
#include <iostream>

PXR_NAMESPACE_USING_DIRECTIVE

/*
    SceneMonitor watches a scene and reports changes
*/
class SceneMonitor : public TfWeakBase
{
public:
    TfNotice::Key objectsChangedNoticeKey;

    ~SceneMonitor() {
        // clean up - stop watching for changes
        TfNotice::Revoke(objectsChangedNoticeKey);
    }

   void OnObjectsChanged(UsdNotice::ObjectsChanged const& notice, UsdStageWeakPtr const& sender)
    {
        std::cout << "GetResyncedPaths\n";
        auto pathsToResync = notice.GetResyncedPaths();
        for (auto & i : pathsToResync)
        {
            std::cout << i.GetString() << "\n";
        }
        std::cout << "GetChangedInfoOnlyPaths\n";
        auto infoPaths = notice.GetChangedInfoOnlyPaths();
        for (auto & i : infoPaths)
        {
            std::cout << i.GetString() << "\n";
        }
    }
};

int main(void)
{

    const std::string path = "test.usda";
    {
        // Create and write a stage

        UsdStageRefPtr stage = UsdStage::CreateNew(path);

        // Start listening for change notices from this stage.
        SceneMonitor monitor;
        auto monitorPtr = TfCreateWeakPtr(monitor);
        monitor.objectsChangedNoticeKey = TfNotice::Register(monitorPtr, &SceneMonitor::OnObjectsChanged, stage);

        // create a cube on the stage and give it a scale
        stage->DefinePrim(SdfPath("/Box"), TfToken("Cube"));
        UsdPrim cube = stage->GetPrimAtPath(SdfPath("/Box"));
        GfVec3f scaleVec = { 5.f, 5.f, 5.f };
        UsdGeomXformable cubeXf(cube);
        cubeXf.AddScaleOp().Set(scaleVec);

        // write to disk
        stage->GetRootLayer()->Save();
    }

    {
        // Load the stage and check for the cube

        std::cout << "\nLoad_Stage " << filePath << "\n";
        auto supported = UsdStage::IsSupportedFile(filePath);
        if (supported)
        {
            std::cout << "File format supported\n";
        }
        else
        {
            std::cerr << filePath << " : File format not supported\n";
            std::cerr << "Most likely, OpenUSD's plugins were not found." << std::endl;
            return EXIT_FAILURE;
        }

        UsdStageRefPtr loadedStage = UsdStage::Open(filePath);

        if (loadedStage)
        {
            auto pseudoRoot = loadedStage->GetPseudoRoot();
            std::cout << "Pseudo root path: " << pseudoRoot.GetPath().GetString() << "\n";
            for (auto const& c : pseudoRoot.GetChildren())
            {
                std::cout << "\tChild path: " << c.GetPath().GetString() << "\n";
            }
        }
        else
        {
            std::cerr << "Stage was not loaded" << std::endl;
        }
    }
    return EXIT_SUCCESS;
}
