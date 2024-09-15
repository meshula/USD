//
// Copyright 2016 Pixar
//
// Licensed under the terms set forth in the LICENSE.txt file available at
// https://openusd.org/license.
//
#include "pxr/usd/usd/colorSpaceAPI.h"
#include "pxr/usd/usd/schemaRegistry.h"
#include "pxr/usd/usd/typed.h"

#include "pxr/usd/sdf/types.h"
#include "pxr/usd/sdf/assetPath.h"

PXR_NAMESPACE_OPEN_SCOPE

// Register the schema with the TfType system.
TF_REGISTRY_FUNCTION(TfType)
{
    TfType::Define<UsdColorSpaceAPI,
        TfType::Bases< UsdAPISchemaBase > >();
    
}

/* virtual */
UsdColorSpaceAPI::~UsdColorSpaceAPI()
{
}

/* static */
UsdColorSpaceAPI
UsdColorSpaceAPI::Get(const UsdStagePtr &stage, const SdfPath &path)
{
    if (!stage) {
        TF_CODING_ERROR("Invalid stage");
        return UsdColorSpaceAPI();
    }
    return UsdColorSpaceAPI(stage->GetPrimAtPath(path));
}


/* virtual */
UsdSchemaKind UsdColorSpaceAPI::_GetSchemaKind() const
{
    return UsdColorSpaceAPI::schemaKind;
}

/* static */
bool
UsdColorSpaceAPI::CanApply(
    const UsdPrim &prim, std::string *whyNot)
{
    return prim.CanApplyAPI<UsdColorSpaceAPI>(whyNot);
}

/* static */
UsdColorSpaceAPI
UsdColorSpaceAPI::Apply(const UsdPrim &prim)
{
    if (prim.ApplyAPI<UsdColorSpaceAPI>()) {
        return UsdColorSpaceAPI(prim);
    }
    return UsdColorSpaceAPI();
}

/* static */
const TfType &
UsdColorSpaceAPI::_GetStaticTfType()
{
    static TfType tfType = TfType::Find<UsdColorSpaceAPI>();
    return tfType;
}

/* static */
bool 
UsdColorSpaceAPI::_IsTypedSchema()
{
    static bool isTyped = _GetStaticTfType().IsA<UsdTyped>();
    return isTyped;
}

/* virtual */
const TfType &
UsdColorSpaceAPI::_GetTfType() const
{
    return _GetStaticTfType();
}

UsdAttribute
UsdColorSpaceAPI::GetColorSpaceNameAttr() const
{
    return GetPrim().GetAttribute(UsdTokens->colorSpaceName);
}

UsdAttribute
UsdColorSpaceAPI::CreateColorSpaceNameAttr(VtValue const &defaultValue, bool writeSparsely) const
{
    return UsdSchemaBase::_CreateAttr(UsdTokens->colorSpaceName,
                       SdfValueTypeNames->Token,
                       /* custom = */ false,
                       SdfVariabilityUniform,
                       defaultValue,
                       writeSparsely);
}

UsdAttribute
UsdColorSpaceAPI::GetColorSpaceRedChromaAttr() const
{
    return GetPrim().GetAttribute(UsdTokens->colorSpaceRedChroma);
}

UsdAttribute
UsdColorSpaceAPI::CreateColorSpaceRedChromaAttr(VtValue const &defaultValue, bool writeSparsely) const
{
    return UsdSchemaBase::_CreateAttr(UsdTokens->colorSpaceRedChroma,
                       SdfValueTypeNames->Float2,
                       /* custom = */ false,
                       SdfVariabilityVarying,
                       defaultValue,
                       writeSparsely);
}

UsdAttribute
UsdColorSpaceAPI::GetColorSpaceGreenChromaAttr() const
{
    return GetPrim().GetAttribute(UsdTokens->colorSpaceGreenChroma);
}

UsdAttribute
UsdColorSpaceAPI::CreateColorSpaceGreenChromaAttr(VtValue const &defaultValue, bool writeSparsely) const
{
    return UsdSchemaBase::_CreateAttr(UsdTokens->colorSpaceGreenChroma,
                       SdfValueTypeNames->Float2,
                       /* custom = */ false,
                       SdfVariabilityVarying,
                       defaultValue,
                       writeSparsely);
}

UsdAttribute
UsdColorSpaceAPI::GetColorSpaceBlueChromaAttr() const
{
    return GetPrim().GetAttribute(UsdTokens->colorSpaceBlueChroma);
}

UsdAttribute
UsdColorSpaceAPI::CreateColorSpaceBlueChromaAttr(VtValue const &defaultValue, bool writeSparsely) const
{
    return UsdSchemaBase::_CreateAttr(UsdTokens->colorSpaceBlueChroma,
                       SdfValueTypeNames->Float2,
                       /* custom = */ false,
                       SdfVariabilityVarying,
                       defaultValue,
                       writeSparsely);
}

