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

#include "pxr/imaging/hd/lensDistortionSchema.h"

#include "pxr/imaging/hd/retainedDataSource.h"
#include "pxr/imaging/hd/cameraSchema.h"

#include "pxr/base/trace/trace.h"

// --(BEGIN CUSTOM CODE: Includes)--
// --(END CUSTOM CODE: Includes)--

PXR_NAMESPACE_OPEN_SCOPE

TF_DEFINE_PUBLIC_TOKENS(HdLensDistortionSchemaTokens,
    HD_LENS_DISTORTION_SCHEMA_TOKENS);

// --(BEGIN CUSTOM CODE: Schema Methods)--
// --(END CUSTOM CODE: Schema Methods)--

HdTokenDataSourceHandle
HdLensDistortionSchema::GetType() const
{
    return _GetTypedDataSource<HdTokenDataSource>(
        HdLensDistortionSchemaTokens->type);
}

HdFloatDataSourceHandle
HdLensDistortionSchema::GetK1() const
{
    return _GetTypedDataSource<HdFloatDataSource>(
        HdLensDistortionSchemaTokens->k1);
}

HdFloatDataSourceHandle
HdLensDistortionSchema::GetK2() const
{
    return _GetTypedDataSource<HdFloatDataSource>(
        HdLensDistortionSchemaTokens->k2);
}

HdVec2fDataSourceHandle
HdLensDistortionSchema::GetCenter() const
{
    return _GetTypedDataSource<HdVec2fDataSource>(
        HdLensDistortionSchemaTokens->center);
}

HdFloatDataSourceHandle
HdLensDistortionSchema::GetAnaSq() const
{
    return _GetTypedDataSource<HdFloatDataSource>(
        HdLensDistortionSchemaTokens->anaSq);
}

HdVec2fDataSourceHandle
HdLensDistortionSchema::GetAsym() const
{
    return _GetTypedDataSource<HdVec2fDataSource>(
        HdLensDistortionSchemaTokens->asym);
}

HdFloatDataSourceHandle
HdLensDistortionSchema::GetScale() const
{
    return _GetTypedDataSource<HdFloatDataSource>(
        HdLensDistortionSchemaTokens->scale);
}

HdFloatDataSourceHandle
HdLensDistortionSchema::GetIor() const
{
    return _GetTypedDataSource<HdFloatDataSource>(
        HdLensDistortionSchemaTokens->ior);
}

/*static*/
HdContainerDataSourceHandle
HdLensDistortionSchema::BuildRetained(
        const HdTokenDataSourceHandle &type,
        const HdFloatDataSourceHandle &k1,
        const HdFloatDataSourceHandle &k2,
        const HdVec2fDataSourceHandle &center,
        const HdFloatDataSourceHandle &anaSq,
        const HdVec2fDataSourceHandle &asym,
        const HdFloatDataSourceHandle &scale,
        const HdFloatDataSourceHandle &ior
)
{
    TfToken _names[8];
    HdDataSourceBaseHandle _values[8];

    size_t _count = 0;

    if (type) {
        _names[_count] = HdLensDistortionSchemaTokens->type;
        _values[_count++] = type;
    }

    if (k1) {
        _names[_count] = HdLensDistortionSchemaTokens->k1;
        _values[_count++] = k1;
    }

    if (k2) {
        _names[_count] = HdLensDistortionSchemaTokens->k2;
        _values[_count++] = k2;
    }

    if (center) {
        _names[_count] = HdLensDistortionSchemaTokens->center;
        _values[_count++] = center;
    }

    if (anaSq) {
        _names[_count] = HdLensDistortionSchemaTokens->anaSq;
        _values[_count++] = anaSq;
    }

    if (asym) {
        _names[_count] = HdLensDistortionSchemaTokens->asym;
        _values[_count++] = asym;
    }

    if (scale) {
        _names[_count] = HdLensDistortionSchemaTokens->scale;
        _values[_count++] = scale;
    }

    if (ior) {
        _names[_count] = HdLensDistortionSchemaTokens->ior;
        _values[_count++] = ior;
    }
    return HdRetainedContainerDataSource::New(_count, _names, _values);
}

HdLensDistortionSchema::Builder &
HdLensDistortionSchema::Builder::SetType(
    const HdTokenDataSourceHandle &type)
{
    _type = type;
    return *this;
}

HdLensDistortionSchema::Builder &
HdLensDistortionSchema::Builder::SetK1(
    const HdFloatDataSourceHandle &k1)
{
    _k1 = k1;
    return *this;
}

HdLensDistortionSchema::Builder &
HdLensDistortionSchema::Builder::SetK2(
    const HdFloatDataSourceHandle &k2)
{
    _k2 = k2;
    return *this;
}

HdLensDistortionSchema::Builder &
HdLensDistortionSchema::Builder::SetCenter(
    const HdVec2fDataSourceHandle &center)
{
    _center = center;
    return *this;
}

HdLensDistortionSchema::Builder &
HdLensDistortionSchema::Builder::SetAnaSq(
    const HdFloatDataSourceHandle &anaSq)
{
    _anaSq = anaSq;
    return *this;
}

HdLensDistortionSchema::Builder &
HdLensDistortionSchema::Builder::SetAsym(
    const HdVec2fDataSourceHandle &asym)
{
    _asym = asym;
    return *this;
}

HdLensDistortionSchema::Builder &
HdLensDistortionSchema::Builder::SetScale(
    const HdFloatDataSourceHandle &scale)
{
    _scale = scale;
    return *this;
}

HdLensDistortionSchema::Builder &
HdLensDistortionSchema::Builder::SetIor(
    const HdFloatDataSourceHandle &ior)
{
    _ior = ior;
    return *this;
}

HdContainerDataSourceHandle
HdLensDistortionSchema::Builder::Build()
{
    return HdLensDistortionSchema::BuildRetained(
        _type,
        _k1,
        _k2,
        _center,
        _anaSq,
        _asym,
        _scale,
        _ior
    );
}

/*static*/
HdLensDistortionSchema
HdLensDistortionSchema::GetFromParent(
        const HdContainerDataSourceHandle &fromParentContainer)
{
    return HdLensDistortionSchema(
        fromParentContainer
        ? HdContainerDataSource::Cast(fromParentContainer->Get(
                HdLensDistortionSchemaTokens->lensDistortion))
        : nullptr);
}

/*static*/
const TfToken &
HdLensDistortionSchema::GetSchemaToken()
{
    return HdLensDistortionSchemaTokens->lensDistortion;
}

/*static*/
const HdDataSourceLocator &
HdLensDistortionSchema::GetDefaultLocator()
{
    static const HdDataSourceLocator locator =
        HdCameraSchema::GetDefaultLocator().Append(GetSchemaToken());
    return locator;
}

/*static*/
HdTokenDataSourceHandle
HdLensDistortionSchema::BuildTypeDataSource(
    const TfToken &type)
{

    if (type == HdLensDistortionSchemaTokens->standard) {
        static const HdRetainedTypedSampledDataSource<TfToken>::Handle ds =
            HdRetainedTypedSampledDataSource<TfToken>::New(type);
        return ds;
    }
    if (type == HdLensDistortionSchemaTokens->fisheye) {
        static const HdRetainedTypedSampledDataSource<TfToken>::Handle ds =
            HdRetainedTypedSampledDataSource<TfToken>::New(type);
        return ds;
    }
    // fallback for unknown token
    return HdRetainedTypedSampledDataSource<TfToken>::New(type);
} 

PXR_NAMESPACE_CLOSE_SCOPE