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

#include "pxr/usdImaging/usdImaging/geomModelSchema.h"

#include "pxr/imaging/hd/retainedDataSource.h"

#include "pxr/base/trace/trace.h"

// --(BEGIN CUSTOM CODE: Includes)--
// --(END CUSTOM CODE: Includes)--

PXR_NAMESPACE_OPEN_SCOPE

TF_DEFINE_PUBLIC_TOKENS(UsdImagingGeomModelSchemaTokens,
    USD_IMAGING_GEOM_MODEL_SCHEMA_TOKENS);

// --(BEGIN CUSTOM CODE: Schema Methods)--
// --(END CUSTOM CODE: Schema Methods)--

HdTokenDataSourceHandle
UsdImagingGeomModelSchema::GetDrawMode()
{
    return _GetTypedDataSource<HdTokenDataSource>(
        UsdImagingGeomModelSchemaTokens->drawMode);
}

HdBoolDataSourceHandle
UsdImagingGeomModelSchema::GetApplyDrawMode()
{
    return _GetTypedDataSource<HdBoolDataSource>(
        UsdImagingGeomModelSchemaTokens->applyDrawMode);
}

HdVec3fDataSourceHandle
UsdImagingGeomModelSchema::GetDrawModeColor()
{
    return _GetTypedDataSource<HdVec3fDataSource>(
        UsdImagingGeomModelSchemaTokens->drawModeColor);
}

HdTokenDataSourceHandle
UsdImagingGeomModelSchema::GetCardGeometry()
{
    return _GetTypedDataSource<HdTokenDataSource>(
        UsdImagingGeomModelSchemaTokens->cardGeometry);
}

HdAssetPathDataSourceHandle
UsdImagingGeomModelSchema::GetCardTextureXPos()
{
    return _GetTypedDataSource<HdAssetPathDataSource>(
        UsdImagingGeomModelSchemaTokens->cardTextureXPos);
}

HdAssetPathDataSourceHandle
UsdImagingGeomModelSchema::GetCardTextureYPos()
{
    return _GetTypedDataSource<HdAssetPathDataSource>(
        UsdImagingGeomModelSchemaTokens->cardTextureYPos);
}

HdAssetPathDataSourceHandle
UsdImagingGeomModelSchema::GetCardTextureZPos()
{
    return _GetTypedDataSource<HdAssetPathDataSource>(
        UsdImagingGeomModelSchemaTokens->cardTextureZPos);
}

HdAssetPathDataSourceHandle
UsdImagingGeomModelSchema::GetCardTextureXNeg()
{
    return _GetTypedDataSource<HdAssetPathDataSource>(
        UsdImagingGeomModelSchemaTokens->cardTextureXNeg);
}

HdAssetPathDataSourceHandle
UsdImagingGeomModelSchema::GetCardTextureYNeg()
{
    return _GetTypedDataSource<HdAssetPathDataSource>(
        UsdImagingGeomModelSchemaTokens->cardTextureYNeg);
}

HdAssetPathDataSourceHandle
UsdImagingGeomModelSchema::GetCardTextureZNeg()
{
    return _GetTypedDataSource<HdAssetPathDataSource>(
        UsdImagingGeomModelSchemaTokens->cardTextureZNeg);
}