UsdAttribute
UsdColorSpaceAPI::GetColorSpaceWhitePointAttr() const
{
    return GetPrim().GetAttribute(UsdTokens->colorSpaceWhitePoint);
}

UsdAttribute
UsdColorSpaceAPI::CreateColorSpaceWhitePointAttr(VtValue const &defaultValue, bool writeSparsely) const
{
    return UsdSchemaBase::_CreateAttr(UsdTokens->colorSpaceWhitePoint,
                       SdfValueTypeNames->Float2,
                       /* custom = */ false,
                       SdfVariabilityVarying,
                       defaultValue,
                       writeSparsely);
}

UsdAttribute
UsdColorSpaceAPI::GetColorSpaceGammaAttr() const
{
    return GetPrim().GetAttribute(UsdTokens->colorSpaceGamma);
}

UsdAttribute
UsdColorSpaceAPI::CreateColorSpaceGammaAttr(VtValue const &defaultValue, bool writeSparsely) const
{
    return UsdSchemaBase::_CreateAttr(UsdTokens->colorSpaceGamma,
                       SdfValueTypeNames->Float,
                       /* custom = */ false,
                       SdfVariabilityVarying,
                       defaultValue,
                       writeSparsely);
}

UsdAttribute
UsdColorSpaceAPI::GetColorSpaceLinearBiasAttr() const
{
    return GetPrim().GetAttribute(UsdTokens->colorSpaceLinearBias);
}

