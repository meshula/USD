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

#include "pxr/imaging/hd/nurbsPatchSchema.h"

#include "pxr/imaging/hd/retainedDataSource.h"

#include "pxr/base/trace/trace.h"

// --(BEGIN CUSTOM CODE: Includes)--
// --(END CUSTOM CODE: Includes)--

PXR_NAMESPACE_OPEN_SCOPE

TF_DEFINE_PUBLIC_TOKENS(HdNurbsPatchSchemaTokens,
    HD_NURBS_PATCH_SCHEMA_TOKENS);

// --(BEGIN CUSTOM CODE: Schema Methods)--
// --(END CUSTOM CODE: Schema Methods)--

HdIntDataSourceHandle
HdNurbsPatchSchema::GetUVertexCount() const
{
    return _GetTypedDataSource<HdIntDataSource>(
        HdNurbsPatchSchemaTokens->uVertexCount);
}

HdIntDataSourceHandle
HdNurbsPatchSchema::GetVVertexCount() const
{
    return _GetTypedDataSource<HdIntDataSource>(
        HdNurbsPatchSchemaTokens->vVertexCount);
}

HdIntDataSourceHandle
HdNurbsPatchSchema::GetUOrder() const
{
    return _GetTypedDataSource<HdIntDataSource>(
        HdNurbsPatchSchemaTokens->uOrder);
}

HdIntDataSourceHandle
HdNurbsPatchSchema::GetVOrder() const
{
    return _GetTypedDataSource<HdIntDataSource>(
        HdNurbsPatchSchemaTokens->vOrder);
}

HdDoubleArrayDataSourceHandle
HdNurbsPatchSchema::GetUKnots() const
{
    return _GetTypedDataSource<HdDoubleArrayDataSource>(
        HdNurbsPatchSchemaTokens->uKnots);
}

HdDoubleArrayDataSourceHandle
HdNurbsPatchSchema::GetVKnots() const
{
    return _GetTypedDataSource<HdDoubleArrayDataSource>(
        HdNurbsPatchSchemaTokens->vKnots);
}

HdTokenDataSourceHandle
HdNurbsPatchSchema::GetUForm() const
{
    return _GetTypedDataSource<HdTokenDataSource>(
        HdNurbsPatchSchemaTokens->uForm);
}

HdTokenDataSourceHandle
HdNurbsPatchSchema::GetVForm() const
{
    return _GetTypedDataSource<HdTokenDataSource>(
        HdNurbsPatchSchemaTokens->vForm);
}

HdVec2dDataSourceHandle
HdNurbsPatchSchema::GetURange() const
{
    return _GetTypedDataSource<HdVec2dDataSource>(
        HdNurbsPatchSchemaTokens->uRange);
}

HdVec2dDataSourceHandle
HdNurbsPatchSchema::GetVRange() const
{
    return _GetTypedDataSource<HdVec2dDataSource>(
        HdNurbsPatchSchemaTokens->vRange);
}

HdNurbsPatchTrimCurveSchema
HdNurbsPatchSchema::GetTrimCurve() const
{
    return HdNurbsPatchTrimCurveSchema(_GetTypedDataSource<HdContainerDataSource>(
        HdNurbsPatchSchemaTokens->trimCurve));
}

HdTokenDataSourceHandle
HdNurbsPatchSchema::GetOrientation() const
{
    return _GetTypedDataSource<HdTokenDataSource>(
        HdNurbsPatchSchemaTokens->orientation);
}

HdBoolDataSourceHandle
HdNurbsPatchSchema::GetDoubleSided() const
{
    return _GetTypedDataSource<HdBoolDataSource>(
        HdNurbsPatchSchemaTokens->doubleSided);
}

