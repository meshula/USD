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
UsdColorSpaceAPI::GetColorSpaceAttr() const
{
    return GetPrim().GetAttribute(UsdTokens->colorSpace);
}

UsdAttribute
UsdColorSpaceAPI::CreateColorSpaceAttr(VtValue const &defaultValue, bool writeSparsely) const
{
    return UsdSchemaBase::_CreateAttr(UsdTokens->colorSpace,
                       SdfValueTypeNames->Token,
                       /* custom = */ false,
                       SdfVariabilityUniform,
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
        UsdTokens->colorSpace,
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

TfToken UsdColorSpaceAPI::ResolveColorSpace(const SdfPath& attribute) const 
{
    UsdAttribute attr = GetPrim().GetAttribute(TfToken(attribute.GetName()));
    if (!attr) {
        TF_CODING_ERROR("Attribute <%s> not found on prim <%s>",
                        attribute.GetString().c_str(),
                        GetPrim().GetPath().GetString().c_str());
        return UsdTokens->lin_rec709;
    }

    TfToken colorSpace = attr.GetColorSpace();
    if (!colorSpace.IsEmpty()) {
        if (GfColorSpace::IsConstructable(colorSpace)) {
            return colorSpace;
        }
    }

    // Check for a colorSpace property on this prim.
    if (UsdAttribute colorSpaceAttr = GetColorSpaceAttr()) {
        if (colorSpaceAttr.Get(&colorSpace)) {
            if (GfColorSpace::IsConstructable(colorSpace)) {
                return colorSpace;
            }
        }
    }

    // Check the prim's parents for a colorSpace property.
    UsdPrim prim = GetPrim().GetParent();
    while (prim) {
        if (UsdAttribute colorSpaceAttr = UsdColorSpaceAPI(prim).GetColorSpaceAttr()) {
            if (colorSpaceAttr.Get(&colorSpace)) {
                if (GfColorSpace::IsConstructable(colorSpace)) {
                    return colorSpace;
                }
            }
        }
        prim = prim.GetParent();
    }

    return GfColorSpaceNames->LinearRec709;
}

PXR_NAMESPACE_CLOSE_SCOPE
