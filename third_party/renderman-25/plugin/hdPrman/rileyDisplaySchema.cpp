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

#include "hdPrman/rileyDisplaySchema.h"

#include "pxr/imaging/hd/retainedDataSource.h"

#include "pxr/base/trace/trace.h"

// --(BEGIN CUSTOM CODE: Includes)--
// --(END CUSTOM CODE: Includes)--

PXR_NAMESPACE_OPEN_SCOPE

TF_DEFINE_PUBLIC_TOKENS(HdPrmanRileyDisplaySchemaTokens,
    HD_PRMAN_RILEY_DISPLAY_SCHEMA_TOKENS);

// --(BEGIN CUSTOM CODE: Schema Methods)--
// --(END CUSTOM CODE: Schema Methods)--

HdPathDataSourceHandle
HdPrmanRileyDisplaySchema::GetRenderTarget()
{
    return _GetTypedDataSource<HdPathDataSource>(
        HdPrmanRileyDisplaySchemaTokens->renderTarget);
}

HdTokenDataSourceHandle
HdPrmanRileyDisplaySchema::GetName()
{
    return _GetTypedDataSource<HdTokenDataSource>(
        HdPrmanRileyDisplaySchemaTokens->name);
}

HdTokenDataSourceHandle
HdPrmanRileyDisplaySchema::GetDriver()
{
    return _GetTypedDataSource<HdTokenDataSource>(
        HdPrmanRileyDisplaySchemaTokens->driver);
}

HdPathArrayDataSourceHandle
HdPrmanRileyDisplaySchema::GetRenderOutputs()
{
    return _GetTypedDataSource<HdPathArrayDataSource>(
        HdPrmanRileyDisplaySchemaTokens->renderOutputs);
}

HdPrmanRileyParamListSchema
HdPrmanRileyDisplaySchema::GetDriverParams()
{
    return HdPrmanRileyParamListSchema(_GetTypedDataSource<HdContainerDataSource>(
        HdPrmanRileyDisplaySchemaTokens->driverParams));
}

/*static*/
HdContainerDataSourceHandle
HdPrmanRileyDisplaySchema::BuildRetained(
        const HdPathDataSourceHandle &renderTarget,
        const HdTokenDataSourceHandle &name,
        const HdTokenDataSourceHandle &driver,
        const HdPathArrayDataSourceHandle &renderOutputs,
        const HdContainerDataSourceHandle &driverParams
)
{
    TfToken _names[5];
    HdDataSourceBaseHandle _values[5];

    size_t _count = 0;

    if (renderTarget) {
        _names[_count] = HdPrmanRileyDisplaySchemaTokens->renderTarget;
        _values[_count++] = renderTarget;
    }

    if (name) {
        _names[_count] = HdPrmanRileyDisplaySchemaTokens->name;
        _values[_count++] = name;
    }

    if (driver) {
        _names[_count] = HdPrmanRileyDisplaySchemaTokens->driver;
        _values[_count++] = driver;
    }

    if (renderOutputs) {
        _names[_count] = HdPrmanRileyDisplaySchemaTokens->renderOutputs;
        _values[_count++] = renderOutputs;
    }

    if (driverParams) {
        _names[_count] = HdPrmanRileyDisplaySchemaTokens->driverParams;
        _values[_count++] = driverParams;
    }
    return HdRetainedContainerDataSource::New(_count, _names, _values);
}

HdPrmanRileyDisplaySchema::Builder &
HdPrmanRileyDisplaySchema::Builder::SetRenderTarget(
    const HdPathDataSourceHandle &renderTarget)
{
    _renderTarget = renderTarget;
    return *this;
}

HdPrmanRileyDisplaySchema::Builder &
HdPrmanRileyDisplaySchema::Builder::SetName(
    const HdTokenDataSourceHandle &name)
{
    _name = name;
    return *this;
}

HdPrmanRileyDisplaySchema::Builder &
HdPrmanRileyDisplaySchema::Builder::SetDriver(
    const HdTokenDataSourceHandle &driver)
{
    _driver = driver;
    return *this;
}

HdPrmanRileyDisplaySchema::Builder &
HdPrmanRileyDisplaySchema::Builder::SetRenderOutputs(
    const HdPathArrayDataSourceHandle &renderOutputs)
{
    _renderOutputs = renderOutputs;
    return *this;
}

HdPrmanRileyDisplaySchema::Builder &
HdPrmanRileyDisplaySchema::Builder::SetDriverParams(
    const HdContainerDataSourceHandle &driverParams)
{
    _driverParams = driverParams;
    return *this;
}

HdContainerDataSourceHandle
HdPrmanRileyDisplaySchema::Builder::Build()
{
    return HdPrmanRileyDisplaySchema::BuildRetained(
        _renderTarget,
        _name,
        _driver,
        _renderOutputs,
        _driverParams
    );
}

/*static*/
HdPrmanRileyDisplaySchema
HdPrmanRileyDisplaySchema::GetFromParent(
        const HdContainerDataSourceHandle &fromParentContainer)
{
    return HdPrmanRileyDisplaySchema(
        fromParentContainer
        ? HdContainerDataSource::Cast(fromParentContainer->Get(
                HdPrmanRileyDisplaySchemaTokens->rileyDisplay))
        : nullptr);
}

/*static*/
const TfToken &
HdPrmanRileyDisplaySchema::GetSchemaToken()
{
    return HdPrmanRileyDisplaySchemaTokens->rileyDisplay;
}

/*static*/
const HdDataSourceLocator &
HdPrmanRileyDisplaySchema::GetDefaultLocator()
{
    static const HdDataSourceLocator locator(GetSchemaToken());
    return locator;
}

/* static */
const HdDataSourceLocator &
HdPrmanRileyDisplaySchema::GetRenderTargetLocator()
{
    static const HdDataSourceLocator locator =
        GetDefaultLocator().Append(
            HdPrmanRileyDisplaySchemaTokens->renderTarget);
    return locator;
}

/* static */
const HdDataSourceLocator &
HdPrmanRileyDisplaySchema::GetNameLocator()
{
    static const HdDataSourceLocator locator =
        GetDefaultLocator().Append(
            HdPrmanRileyDisplaySchemaTokens->name);
    return locator;
}

/* static */
const HdDataSourceLocator &
HdPrmanRileyDisplaySchema::GetDriverLocator()
{
    static const HdDataSourceLocator locator =
        GetDefaultLocator().Append(
            HdPrmanRileyDisplaySchemaTokens->driver);
    return locator;
}

/* static */
const HdDataSourceLocator &
HdPrmanRileyDisplaySchema::GetRenderOutputsLocator()
{
    static const HdDataSourceLocator locator =
        GetDefaultLocator().Append(
            HdPrmanRileyDisplaySchemaTokens->renderOutputs);
    return locator;
}

/* static */
const HdDataSourceLocator &
HdPrmanRileyDisplaySchema::GetDriverParamsLocator()
{
    static const HdDataSourceLocator locator =
        GetDefaultLocator().Append(
            HdPrmanRileyDisplaySchemaTokens->driverParams);
    return locator;
} 

PXR_NAMESPACE_CLOSE_SCOPE