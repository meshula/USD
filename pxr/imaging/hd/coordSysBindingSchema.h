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

#ifndef PXR_IMAGING_HD_COORD_SYS_BINDING_SCHEMA_H
#define PXR_IMAGING_HD_COORD_SYS_BINDING_SCHEMA_H

/// \file

#include "pxr/imaging/hd/api.h"

#include "pxr/imaging/hd/schema.h"

// --(BEGIN CUSTOM CODE: Includes)--
// --(END CUSTOM CODE: Includes)--

PXR_NAMESPACE_OPEN_SCOPE

// --(BEGIN CUSTOM CODE: Declares)--
// --(END CUSTOM CODE: Declares)--

#define HD_COORD_SYS_BINDING_SCHEMA_TOKENS \
    (coordSysBinding) \

TF_DECLARE_PUBLIC_TOKENS(HdCoordSysBindingSchemaTokens, HD_API,
    HD_COORD_SYS_BINDING_SCHEMA_TOKENS);

//-----------------------------------------------------------------------------


class HdCoordSysBindingSchema : public HdSchema
{
public:
    /// \name Schema retrieval
    /// @{

    HdCoordSysBindingSchema(HdContainerDataSourceHandle container)
      : HdSchema(container) {}

    /// Retrieves a container data source with the schema's default name token
    /// "coordSysBinding" from the parent container and constructs a
    /// HdCoordSysBindingSchema instance.
    /// Because the requested container data source may not exist, the result
    /// should be checked with IsDefined() or a bool comparison before use.
    HD_API
    static HdCoordSysBindingSchema GetFromParent(
        const HdContainerDataSourceHandle &fromParentContainer);

    /// @}

// --(BEGIN CUSTOM CODE: Schema Methods)--
// --(END CUSTOM CODE: Schema Methods)--

    /// \name Member accessor
    /// @{

    HD_API
    TfTokenVector GetCoordSysBindingNames() const;

    HD_API
    HdPathDataSourceHandle GetCoordSysBinding(const TfToken &name) const; 

    /// @}

    /// \name Schema location
    /// @{

    /// Returns a token where the container representing this schema is found in
    /// a container by default.
    HD_API
    static const TfToken &GetSchemaToken();

    /// Returns an HdDataSourceLocator (relative to the prim-level data source)
    /// where the container representing this schema is found by default.
    HD_API
    static const HdDataSourceLocator &GetDefaultLocator();

    /// @} 

    /// \name Schema construction
    /// @{
    HD_API
    static HdContainerDataSourceHandle
    BuildRetained(
        size_t count,
        const TfToken *names,
        const HdDataSourceBaseHandle *values);

    /// @}
};

PXR_NAMESPACE_CLOSE_SCOPE

#endif