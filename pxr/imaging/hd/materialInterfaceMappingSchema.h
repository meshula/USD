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

#ifndef PXR_IMAGING_HD_MATERIAL_INTERFACE_MAPPING_SCHEMA_H
#define PXR_IMAGING_HD_MATERIAL_INTERFACE_MAPPING_SCHEMA_H

/// \file

#include "pxr/imaging/hd/api.h"

#include "pxr/imaging/hd/schema.h"

// --(BEGIN CUSTOM CODE: Includes)--
// --(END CUSTOM CODE: Includes)--

PXR_NAMESPACE_OPEN_SCOPE

// --(BEGIN CUSTOM CODE: Declares)--
// --(END CUSTOM CODE: Declares)--

#define HD_MATERIAL_INTERFACE_MAPPING_SCHEMA_TOKENS \
    (nodePath) \
    (inputName) \

TF_DECLARE_PUBLIC_TOKENS(HdMaterialInterfaceMappingSchemaTokens, HD_API,
    HD_MATERIAL_INTERFACE_MAPPING_SCHEMA_TOKENS);

//-----------------------------------------------------------------------------


class HdMaterialInterfaceMappingSchema : public HdSchema
{
public:
    /// \name Schema retrieval
    /// @{

    HdMaterialInterfaceMappingSchema(HdContainerDataSourceHandle container)
      : HdSchema(container) {}

    /// @}

// --(BEGIN CUSTOM CODE: Schema Methods)--
    HdDataSourceLocator BuildNetworkRelativeLocator();
// --(END CUSTOM CODE: Schema Methods)--

    /// \name Member accessor
    /// @{

    HD_API
    HdTokenDataSourceHandle GetNodePath() const;

    HD_API
    HdTokenDataSourceHandle GetInputName() const; 

    /// @} 

    /// \name Schema construction
    /// @{

    /// \deprecated Use Builder instead.
    ///
    /// Builds a container data source which includes the provided child data
    /// sources. Parameters with nullptr values are excluded. This is a
    /// low-level interface. For cases in which it's desired to define
    /// the container with a sparse set of child fields, the Builder class
    /// is often more convenient and readable.
    HD_API
    static HdContainerDataSourceHandle
    BuildRetained(
        const HdTokenDataSourceHandle &nodePath,
        const HdTokenDataSourceHandle &inputName
    );

    /// \class HdMaterialInterfaceMappingSchema::Builder
    /// 
    /// Utility class for setting sparse sets of child data source fields to be
    /// filled as arguments into BuildRetained. Because all setter methods
    /// return a reference to the instance, this can be used in the "builder
    /// pattern" form.
    class Builder
    {
    public:
        HD_API
        Builder &SetNodePath(
            const HdTokenDataSourceHandle &nodePath);
        HD_API
        Builder &SetInputName(
            const HdTokenDataSourceHandle &inputName);

        /// Returns a container data source containing the members set thus far.
        HD_API
        HdContainerDataSourceHandle Build();

    private:
        HdTokenDataSourceHandle _nodePath;
        HdTokenDataSourceHandle _inputName;

    };

    /// @}
};

PXR_NAMESPACE_CLOSE_SCOPE

#endif