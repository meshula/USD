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

#include "pxr/imaging/hd/instanceSchema.h"

#include "pxr/imaging/hd/retainedDataSource.h"

#include "pxr/base/trace/trace.h"

// --(BEGIN CUSTOM CODE: Includes)--
// --(END CUSTOM CODE: Includes)--

PXR_NAMESPACE_OPEN_SCOPE

TF_DEFINE_PUBLIC_TOKENS(HdInstanceSchemaTokens,
    HD_INSTANCE_SCHEMA_TOKENS);

// --(BEGIN CUSTOM CODE: Schema Methods)--
// --(END CUSTOM CODE: Schema Methods)--

HdPathDataSourceHandle
HdInstanceSchema::GetInstancer() const
{
    return _GetTypedDataSource<HdPathDataSource>(
        HdInstanceSchemaTokens->instancer);
}

HdIntDataSourceHandle
HdInstanceSchema::GetPrototypeIndex() const
{
    return _GetTypedDataSource<HdIntDataSource>(
        HdInstanceSchemaTokens->prototypeIndex);
}

HdIntDataSourceHandle
HdInstanceSchema::GetInstanceIndex() const
{
    return _GetTypedDataSource<HdIntDataSource>(
        HdInstanceSchemaTokens->instanceIndex);
}

/*static*/
HdContainerDataSourceHandle
HdInstanceSchema::BuildRetained(
        const HdPathDataSourceHandle &instancer,
        const HdIntDataSourceHandle &prototypeIndex,
        const HdIntDataSourceHandle &instanceIndex
)
{
    TfToken _names[3];
    HdDataSourceBaseHandle _values[3];

    size_t _count = 0;

    if (instancer) {
        _names[_count] = HdInstanceSchemaTokens->instancer;
        _values[_count++] = instancer;
    }

    if (prototypeIndex) {
        _names[_count] = HdInstanceSchemaTokens->prototypeIndex;
        _values[_count++] = prototypeIndex;
    }

    if (instanceIndex) {
        _names[_count] = HdInstanceSchemaTokens->instanceIndex;
        _values[_count++] = instanceIndex;
    }
    return HdRetainedContainerDataSource::New(_count, _names, _values);
}

HdInstanceSchema::Builder &
HdInstanceSchema::Builder::SetInstancer(
    const HdPathDataSourceHandle &instancer)
{
    _instancer = instancer;
    return *this;
}

HdInstanceSchema::Builder &
HdInstanceSchema::Builder::SetPrototypeIndex(
    const HdIntDataSourceHandle &prototypeIndex)
{
    _prototypeIndex = prototypeIndex;
    return *this;
}

HdInstanceSchema::Builder &
HdInstanceSchema::Builder::SetInstanceIndex(
    const HdIntDataSourceHandle &instanceIndex)
{
    _instanceIndex = instanceIndex;
    return *this;
}

HdContainerDataSourceHandle
HdInstanceSchema::Builder::Build()
{
    return HdInstanceSchema::BuildRetained(
        _instancer,
        _prototypeIndex,
        _instanceIndex
    );
}

/*static*/
HdInstanceSchema
HdInstanceSchema::GetFromParent(
        const HdContainerDataSourceHandle &fromParentContainer)
{
    return HdInstanceSchema(
        fromParentContainer
        ? HdContainerDataSource::Cast(fromParentContainer->Get(
                HdInstanceSchemaTokens->instance))
        : nullptr);
}

/*static*/
const TfToken &
HdInstanceSchema::GetSchemaToken()
{
    return HdInstanceSchemaTokens->instance;
}

/*static*/
const HdDataSourceLocator &
HdInstanceSchema::GetDefaultLocator()
{
    static const HdDataSourceLocator locator(GetSchemaToken());
    return locator;
} 

PXR_NAMESPACE_CLOSE_SCOPE