/*static*/
HdContainerDataSourceHandle
UsdImagingGeomModelSchema::BuildRetained(
        const HdTokenDataSourceHandle &drawMode,
        const HdBoolDataSourceHandle &applyDrawMode,
        const HdVec3fDataSourceHandle &drawModeColor,
        const HdTokenDataSourceHandle &cardGeometry,
        const HdAssetPathDataSourceHandle &cardTextureXPos,
        const HdAssetPathDataSourceHandle &cardTextureYPos,
        const HdAssetPathDataSourceHandle &cardTextureZPos,
        const HdAssetPathDataSourceHandle &cardTextureXNeg,
        const HdAssetPathDataSourceHandle &cardTextureYNeg,
        const HdAssetPathDataSourceHandle &cardTextureZNeg
)
{
    TfToken _names[10];
    HdDataSourceBaseHandle _values[10];

    size_t _count = 0;

    if (drawMode) {
        _names[_count] = UsdImagingGeomModelSchemaTokens->drawMode;
        _values[_count++] = drawMode;
    }

    if (applyDrawMode) {
        _names[_count] = UsdImagingGeomModelSchemaTokens->applyDrawMode;
        _values[_count++] = applyDrawMode;
    }

    if (drawModeColor) {
        _names[_count] = UsdImagingGeomModelSchemaTokens->drawModeColor;
        _values[_count++] = drawModeColor;
    }

    if (cardGeometry) {
        _names[_count] = UsdImagingGeomModelSchemaTokens->cardGeometry;
        _values[_count++] = cardGeometry;
    }

    if (cardTextureXPos) {
        _names[_count] = UsdImagingGeomModelSchemaTokens->cardTextureXPos;
        _values[_count++] = cardTextureXPos;
    }

    if (cardTextureYPos) {
        _names[_count] = UsdImagingGeomModelSchemaTokens->cardTextureYPos;
        _values[_count++] = cardTextureYPos;
    }

    if (cardTextureZPos) {
        _names[_count] = UsdImagingGeomModelSchemaTokens->cardTextureZPos;
        _values[_count++] = cardTextureZPos;
    }

    if (cardTextureXNeg) {
        _names[_count] = UsdImagingGeomModelSchemaTokens->cardTextureXNeg;
        _values[_count++] = cardTextureXNeg;
    }

    if (cardTextureYNeg) {
        _names[_count] = UsdImagingGeomModelSchemaTokens->cardTextureYNeg;
        _values[_count++] = cardTextureYNeg;
    }

    if (cardTextureZNeg) {
        _names[_count] = UsdImagingGeomModelSchemaTokens->cardTextureZNeg;
        _values[_count++] = cardTextureZNeg;
    }
    return HdRetainedContainerDataSource::New(_count, _names, _values);
}

UsdImagingGeomModelSchema::Builder &
UsdImagingGeomModelSchema::Builder::SetDrawMode(
    const HdTokenDataSourceHandle &drawMode)
{
    _drawMode = drawMode;
    return *this;
}

UsdImagingGeomModelSchema::Builder &
UsdImagingGeomModelSchema::Builder::SetApplyDrawMode(
    const HdBoolDataSourceHandle &applyDrawMode)
{
    _applyDrawMode = applyDrawMode;
    return *this;
}

UsdImagingGeomModelSchema::Builder &
UsdImagingGeomModelSchema::Builder::SetDrawModeColor(
    const HdVec3fDataSourceHandle &drawModeColor)
{
    _drawModeColor = drawModeColor;
    return *this;
}

UsdImagingGeomModelSchema::Builder &
UsdImagingGeomModelSchema::Builder::SetCardGeometry(
    const HdTokenDataSourceHandle &cardGeometry)
{
    _cardGeometry = cardGeometry;
    return *this;
}

UsdImagingGeomModelSchema::Builder &
UsdImagingGeomModelSchema::Builder::SetCardTextureXPos(
    const HdAssetPathDataSourceHandle &cardTextureXPos)
{
    _cardTextureXPos = cardTextureXPos;
    return *this;
}

UsdImagingGeomModelSchema::Builder &
UsdImagingGeomModelSchema::Builder::SetCardTextureYPos(
    const HdAssetPathDataSourceHandle &cardTextureYPos)
{
    _cardTextureYPos = cardTextureYPos;
    return *this;
}

UsdImagingGeomModelSchema::Builder &
UsdImagingGeomModelSchema::Builder::SetCardTextureZPos(
    const HdAssetPathDataSourceHandle &cardTextureZPos)
{
    _cardTextureZPos = cardTextureZPos;
    return *this;
}

UsdImagingGeomModelSchema::Builder &
UsdImagingGeomModelSchema::Builder::SetCardTextureXNeg(
    const HdAssetPathDataSourceHandle &cardTextureXNeg)
{
    _cardTextureXNeg = cardTextureXNeg;
    return *this;
}

UsdImagingGeomModelSchema::Builder &
UsdImagingGeomModelSchema::Builder::SetCardTextureYNeg(
    const HdAssetPathDataSourceHandle &cardTextureYNeg)
{
    _cardTextureYNeg = cardTextureYNeg;
    return *this;
}

UsdImagingGeomModelSchema::Builder &
UsdImagingGeomModelSchema::Builder::SetCardTextureZNeg(
    const HdAssetPathDataSourceHandle &cardTextureZNeg)
{
    _cardTextureZNeg = cardTextureZNeg;
    return *this;
}

