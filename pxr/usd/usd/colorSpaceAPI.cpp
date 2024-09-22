//
// Copyright 2016 Pixar
//
// Licensed under the terms set forth in the LICENSE.txt file available at
// https://openusd.org/license.
//
#include "pxr/usd/usd/colorSpaceAPI.h"
#include "pxr/usd/usd/colorSpaceDefinitionAPI.h"
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
    if (GfColorSpace::IsValid(name)) {
        CreateColorSpaceNameAttr(VtValue(name));
    } 
    else {
        TF_CODING_ERROR("Invalid color space name: %s", name.GetText());
    }
}

namespace {

bool _IsValidColorSpaceName(UsdPrim prim, const TfToken& name)
{
    if (GfColorSpace::IsValid(name))
        return true;
    
    // test if the name is defined at the current prim or a parent.
    while (prim) {
        UsdColorSpaceDefinitionAPI defn(prim);
        if (UsdAttribute colorSpacDefnNameAttr = defn.GetColorSpaceDefinitionNameAttr()) {
            TfToken colorSpace;
            if (colorSpacDefnNameAttr.Get(&colorSpace)) {
                if (name == colorSpace)
                    return true;
            }
        }
        prim = prim.GetParent();
    }
    return false;
}

} // anon

TfToken UsdColorSpaceAPI::ComputeColorSpaceName() const
{
    // test this prim, and all of it's parents for an assigned colorspace.
    UsdPrim prim = GetPrim();
    while (prim) {
        if (UsdAttribute colorSpaceAttr = UsdColorSpaceAPI(prim).GetColorSpaceNameAttr()) {
            TfToken colorSpace;
            if (colorSpaceAttr.Get(&colorSpace)) {
                // if this prim, or any of its parents defines the color space return it
                if (_IsValidColorSpaceName(prim, colorSpace)) {
                    return colorSpace;
                }
                // treat an unknown color space as raw (ie, no transform is appropriate)
                return GfColorSpaceNames->Raw;
            }
        }
        prim = prim.GetParent();
    }
    
    // a color space is defined nowhere, so the default is LinearRec709.
    return GfColorSpaceNames->LinearRec709;
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
        // if this prim, or any of its parents defines the color space return it
        if (_IsValidColorSpaceName(prim, colorSpace)) {
            return colorSpace;
        }
        // treat an unknown color space as raw (ie, no transform is appropriate)
        return GfColorSpaceNames->Raw;
    }

    // the attribute will use the prim's computed color space.
    return ComputeColorSpaceName();
}

GfColorSpace UsdColorSpaceAPI::ComputeColorSpace() const
{
    TfToken colorSpace = ComputeColorSpaceName();
    if (GfColorSpace::IsValid(colorSpace)) {
        return GfColorSpace(colorSpace);
    }

    // If it wasn't a stock named GfColorSpace it must've been defined
    // on the prim, or an ancestor.
    UsdPrim prim = GetPrim();
    while (prim) {
        UsdColorSpaceDefinitionAPI defn(prim);
        if (UsdAttribute colorSpacDefnNameAttr = defn.GetColorSpaceDefinitionNameAttr()) {
            TfToken defColorSpace;
            if (colorSpacDefnNameAttr.Get(&defColorSpace)) {
                if (colorSpace == defColorSpace) {
                    return defn.ComputeColorSpaceFromDefinitionAttributes();
                }
            }
        }
        prim = prim.GetParent();
    }
    
    // should never get here, but this is a reasonable fail safe.
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
        if (GfColorSpace::IsValid(colorSpace)) {
            return GfColorSpace(colorSpace);
        }
        
        // If it wasn't a stock named GfColorSpace it might've been defined
        // on the prim, or an ancestor.
        UsdPrim prim = GetPrim();
        while (prim) {
            UsdColorSpaceDefinitionAPI defn(prim);
            if (UsdAttribute colorSpacDefnNameAttr = defn.GetColorSpaceDefinitionNameAttr()) {
                TfToken defColorSpace;
                if (colorSpacDefnNameAttr.Get(&defColorSpace)) {
                    if (colorSpace == defColorSpace) {
                        return defn.ComputeColorSpaceFromDefinitionAttributes();
                    }
                }
            }
            prim = prim.GetParent();
        }

        return GfColorSpace(GfColorSpaceNames->Raw);
    }

    // color space wasn't on the attribute so check if it's on the prim
    return ComputeColorSpace();
}

PXR_NAMESPACE_CLOSE_SCOPE
