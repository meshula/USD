//
// Copyright 2016 Pixar
//
// Licensed under the terms set forth in the LICENSE.txt file available at
// https://openusd.org/license.
//
#include "pxr/usd/usd/colorSpaceDefinitionAPI.h"
#include "pxr/usd/usd/schemaRegistry.h"
#include "pxr/usd/usd/typed.h"

#include "pxr/usd/sdf/types.h"
#include "pxr/usd/sdf/assetPath.h"

PXR_NAMESPACE_OPEN_SCOPE

// Register the schema with the TfType system.
TF_REGISTRY_FUNCTION(TfType)
{
    TfType::Define<UsdColorSpaceDefinitionAPI,
        TfType::Bases< UsdAPISchemaBase > >();
    
}

/* virtual */
UsdColorSpaceDefinitionAPI::~UsdColorSpaceDefinitionAPI()
{
}

/* static */
UsdColorSpaceDefinitionAPI
UsdColorSpaceDefinitionAPI::Get(const UsdStagePtr &stage, const SdfPath &path)
{
    if (!stage) {
        TF_CODING_ERROR("Invalid stage");
        return UsdColorSpaceDefinitionAPI();
    }
    return UsdColorSpaceDefinitionAPI(stage->GetPrimAtPath(path));
}


/* virtual */
UsdSchemaKind UsdColorSpaceDefinitionAPI::_GetSchemaKind() const
{
    return UsdColorSpaceDefinitionAPI::schemaKind;
}

/* static */
bool
UsdColorSpaceDefinitionAPI::CanApply(
    const UsdPrim &prim, std::string *whyNot)
{
    return prim.CanApplyAPI<UsdColorSpaceDefinitionAPI>(whyNot);
}

/* static */
UsdColorSpaceDefinitionAPI
UsdColorSpaceDefinitionAPI::Apply(const UsdPrim &prim)
{
    if (prim.ApplyAPI<UsdColorSpaceDefinitionAPI>()) {
        return UsdColorSpaceDefinitionAPI(prim);
    }
    return UsdColorSpaceDefinitionAPI();
}

/* static */
const TfType &
UsdColorSpaceDefinitionAPI::_GetStaticTfType()
{
    static TfType tfType = TfType::Find<UsdColorSpaceDefinitionAPI>();
    return tfType;
}

/* static */
bool 
UsdColorSpaceDefinitionAPI::_IsTypedSchema()
{
    static bool isTyped = _GetStaticTfType().IsA<UsdTyped>();
    return isTyped;
}

/* virtual */
const TfType &
UsdColorSpaceDefinitionAPI::_GetTfType() const
{
    return _GetStaticTfType();
}

UsdAttribute
UsdColorSpaceDefinitionAPI::GetColorSpaceDefinitionNameAttr() const
{
    return GetPrim().GetAttribute(UsdTokens->colorSpaceDefinitionName);
}

UsdAttribute
UsdColorSpaceDefinitionAPI::CreateColorSpaceDefinitionNameAttr(VtValue const &defaultValue, bool writeSparsely) const
{
    return UsdSchemaBase::_CreateAttr(UsdTokens->colorSpaceDefinitionName,
                       SdfValueTypeNames->Token,
                       /* custom = */ false,
                       SdfVariabilityUniform,
                       defaultValue,
                       writeSparsely);
}

UsdAttribute
UsdColorSpaceDefinitionAPI::GetColorSpaceRedChromaAttr() const
{
    return GetPrim().GetAttribute(UsdTokens->colorSpaceRedChroma);
}

UsdAttribute
UsdColorSpaceDefinitionAPI::CreateColorSpaceRedChromaAttr(VtValue const &defaultValue, bool writeSparsely) const
{
    return UsdSchemaBase::_CreateAttr(UsdTokens->colorSpaceRedChroma,
                       SdfValueTypeNames->Float2,
                       /* custom = */ false,
                       SdfVariabilityVarying,
                       defaultValue,
                       writeSparsely);
}

UsdAttribute
UsdColorSpaceDefinitionAPI::GetColorSpaceGreenChromaAttr() const
{
    return GetPrim().GetAttribute(UsdTokens->colorSpaceGreenChroma);
}