HdContainerDataSourceHandle
UsdImagingGeomModelSchema::Builder::Build()
{
    return UsdImagingGeomModelSchema::BuildRetained(
        _drawMode,
        _applyDrawMode,
        _drawModeColor,
        _cardGeometry,
        _cardTextureXPos,
        _cardTextureYPos,
        _cardTextureZPos,
        _cardTextureXNeg,
        _cardTextureYNeg,
        _cardTextureZNeg
    );
}

/*static*/
UsdImagingGeomModelSchema
UsdImagingGeomModelSchema::GetFromParent(
        const HdContainerDataSourceHandle &fromParentContainer)
{
    return UsdImagingGeomModelSchema(
        fromParentContainer
        ? HdContainerDataSource::Cast(fromParentContainer->Get(
                UsdImagingGeomModelSchemaTokens->geomModel))
        : nullptr);
}

/*static*/
const TfToken &
UsdImagingGeomModelSchema::GetSchemaToken()
{
    return UsdImagingGeomModelSchemaTokens->geomModel;
}

/*static*/
const HdDataSourceLocator &
UsdImagingGeomModelSchema::GetDefaultLocator()
{
    static const HdDataSourceLocator locator(GetSchemaToken());
    return locator;
}

/* static */
const HdDataSourceLocator &
UsdImagingGeomModelSchema::GetDrawModeLocator()
{
    static const HdDataSourceLocator locator =
        GetDefaultLocator().Append(
            UsdImagingGeomModelSchemaTokens->drawMode);
    return locator;
}

/*static*/
HdTokenDataSourceHandle
UsdImagingGeomModelSchema::BuildDrawModeDataSource(
    const TfToken &drawMode)
{

    if (drawMode == UsdImagingGeomModelSchemaTokens->default_) {
        static const HdRetainedTypedSampledDataSource<TfToken>::Handle ds =
            HdRetainedTypedSampledDataSource<TfToken>::New(drawMode);
        return ds;
    }
    if (drawMode == UsdImagingGeomModelSchemaTokens->origin) {
        static const HdRetainedTypedSampledDataSource<TfToken>::Handle ds =
            HdRetainedTypedSampledDataSource<TfToken>::New(drawMode);
        return ds;
    }
    if (drawMode == UsdImagingGeomModelSchemaTokens->bounds) {
        static const HdRetainedTypedSampledDataSource<TfToken>::Handle ds =
            HdRetainedTypedSampledDataSource<TfToken>::New(drawMode);
        return ds;
    }
    if (drawMode == UsdImagingGeomModelSchemaTokens->cards) {
        static const HdRetainedTypedSampledDataSource<TfToken>::Handle ds =
            HdRetainedTypedSampledDataSource<TfToken>::New(drawMode);
        return ds;
    }
    if (drawMode == UsdImagingGeomModelSchemaTokens->inherited) {
        static const HdRetainedTypedSampledDataSource<TfToken>::Handle ds =
            HdRetainedTypedSampledDataSource<TfToken>::New(drawMode);
        return ds;
    }
    // fallback for unknown token
    return HdRetainedTypedSampledDataSource<TfToken>::New(drawMode);
}

/*static*/
HdTokenDataSourceHandle
UsdImagingGeomModelSchema::BuildCardGeometryDataSource(
    const TfToken &cardGeometry)
{

    if (cardGeometry == UsdImagingGeomModelSchemaTokens->cross) {
        static const HdRetainedTypedSampledDataSource<TfToken>::Handle ds =
            HdRetainedTypedSampledDataSource<TfToken>::New(cardGeometry);
        return ds;
    }
    if (cardGeometry == UsdImagingGeomModelSchemaTokens->box) {
        static const HdRetainedTypedSampledDataSource<TfToken>::Handle ds =
            HdRetainedTypedSampledDataSource<TfToken>::New(cardGeometry);
        return ds;
    }
    if (cardGeometry == UsdImagingGeomModelSchemaTokens->fromTexture) {
        static const HdRetainedTypedSampledDataSource<TfToken>::Handle ds =
            HdRetainedTypedSampledDataSource<TfToken>::New(cardGeometry);
        return ds;
    }
    // fallback for unknown token
    return HdRetainedTypedSampledDataSource<TfToken>::New(cardGeometry);
} 

PXR_NAMESPACE_CLOSE_SCOPE