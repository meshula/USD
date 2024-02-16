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

#include "pxr/imaging/hd/cameraSchema.h"

#include "pxr/imaging/hd/retainedDataSource.h"

#include "pxr/base/trace/trace.h"

// --(BEGIN CUSTOM CODE: Includes)--
// --(END CUSTOM CODE: Includes)--

PXR_NAMESPACE_OPEN_SCOPE

TF_DEFINE_PUBLIC_TOKENS(HdCameraSchemaTokens,
    HD_CAMERA_SCHEMA_TOKENS);

// --(BEGIN CUSTOM CODE: Schema Methods)--
// --(END CUSTOM CODE: Schema Methods)--

HdTokenDataSourceHandle
HdCameraSchema::GetProjection()
{
    return _GetTypedDataSource<HdTokenDataSource>(
        HdCameraSchemaTokens->projection);
}

HdFloatDataSourceHandle
HdCameraSchema::GetHorizontalAperture()
{
    return _GetTypedDataSource<HdFloatDataSource>(
        HdCameraSchemaTokens->horizontalAperture);
}

HdFloatDataSourceHandle
HdCameraSchema::GetVerticalAperture()
{
    return _GetTypedDataSource<HdFloatDataSource>(
        HdCameraSchemaTokens->verticalAperture);
}

HdFloatDataSourceHandle
HdCameraSchema::GetHorizontalApertureOffset()
{
    return _GetTypedDataSource<HdFloatDataSource>(
        HdCameraSchemaTokens->horizontalApertureOffset);
}

HdFloatDataSourceHandle
HdCameraSchema::GetVerticalApertureOffset()
{
    return _GetTypedDataSource<HdFloatDataSource>(
        HdCameraSchemaTokens->verticalApertureOffset);
}

HdFloatDataSourceHandle
HdCameraSchema::GetFocalLength()
{
    return _GetTypedDataSource<HdFloatDataSource>(
        HdCameraSchemaTokens->focalLength);
}

HdVec2fDataSourceHandle
HdCameraSchema::GetClippingRange()
{
    return _GetTypedDataSource<HdVec2fDataSource>(
        HdCameraSchemaTokens->clippingRange);
}

HdVec4dArrayDataSourceHandle
HdCameraSchema::GetClippingPlanes()
{
    return _GetTypedDataSource<HdVec4dArrayDataSource>(
        HdCameraSchemaTokens->clippingPlanes);
}

HdFloatDataSourceHandle
HdCameraSchema::GetFStop()
{
    return _GetTypedDataSource<HdFloatDataSource>(
        HdCameraSchemaTokens->fStop);
}

HdFloatDataSourceHandle
HdCameraSchema::GetFocusDistance()
{
    return _GetTypedDataSource<HdFloatDataSource>(
        HdCameraSchemaTokens->focusDistance);
}

HdDoubleDataSourceHandle
HdCameraSchema::GetShutterOpen()
{
    return _GetTypedDataSource<HdDoubleDataSource>(
        HdCameraSchemaTokens->shutterOpen);
}

HdDoubleDataSourceHandle
HdCameraSchema::GetShutterClose()
{
    return _GetTypedDataSource<HdDoubleDataSource>(
        HdCameraSchemaTokens->shutterClose);
}

HdFloatDataSourceHandle
HdCameraSchema::GetExposure()
{
    return _GetTypedDataSource<HdFloatDataSource>(
        HdCameraSchemaTokens->exposure);
}

HdBoolDataSourceHandle
HdCameraSchema::GetFocusOn()
{
    return _GetTypedDataSource<HdBoolDataSource>(
        HdCameraSchemaTokens->focusOn);
}

HdFloatDataSourceHandle
HdCameraSchema::GetDofAspect()
{
    return _GetTypedDataSource<HdFloatDataSource>(
        HdCameraSchemaTokens->dofAspect);
}

HdSplitDiopterSchema
HdCameraSchema::GetSplitDiopter()
{
    return HdSplitDiopterSchema(_GetTypedDataSource<HdContainerDataSource>(
        HdCameraSchemaTokens->splitDiopter));
}

HdLensDistortionSchema
HdCameraSchema::GetLensDistortion()
{
    return HdLensDistortionSchema(_GetTypedDataSource<HdContainerDataSource>(
        HdCameraSchemaTokens->lensDistortion));
}