UsdAttribute
UsdColorSpaceDefinitionAPI::CreateColorSpaceGreenChromaAttr(VtValue const &defaultValue, bool writeSparsely) const
{
    return UsdSchemaBase::_CreateAttr(UsdTokens->colorSpaceGreenChroma,
                       SdfValueTypeNames->Float2,
                       /* custom = */ false,
                       SdfVariabilityVarying,
                       defaultValue,
                       writeSparsely);
}

UsdAttribute
UsdColorSpaceDefinitionAPI::GetColorSpaceBlueChromaAttr() const
{
    return GetPrim().GetAttribute(UsdTokens->colorSpaceBlueChroma);
}

UsdAttribute
UsdColorSpaceDefinitionAPI::CreateColorSpaceBlueChromaAttr(VtValue const &defaultValue, bool writeSparsely) const
{
    return UsdSchemaBase::_CreateAttr(UsdTokens->colorSpaceBlueChroma,
                       SdfValueTypeNames->Float2,
                       /* custom = */ false,
                       SdfVariabilityVarying,
                       defaultValue,
                       writeSparsely);
}

UsdAttribute
UsdColorSpaceDefinitionAPI::GetColorSpaceWhitePointAttr() const
{
    return GetPrim().GetAttribute(UsdTokens->colorSpaceWhitePoint);
}

UsdAttribute
UsdColorSpaceDefinitionAPI::CreateColorSpaceWhitePointAttr(VtValue const &defaultValue, bool writeSparsely) const
{
    return UsdSchemaBase::_CreateAttr(UsdTokens->colorSpaceWhitePoint,
                       SdfValueTypeNames->Float2,
                       /* custom = */ false,
                       SdfVariabilityVarying,
                       defaultValue,
                       writeSparsely);
}

UsdAttribute
UsdColorSpaceDefinitionAPI::GetColorSpaceGammaAttr() const
{
    return GetPrim().GetAttribute(UsdTokens->colorSpaceGamma);
}

UsdAttribute
UsdColorSpaceDefinitionAPI::CreateColorSpaceGammaAttr(VtValue const &defaultValue, bool writeSparsely) const
{
    return UsdSchemaBase::_CreateAttr(UsdTokens->colorSpaceGamma,
                       SdfValueTypeNames->Float,
                       /* custom = */ false,
                       SdfVariabilityVarying,
                       defaultValue,
                       writeSparsely);
}

UsdAttribute
UsdColorSpaceDefinitionAPI::GetColorSpaceLinearBiasAttr() const
{
    return GetPrim().GetAttribute(UsdTokens->colorSpaceLinearBias);
}

UsdAttribute
UsdColorSpaceDefinitionAPI::CreateColorSpaceLinearBiasAttr(VtValue const &defaultValue, bool writeSparsely) const
{
    return UsdSchemaBase::_CreateAttr(UsdTokens->colorSpaceLinearBias,
                       SdfValueTypeNames->Float,
                       /* custom = */ false,
                       SdfVariabilityVarying,
                       defaultValue,
                       writeSparsely);
}

namespace {
static inline TfTokenVector
_ConcatenateAttributeNames(const TfTokenVector& left,const TfTokenVector& right)
{
    TfTokenVector result;
    result.reserve(left.size() + right.size());
    result.insert(result.end(), left.begin(), left.end());
    result.insert(result.end(), right.begin(), right.end());
    return result;
}
}

/*static*/
const TfTokenVector&
UsdColorSpaceDefinitionAPI::GetSchemaAttributeNames(bool includeInherited)
{
    static TfTokenVector localNames = {
        UsdTokens->colorSpaceDefinitionName,
        UsdTokens->colorSpaceRedChroma,
        UsdTokens->colorSpaceGreenChroma,
        UsdTokens->colorSpaceBlueChroma,
        UsdTokens->colorSpaceWhitePoint,
        UsdTokens->colorSpaceGamma,
        UsdTokens->colorSpaceLinearBias,
    };
    static TfTokenVector allNames =
        _ConcatenateAttributeNames(
            UsdAPISchemaBase::GetSchemaAttributeNames(true),
            localNames);

    if (includeInherited)
        return allNames;
    else
        return localNames;
}

PXR_NAMESPACE_CLOSE_SCOPE

// ===================================================================== //
// Feel free to add custom code below this line. It will be preserved by
// the code generator.
//
// Just remember to wrap code in the appropriate delimiters:
// 'PXR_NAMESPACE_OPEN_SCOPE', 'PXR_NAMESPACE_CLOSE_SCOPE'.
// ===================================================================== //
// --(BEGIN CUSTOM CODE)--
