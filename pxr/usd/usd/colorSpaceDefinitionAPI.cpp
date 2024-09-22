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

PXR_NAMESPACE_OPEN_SCOPE

void UsdColorSpaceDefinitionAPI::CreateColorSpaceAttrsWithChroma(
        const GfVec2f& redChroma,
        const GfVec2f& greenChroma,
        const GfVec2f& blueChroma,
        const GfVec2f& whitePoint,
        float gamma, float linearBias)
{
    CreateColorSpaceRedChromaAttr(VtValue(redChroma));
    CreateColorSpaceGreenChromaAttr(VtValue(greenChroma));
    CreateColorSpaceBlueChromaAttr(VtValue(blueChroma));
    CreateColorSpaceWhitePointAttr(VtValue(whitePoint));
    CreateColorSpaceGammaAttr(VtValue(gamma));
    CreateColorSpaceLinearBiasAttr(VtValue(linearBias));
}

void UsdColorSpaceDefinitionAPI::CreateColorSpaceAttrsWithMatrix(
        const GfMatrix3f& rgbToXYZ,
        float gamma, float linearBias)
{
    CreateColorSpaceGammaAttr(VtValue(gamma));
    CreateColorSpaceLinearBiasAttr(VtValue(linearBias));

    GfColorSpace colorSpace(TfToken("temp"), rgbToXYZ, gamma, linearBias);

    std::tuple<GfVec2f, GfVec2f, GfVec2f, GfVec2f> primariesAndWhitePoint =
        colorSpace.GetPrimariesAndWhitePoint();
    GfVec2f redChroma   = std::get<0>(primariesAndWhitePoint);
    GfVec2f greenChroma = std::get<1>(primariesAndWhitePoint);
    GfVec2f blueChroma  = std::get<2>(primariesAndWhitePoint);
    GfVec2f whitePoint  = std::get<3>(primariesAndWhitePoint);

    CreateColorSpaceRedChromaAttr(VtValue(redChroma));
    CreateColorSpaceGreenChromaAttr(VtValue(greenChroma));
    CreateColorSpaceBlueChromaAttr(VtValue(blueChroma));
    CreateColorSpaceWhitePointAttr(VtValue(whitePoint));
}


GfColorSpace UsdColorSpaceDefinitionAPI::ComputeColorSpaceFromDefinitionAttributes() const
{
    do {
        auto r = GetColorSpaceRedChromaAttr();
        if (!r)
            break;
        auto g = GetColorSpaceGreenChromaAttr();
        if (!g)
            break;
        auto b = GetColorSpaceBlueChromaAttr();
        if (!b)
            break;
        auto wp = GetColorSpaceWhitePointAttr();
        if (!wp)
            break;
        auto gm = GetColorSpaceGammaAttr();
        if (!gm)
            break;
        auto lb = GetColorSpaceLinearBiasAttr();
        if (!lb)
            break;
        auto n = GetColorSpaceDefinitionNameAttr();
        if (!n)
            break;

        GfVec2f redChroma;
        GfVec2f greenChroma;
        GfVec2f blueChroma;
        GfVec2f whitePoint;
        float gamma;
        float linearBias;
        TfToken name;
        
        r.Get(&redChroma);
        g.Get(&greenChroma);
        b.Get(&blueChroma);
        wp.Get(&whitePoint);
        gm.Get(&gamma);
        lb.Get(&linearBias);
        n.Get(&name);
        
        return GfColorSpace(name,
                            redChroma,
                            greenChroma,
                            blueChroma,
                            whitePoint,
                            gamma,
                            linearBias);
    } while(true);

    return GfColorSpace(GfColorSpaceNames->Raw);
}


PXR_NAMESPACE_CLOSE_SCOPE