UsdAttribute
UsdColorSpaceAPI::CreateColorSpaceLinearBiasAttr(VtValue const &defaultValue, bool writeSparsely) const
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
UsdColorSpaceAPI::GetSchemaAttributeNames(bool includeInherited)
{
    static TfTokenVector localNames = {
        UsdTokens->colorSpaceName,
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

void UsdColorSpaceAPI::CreateColorSpaceByName(const TfToken& name)
{
    if (GfColorSpace::IsConstructable(name)) {
        CreateColorSpaceNameAttr(VtValue(name));
    } 
    else {
        TF_CODING_ERROR("Invalid color space name: %s", name.GetText());
    }
}

void UsdColorSpaceAPI::CreateColorSpaceWithChroma(const GfVec2f& redChroma,
                                                  const GfVec2f& greenChroma,
                                                  const GfVec2f& blueChroma,
                                                  const GfVec2f& whitePoint,
                                                  float gamma, float linearBias)
{
    // Create the colorSpaceName attribute.
    CreateColorSpaceNameAttr(VtValue(GfColorSpaceNames->Custom));
    CreateColorSpaceRedChromaAttr(VtValue(redChroma));
    CreateColorSpaceGreenChromaAttr(VtValue(greenChroma));
    CreateColorSpaceBlueChromaAttr(VtValue(blueChroma));
    CreateColorSpaceWhitePointAttr(VtValue(whitePoint));
    CreateColorSpaceGammaAttr(VtValue(gamma));
    CreateColorSpaceLinearBiasAttr(VtValue(linearBias));
}

void UsdColorSpaceAPI::CreateColorSpaceWithMatrix(const GfMatrix3f& rgbToXYZ,
                                                  float gamma, float linearBias)
{
    // Create the colorSpaceName attribute.
    CreateColorSpaceNameAttr(VtValue(GfColorSpaceNames->Custom));
    CreateColorSpaceGammaAttr(VtValue(gamma));
    CreateColorSpaceLinearBiasAttr(VtValue(linearBias));

    GfColorSpace colorSpace(GfColorSpaceNames->Custom, rgbToXYZ, gamma, linearBias);

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

TfToken UsdColorSpaceAPI::ComputeColorSpaceName() const 
{
    // Check for a colorSpace property on this prim.
    if (UsdAttribute colorSpaceAttr = GetColorSpaceNameAttr()) {
        TfToken colorSpace;
        if (colorSpaceAttr.Get(&colorSpace)) {
            if ((colorSpace == GfColorSpaceNames->Custom) ||
                GfColorSpace::IsConstructable(colorSpace)) {
                return colorSpace;
            }
        }
    }

    // Check the prim's parents for a colorSpace property.
    UsdPrim prim = GetPrim().GetParent();
    while (prim) {
        if (UsdAttribute colorSpaceAttr = UsdColorSpaceAPI(prim).GetColorSpaceNameAttr()) {
            TfToken colorSpace;
            if (colorSpaceAttr.Get(&colorSpace)) {
                if ((colorSpace == GfColorSpaceNames->Custom) ||
                    GfColorSpace::IsConstructable(colorSpace)) {
                    return colorSpace;
                }
            }
        }
        prim = prim.GetParent();
    }

    return GfColorSpaceNames->Raw;
}

TfToken UsdColorSpaceAPI::ComputeColorSpaceName(const UsdAttribute& attr) const
{
    UsdPrim prim = GetPrim();
    UsdPrim attrPrim = attr.GetPrim();

    if (prim != attrPrim) {
        TF_CODING_ERROR("Attribute <%s> does not belong to the prim <%s>",
                        attr.GetPath().GetText(),
                        prim.GetPath().GetText());
        return GfColorSpaceNames->LinearRec709;
    }

    TfToken colorSpace = attr.GetColorSpace();
    if (!colorSpace.IsEmpty()) {
        if ((colorSpace == GfColorSpaceNames->Custom) ||
             GfColorSpace::IsConstructable(colorSpace)) {
            return colorSpace;
        }
    }

    return ComputeColorSpaceName();
}

GfColorSpace UsdColorSpaceAPI::ComputeColorSpace() const
{
    // Check for a colorSpace property on this prim.
    if (UsdAttribute colorSpaceAttr = GetColorSpaceNameAttr()) {
        TfToken colorSpace;
        if (colorSpaceAttr.Get(&colorSpace)) {
            if (colorSpace == GfColorSpaceNames->Custom) {
                return _ColorSpaceFromAttributes();
            }
            if (GfColorSpace::IsConstructable(colorSpace)) {
                return GfColorSpace(colorSpace);
            }
            return GfColorSpace(GfColorSpaceNames->Raw);
        }
    }

    // Check the prim's parents for a colorSpace property.
    UsdPrim prim = GetPrim().GetParent();
    while (prim) {
        if (UsdAttribute colorSpaceAttr = UsdColorSpaceAPI(prim).GetColorSpaceNameAttr()) {
            TfToken colorSpace;
            if (colorSpaceAttr.Get(&colorSpace)) {
                if (colorSpace == GfColorSpaceNames->Custom) {
                    return _ColorSpaceFromAttributes();
                }
                if (GfColorSpace::IsConstructable(colorSpace)) {
                    return GfColorSpace(colorSpace);
                }
                return GfColorSpace(GfColorSpaceNames->Raw);
            }
        }
        prim = prim.GetParent();
    }

    return GfColorSpace(GfColorSpaceNames->Raw);
}

GfColorSpace UsdColorSpaceAPI::ComputeColorSpace(const UsdAttribute& attr) const
{
    UsdPrim prim = GetPrim();
    UsdPrim attrPrim = attr.GetPrim();

    if (prim != attrPrim) {
        TF_CODING_ERROR("Attribute <%s> does not belong to the prim <%s>",
                        attr.GetPath().GetText(),
                        prim.GetPath().GetText());
        return GfColorSpace(GfColorSpaceNames->Raw);
    }

    TfToken colorSpace = attr.GetColorSpace();
    if (!colorSpace.IsEmpty()) {
        if ((colorSpace == GfColorSpaceNames->Custom)) {
            return _ColorSpaceFromAttributes();
        }
        if (GfColorSpace::IsConstructable(colorSpace)) {
            return GfColorSpace(colorSpace);
        }
        return GfColorSpace(GfColorSpaceNames->Raw);
    }

    return ComputeColorSpace();
}

GfColorSpace UsdColorSpaceAPI::_ColorSpaceFromAttributes() const
{
    // Check for a colorSpace property on this prim.
    if (UsdAttribute colorSpaceAttr = GetColorSpaceNameAttr()) {
        TfToken colorSpace;
        if (colorSpaceAttr.Get(&colorSpace)) {
            if (colorSpace == GfColorSpaceNames->Custom) {
                GfVec2f redChroma;
                GfVec2f greenChroma;
                GfVec2f blueChroma;
                GfVec2f whitePoint;
                float gamma;
                float linearBias;
                GetColorSpaceRedChromaAttr().Get(&redChroma);
                GetColorSpaceGreenChromaAttr().Get(&greenChroma);
                GetColorSpaceBlueChromaAttr().Get(&blueChroma);
                GetColorSpaceWhitePointAttr().Get(&whitePoint);
                GetColorSpaceGammaAttr().Get(&gamma);
                GetColorSpaceLinearBiasAttr().Get(&linearBias);
                return GfColorSpace(
                    GfColorSpaceNames->Custom,
                    redChroma,
                    greenChroma,
                    blueChroma,
                    whitePoint,
                    gamma,
                    linearBias);
            }
        }
    }

    return GfColorSpace(GfColorSpaceNames->Raw);
}

PXR_NAMESPACE_CLOSE_SCOPE
