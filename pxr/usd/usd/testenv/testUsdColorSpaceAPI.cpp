//
// Copyright 2024 Pixar
//
// Licensed under the terms set forth in the LICENSE.txt file available at
// https://openusd.org/license.
//

#include "pxr/pxr.h"
#include "pxr/base/gf/color.h"
#include "pxr/base/vt/array.h"
#include "pxr/base/vt/dictionary.h"
#include "pxr/base/tf/diagnostic.h"
#include "pxr/base/tf/token.h"
#include "pxr/usd/usd/stage.h"
#include "pxr/usd/usd/object.h"
#include "pxr/usd/usd/attribute.h"
#include "pxr/usd/usd/colorSpaceAPI.h"
#include "pxr/usd/usd/relationship.h"
#include "pxr/usd/sdf/path.h"
#if 0
#include "pxr/usd/usdGeom/cube.h"
#include "pxr/usd/usdGeom/xform.h"
#include "pxr/usd/usdShade/material.h"
#include "pxr/usd/usdShade/materialBindingAPI.h"
#endif

PXR_NAMESPACE_USING_DIRECTIVE
using std::string;

#if 0

// ISO 17321-1:2012 Table D.1
// The colors in the specification are listed in linear AP0.

static GfVec3f _ISO17321_ap0[24] = {
    { 0.11877f, 0.08709f, 0.05895f }, // patch 1
    { 0.40003f, 0.31916f, 0.23737f }, // patch 2
    { 0.18476f, 0.20398f, 0.31310f }, // patch 3
    { 0.10901f, 0.13511f, 0.06493f }, // patch 4
    { 0.26684f, 0.24604f, 0.40932f }, // patch 5
    { 0.32283f, 0.46208f, 0.40606f }, // patch 6
    { 0.38607f, 0.22744f, 0.05777f }, // patch 7
    { 0.13822f, 0.13037f, 0.33703f }, // patch 8
    { 0.30203f, 0.13752f, 0.12758f }, // patch 9
    { 0.09310f, 0.06347f, 0.13525f },
    { 0.34877f, 0.43655f, 0.10613f },
    { 0.48657f, 0.36686f, 0.08061f },
    { 0.08731f, 0.07443f, 0.27274f },
    { 0.15366f, 0.25692f, 0.09071f },
    { 0.21743f, 0.07070f, 0.05130f },
    { 0.58921f, 0.53944f, 0.09157f },
    { 0.30904f, 0.14818f, 0.27426f },
    //{ 0.14900f, 0.23377f, 0.35939f }, // spec is slightly out of Rec709 gamut.
    { 0.0f, 0.23377f, 0.35939f }, // tweaked red channel
    { 0.86653f, 0.86792f, 0.85818f },
    { 0.57356f, 0.57256f, 0.57169f },
    { 0.35346f, 0.35337f, 0.35391f },
    { 0.20253f, 0.20243f, 0.20287f },
    { 0.09467f, 0.09520f, 0.09637f },
    { 0.03745f, 0.03766f, 0.03895f }, // patch 24
};

static const char* _ISO17321_names[24] = {
    "Dark skin",
    "Light skin",
    "Blue sky",
    "Foliage",
    "Blue flower",
    "Bluish green",
    "Orange",
    "Purplish blue",
    "Moderate red",
    "Purple",
    "Yellow green",
    "Orange yellow",
    "Blue",
    "Green",
    "Red",
    "Yellow",
    "Magenta",
    "Cyan",
    "White",
    "Neutral 8",
    "Neutral 6.5",
    "Neutral 5",
    "Neutral 3.5",
    "Black"
};