/*static*/
HdContainerDataSourceHandle
HdCameraSchema::BuildRetained(
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
)
{
    TfToken _names[17];
    HdDataSourceBaseHandle _values[17];

    size_t _count = 0;

    if (projection) {
        _names[_count] = HdCameraSchemaTokens->projection;
        _values[_count++] = projection;
    }

    if (horizontalAperture) {
        _names[_count] = HdCameraSchemaTokens->horizontalAperture;
        _values[_count++] = horizontalAperture;
    }

    if (verticalAperture) {
        _names[_count] = HdCameraSchemaTokens->verticalAperture;
        _values[_count++] = verticalAperture;
    }

    if (horizontalApertureOffset) {
        _names[_count] = HdCameraSchemaTokens->horizontalApertureOffset;
        _values[_count++] = horizontalApertureOffset;
    }

    if (verticalApertureOffset) {
        _names[_count] = HdCameraSchemaTokens->verticalApertureOffset;
        _values[_count++] = verticalApertureOffset;
    }

    if (focalLength) {
        _names[_count] = HdCameraSchemaTokens->focalLength;
        _values[_count++] = focalLength;
    }

    if (clippingRange) {
        _names[_count] = HdCameraSchemaTokens->clippingRange;
        _values[_count++] = clippingRange;
    }

    if (clippingPlanes) {
        _names[_count] = HdCameraSchemaTokens->clippingPlanes;
        _values[_count++] = clippingPlanes;
    }

    if (fStop) {
        _names[_count] = HdCameraSchemaTokens->fStop;
        _values[_count++] = fStop;
    }

    if (focusDistance) {
        _names[_count] = HdCameraSchemaTokens->focusDistance;
        _values[_count++] = focusDistance;
    }

    if (shutterOpen) {
        _names[_count] = HdCameraSchemaTokens->shutterOpen;
        _values[_count++] = shutterOpen;
    }

    if (shutterClose) {
        _names[_count] = HdCameraSchemaTokens->shutterClose;
        _values[_count++] = shutterClose;
    }

    if (exposure) {
        _names[_count] = HdCameraSchemaTokens->exposure;
        _values[_count++] = exposure;
    }

    if (focusOn) {
        _names[_count] = HdCameraSchemaTokens->focusOn;
        _values[_count++] = focusOn;
    }

    if (dofAspect) {
        _names[_count] = HdCameraSchemaTokens->dofAspect;
        _values[_count++] = dofAspect;
    }

    if (splitDiopter) {
        _names[_count] = HdCameraSchemaTokens->splitDiopter;
        _values[_count++] = splitDiopter;
    }

    if (lensDistortion) {
        _names[_count] = HdCameraSchemaTokens->lensDistortion;
        _values[_count++] = lensDistortion;
    }
    return HdRetainedContainerDataSource::New(_count, _names, _values);
}

HdCameraSchema::Builder &
HdCameraSchema::Builder::SetProjection(
    const HdTokenDataSourceHandle &projection)
{
    _projection = projection;
    return *this;
}

HdCameraSchema::Builder &
HdCameraSchema::Builder::SetHorizontalAperture(
    const HdFloatDataSourceHandle &horizontalAperture)
{
    _horizontalAperture = horizontalAperture;
    return *this;
}

HdCameraSchema::Builder &
HdCameraSchema::Builder::SetVerticalAperture(
    const HdFloatDataSourceHandle &verticalAperture)
{
    _verticalAperture = verticalAperture;
    return *this;
}

HdCameraSchema::Builder &
HdCameraSchema::Builder::SetHorizontalApertureOffset(
    const HdFloatDataSourceHandle &horizontalApertureOffset)
{
    _horizontalApertureOffset = horizontalApertureOffset;
    return *this;
}

HdCameraSchema::Builder &
HdCameraSchema::Builder::SetVerticalApertureOffset(
    const HdFloatDataSourceHandle &verticalApertureOffset)
{
    _verticalApertureOffset = verticalApertureOffset;
    return *this;
}

HdCameraSchema::Builder &
HdCameraSchema::Builder::SetFocalLength(
    const HdFloatDataSourceHandle &focalLength)
{
    _focalLength = focalLength;
    return *this;
}

HdCameraSchema::Builder &
HdCameraSchema::Builder::SetClippingRange(
    const HdVec2fDataSourceHandle &clippingRange)
{
    _clippingRange = clippingRange;
    return *this;
}

HdCameraSchema::Builder &
HdCameraSchema::Builder::SetClippingPlanes(
    const HdVec4dArrayDataSourceHandle &clippingPlanes)
{
    _clippingPlanes = clippingPlanes;
    return *this;
}