/*static*/
HdContainerDataSourceHandle
HdNurbsPatchSchema::BuildRetained(
        const HdIntDataSourceHandle &uVertexCount,
        const HdIntDataSourceHandle &vVertexCount,
        const HdIntDataSourceHandle &uOrder,
        const HdIntDataSourceHandle &vOrder,
        const HdDoubleArrayDataSourceHandle &uKnots,
        const HdDoubleArrayDataSourceHandle &vKnots,
        const HdTokenDataSourceHandle &uForm,
        const HdTokenDataSourceHandle &vForm,
        const HdVec2dDataSourceHandle &uRange,
        const HdVec2dDataSourceHandle &vRange,
        const HdContainerDataSourceHandle &trimCurve,
        const HdTokenDataSourceHandle &orientation,
        const HdBoolDataSourceHandle &doubleSided
)
{
    TfToken _names[13];
    HdDataSourceBaseHandle _values[13];

    size_t _count = 0;

    if (uVertexCount) {
        _names[_count] = HdNurbsPatchSchemaTokens->uVertexCount;
        _values[_count++] = uVertexCount;
    }

    if (vVertexCount) {
        _names[_count] = HdNurbsPatchSchemaTokens->vVertexCount;
        _values[_count++] = vVertexCount;
    }

    if (uOrder) {
        _names[_count] = HdNurbsPatchSchemaTokens->uOrder;
        _values[_count++] = uOrder;
    }

    if (vOrder) {
        _names[_count] = HdNurbsPatchSchemaTokens->vOrder;
        _values[_count++] = vOrder;
    }

    if (uKnots) {
        _names[_count] = HdNurbsPatchSchemaTokens->uKnots;
        _values[_count++] = uKnots;
    }

    if (vKnots) {
        _names[_count] = HdNurbsPatchSchemaTokens->vKnots;
        _values[_count++] = vKnots;
    }

    if (uForm) {
        _names[_count] = HdNurbsPatchSchemaTokens->uForm;
        _values[_count++] = uForm;
    }

    if (vForm) {
        _names[_count] = HdNurbsPatchSchemaTokens->vForm;
        _values[_count++] = vForm;
    }

    if (uRange) {
        _names[_count] = HdNurbsPatchSchemaTokens->uRange;
        _values[_count++] = uRange;
    }

    if (vRange) {
        _names[_count] = HdNurbsPatchSchemaTokens->vRange;
        _values[_count++] = vRange;
    }

    if (trimCurve) {
        _names[_count] = HdNurbsPatchSchemaTokens->trimCurve;
        _values[_count++] = trimCurve;
    }

    if (orientation) {
        _names[_count] = HdNurbsPatchSchemaTokens->orientation;
        _values[_count++] = orientation;
    }

    if (doubleSided) {
        _names[_count] = HdNurbsPatchSchemaTokens->doubleSided;
        _values[_count++] = doubleSided;
    }
    return HdRetainedContainerDataSource::New(_count, _names, _values);
}

HdNurbsPatchSchema::Builder &
HdNurbsPatchSchema::Builder::SetUVertexCount(
    const HdIntDataSourceHandle &uVertexCount)
{
    _uVertexCount = uVertexCount;
    return *this;
}

HdNurbsPatchSchema::Builder &
HdNurbsPatchSchema::Builder::SetVVertexCount(
    const HdIntDataSourceHandle &vVertexCount)
{
    _vVertexCount = vVertexCount;
    return *this;
}

HdNurbsPatchSchema::Builder &
HdNurbsPatchSchema::Builder::SetUOrder(
    const HdIntDataSourceHandle &uOrder)
{
    _uOrder = uOrder;
    return *this;
}

HdNurbsPatchSchema::Builder &
HdNurbsPatchSchema::Builder::SetVOrder(
    const HdIntDataSourceHandle &vOrder)
{
    _vOrder = vOrder;
    return *this;
}

HdNurbsPatchSchema::Builder &
HdNurbsPatchSchema::Builder::SetUKnots(
    const HdDoubleArrayDataSourceHandle &uKnots)
{
    _uKnots = uKnots;
    return *this;
}

HdNurbsPatchSchema::Builder &
HdNurbsPatchSchema::Builder::SetVKnots(
    const HdDoubleArrayDataSourceHandle &vKnots)
{
    _vKnots = vKnots;
    return *this;
}

HdNurbsPatchSchema::Builder &
HdNurbsPatchSchema::Builder::SetUForm(
    const HdTokenDataSourceHandle &uForm)
{
    _uForm = uForm;
    return *this;
}

HdNurbsPatchSchema::Builder &
HdNurbsPatchSchema::Builder::SetVForm(
    const HdTokenDataSourceHandle &vForm)
{
    _vForm = vForm;
    return *this;
}