pxr::SdfPath _CreateColorCheckerChart(pxr::UsdStageRefPtr stage,
                                      const std::string& chipsColorspace)
{
    std::string chartName = "Chart_" + chipsColorspace;
    GfColorSpace source_cs(TfToken("lin_ap0"));
    TfToken resultColorSpaceName(chipsColorspace);
    GfColorSpace result_cs(resultColorSpaceName);
    
    pxr::SdfPath primPath = stage->GetDefaultPrim().GetPath();
    primPath = primPath.AppendChild(TfToken("Shapes"));
    primPath = primPath.AppendChild(TfToken(chartName));
    string primPathStr = primPath.GetString();
    pxr::SdfPath r(primPathStr);
    primPath = r;
    auto prim = pxr::UsdGeomXform::Define(stage, r);
    UsdColorSpaceAPI cs(prim);
    cs.CreateColorSpaceNameAttr(VtValue(GfColorSpaceNames->LinearRec2020));
    
    pxr::SdfPath materialPath = stage->GetDefaultPrim().GetPath();
    materialPath = materialPath.AppendChild(TfToken("Materials"));
    materialPath = materialPath.AppendChild(TfToken(chartName));
    string materialPathStr = materialPath.GetString();
    pxr::SdfPath rm(materialPathStr);
    materialPath = rm;

    GfVec3f* chips = _ISO17321_ap0;
    const char** chipNames = _ISO17321_names;

    for (int i = 0; i < 24; ++i) {
        std::string n = chipNames[i];
        std::replace(n.begin(), n.end(), ' ', '_');
        std::replace(n.begin(), n.end(), '.', '_');
        pxr::SdfPath chipPath = primPath.AppendChild(TfToken(n));
        pxr::UsdGeomGprim cube = pxr::UsdGeomCube::Define(stage, chipPath);
        pxr::UsdAttribute color = cube.GetDisplayColorAttr();
        color.SetColorSpace(TfToken(chipsColorspace));
        pxr::VtArray<GfVec3f> array;
        
        // display color
        GfColor source_c(chips[i], source_cs);
        GfColor result_c(source_c, result_cs);
        array.push_back(result_c.GetRGB());
        color.Set(array);

        // emissive standard surface
        pxr::SdfPath chipMaterialPath = materialPath.AppendChild(TfToken(n));
        UsdShadeMaterial material =
            pxr::UsdShadeMaterial::Define(stage, chipMaterialPath);
        pxr::SdfPath chipShaderPath =
            materialPath.AppendChild(TfToken(n + "Shader"));
        auto pbrShader = UsdShadeShader::Define(stage, chipShaderPath);
        pbrShader.CreateIdAttr(VtValue(TfToken("UsdPreviewSurface")));
        pbrShader.CreateInput(TfToken("diffuseColor"),
                              SdfValueTypeNames->Color3f).Set(GfVec3f(0,0,0));
        pbrShader.CreateInput(TfToken("specularColor"),
                              SdfValueTypeNames->Color3f).Set(GfVec3f(0,0,0));
        pbrShader.CreateInput(TfToken("emissiveColor"),
                              SdfValueTypeNames->Color3f).Set(result_c.GetRGB());
        material.CreateSurfaceOutput().ConnectToSource(
                              pbrShader.ConnectableAPI(), TfToken("surface"));
        cube.GetPrim().ApplyAPI<UsdShadeMaterialBindingAPI>();
        auto binding = UsdShadeMaterialBindingAPI(cube);
        binding.Bind(material);

        pxr::UsdGeomXformable xformable(cube);
        double x = (i % 6) * 2.0;
        double y = ((23-i) / 6) * 2.0;
        double z = 0;
        xformable.ClearXformOpOrder();
        
        // usd prim cubes are centered at the origin, so offset the cubes so
        // the chart sits entirely in the positive octant
        xformable.AddTranslateOp().Set(GfVec3d(x + 1.0, y + 1.0, z + 1.0));
    }
    return r;
}
#endif

