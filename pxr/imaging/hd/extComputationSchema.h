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

#ifndef PXR_IMAGING_HD_EXT_COMPUTATION_SCHEMA_H
#define PXR_IMAGING_HD_EXT_COMPUTATION_SCHEMA_H

/// \file

#include "pxr/imaging/hd/api.h"

#include "pxr/imaging/hd/schema.h"

// --(BEGIN CUSTOM CODE: Includes)--
// --(END CUSTOM CODE: Includes)--

PXR_NAMESPACE_OPEN_SCOPE

// --(BEGIN CUSTOM CODE: Declares)--
// --(END CUSTOM CODE: Declares)--

#define HD_EXT_COMPUTATION_SCHEMA_TOKENS \
    (extComputation) \
    (inputValues) \
    (inputComputations) \
    (outputs) \
    (glslKernel) \
    (cpuCallback) \
    (dispatchCount) \
    (elementCount) \

TF_DECLARE_PUBLIC_TOKENS(HdExtComputationSchemaTokens, HD_API,
    HD_EXT_COMPUTATION_SCHEMA_TOKENS);

//-----------------------------------------------------------------------------


class HdExtComputationSchema : public HdSchema
{
public:
    /// \name Schema retrieval
    /// @{

    HdExtComputationSchema(HdContainerDataSourceHandle container)
      : HdSchema(container) {}

    /// Retrieves a container data source with the schema's default name token
    /// "extComputation" from the parent container and constructs a
    /// HdExtComputationSchema instance.
    /// Because the requested container data source may not exist, the result
    /// should be checked with IsDefined() or a bool comparison before use.
    HD_API
    static HdExtComputationSchema GetFromParent(
        const HdContainerDataSourceHandle &fromParentContainer);

    /// @}

// --(BEGIN CUSTOM CODE: Schema Methods)--
// --(END CUSTOM CODE: Schema Methods)--

    /// \name Member accessor
    /// @{

    HD_API
    HdContainerDataSourceHandle GetInputValues();

    HD_API
    HdVectorDataSourceHandle GetInputComputations();

    HD_API
    HdVectorDataSourceHandle GetOutputs();

    HD_API
    HdStringDataSourceHandle GetGlslKernel();

    HD_API
    HdDataSourceBaseHandle GetCpuCallback();

    HD_API
    HdSizetDataSourceHandle GetDispatchCount();

    HD_API
    HdSizetDataSourceHandle GetElementCount(); 

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

    /// \name Data source locators for members
    ///
    /// The following methods return an HdDataSourceLocator (relative to the
    /// prim-level data source) where the data source for a member can be found.
    ///
    /// This is often useful for checking intersection against the
    /// HdDataSourceLocatorSet sent with HdDataSourceObserver::PrimsDirtied.
    /// @{

    /// Prim-level relative data source locator to locate inputValues.
    HD_API
    static const HdDataSourceLocator &GetInputValuesLocator();

    /// Prim-level relative data source locator to locate inputComputations.
    HD_API
    static const HdDataSourceLocator &GetInputComputationsLocator();

    /// Prim-level relative data source locator to locate outputs.
    HD_API
    static const HdDataSourceLocator &GetOutputsLocator();

    /// Prim-level relative data source locator to locate glslKernel.
    HD_API
    static const HdDataSourceLocator &GetGlslKernelLocator();

    /// Prim-level relative data source locator to locate cpuCallback.
    HD_API
    static const HdDataSourceLocator &GetCpuCallbackLocator();

    /// Prim-level relative data source locator to locate dispatchCount.
    HD_API
    static const HdDataSourceLocator &GetDispatchCountLocator();

    /// Prim-level relative data source locator to locate elementCount.
    HD_API
    static const HdDataSourceLocator &GetElementCountLocator();
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
        const HdContainerDataSourceHandle &inputValues,
        const HdVectorDataSourceHandle &inputComputations,
        const HdVectorDataSourceHandle &outputs,
        const HdStringDataSourceHandle &glslKernel,
        const HdDataSourceBaseHandle &cpuCallback,
        const HdSizetDataSourceHandle &dispatchCount,
        const HdSizetDataSourceHandle &elementCount
    );

    /// \class HdExtComputationSchema::Builder
    /// 
    /// Utility class for setting sparse sets of child data source fields to be
    /// filled as arguments into BuildRetained. Because all setter methods
    /// return a reference to the instance, this can be used in the "builder
    /// pattern" form.
    class Builder
    {
    public:
        HD_API
        Builder &SetInputValues(
            const HdContainerDataSourceHandle &inputValues);
        HD_API
        Builder &SetInputComputations(
            const HdVectorDataSourceHandle &inputComputations);
        HD_API
        Builder &SetOutputs(
            const HdVectorDataSourceHandle &outputs);
        HD_API
        Builder &SetGlslKernel(
            const HdStringDataSourceHandle &glslKernel);
        HD_API
        Builder &SetCpuCallback(
            const HdDataSourceBaseHandle &cpuCallback);
        HD_API
        Builder &SetDispatchCount(
            const HdSizetDataSourceHandle &dispatchCount);
        HD_API
        Builder &SetElementCount(
            const HdSizetDataSourceHandle &elementCount);

        /// Returns a container data source containing the members set thus far.
        HD_API
        HdContainerDataSourceHandle Build();

    private:
        HdContainerDataSourceHandle _inputValues;
        HdVectorDataSourceHandle _inputComputations;
        HdVectorDataSourceHandle _outputs;
        HdStringDataSourceHandle _glslKernel;
        HdDataSourceBaseHandle _cpuCallback;
        HdSizetDataSourceHandle _dispatchCount;
        HdSizetDataSourceHandle _elementCount;

    };

    /// @}
};

PXR_NAMESPACE_CLOSE_SCOPE

#endif