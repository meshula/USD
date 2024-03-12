//
// Copyright 2023 Pixar
//
// Licensed under the Apache License, Version 2.0 (the "Apache License")
// with the following modification; you may not use this file except in
// compliance with the Apache License and the following modification to it:
// Section 6. Trademarks. is deleted and replaced with:
//
// 6. Trademarks. This License does not grant permission to use the trade
//    names, trademarks, service marks, or product names of the Licensor
//    and its affiliates, except as required to comply with Section 4(c) of
//    the License and to reproduce the content of the NOTICE file.
//
// You may obtain a copy of the Apache License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the Apache License with the above modification is
// distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied. See the Apache License for the specific
// language governing permissions and limitations under the Apache License.
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

#ifndef EXT_RMANPKG_25_0_PLUGIN_RENDERMAN_PLUGIN_HD_PRMAN_RILEY_RENDER_TARGET_SCHEMA_H
#define EXT_RMANPKG_25_0_PLUGIN_RENDERMAN_PLUGIN_HD_PRMAN_RILEY_RENDER_TARGET_SCHEMA_H

/// \file

#include "hdPrman/api.h"
#include "hdPrman/rileyParamListSchema.h"

#include "pxr/imaging/hd/schema.h"
#include "pxr/imaging/hd/version.h"

// --(BEGIN CUSTOM CODE: Includes)--
// --(END CUSTOM CODE: Includes)--

PXR_NAMESPACE_OPEN_SCOPE

// --(BEGIN CUSTOM CODE: Declares)--
// --(END CUSTOM CODE: Declares)--

#define HD_PRMAN_RILEY_RENDER_TARGET_SCHEMA_TOKENS \
    (rileyRenderTarget) \
    (renderOutputs) \
    (extent) \
    (filterMode) \
    (pixelVariance) \
    (params) \
    (importance) \
    (weighted) \

TF_DECLARE_PUBLIC_TOKENS(HdPrmanRileyRenderTargetSchemaTokens, HDPRMAN_API,
    HD_PRMAN_RILEY_RENDER_TARGET_SCHEMA_TOKENS);

//-----------------------------------------------------------------------------


class HdPrmanRileyRenderTargetSchema : public HdSchema
{
public:
    /// \name Schema retrieval
    /// @{

    HdPrmanRileyRenderTargetSchema(HdContainerDataSourceHandle container)
      : HdSchema(container) {}

    /// Retrieves a container data source with the schema's default name token
    /// "rileyRenderTarget" from the parent container and constructs a
    /// HdPrmanRileyRenderTargetSchema instance.
    /// Because the requested container data source may not exist, the result
    /// should be checked with IsDefined() or a bool comparison before use.
    HDPRMAN_API
    static HdPrmanRileyRenderTargetSchema GetFromParent(
        const HdContainerDataSourceHandle &fromParentContainer);

    /// @}

// --(BEGIN CUSTOM CODE: Schema Methods)--
// --(END CUSTOM CODE: Schema Methods)--

    /// \name Member accessor
    /// @{

    HDPRMAN_API
    HdPathArrayDataSourceHandle GetRenderOutputs()
#if HD_API_VERSION >= 66
                                            const;
#else
                                                 ;
#endif


    HDPRMAN_API
    HdVec3iDataSourceHandle GetExtent()
#if HD_API_VERSION >= 66
                                            const;
#else
                                                 ;
#endif


    HDPRMAN_API
    HdTokenDataSourceHandle GetFilterMode()
#if HD_API_VERSION >= 66
                                            const;
#else
                                                 ;
#endif


    HDPRMAN_API
    HdFloatDataSourceHandle GetPixelVariance()
#if HD_API_VERSION >= 66
                                            const;
#else
                                                 ;
#endif


    HDPRMAN_API
    HdPrmanRileyParamListSchema GetParams()
#if HD_API_VERSION >= 66
                                            const;
#else
                                                 ;
#endif
 

    /// @}

    /// \name Schema location
    /// @{

