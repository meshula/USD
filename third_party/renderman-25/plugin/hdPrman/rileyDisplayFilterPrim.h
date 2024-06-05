//
// Copyright 2023 Pixar
//
// Licensed under the terms set forth in the LICENSE.txt file available at
// https://openusd.org/license.
//
////////////////////////////////////////////////////////////////////////

/* ************************************************************************** */
/* **                                                                      ** */
/* ** This file is generated by a script.                                  ** */
/* **                                                                      ** */
/* ** Do not edit it directly (unless it is within a CUSTOM CODE section)! ** */
/* ** Edit hdSchemaDefs.py instead to make changes.                        ** */
/* **                                                                      ** */
/* ************************************************************************** */

#ifndef EXT_RMANPKG_25_0_PLUGIN_RENDERMAN_PLUGIN_HD_PRMAN_RILEY_DISPLAY_FILTER_PRIM_H
#define EXT_RMANPKG_25_0_PLUGIN_RENDERMAN_PLUGIN_HD_PRMAN_RILEY_DISPLAY_FILTER_PRIM_H

#include "pxr/pxr.h"
#include "hdPrman/api.h"
#include "hdPrman/sceneIndexObserverApi.h"

#ifdef HDPRMAN_USE_SCENE_INDEX_OBSERVER

#include "hdPrman/rileyPrimBase.h"

PXR_NAMESPACE_OPEN_SCOPE


using HdPrman_RileyDisplayFilterPrimHandle =
    std::shared_ptr<class HdPrman_RileyDisplayFilterPrim>;

/// \class HdPrman_RileyDisplayFilterPrim
///
/// Wraps a riley DisplayFilter object, initializing or updating it
/// using the HdPrmanRileyDisplayFilterSchema.
///
class HdPrman_RileyDisplayFilterPrim : public HdPrman_RileyPrimBase
{
public:
    HdPrman_RileyDisplayFilterPrim(
        HdContainerDataSourceHandle const &primSource,
        const HdsiPrimManagingSceneIndexObserver *observer,
        HdPrman_RenderParam * renderParam);

    ~HdPrman_RileyDisplayFilterPrim() override;

    using RileyId = riley::DisplayFilterId;
    using RileyIdList = riley::DisplayFilterList;


    const RileyId &GetRileyId() const { return _rileyId; }

protected:
    void _Dirty(
        const HdSceneIndexObserver::DirtiedPrimEntry &entry,
        const HdsiPrimManagingSceneIndexObserver * observer) override;

private:

// --(BEGIN CUSTOM CODE: Fields)--
// --(END CUSTOM CODE: Fields)--

    RileyId _rileyId;
};

PXR_NAMESPACE_CLOSE_SCOPE

#endif // #ifdef HDPRMAN_USE_SCENE_INDEX_OBSERVER

#endif
