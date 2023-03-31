//
// Copyright 2022 Pixar
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
/* ** This file is generated by a script.  Do not edit directly.  Edit     ** */
/* ** defs.py or the (*)Schema.template.h files to make changes.           ** */
/* ************************************************************************** */

#ifndef PXR_USD_IMAGING_USD_IMAGING_USD_RENDER_PRODUCT_SCHEMA_H
#define PXR_USD_IMAGING_USD_IMAGING_USD_RENDER_PRODUCT_SCHEMA_H

#include "pxr/usdImaging/usdImaging/api.h"

#include "pxr/imaging/hd/schema.h" 

PXR_NAMESPACE_OPEN_SCOPE

//-----------------------------------------------------------------------------

#define USDIMAGINGUSDRENDERPRODUCT_SCHEMA_TOKENS \
    (__usdRenderProduct) \
    (resolution) \
    (pixelAspectRatio) \
    (aspectRatioConformPolicy) \
    (dataWindowNDC) \
    (disableMotionBlur) \
    (camera) \
    (productType) \
    (productName) \
    (orderedVars) \
    (namespacedSettings) \

TF_DECLARE_PUBLIC_TOKENS(UsdImagingUsdRenderProductSchemaTokens, USDIMAGING_API,
    USDIMAGINGUSDRENDERPRODUCT_SCHEMA_TOKENS);

//-----------------------------------------------------------------------------

class UsdImagingUsdRenderProductSchema : public HdSchema
{
public:
    UsdImagingUsdRenderProductSchema(HdContainerDataSourceHandle container)
    : HdSchema(container) {}

    //ACCESSORS

    USDIMAGING_API
    HdVec2iDataSourceHandle GetResolution();
    USDIMAGING_API
    HdFloatDataSourceHandle GetPixelAspectRatio();
    USDIMAGING_API
    HdTokenDataSourceHandle GetAspectRatioConformPolicy();
    USDIMAGING_API
    HdVec4fDataSourceHandle GetDataWindowNDC();
    USDIMAGING_API
    HdBoolDataSourceHandle GetDisableMotionBlur();
    USDIMAGING_API
    HdPathDataSourceHandle GetCamera();
    USDIMAGING_API
    HdTokenDataSourceHandle GetProductType();
    USDIMAGING_API
    HdTokenDataSourceHandle GetProductName();
    USDIMAGING_API
    HdPathArrayDataSourceHandle GetOrderedVars();
    USDIMAGING_API
    HdContainerDataSourceHandle GetNamespacedSettings();

    // RETRIEVING AND CONSTRUCTING

    /// Builds a container data source which includes the provided child data
    /// sources. Parameters with nullptr values are excluded. This is a
    /// low-level interface. For cases in which it's desired to define
    /// the container with a sparse set of child fields, the Builder class
    /// is often more convenient and readable.
    USDIMAGING_API
    static HdContainerDataSourceHandle
    BuildRetained(
        const HdVec2iDataSourceHandle &resolution,
        const HdFloatDataSourceHandle &pixelAspectRatio,
        const HdTokenDataSourceHandle &aspectRatioConformPolicy,
        const HdVec4fDataSourceHandle &dataWindowNDC,
        const HdBoolDataSourceHandle &disableMotionBlur,
        const HdPathDataSourceHandle &camera,
        const HdTokenDataSourceHandle &productType,
        const HdTokenDataSourceHandle &productName,
        const HdPathArrayDataSourceHandle &orderedVars,
        const HdContainerDataSourceHandle &namespacedSettings
    );

    /// \class UsdImagingUsdRenderProductSchema::Builder
    /// 
    /// Utility class for setting sparse sets of child data source fields to be
    /// filled as arguments into BuildRetained. Because all setter methods
    /// return a reference to the instance, this can be used in the "builder
    /// pattern" form.
    class Builder
    {
    public:
        USDIMAGING_API
        Builder &SetResolution(
            const HdVec2iDataSourceHandle &resolution);
        USDIMAGING_API
        Builder &SetPixelAspectRatio(
            const HdFloatDataSourceHandle &pixelAspectRatio);
        USDIMAGING_API
        Builder &SetAspectRatioConformPolicy(
            const HdTokenDataSourceHandle &aspectRatioConformPolicy);
        USDIMAGING_API
        Builder &SetDataWindowNDC(
            const HdVec4fDataSourceHandle &dataWindowNDC);
        USDIMAGING_API
        Builder &SetDisableMotionBlur(
            const HdBoolDataSourceHandle &disableMotionBlur);
        USDIMAGING_API
        Builder &SetCamera(
            const HdPathDataSourceHandle &camera);
        USDIMAGING_API
        Builder &SetProductType(
            const HdTokenDataSourceHandle &productType);
        USDIMAGING_API
        Builder &SetProductName(
            const HdTokenDataSourceHandle &productName);
        USDIMAGING_API
        Builder &SetOrderedVars(
            const HdPathArrayDataSourceHandle &orderedVars);
        USDIMAGING_API
        Builder &SetNamespacedSettings(
            const HdContainerDataSourceHandle &namespacedSettings);

        /// Returns a container data source containing the members set thus far.
        USDIMAGING_API
        HdContainerDataSourceHandle Build();

    private:
        HdVec2iDataSourceHandle _resolution;
        HdFloatDataSourceHandle _pixelAspectRatio;
        HdTokenDataSourceHandle _aspectRatioConformPolicy;
        HdVec4fDataSourceHandle _dataWindowNDC;
        HdBoolDataSourceHandle _disableMotionBlur;
        HdPathDataSourceHandle _camera;
        HdTokenDataSourceHandle _productType;
        HdTokenDataSourceHandle _productName;
        HdPathArrayDataSourceHandle _orderedVars;
        HdContainerDataSourceHandle _namespacedSettings;
    };

    /// Retrieves a container data source with the schema's default name token
    /// "__usdRenderProduct" from the parent container and constructs a
    /// UsdImagingUsdRenderProductSchema instance.
    /// Because the requested container data source may not exist, the result
    /// should be checked with IsDefined() or a bool comparison before use.
    USDIMAGING_API
    static UsdImagingUsdRenderProductSchema GetFromParent(
        const HdContainerDataSourceHandle &fromParentContainer);

    /// Returns an HdDataSourceLocator (relative to the prim-level data source)
    /// where the container representing this schema is found by default.
    USDIMAGING_API
    static const HdDataSourceLocator &GetDefaultLocator();


    /// Returns an HdDataSourceLocator (relative to the prim-level data source)
    /// where the namespacedsettings data source can be found.
    /// This is often useful for checking intersection against the
    /// HdDataSourceLocatorSet sent with HdDataSourceObserver::PrimsDirtied.
    USDIMAGING_API
    static const HdDataSourceLocator &GetNamespacedSettingsLocator();

};

PXR_NAMESPACE_CLOSE_SCOPE

#endif