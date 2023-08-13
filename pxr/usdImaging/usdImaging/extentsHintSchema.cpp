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

#include "pxr/usdImaging/usdImaging/extentsHintSchema.h"

#include "pxr/imaging/hd/retainedDataSource.h"

PXR_NAMESPACE_OPEN_SCOPE

TF_DEFINE_PUBLIC_TOKENS(UsdImagingExtentsHintSchemaTokens,
    USDIMAGINGEXTENTSHINT_SCHEMA_TOKENS);


HdExtentSchema
UsdImagingExtentsHintSchema::GetExtent(const TfToken &purpose)
{
    return HdExtentSchema(_GetTypedDataSource<HdContainerDataSource>(purpose));
}

/*static*/
HdContainerDataSourceHandle
UsdImagingExtentsHintSchema::BuildRetained(
    const size_t count,
    const TfToken * const names,
    const HdDataSourceBaseHandle * const values)
{
    return HdRetainedContainerDataSource::New(count, names, values);
}

/*static*/
UsdImagingExtentsHintSchema
UsdImagingExtentsHintSchema::GetFromParent(
        const HdContainerDataSourceHandle &fromParentContainer)
{
    return UsdImagingExtentsHintSchema(
        fromParentContainer
        ? HdContainerDataSource::Cast(
            fromParentContainer->Get(
                GetSchemaToken()))
        : nullptr);
}

/*static*/
const TfToken &
UsdImagingExtentsHintSchema::GetSchemaToken()
{
    return UsdImagingExtentsHintSchemaTokens->extentsHint;
}

/*static*/
const HdDataSourceLocator &
UsdImagingExtentsHintSchema::GetDefaultLocator()
{
    static const HdDataSourceLocator locator(
        UsdImagingExtentsHintSchemaTokens->extentsHint
    );
    return locator;
} 

PXR_NAMESPACE_CLOSE_SCOPE