int main() {
    // Create a new stage
    UsdStageRefPtr stage = UsdStage::CreateInMemory();
    
    // Create a fallback color space
    UsdPrim rootPrim = stage->OverridePrim(SdfPath("/rec2020"));
    UsdColorSpaceAPI rootCsAPI = UsdColorSpaceAPI::Apply(rootPrim);
    TF_VERIFY(rootPrim.HasAPI<UsdColorSpaceAPI>());
    TF_VERIFY(rootPrim.HasAPI(TfToken("ColorSpaceAPI")));
    TF_VERIFY(rootPrim.HasAPI(TfToken("ColorSpaceAPI"), /*schemaVersion*/ 0));
    auto rootCsAttr = rootCsAPI.CreateColorSpaceNameAttr(VtValue(GfColorSpaceNames->LinearRec2020));
    TF_VERIFY(rootCsAttr);

    // Fetch the color space in a variety of ways
    TfToken colorSpace;
    TF_VERIFY(rootCsAttr.Get(&colorSpace));
    TF_VERIFY(colorSpace == GfColorSpaceNames->LinearRec2020);
    TF_VERIFY(GfColorSpace::IsValid(colorSpace));

    TF_VERIFY(rootCsAPI.GetColorSpaceNameAttr().Get(&colorSpace));
    TF_VERIFY(colorSpace == GfColorSpaceNames->LinearRec2020);

    // Compute the color space.
    TF_VERIFY(rootCsAPI.ComputeColorSpaceName() == GfColorSpaceNames->LinearRec2020);
    TF_VERIFY(rootCsAPI.ComputeColorSpaceName(rootCsAttr) == GfColorSpaceNames->LinearRec2020);

    // Create a color attribute on rootPrim, and verify it inherits the color space from rootPrim
    UsdAttribute colorAttr = rootPrim.CreateAttribute(TfToken("color"), SdfValueTypeNames->Color3f);
    TF_VERIFY(colorAttr.GetColorSpace() == GfColorSpaceNames->LinearRec2020);

    // Create a child prim with a different color space, and verify it overrides the parent
    UsdPrim childPrim = stage->OverridePrim(SdfPath("/rec2020/linSRGB"));
    auto childCsAPI = UsdColorSpaceAPI::Apply(childPrim);
    auto childCsAttr = childCsAPI.CreateColorSpaceNameAttr(VtValue(GfColorSpaceNames->LinearRec709));
    TF_VERIFY(childCsAPI.ComputeColorSpaceName() == GfColorSpaceNames->LinearRec709);

    // Create a color attribute on childPrim, and verify it inherits the color space from childPrim
    UsdAttribute childColorAttr = childPrim.CreateAttribute(TfToken("color"), SdfValueTypeNames->Color3f);
    TF_VERIFY(childColorAttr.GetColorSpace() == GfColorSpaceNames->LinearRec709);

    // Create a grandchild prim with no color space, and verify it inherits the parent's color space
    UsdPrim grandchildPrim = stage->OverridePrim(SdfPath("/rec2020/linSRGB/noColorSpace"));
    auto grandchildCsAPI = UsdColorSpaceAPI::Apply(grandchildPrim);
    TF_VERIFY(grandchildCsAPI.ComputeColorSpaceName() == GfColorSpaceNames->LinearRec709);

    // Create a color attribute on grandchildPrim, and verify it inherits the color space from childPrim
    UsdAttribute grandchildColorAttr = grandchildPrim.CreateAttribute(TfToken("color"), SdfValueTypeNames->Color3f);
    TF_VERIFY(grandchildColorAttr.GetColorSpace() == GfColorSpaceNames->LinearRec709);

    // Create a root prim without assigning color space, and verify the fallback is Linear Rec709
    UsdPrim rootPrim2 = stage->OverridePrim(SdfPath("/noColorSpace"));
    auto root2CsAPI = UsdColorSpaceAPI::Apply(rootPrim2);
    TF_VERIFY(root2CsAPI.ComputeColorSpaceName() == GfColorSpaceNames->LinearRec709);

    // Repeat all of the preceding rootPrim tests on rootPrim2.
    UsdAttribute colorAttr2 = rootPrim2.CreateAttribute(TfToken("color"), SdfValueTypeNames->Color3f);
    TF_VERIFY(colorAttr2.GetColorSpace() == GfColorSpaceNames->LinearRec709);

    // Create a child prim with a different color space, and verify it overrides the parent
    UsdPrim childPrim2 = stage->OverridePrim(SdfPath("/noColorSpace/linSRGB"));
    auto child2CsAPI = UsdColorSpaceAPI::Apply(childPrim2);
    auto child2CsAttr = child2CsAPI.CreateColorSpaceNameAttr(VtValue(GfColorSpaceNames->LinearRec709));
    TF_VERIFY(child2CsAPI.ComputeColorSpaceName() == GfColorSpaceNames->LinearRec709);

    // Create a color attribute on childPrim2, and verify it inherits the color space from childPrim2
    UsdAttribute childColorAttr2 = childPrim2.CreateAttribute(TfToken("color"), SdfValueTypeNames->Color3f);
    TF_VERIFY(childColorAttr2.GetColorSpace() == GfColorSpaceNames->LinearRec709);

    // Create a grandchild prim with no color space, and verify it inherits the parent's color space
    UsdPrim grandchildPrim2 = stage->OverridePrim(SdfPath("/noColorSpace/linSRGB/noColorSpace"));
    auto grandchild2CsAPI = UsdColorSpaceAPI::Apply(grandchildPrim2);
    TF_VERIFY(grandchild2CsAPI.ComputeColorSpaceName() == GfColorSpaceNames->LinearRec709);

    // Create a color attribute on grandchildPrim2, and verify it inherits the color space from childPrim2
    UsdAttribute grandchildColorAttr2 = grandchildPrim2.CreateAttribute(TfToken("color"), SdfValueTypeNames->Color3f);
    TF_VERIFY(grandchildColorAttr2.GetColorSpace() == GfColorSpaceNames->LinearRec709);

    // Test the CanApply method
    TF_VERIFY(UsdColorSpaceAPI::CanApply(rootPrim));
    TF_VERIFY(UsdColorSpaceAPI::CanApply(childPrim));
    TF_VERIFY(UsdColorSpaceAPI::CanApply(grandchildPrim));

    // Test the Get method
    TF_VERIFY(UsdColorSpaceAPI::Get(stage, SdfPath("/rec2020")).GetPrim() == rootPrim);
 
    printf("UsdColorSpaceAPI test passed\n");
    return EXIT_SUCCESS;
}
