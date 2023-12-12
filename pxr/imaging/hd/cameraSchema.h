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
/* ** Edit hdGen/schema.py instead to make changes.                        ** */
/* **                                                                      ** */
/* ************************************************************************** */

#ifndef PXR_IMAGING_HD_CAMERA_SCHEMA_H
#define PXR_IMAGING_HD_CAMERA_SCHEMA_H

#include "pxr/imaging/hd/api.h"

#include "pxr/imaging/hd/splitDiopterSchema.h"
#include "pxr/imaging/hd/lensDistortionSchema.h"


// --(BEGIN CUSTOM CODE: Includes)--
// --(END CUSTOM CODE: Includes)--

PXR_NAMESPACE_OPEN_SCOPE

//-----------------------------------------------------------------------------

// --(BEGIN CUSTOM CODE: Declares)--
// --(END CUSTOM CODE: Declares)--

//-----------------------------------------------------------------------------

#define HD_CAMERA_SCHEMA_TOKENS \
    (camera) \
    (projection) \
    (horizontalAperture) \
    (verticalAperture) \
    (horizontalApertureOffset) \
    (verticalApertureOffset) \
    (focalLength) \
    (clippingRange) \
    (clippingPlanes) \
    (fStop) \
    (focusDistance) \
    (shutterOpen) \
    (shutterClose) \
    (exposure) \
    (focusOn) \
    (dofAspect) \
    (splitDiopter) \
    (lensDistortion) \
    (perspective) \
    (orthographic) \

TF_DECLARE_PUBLIC_TOKENS(HdCameraSchemaTokens, HD_API,
    HD_CAMERA_SCHEMA_TOKENS);

//-----------------------------------------------------------------------------
class HdCameraSchema : public HdSchema
{
public:
    HdCameraSchema(HdContainerDataSourceHandle container)
    : HdSchema(container) {}

// --(BEGIN CUSTOM CODE: Schema Methods)--
// --(END CUSTOM CODE: Schema Methods)--

    //ACCESSORS


    HD_API
    HdTokenDataSourceHandle GetProjection();

    HD_API
    HdFloatDataSourceHandle GetHorizontalAperture();

    HD_API
    HdFloatDataSourceHandle GetVerticalAperture();

    HD_API
    HdFloatDataSourceHandle GetHorizontalApertureOffset();

    HD_API
    HdFloatDataSourceHandle GetVerticalApertureOffset();

    HD_API
    HdFloatDataSourceHandle GetFocalLength();

    HD_API
    HdVec2fDataSourceHandle GetClippingRange();

    HD_API
    HdVec4dArrayDataSourceHandle GetClippingPlanes();

    HD_API
    HdFloatDataSourceHandle GetFStop();

    HD_API
    HdFloatDataSourceHandle GetFocusDistance();

    HD_API
    HdDoubleDataSourceHandle GetShutterOpen();

    HD_API
    HdDoubleDataSourceHandle GetShutterClose();

    HD_API
    HdFloatDataSourceHandle GetExposure();

    HD_API
    HdBoolDataSourceHandle GetFocusOn();

    HD_API
    HdFloatDataSourceHandle GetDofAspect();

    HD_API
    HdSplitDiopterSchema GetSplitDiopter();

    HD_API
    HdLensDistortionSchema GetLensDistortion();

    // RETRIEVING AND CONSTRUCTING

    /// Builds a container data source which includes the provided child data
    /// sources. Parameters with nullptr values are excluded. This is a
    /// low-level interface. For cases in which it's desired to define
    /// the container with a sparse set of child fields, the Builder class
    /// is often more convenient and readable.
    HD_API
    static HdContainerDataSourceHandle
    BuildRetained(
        const HdTokenDataSourceHandle &projection,
        const HdFloatDataSourceHandle &horizontalAperture,
        const HdFloatDataSourceHandle &verticalAperture,
        const HdFloatDataSourceHandle &horizontalApertureOffset,
        const HdFloatDataSourceHandle &verticalApertureOffset,
        const HdFloatDataSourceHandle &focalLength,
        const HdVec2fDataSourceHandle &clippingRange,
        const HdVec4dArrayDataSourceHandle &clippingPlanes,
        const HdFloatDataSourceHandle &fStop,
        const HdFloatDataSourceHandle &focusDistance,
        const HdDoubleDataSourceHandle &shutterOpen,
        const HdDoubleDataSourceHandle &shutterClose,
        const HdFloatDataSourceHandle &exposure,
        const HdBoolDataSourceHandle &focusOn,
        const HdFloatDataSourceHandle &dofAspect,
        const HdContainerDataSourceHandle &splitDiopter,
        const HdContainerDataSourceHandle &lensDistortion
    );

