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

#include "pxr/imaging/hd/selectionSchema.h"

#include "pxr/imaging/hd/retainedDataSource.h"

#include "pxr/base/trace/trace.h"

// --(BEGIN CUSTOM CODE: Includes)--
// --(END CUSTOM CODE: Includes)--

PXR_NAMESPACE_OPEN_SCOPE

TF_DEFINE_PUBLIC_TOKENS(HdSelectionSchemaTokens,
    HD_SELECTION_SCHEMA_TOKENS);

// --(BEGIN CUSTOM CODE: Schema Methods)--
// --(END CUSTOM CODE: Schema Methods)--

HdBoolDataSourceHandle
HdSelectionSchema::GetFullySelected()
{
    return _GetTypedDataSource<HdBoolDataSource>(
        HdSelectionSchemaTokens->fullySelected);
}

HdInstanceIndicesVectorSchema
HdSelectionSchema::GetNestedInstanceIndices()
{
    return HdInstanceIndicesVectorSchema(_GetTypedDataSource<HdVectorDataSource>(
        HdSelectionSchemaTokens->nestedInstanceIndices));
}

/*static*/
HdContainerDataSourceHandle
HdSelectionSchema::BuildRetained(
        const HdBoolDataSourceHandle &fullySelected,
        const HdVectorDataSourceHandle &nestedInstanceIndices
)
{
    TfToken _names[2];
    HdDataSourceBaseHandle _values[2];

    size_t _count = 0;

    if (fullySelected) {
        _names[_count] = HdSelectionSchemaTokens->fullySelected;
        _values[_count++] = fullySelected;
    }

    if (nestedInstanceIndices) {
        _names[_count] = HdSelectionSchemaTokens->nestedInstanceIndices;
        _values[_count++] = nestedInstanceIndices;
    }
    return HdRetainedContainerDataSource::New(_count, _names, _values);
}

HdSelectionSchema::Builder &
HdSelectionSchema::Builder::SetFullySelected(
    const HdBoolDataSourceHandle &fullySelected)
{
    _fullySelected = fullySelected;
    return *this;
}

HdSelectionSchema::Builder &
HdSelectionSchema::Builder::SetNestedInstanceIndices(
    const HdVectorDataSourceHandle &nestedInstanceIndices)
{
    _nestedInstanceIndices = nestedInstanceIndices;
    return *this;
}

HdContainerDataSourceHandle
HdSelectionSchema::Builder::Build()
{
    return HdSelectionSchema::BuildRetained(
        _fullySelected,
        _nestedInstanceIndices
    );
} 

PXR_NAMESPACE_CLOSE_SCOPE