HdNurbsPatchSchema::Builder &
HdNurbsPatchSchema::Builder::SetURange(
    const HdVec2dDataSourceHandle &uRange)
{
    _uRange = uRange;
    return *this;
}

HdNurbsPatchSchema::Builder &
HdNurbsPatchSchema::Builder::SetVRange(
    const HdVec2dDataSourceHandle &vRange)
{
    _vRange = vRange;
    return *this;
}

HdNurbsPatchSchema::Builder &
HdNurbsPatchSchema::Builder::SetTrimCurve(
    const HdContainerDataSourceHandle &trimCurve)
{
    _trimCurve = trimCurve;
    return *this;
}

HdNurbsPatchSchema::Builder &
HdNurbsPatchSchema::Builder::SetOrientation(
    const HdTokenDataSourceHandle &orientation)
{
    _orientation = orientation;
    return *this;
}

HdNurbsPatchSchema::Builder &
HdNurbsPatchSchema::Builder::SetDoubleSided(
    const HdBoolDataSourceHandle &doubleSided)
{
    _doubleSided = doubleSided;
    return *this;
}

HdContainerDataSourceHandle
HdNurbsPatchSchema::Builder::Build()
{
    return HdNurbsPatchSchema::BuildRetained(
        _uVertexCount,
        _vVertexCount,
        _uOrder,
        _vOrder,
        _uKnots,
        _vKnots,
        _uForm,
        _vForm,
        _uRange,
        _vRange,
        _trimCurve,
        _orientation,
        _doubleSided
    );
}

/*static*/
HdNurbsPatchSchema
HdNurbsPatchSchema::GetFromParent(
        const HdContainerDataSourceHandle &fromParentContainer)
{
    return HdNurbsPatchSchema(
        fromParentContainer
        ? HdContainerDataSource::Cast(fromParentContainer->Get(
                HdNurbsPatchSchemaTokens->nurbsPatch))
        : nullptr);
}

/*static*/
const TfToken &
HdNurbsPatchSchema::GetSchemaToken()
{
    return HdNurbsPatchSchemaTokens->nurbsPatch;
}

/*static*/
const HdDataSourceLocator &
HdNurbsPatchSchema::GetDefaultLocator()
{
    static const HdDataSourceLocator locator(GetSchemaToken());
    return locator;
}

/*static*/
HdTokenDataSourceHandle
HdNurbsPatchSchema::BuildFormDataSource(
    const TfToken &form)
{

    if (form == HdNurbsPatchSchemaTokens->open) {
        static const HdRetainedTypedSampledDataSource<TfToken>::Handle ds =
            HdRetainedTypedSampledDataSource<TfToken>::New(form);
        return ds;
    }
    if (form == HdNurbsPatchSchemaTokens->closed) {
        static const HdRetainedTypedSampledDataSource<TfToken>::Handle ds =
            HdRetainedTypedSampledDataSource<TfToken>::New(form);
        return ds;
    }
    if (form == HdNurbsPatchSchemaTokens->periodic) {
        static const HdRetainedTypedSampledDataSource<TfToken>::Handle ds =
            HdRetainedTypedSampledDataSource<TfToken>::New(form);
        return ds;
    }
    // fallback for unknown token
    return HdRetainedTypedSampledDataSource<TfToken>::New(form);
}

/*static*/
HdTokenDataSourceHandle
HdNurbsPatchSchema::BuildOrientationDataSource(
    const TfToken &orientation)
{

    if (orientation == HdNurbsPatchSchemaTokens->leftHanded) {
        static const HdRetainedTypedSampledDataSource<TfToken>::Handle ds =
            HdRetainedTypedSampledDataSource<TfToken>::New(orientation);
        return ds;
    }
    if (orientation == HdNurbsPatchSchemaTokens->rightHanded) {
        static const HdRetainedTypedSampledDataSource<TfToken>::Handle ds =
            HdRetainedTypedSampledDataSource<TfToken>::New(orientation);
        return ds;
    }
    // fallback for unknown token
    return HdRetainedTypedSampledDataSource<TfToken>::New(orientation);
} 

PXR_NAMESPACE_CLOSE_SCOPE