HdCameraSchema::Builder &
HdCameraSchema::Builder::SetFStop(
    const HdFloatDataSourceHandle &fStop)
{
    _fStop = fStop;
    return *this;
}

HdCameraSchema::Builder &
HdCameraSchema::Builder::SetFocusDistance(
    const HdFloatDataSourceHandle &focusDistance)
{
    _focusDistance = focusDistance;
    return *this;
}

HdCameraSchema::Builder &
HdCameraSchema::Builder::SetShutterOpen(
    const HdDoubleDataSourceHandle &shutterOpen)
{
    _shutterOpen = shutterOpen;
    return *this;
}

HdCameraSchema::Builder &
HdCameraSchema::Builder::SetShutterClose(
    const HdDoubleDataSourceHandle &shutterClose)
{
    _shutterClose = shutterClose;
    return *this;
}

HdCameraSchema::Builder &
HdCameraSchema::Builder::SetExposure(
    const HdFloatDataSourceHandle &exposure)
{
    _exposure = exposure;
    return *this;
}

HdCameraSchema::Builder &
HdCameraSchema::Builder::SetFocusOn(
    const HdBoolDataSourceHandle &focusOn)
{
    _focusOn = focusOn;
    return *this;
}

HdCameraSchema::Builder &
HdCameraSchema::Builder::SetDofAspect(
    const HdFloatDataSourceHandle &dofAspect)
{
    _dofAspect = dofAspect;
    return *this;
}

HdCameraSchema::Builder &
HdCameraSchema::Builder::SetSplitDiopter(
    const HdContainerDataSourceHandle &splitDiopter)
{
    _splitDiopter = splitDiopter;
    return *this;
}

HdCameraSchema::Builder &
HdCameraSchema::Builder::SetLensDistortion(
    const HdContainerDataSourceHandle &lensDistortion)
{
    _lensDistortion = lensDistortion;
    return *this;
}

HdContainerDataSourceHandle
HdCameraSchema::Builder::Build()
{
    return HdCameraSchema::BuildRetained(
        _projection,
        _horizontalAperture,
        _verticalAperture,
        _horizontalApertureOffset,
        _verticalApertureOffset,
        _focalLength,
        _clippingRange,
        _clippingPlanes,
        _fStop,
        _focusDistance,
        _shutterOpen,
        _shutterClose,
        _exposure,
        _focusOn,
        _dofAspect,
        _splitDiopter,
        _lensDistortion
    );
}

/*static*/
HdCameraSchema
HdCameraSchema::GetFromParent(
        const HdContainerDataSourceHandle &fromParentContainer)
{
    return HdCameraSchema(
        fromParentContainer
        ? HdContainerDataSource::Cast(fromParentContainer->Get(
                HdCameraSchemaTokens->camera))
        : nullptr);
}

/*static*/
const TfToken &
HdCameraSchema::GetSchemaToken()
{
    return HdCameraSchemaTokens->camera;
}

/*static*/
const HdDataSourceLocator &
HdCameraSchema::GetDefaultLocator()
{
    static const HdDataSourceLocator locator(GetSchemaToken());
    return locator;
}

/* static */
const HdDataSourceLocator &
HdCameraSchema::GetShutterOpenLocator()
{
    static const HdDataSourceLocator locator =
        GetDefaultLocator().Append(
            HdCameraSchemaTokens->shutterOpen);
    return locator;
}

/* static */
const HdDataSourceLocator &
HdCameraSchema::GetShutterCloseLocator()
{
    static const HdDataSourceLocator locator =
        GetDefaultLocator().Append(
            HdCameraSchemaTokens->shutterClose);
    return locator;
}

/*static*/
HdTokenDataSourceHandle
HdCameraSchema::BuildProjectionDataSource(
    const TfToken &projection)
{

    if (projection == HdCameraSchemaTokens->perspective) {
        static const HdRetainedTypedSampledDataSource<TfToken>::Handle ds =
            HdRetainedTypedSampledDataSource<TfToken>::New(projection);
        return ds;
    }
    if (projection == HdCameraSchemaTokens->orthographic) {
        static const HdRetainedTypedSampledDataSource<TfToken>::Handle ds =
            HdRetainedTypedSampledDataSource<TfToken>::New(projection);
        return ds;
    }
    // fallback for unknown token
    return HdRetainedTypedSampledDataSource<TfToken>::New(projection);
} 

PXR_NAMESPACE_CLOSE_SCOPE