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

#include "pxr/imaging/hd/materialNetworkSchema.h"

#include "pxr/imaging/hd/retainedDataSource.h"

#include "pxr/base/trace/trace.h"

// --(BEGIN CUSTOM CODE: Includes)--
// --(END CUSTOM CODE: Includes)--

PXR_NAMESPACE_OPEN_SCOPE

TF_DEFINE_PUBLIC_TOKENS(HdMaterialNetworkSchemaTokens,
    HD_MATERIAL_NETWORK_SCHEMA_TOKENS);

// --(BEGIN CUSTOM CODE: Schema Methods)--
// --(END CUSTOM CODE: Schema Methods)--

HdMaterialNodeContainerSchema
HdMaterialNetworkSchema::GetNodes()
{
    return HdMaterialNodeContainerSchema(_GetTypedDataSource<HdContainerDataSource>(
        HdMaterialNetworkSchemaTokens->nodes));
}

HdMaterialConnectionContainerSchema
HdMaterialNetworkSchema::GetTerminals()
{
    return HdMaterialConnectionContainerSchema(_GetTypedDataSource<HdContainerDataSource>(
        HdMaterialNetworkSchemaTokens->terminals));
}

HdMaterialInterfaceMappingsContainerSchema
HdMaterialNetworkSchema::GetInterfaceMappings()
{
    return HdMaterialInterfaceMappingsContainerSchema(_GetTypedDataSource<HdContainerDataSource>(
        HdMaterialNetworkSchemaTokens->interfaceMappings));
}

/*static*/
HdContainerDataSourceHandle
HdMaterialNetworkSchema::BuildRetained(
        const HdContainerDataSourceHandle &nodes,
        const HdContainerDataSourceHandle &terminals,
        const HdContainerDataSourceHandle &interfaceMappings
)
{
    TfToken _names[3];
    HdDataSourceBaseHandle _values[3];

    size_t _count = 0;

    if (nodes) {
        _names[_count] = HdMaterialNetworkSchemaTokens->nodes;
        _values[_count++] = nodes;
    }

    if (terminals) {
        _names[_count] = HdMaterialNetworkSchemaTokens->terminals;
        _values[_count++] = terminals;
    }

    if (interfaceMappings) {
        _names[_count] = HdMaterialNetworkSchemaTokens->interfaceMappings;
        _values[_count++] = interfaceMappings;
    }
    return HdRetainedContainerDataSource::New(_count, _names, _values);
}

HdMaterialNetworkSchema::Builder &
HdMaterialNetworkSchema::Builder::SetNodes(
    const HdContainerDataSourceHandle &nodes)
{
    _nodes = nodes;
    return *this;
}

HdMaterialNetworkSchema::Builder &
HdMaterialNetworkSchema::Builder::SetTerminals(
    const HdContainerDataSourceHandle &terminals)
{
    _terminals = terminals;
    return *this;
}

HdMaterialNetworkSchema::Builder &
HdMaterialNetworkSchema::Builder::SetInterfaceMappings(
    const HdContainerDataSourceHandle &interfaceMappings)
{
    _interfaceMappings = interfaceMappings;
    return *this;
}

HdContainerDataSourceHandle
HdMaterialNetworkSchema::Builder::Build()
{
    return HdMaterialNetworkSchema::BuildRetained(
        _nodes,
        _terminals,
        _interfaceMappings
    );
} 

PXR_NAMESPACE_CLOSE_SCOPE