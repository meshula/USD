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

#include "pxr/usdImaging/usdImaging/collectionMaterialBindingSchema.h"

#include "pxr/imaging/hd/retainedDataSource.h"

#include "pxr/base/trace/trace.h"

// --(BEGIN CUSTOM CODE: Includes)--
// --(END CUSTOM CODE: Includes)--

PXR_NAMESPACE_OPEN_SCOPE

TF_DEFINE_PUBLIC_TOKENS(UsdImagingCollectionMaterialBindingSchemaTokens,
    USD_IMAGING_COLLECTION_MATERIAL_BINDING_SCHEMA_TOKENS);

// --(BEGIN CUSTOM CODE: Schema Methods)--
// --(END CUSTOM CODE: Schema Methods)--

HdPathDataSourceHandle
UsdImagingCollectionMaterialBindingSchema::GetCollectionPath()
{
    return _GetTypedDataSource<HdPathDataSource>(
        UsdImagingCollectionMaterialBindingSchemaTokens->collectionPath);
}

HdPathDataSourceHandle
UsdImagingCollectionMaterialBindingSchema::GetMaterialPath()
{
    return _GetTypedDataSource<HdPathDataSource>(
        UsdImagingCollectionMaterialBindingSchemaTokens->materialPath);
}

HdTokenDataSourceHandle
UsdImagingCollectionMaterialBindingSchema::GetBindingStrength()
{
    return _GetTypedDataSource<HdTokenDataSource>(
        UsdImagingCollectionMaterialBindingSchemaTokens->bindingStrength);
}

/*static*/
HdContainerDataSourceHandle
UsdImagingCollectionMaterialBindingSchema::BuildRetained(
        const HdPathDataSourceHandle &collectionPath,
        const HdPathDataSourceHandle &materialPath,
        const HdTokenDataSourceHandle &bindingStrength
)
{
    TfToken _names[3];
    HdDataSourceBaseHandle _values[3];

    size_t _count = 0;

    if (collectionPath) {
        _names[_count] = UsdImagingCollectionMaterialBindingSchemaTokens->collectionPath;
        _values[_count++] = collectionPath;
    }

    if (materialPath) {
        _names[_count] = UsdImagingCollectionMaterialBindingSchemaTokens->materialPath;
        _values[_count++] = materialPath;
    }

    if (bindingStrength) {
        _names[_count] = UsdImagingCollectionMaterialBindingSchemaTokens->bindingStrength;
        _values[_count++] = bindingStrength;
    }
    return HdRetainedContainerDataSource::New(_count, _names, _values);
}

UsdImagingCollectionMaterialBindingSchema::Builder &
UsdImagingCollectionMaterialBindingSchema::Builder::SetCollectionPath(
    const HdPathDataSourceHandle &collectionPath)
{
    _collectionPath = collectionPath;
    return *this;
}

UsdImagingCollectionMaterialBindingSchema::Builder &
UsdImagingCollectionMaterialBindingSchema::Builder::SetMaterialPath(
    const HdPathDataSourceHandle &materialPath)
{
    _materialPath = materialPath;
    return *this;
}

UsdImagingCollectionMaterialBindingSchema::Builder &
UsdImagingCollectionMaterialBindingSchema::Builder::SetBindingStrength(
    const HdTokenDataSourceHandle &bindingStrength)
{
    _bindingStrength = bindingStrength;
    return *this;
}

HdContainerDataSourceHandle
UsdImagingCollectionMaterialBindingSchema::Builder::Build()
{
    return UsdImagingCollectionMaterialBindingSchema::BuildRetained(
        _collectionPath,
        _materialPath,
        _bindingStrength
    );
}

/*static*/
UsdImagingCollectionMaterialBindingSchema
UsdImagingCollectionMaterialBindingSchema::GetFromParent(
        const HdContainerDataSourceHandle &fromParentContainer)
{
    return UsdImagingCollectionMaterialBindingSchema(
        fromParentContainer
        ? HdContainerDataSource::Cast(fromParentContainer->Get(
                UsdImagingCollectionMaterialBindingSchemaTokens->collectionMaterialBinding))
        : nullptr);
}

/*static*/
const TfToken &
UsdImagingCollectionMaterialBindingSchema::GetSchemaToken()
{
    return UsdImagingCollectionMaterialBindingSchemaTokens->collectionMaterialBinding;
}

/*static*/
const HdDataSourceLocator &
UsdImagingCollectionMaterialBindingSchema::GetDefaultLocator()
{
    static const HdDataSourceLocator locator(GetSchemaToken());
    return locator;
} 

PXR_NAMESPACE_CLOSE_SCOPE