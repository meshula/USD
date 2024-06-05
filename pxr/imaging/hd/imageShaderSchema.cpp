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

#include "pxr/imaging/hd/imageShaderSchema.h"

#include "pxr/imaging/hd/retainedDataSource.h"

#include "pxr/base/trace/trace.h"

// --(BEGIN CUSTOM CODE: Includes)--
// --(END CUSTOM CODE: Includes)--

PXR_NAMESPACE_OPEN_SCOPE

TF_DEFINE_PUBLIC_TOKENS(HdImageShaderSchemaTokens,
    HD_IMAGE_SHADER_SCHEMA_TOKENS);

// --(BEGIN CUSTOM CODE: Schema Methods)--
// --(END CUSTOM CODE: Schema Methods)--

HdBoolDataSourceHandle
HdImageShaderSchema::GetEnabled() const
{
    return _GetTypedDataSource<HdBoolDataSource>(
        HdImageShaderSchemaTokens->enabled);
}

HdIntDataSourceHandle
HdImageShaderSchema::GetPriority() const
{
    return _GetTypedDataSource<HdIntDataSource>(
        HdImageShaderSchemaTokens->priority);
}

HdStringDataSourceHandle
HdImageShaderSchema::GetFilePath() const
{
    return _GetTypedDataSource<HdStringDataSource>(
        HdImageShaderSchemaTokens->filePath);
}

HdSampledDataSourceContainerSchema
HdImageShaderSchema::GetConstants() const
{
    return HdSampledDataSourceContainerSchema(_GetTypedDataSource<HdContainerDataSource>(
        HdImageShaderSchemaTokens->constants));
}

HdMaterialNetworkSchema
HdImageShaderSchema::GetMaterialNetwork() const
{
    return HdMaterialNetworkSchema(_GetTypedDataSource<HdContainerDataSource>(
        HdImageShaderSchemaTokens->materialNetwork));
}

/*static*/
HdContainerDataSourceHandle
HdImageShaderSchema::BuildRetained(
        const HdBoolDataSourceHandle &enabled,
        const HdIntDataSourceHandle &priority,
        const HdStringDataSourceHandle &filePath,
        const HdContainerDataSourceHandle &constants,
        const HdContainerDataSourceHandle &materialNetwork
)
{
    TfToken _names[5];
    HdDataSourceBaseHandle _values[5];

    size_t _count = 0;

    if (enabled) {
        _names[_count] = HdImageShaderSchemaTokens->enabled;
        _values[_count++] = enabled;
    }

    if (priority) {
        _names[_count] = HdImageShaderSchemaTokens->priority;
        _values[_count++] = priority;
    }

    if (filePath) {
        _names[_count] = HdImageShaderSchemaTokens->filePath;
        _values[_count++] = filePath;
    }

    if (constants) {
        _names[_count] = HdImageShaderSchemaTokens->constants;
        _values[_count++] = constants;
    }

    if (materialNetwork) {
        _names[_count] = HdImageShaderSchemaTokens->materialNetwork;
        _values[_count++] = materialNetwork;
    }
    return HdRetainedContainerDataSource::New(_count, _names, _values);
}

HdImageShaderSchema::Builder &
HdImageShaderSchema::Builder::SetEnabled(
    const HdBoolDataSourceHandle &enabled)
{
    _enabled = enabled;
    return *this;
}

HdImageShaderSchema::Builder &
HdImageShaderSchema::Builder::SetPriority(
    const HdIntDataSourceHandle &priority)
{
    _priority = priority;
    return *this;
}

HdImageShaderSchema::Builder &
HdImageShaderSchema::Builder::SetFilePath(
    const HdStringDataSourceHandle &filePath)
{
    _filePath = filePath;
    return *this;
}

HdImageShaderSchema::Builder &
HdImageShaderSchema::Builder::SetConstants(
    const HdContainerDataSourceHandle &constants)
{
    _constants = constants;
    return *this;
}

HdImageShaderSchema::Builder &
HdImageShaderSchema::Builder::SetMaterialNetwork(
    const HdContainerDataSourceHandle &materialNetwork)
{
    _materialNetwork = materialNetwork;
    return *this;
}

HdContainerDataSourceHandle
HdImageShaderSchema::Builder::Build()
{
    return HdImageShaderSchema::BuildRetained(
        _enabled,
        _priority,
        _filePath,
        _constants,
        _materialNetwork
    );
}

/*static*/
HdImageShaderSchema
HdImageShaderSchema::GetFromParent(
        const HdContainerDataSourceHandle &fromParentContainer)
{
    return HdImageShaderSchema(
        fromParentContainer
        ? HdContainerDataSource::Cast(fromParentContainer->Get(
                HdImageShaderSchemaTokens->imageShader))
        : nullptr);
}

/*static*/
const TfToken &
HdImageShaderSchema::GetSchemaToken()
{
    return HdImageShaderSchemaTokens->imageShader;
}

/*static*/
const HdDataSourceLocator &
HdImageShaderSchema::GetDefaultLocator()
{
    static const HdDataSourceLocator locator(GetSchemaToken());
    return locator;
}

/* static */
const HdDataSourceLocator &
HdImageShaderSchema::GetEnabledLocator()
{
    static const HdDataSourceLocator locator =
        GetDefaultLocator().Append(
            HdImageShaderSchemaTokens->enabled);
    return locator;
}

/* static */
const HdDataSourceLocator &
HdImageShaderSchema::GetPriorityLocator()
{
    static const HdDataSourceLocator locator =
        GetDefaultLocator().Append(
            HdImageShaderSchemaTokens->priority);
    return locator;
}

/* static */
const HdDataSourceLocator &
HdImageShaderSchema::GetFilePathLocator()
{
    static const HdDataSourceLocator locator =
        GetDefaultLocator().Append(
            HdImageShaderSchemaTokens->filePath);
    return locator;
}

/* static */
const HdDataSourceLocator &
HdImageShaderSchema::GetConstantsLocator()
{
    static const HdDataSourceLocator locator =
        GetDefaultLocator().Append(
            HdImageShaderSchemaTokens->constants);
    return locator;
}

/* static */
const HdDataSourceLocator &
HdImageShaderSchema::GetMaterialNetworkLocator()
{
    static const HdDataSourceLocator locator =
        GetDefaultLocator().Append(
            HdImageShaderSchemaTokens->materialNetwork);
    return locator;
} 

PXR_NAMESPACE_CLOSE_SCOPE