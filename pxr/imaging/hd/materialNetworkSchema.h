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

#ifndef PXR_IMAGING_HD_MATERIAL_NETWORK_SCHEMA_H
#define PXR_IMAGING_HD_MATERIAL_NETWORK_SCHEMA_H

/// \file

#include "pxr/imaging/hd/api.h"
#include "pxr/imaging/hd/schemaTypeDefs.h"

#include "pxr/imaging/hd/schema.h"

// --(BEGIN CUSTOM CODE: Includes)--
// --(END CUSTOM CODE: Includes)--

PXR_NAMESPACE_OPEN_SCOPE

// --(BEGIN CUSTOM CODE: Declares)--
// --(END CUSTOM CODE: Declares)--

#define HD_MATERIAL_NETWORK_SCHEMA_TOKENS \
    (nodes) \
    (terminals) \
    (interfaceMappings) \

TF_DECLARE_PUBLIC_TOKENS(HdMaterialNetworkSchemaTokens, HD_API,
    HD_MATERIAL_NETWORK_SCHEMA_TOKENS);

//-----------------------------------------------------------------------------


class HdMaterialNetworkSchema : public HdSchema
{
public:
    /// \name Schema retrieval
    /// @{

    HdMaterialNetworkSchema(HdContainerDataSourceHandle container)
      : HdSchema(container) {}

    /// @}

// --(BEGIN CUSTOM CODE: Schema Methods)--
// --(END CUSTOM CODE: Schema Methods)--

    /// \name Member accessor
    /// @{

    HD_API
    HdMaterialNodeContainerSchema GetNodes() const;

    HD_API
    HdMaterialConnectionContainerSchema GetTerminals() const;

    HD_API
    HdMaterialInterfaceMappingsContainerSchema GetInterfaceMappings() const; 

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
        const HdContainerDataSourceHandle &nodes,
        const HdContainerDataSourceHandle &terminals,
        const HdContainerDataSourceHandle &interfaceMappings
    );

    /// \class HdMaterialNetworkSchema::Builder
    /// 
    /// Utility class for setting sparse sets of child data source fields to be
    /// filled as arguments into BuildRetained. Because all setter methods
    /// return a reference to the instance, this can be used in the "builder
    /// pattern" form.
    class Builder
    {
    public:
        HD_API
        Builder &SetNodes(
            const HdContainerDataSourceHandle &nodes);
        HD_API
        Builder &SetTerminals(
            const HdContainerDataSourceHandle &terminals);
        HD_API
        Builder &SetInterfaceMappings(
            const HdContainerDataSourceHandle &interfaceMappings);

        /// Returns a container data source containing the members set thus far.
        HD_API
        HdContainerDataSourceHandle Build();

    private:
        HdContainerDataSourceHandle _nodes;
        HdContainerDataSourceHandle _terminals;
        HdContainerDataSourceHandle _interfaceMappings;

    };

    /// @}
};

PXR_NAMESPACE_CLOSE_SCOPE

#endif