    /// \class HdCameraSchema::Builder
    /// 
    /// Utility class for setting sparse sets of child data source fields to be
    /// filled as arguments into BuildRetained. Because all setter methods
    /// return a reference to the instance, this can be used in the "builder
    /// pattern" form.
    class Builder
    {
    public:
        HD_API
        Builder &SetProjection(
            const HdTokenDataSourceHandle &projection);
        HD_API
        Builder &SetHorizontalAperture(
            const HdFloatDataSourceHandle &horizontalAperture);
        HD_API
        Builder &SetVerticalAperture(
            const HdFloatDataSourceHandle &verticalAperture);
        HD_API
        Builder &SetHorizontalApertureOffset(
            const HdFloatDataSourceHandle &horizontalApertureOffset);
        HD_API
        Builder &SetVerticalApertureOffset(
            const HdFloatDataSourceHandle &verticalApertureOffset);
        HD_API
        Builder &SetFocalLength(
            const HdFloatDataSourceHandle &focalLength);
        HD_API
        Builder &SetClippingRange(
            const HdVec2fDataSourceHandle &clippingRange);
        HD_API
        Builder &SetClippingPlanes(
            const HdVec4dArrayDataSourceHandle &clippingPlanes);
        HD_API
        Builder &SetFStop(
            const HdFloatDataSourceHandle &fStop);
        HD_API
        Builder &SetFocusDistance(
            const HdFloatDataSourceHandle &focusDistance);
        HD_API
        Builder &SetShutterOpen(
            const HdDoubleDataSourceHandle &shutterOpen);
        HD_API
        Builder &SetShutterClose(
            const HdDoubleDataSourceHandle &shutterClose);
        HD_API
        Builder &SetExposure(
            const HdFloatDataSourceHandle &exposure);
        HD_API
        Builder &SetFocusOn(
            const HdBoolDataSourceHandle &focusOn);
        HD_API
        Builder &SetDofAspect(
            const HdFloatDataSourceHandle &dofAspect);
        HD_API
        Builder &SetSplitDiopter(
            const HdContainerDataSourceHandle &splitDiopter);
        HD_API
        Builder &SetLensDistortion(
            const HdContainerDataSourceHandle &lensDistortion);

        /// Returns a container data source containing the members set thus far.
        HD_API
        HdContainerDataSourceHandle Build();

    private:
        HdTokenDataSourceHandle _projection;
        HdFloatDataSourceHandle _horizontalAperture;
        HdFloatDataSourceHandle _verticalAperture;
        HdFloatDataSourceHandle _horizontalApertureOffset;
        HdFloatDataSourceHandle _verticalApertureOffset;
        HdFloatDataSourceHandle _focalLength;
        HdVec2fDataSourceHandle _clippingRange;
        HdVec4dArrayDataSourceHandle _clippingPlanes;
        HdFloatDataSourceHandle _fStop;
        HdFloatDataSourceHandle _focusDistance;
        HdDoubleDataSourceHandle _shutterOpen;
        HdDoubleDataSourceHandle _shutterClose;
        HdFloatDataSourceHandle _exposure;
        HdBoolDataSourceHandle _focusOn;
        HdFloatDataSourceHandle _dofAspect;
        HdContainerDataSourceHandle _splitDiopter;
        HdContainerDataSourceHandle _lensDistortion;
    };

    /// Retrieves a container data source with the schema's default name token
    /// "camera" from the parent container and constructs a
    /// HdCameraSchema instance.
    /// Because the requested container data source may not exist, the result
    /// should be checked with IsDefined() or a bool comparison before use.
    HD_API
    static HdCameraSchema GetFromParent(
        const HdContainerDataSourceHandle &fromParentContainer);

    /// Returns a token where the container representing this schema is found in
    /// a container by default.
    HD_API
    static const TfToken &GetSchemaToken();

    /// Returns an HdDataSourceLocator (relative to the prim-level data source)
    /// where the container representing this schema is found by default.
    HD_API
    static const HdDataSourceLocator &GetDefaultLocator();

    // DATA SOURCE LOCATORS FOR MEMBERS
    //
    // The following methods return an HdDataSourceLocator (relative to the
    // prim-level data source) where the data source for a member can be found.
    //
    // This is often useful for checking intersection against the
    // HdDataSourceLocatorSet sent with HdDataSourceObserver::PrimsDirtied.


    /// Prim-level relative data source locator to locate shutterOpen.
    HD_API
    static const HdDataSourceLocator &GetShutterOpenLocator();

    /// Prim-level relative data source locator to locate shutterClose.
    HD_API
    static const HdDataSourceLocator &GetShutterCloseLocator();


    /// Returns token data source for use as projection value.
    /// Values of...
    /// - HdCameraSchemaTokens->perspective
    /// - HdCameraSchemaTokens->orthographic
    ///     ...will be stored statically and reused for future calls.
    HD_API
    static HdTokenDataSourceHandle BuildProjectionDataSource(
        const TfToken &projection);

};

PXR_NAMESPACE_CLOSE_SCOPE

#endif