    /// Returns a token where the container representing this schema is found in
    /// a container by default.
    HDPRMAN_API
    static const TfToken &GetSchemaToken();

    /// Returns an HdDataSourceLocator (relative to the prim-level data source)
    /// where the container representing this schema is found by default.
    HDPRMAN_API
    static const HdDataSourceLocator &GetDefaultLocator();

    /// @}

    /// \name Data source locators for members
    ///
    /// The following methods return an HdDataSourceLocator (relative to the
    /// prim-level data source) where the data source for a member can be found.
    ///
    /// This is often useful for checking intersection against the
    /// HdDataSourceLocatorSet sent with HdDataSourceObserver::PrimsDirtied.
    /// @{

    /// Prim-level relative data source locator to locate renderOutputs.
    HDPRMAN_API
    static const HdDataSourceLocator &GetRenderOutputsLocator();

    /// Prim-level relative data source locator to locate extent.
    HDPRMAN_API
    static const HdDataSourceLocator &GetExtentLocator();

    /// Prim-level relative data source locator to locate filterMode.
    HDPRMAN_API
    static const HdDataSourceLocator &GetFilterModeLocator();

    /// Prim-level relative data source locator to locate pixelVariance.
    HDPRMAN_API
    static const HdDataSourceLocator &GetPixelVarianceLocator();

    /// Prim-level relative data source locator to locate params.
    HDPRMAN_API
    static const HdDataSourceLocator &GetParamsLocator();
    /// @} 

    /// \name Schema construction
    /// @{

    /// \deprecated Use Builder instead.
    ///
    /// Builds a container data source which includes the provided child data
    /// sources. Parameters with nullptr values are excluded. This is a
    /// low-level interface. For cases in which it's desired to define
    /// the container with a sparse set of child fields, the Builder class
    /// is often more convenient and readable.
    HDPRMAN_API
    static HdContainerDataSourceHandle
    BuildRetained(
        const HdPathArrayDataSourceHandle &renderOutputs,
        const HdVec3iDataSourceHandle &extent,
        const HdTokenDataSourceHandle &filterMode,
        const HdFloatDataSourceHandle &pixelVariance,
        const HdContainerDataSourceHandle &params
    );

    /// \class HdPrmanRileyRenderTargetSchema::Builder
    /// 
    /// Utility class for setting sparse sets of child data source fields to be
    /// filled as arguments into BuildRetained. Because all setter methods
    /// return a reference to the instance, this can be used in the "builder
    /// pattern" form.
    class Builder
    {
    public:
        HDPRMAN_API
        Builder &SetRenderOutputs(
            const HdPathArrayDataSourceHandle &renderOutputs);
        HDPRMAN_API
        Builder &SetExtent(
            const HdVec3iDataSourceHandle &extent);
        HDPRMAN_API
        Builder &SetFilterMode(
            const HdTokenDataSourceHandle &filterMode);
        HDPRMAN_API
        Builder &SetPixelVariance(
            const HdFloatDataSourceHandle &pixelVariance);
        HDPRMAN_API
        Builder &SetParams(
            const HdContainerDataSourceHandle &params);

        /// Returns a container data source containing the members set thus far.
        HDPRMAN_API
        HdContainerDataSourceHandle Build();

    private:
        HdPathArrayDataSourceHandle _renderOutputs;
        HdVec3iDataSourceHandle _extent;
        HdTokenDataSourceHandle _filterMode;
        HdFloatDataSourceHandle _pixelVariance;
        HdContainerDataSourceHandle _params;

    };

    /// Returns token data source for use as filterMode value.
    ///
    /// The following values will be stored statically and reused for future
    /// calls:
    /// - HdPrmanRileyRenderTargetSchemaTokens->importance
    /// - HdPrmanRileyRenderTargetSchemaTokens->weighted
    HDPRMAN_API
    static HdTokenDataSourceHandle BuildFilterModeDataSource(
        const TfToken &filterMode);

    /// @}
};

PXR_NAMESPACE_CLOSE_SCOPE

#endif