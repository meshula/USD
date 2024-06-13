//
// Copyright 2022 Pixar
//
// Licensed under the terms set forth in the LICENSE.txt file available at
// https://openusd.org/license.
//
#ifndef EXT_RMANPKG_25_0_PLUGIN_RENDERMAN_PLUGIN_HD_PRMAN_SPHERE_H
#define EXT_RMANPKG_25_0_PLUGIN_RENDERMAN_PLUGIN_HD_PRMAN_SPHERE_H

#include "pxr/pxr.h"
#include "hdPrman/gprim.h"
#include "pxr/imaging/hd/rprim.h"

#include "Riley.h"

PXR_NAMESPACE_OPEN_SCOPE

class HdPrman_Sphere final : public HdPrman_Gprim<HdRprim>
{
public:
    using BASE = HdPrman_Gprim<HdRprim>;

    HF_MALLOC_TAG_NEW("new HdPrman_Sphere");

    HdPrman_Sphere(SdfPath const& id);

    HdDirtyBits GetInitialDirtyBitsMask() const override;

    TfTokenVector const &GetBuiltinPrimvarNames() const override;

protected:
    RtPrimVarList
    _ConvertGeometry(HdPrman_RenderParam *renderParam,
                     HdSceneDelegate *sceneDelegate,
                     const SdfPath &id,
                     RtUString *primType,
                     std::vector<HdGeomSubset> *geomSubsets) override;
};

PXR_NAMESPACE_CLOSE_SCOPE

#endif // EXT_RMANPKG_25_0_PLUGIN_RENDERMAN_PLUGIN_HD_PRMAN_SPHERE_H
