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

#include "pxr/imaging/hd/primvarsSchema.h"

#include "pxr/imaging/hd/retainedDataSource.h"

#include "pxr/base/trace/trace.h"

// --(BEGIN CUSTOM CODE: Includes)--
// --(END CUSTOM CODE: Includes)--

PXR_NAMESPACE_OPEN_SCOPE

TF_DEFINE_PUBLIC_TOKENS(HdPrimvarsSchemaTokens,
    HD_PRIMVARS_SCHEMA_TOKENS);

// --(BEGIN CUSTOM CODE: Schema Methods)--
// --(END CUSTOM CODE: Schema Methods)--

TfTokenVector
HdPrimvarsSchema::GetPrimvarNames()
{
    if (_container) {
        return _container->GetNames();
    } else {
        return {};
    }
}

HdPrimvarSchema
HdPrimvarsSchema::GetPrimvar(const TfToken &name)
{
    return HdPrimvarSchema(
        _GetTypedDataSource<HdContainerDataSource>(name));
}

/*static*/
HdContainerDataSourceHandle
HdPrimvarsSchema::BuildRetained(
    const size_t count,
    const TfToken * const names,
    const HdDataSourceBaseHandle * const values)
{
    return HdRetainedContainerDataSource::New(count, names, values);
}

/*static*/
HdPrimvarsSchema
HdPrimvarsSchema::GetFromParent(
        const HdContainerDataSourceHandle &fromParentContainer)
{
    return HdPrimvarsSchema(
        fromParentContainer
        ? HdContainerDataSource::Cast(fromParentContainer->Get(
                HdPrimvarsSchemaTokens->primvars))
        : nullptr);
}

/*static*/
const TfToken &
HdPrimvarsSchema::GetSchemaToken()
{
    return HdPrimvarsSchemaTokens->primvars;
}

/*static*/
const HdDataSourceLocator &
HdPrimvarsSchema::GetDefaultLocator()
{
    static const HdDataSourceLocator locator(GetSchemaToken());
    return locator;
}

/* static */
const HdDataSourceLocator &
HdPrimvarsSchema::GetPointsLocator()
{
    static const HdDataSourceLocator locator =
        GetDefaultLocator().Append(
            HdPrimvarsSchemaTokens->points);
    return locator;
}

/* static */
const HdDataSourceLocator &
HdPrimvarsSchema::GetNormalsLocator()
{
    static const HdDataSourceLocator locator =
        GetDefaultLocator().Append(
            HdPrimvarsSchemaTokens->normals);
    return locator;
}

/* static */
const HdDataSourceLocator &
HdPrimvarsSchema::GetWidthsLocator()
{
    static const HdDataSourceLocator locator =
        GetDefaultLocator().Append(
            HdPrimvarsSchemaTokens->widths);
    return locator;
} 

PXR_NAMESPACE_CLOSE_SCOPE