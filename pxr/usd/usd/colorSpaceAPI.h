//
// Copyright 2016 Pixar
//
// Licensed under the terms set forth in the LICENSE.txt file available at
// https://openusd.org/license.
//
#ifndef USD_GENERATED_COLORSPACEAPI_H
#define USD_GENERATED_COLORSPACEAPI_H

/// \file usd/colorSpaceAPI.h

#include "pxr/pxr.h"
#include "pxr/usd/usd/api.h"
#include "pxr/usd/usd/apiSchemaBase.h"
#include "pxr/usd/usd/prim.h"
#include "pxr/usd/usd/stage.h"
#include "pxr/usd/usd/tokens.h"

#include "pxr/base/gf/colorSpace.h"


#include "pxr/base/vt/value.h"

#include "pxr/base/gf/vec3d.h"
#include "pxr/base/gf/vec3f.h"
#include "pxr/base/gf/matrix4d.h"

#include "pxr/base/tf/token.h"
#include "pxr/base/tf/type.h"

PXR_NAMESPACE_OPEN_SCOPE

class SdfAssetPath;

// -------------------------------------------------------------------------- //
// COLORSPACEAPI                                                              //
// -------------------------------------------------------------------------- //

/// \class UsdColorSpaceAPI
///
/// UsdColorSpaceAPI is an API schema that introduces a property for
/// authoring color space opinions.
/// 
/// In the absence of authored color space information on an attribute, the
/// colorSpace property on a prim indicates the color space to use on such an
/// attribute, as well as for any attribute without authored color space on a
/// descendant prim which does not itself specify a color space.
/// 
/// This schema may be applied to any prim to hierarchly introduce a color
/// space at any point in a compositional hierarchy.
/// 
/// ComputeColorSpaceName is supplied to calculate the effective color space of
/// a prim, as defined by its most local color space opinion, if any exists.
/// If none is found, the default is lin_rec709.
/// 
/// Color space conversions may be performed by creating a GfColorSpace using
/// the token returned by ComputeColorSpaceName, assigning that color space and
/// the color property values of interest to a GfColor, and finally creating 
/// a new GfColor using the GfColor of interest and a target GfColorSpace.
/// 
/// It is recommended that an application perform such conversions infrequently
/// and cache results in situations where performance is of concern.
/// 
/// 
///
/// For any described attribute \em Fallback \em Value or \em Allowed \em Values below
/// that are text/tokens, the actual token is published and defined in \ref UsdTokens.
/// So to set an attribute to the value "rightHanded", use UsdTokens->rightHanded
/// as the value.
///
class UsdColorSpaceAPI : public UsdAPISchemaBase
{
public:
    /// Compile time constant representing what kind of schema this class is.
    ///
    /// \sa UsdSchemaKind
    static const UsdSchemaKind schemaKind = UsdSchemaKind::SingleApplyAPI;

    /// Construct a UsdColorSpaceAPI on UsdPrim \p prim .
    /// Equivalent to UsdColorSpaceAPI::Get(prim.GetStage(), prim.GetPath())
    /// for a \em valid \p prim, but will not immediately throw an error for
    /// an invalid \p prim
    explicit UsdColorSpaceAPI(const UsdPrim& prim=UsdPrim())
        : UsdAPISchemaBase(prim)
    {
    }

    /// Construct a UsdColorSpaceAPI on the prim held by \p schemaObj .
    /// Should be preferred over UsdColorSpaceAPI(schemaObj.GetPrim()),
    /// as it preserves SchemaBase state.
    explicit UsdColorSpaceAPI(const UsdSchemaBase& schemaObj)
        : UsdAPISchemaBase(schemaObj)
    {
    }

    /// Destructor.
    USD_API
    virtual ~UsdColorSpaceAPI();

    /// Return a vector of names of all pre-declared attributes for this schema
    /// class and all its ancestor classes.  Does not include attributes that
    /// may be authored by custom/extended methods of the schemas involved.
    USD_API
    static const TfTokenVector &
    GetSchemaAttributeNames(bool includeInherited=true);

    /// Return a UsdColorSpaceAPI holding the prim adhering to this
    /// schema at \p path on \p stage.  If no prim exists at \p path on
    /// \p stage, or if the prim at that path does not adhere to this schema,
    /// return an invalid schema object.  This is shorthand for the following:
    ///
    /// \code
    /// UsdColorSpaceAPI(stage->GetPrimAtPath(path));
    /// \endcode
    ///
    USD_API
    static UsdColorSpaceAPI
    Get(const UsdStagePtr &stage, const SdfPath &path);


    /// Returns true if this <b>single-apply</b> API schema can be applied to 
    /// the given \p prim. If this schema can not be a applied to the prim, 
    /// this returns false and, if provided, populates \p whyNot with the 
    /// reason it can not be applied.
    /// 
    /// Note that if CanApply returns false, that does not necessarily imply
    /// that calling Apply will fail. Callers are expected to call CanApply
    /// before calling Apply if they want to ensure that it is valid to 
    /// apply a schema.
    /// 
    /// \sa UsdPrim::GetAppliedSchemas()
    /// \sa UsdPrim::HasAPI()
    /// \sa UsdPrim::CanApplyAPI()
    /// \sa UsdPrim::ApplyAPI()
    /// \sa UsdPrim::RemoveAPI()
    ///
    USD_API
    static bool 
    CanApply(const UsdPrim &prim, std::string *whyNot=nullptr);

    /// Applies this <b>single-apply</b> API schema to the given \p prim.
    /// This information is stored by adding "ColorSpaceAPI" to the 
    /// token-valued, listOp metadata \em apiSchemas on the prim.
    /// 
    /// \return A valid UsdColorSpaceAPI object is returned upon success. 
    /// An invalid (or empty) UsdColorSpaceAPI object is returned upon 
    /// failure. See \ref UsdPrim::ApplyAPI() for conditions 
    /// resulting in failure. 
    /// 
    /// \sa UsdPrim::GetAppliedSchemas()
    /// \sa UsdPrim::HasAPI()
    /// \sa UsdPrim::CanApplyAPI()
    /// \sa UsdPrim::ApplyAPI()
    /// \sa UsdPrim::RemoveAPI()
    ///
    USD_API
    static UsdColorSpaceAPI 
    Apply(const UsdPrim &prim);

protected:
    /// Returns the kind of schema this class belongs to.
    ///
    /// \sa UsdSchemaKind
    USD_API
    UsdSchemaKind _GetSchemaKind() const override;

private:
    // needs to invoke _GetStaticTfType.
    friend class UsdSchemaRegistry;
    USD_API
    static const TfType &_GetStaticTfType();

    static bool _IsTypedSchema();

    // override SchemaBase virtuals.
    USD_API
    const TfType &_GetTfType() const override;

public:
    // --------------------------------------------------------------------- //
    // COLORSPACENAME 
    // --------------------------------------------------------------------- //
    /// The color space that applies to attributes with
    /// unauthored color spaces on this prim and its descendents.
    /// 
    ///
    /// | ||
    /// | -- | -- |
    /// | Declaration | `uniform token colorSpace:name = "lin_rec709"` |
    /// | C++ Type | TfToken |
    /// | \ref Usd_Datatypes "Usd Type" | SdfValueTypeNames->Token |
    /// | \ref SdfVariability "Variability" | SdfVariabilityUniform |
    USD_API
    UsdAttribute GetColorSpaceNameAttr() const;

    /// See GetColorSpaceNameAttr(), and also 
    /// \ref Usd_Create_Or_Get_Property for when to use Get vs Create.
    /// If specified, author \p defaultValue as the attribute's default,
    /// sparsely (when it makes sense to do so) if \p writeSparsely is \c true -
    /// the default for \p writeSparsely is \c false.
    USD_API
    UsdAttribute CreateColorSpaceNameAttr(VtValue const &defaultValue = VtValue(), bool writeSparsely=false) const;

public:
    // --------------------------------------------------------------------- //
    // COLORSPACEREDCHROMA 
    // --------------------------------------------------------------------- //
    /// Red chromaticity coordinates
    ///
    /// | ||
    /// | -- | -- |
    /// | Declaration | `float2 colorSpace:redChroma = (0.64, 0.33)` |
    /// | C++ Type | GfVec2f |
    /// | \ref Usd_Datatypes "Usd Type" | SdfValueTypeNames->Float2 |
    USD_API
    UsdAttribute GetColorSpaceRedChromaAttr() const;

    /// See GetColorSpaceRedChromaAttr(), and also 
    /// \ref Usd_Create_Or_Get_Property for when to use Get vs Create.
    /// If specified, author \p defaultValue as the attribute's default,
    /// sparsely (when it makes sense to do so) if \p writeSparsely is \c true -
    /// the default for \p writeSparsely is \c false.
    USD_API
    UsdAttribute CreateColorSpaceRedChromaAttr(VtValue const &defaultValue = VtValue(), bool writeSparsely=false) const;

public:
    // --------------------------------------------------------------------- //
    // COLORSPACEGREENCHROMA 
    // --------------------------------------------------------------------- //
    /// Green chromaticity coordinates
    ///
    /// | ||
    /// | -- | -- |
    /// | Declaration | `float2 colorSpace:greenChroma = (0.3, 0.6)` |
    /// | C++ Type | GfVec2f |
    /// | \ref Usd_Datatypes "Usd Type" | SdfValueTypeNames->Float2 |
    USD_API
    UsdAttribute GetColorSpaceGreenChromaAttr() const;

    /// See GetColorSpaceGreenChromaAttr(), and also 
    /// \ref Usd_Create_Or_Get_Property for when to use Get vs Create.
    /// If specified, author \p defaultValue as the attribute's default,
    /// sparsely (when it makes sense to do so) if \p writeSparsely is \c true -
    /// the default for \p writeSparsely is \c false.
    USD_API
    UsdAttribute CreateColorSpaceGreenChromaAttr(VtValue const &defaultValue = VtValue(), bool writeSparsely=false) const;

public:
    // --------------------------------------------------------------------- //
    // COLORSPACEBLUECHROMA 
    // --------------------------------------------------------------------- //
    /// Blue chromaticity coordinates
    ///
    /// | ||
    /// | -- | -- |
    /// | Declaration | `float2 colorSpace:blueChroma = (0.15, 0.06)` |
    /// | C++ Type | GfVec2f |
    /// | \ref Usd_Datatypes "Usd Type" | SdfValueTypeNames->Float2 |
    USD_API
    UsdAttribute GetColorSpaceBlueChromaAttr() const;

    /// See GetColorSpaceBlueChromaAttr(), and also 
    /// \ref Usd_Create_Or_Get_Property for when to use Get vs Create.
    /// If specified, author \p defaultValue as the attribute's default,
    /// sparsely (when it makes sense to do so) if \p writeSparsely is \c true -
    /// the default for \p writeSparsely is \c false.
    USD_API
    UsdAttribute CreateColorSpaceBlueChromaAttr(VtValue const &defaultValue = VtValue(), bool writeSparsely=false) const;

public:
    // --------------------------------------------------------------------- //
    // COLORSPACEWHITEPOINT 
    // --------------------------------------------------------------------- //
    /// Whitepoint chromaticity coordinates
    ///
    /// | ||
    /// | -- | -- |
    /// | Declaration | `float2 colorSpace:whitePoint = (0.3127, 0.329)` |
    /// | C++ Type | GfVec2f |
    /// | \ref Usd_Datatypes "Usd Type" | SdfValueTypeNames->Float2 |
    USD_API
    UsdAttribute GetColorSpaceWhitePointAttr() const;

    /// See GetColorSpaceWhitePointAttr(), and also 
    /// \ref Usd_Create_Or_Get_Property for when to use Get vs Create.
    /// If specified, author \p defaultValue as the attribute's default,
    /// sparsely (when it makes sense to do so) if \p writeSparsely is \c true -
    /// the default for \p writeSparsely is \c false.
    USD_API
    UsdAttribute CreateColorSpaceWhitePointAttr(VtValue const &defaultValue = VtValue(), bool writeSparsely=false) const;

public:
    // --------------------------------------------------------------------- //
    // COLORSPACEGAMMA 
    // --------------------------------------------------------------------- //
    /// Gamma value of the log section
    ///
    /// | ||
    /// | -- | -- |
    /// | Declaration | `float colorSpace:gamma = 1` |
    /// | C++ Type | float |
    /// | \ref Usd_Datatypes "Usd Type" | SdfValueTypeNames->Float |
    USD_API
    UsdAttribute GetColorSpaceGammaAttr() const;

    /// See GetColorSpaceGammaAttr(), and also 
    /// \ref Usd_Create_Or_Get_Property for when to use Get vs Create.
    /// If specified, author \p defaultValue as the attribute's default,
    /// sparsely (when it makes sense to do so) if \p writeSparsely is \c true -
    /// the default for \p writeSparsely is \c false.
    USD_API
    UsdAttribute CreateColorSpaceGammaAttr(VtValue const &defaultValue = VtValue(), bool writeSparsely=false) const;

public:
    // --------------------------------------------------------------------- //
    // COLORSPACELINEARBIAS 
    // --------------------------------------------------------------------- //
    /// Linear bias of the log section
    ///
    /// | ||
    /// | -- | -- |
    /// | Declaration | `float colorSpace:linearBias = 0` |
    /// | C++ Type | float |
    /// | \ref Usd_Datatypes "Usd Type" | SdfValueTypeNames->Float |
    USD_API
    UsdAttribute GetColorSpaceLinearBiasAttr() const;

    /// See GetColorSpaceLinearBiasAttr(), and also 
    /// \ref Usd_Create_Or_Get_Property for when to use Get vs Create.
    /// If specified, author \p defaultValue as the attribute's default,
    /// sparsely (when it makes sense to do so) if \p writeSparsely is \c true -
    /// the default for \p writeSparsely is \c false.
    USD_API
    UsdAttribute CreateColorSpaceLinearBiasAttr(VtValue const &defaultValue = VtValue(), bool writeSparsely=false) const;

public:
    // ===================================================================== //
    // Feel free to add custom code below this line, it will be preserved by 
    // the code generator. 
    //
    // Just remember to: 
    //  - Close the class declaration with }; 
    //  - Close the namespace with PXR_NAMESPACE_CLOSE_SCOPE
    //  - Close the include guard with #endif
    // ===================================================================== //
    // --(BEGIN CUSTOM CODE)--

    /// Creates the color space name attribute with the given name, but not the
    /// other color space attributes. If the name is not a valid color space
    /// name, a TF_CODING_ERROR is issued.
    USD_API
    void CreateColorSpaceByName(const TfToken& name);

    /// Creates the color space attributes with the given values. The color space
    /// name attribute is set to Custom.
    USD_API
    void CreateColorSpaceWithChroma(const GfVec2f& redChroma,
                                    const GfVec2f& greenChroma,
                                    const GfVec2f& blueChroma,
                                    const GfVec2f& whitePoint,
                                    float gamma, float linearBias);

    /// Create the color space attributes by deriving the color space from the
    /// given RGB to XYZ matrix and linearization parameters. The color space
    /// name attribute is set to Custom.
    USD_API
    void CreateColorSpaceWithMatrix(const GfMatrix3f& rgbToXYZ,
                                    float gamma, float linearBias);

    /// Calculate the color space for the given attribute on this prim. If the
    /// attribute is not on this prim, a TF_CODING_ERROR is issued. Otherwise,
    /// the color space is resolved by checking the colorSpace property on this
    /// attribute. If it is not authored there, this prim is checked for a
    /// colorSpace property. If no colorSpace property is authored on this prim,
    /// the prim's parents are checked until a colorSpace property is found or
    /// the root prim is reached. If no colorSpaceName property is found, the
    /// fallback color space, lin_rec709, is returned.
    ///
    /// If a color space name attribute is found, but it is not one of the
    /// standard color spaces, the value is raw is returned.
    ///
    /// This function should be considered a reference implementation for
    /// determining the color space of an attribute. As the algorithm is
    /// implemented as an exhaustive search performed on the prim hierarchy, an
    /// application may wish to implement a caching mechanism to avoid
    /// redundant searches.
    ///
    /// \sa UsdColorSpaceAPI::Apply
    /// \sa UsdColorSpaceAPI::GetColorSpaceAttr
    /// \param attribute The attribute to compute the color space for.
    USD_API
    ComputeColorSpaceName(const UsdAttribute& attribute) const;

    /// Calculate the color space for this prim.
    /// The color space is resolved by checking this prim for a
    /// colorSpace property. If no colorSpace property is authored,
    /// the prim's parents are checked until a colorSpace property is found or
    /// the root prim is reached. If no colorSpace property is found, the
    /// fallback color space, lin_rec709, is returned.
    ///
    /// If a color space name attribute is found, but it is not one of the
    /// standard color spaces, the value is raw is returned.
    ///
    /// This function should be considered a reference implementation for
    /// determining the color space of an attribute. As the algorithm is
    /// implemented as an exhaustive search performed on the prim hierarchy, an
    /// application may wish to implement a caching mechanism to avoid
    /// redundant searches.
    ///
    /// \sa UsdColorSpaceAPI::Apply
    /// \sa UsdColorSpaceAPI::GetColorSpaceAttr
    USD_API
    TfToken ComputeColorSpaceName() const;

    USD_API
    GfColorSpace ComputeColorSpace(const UsdAttribute& attribute) const;
    USD_API
    GfColorSpace ComputeColorSpace() const;

private:
    GfColorSpace _ColorSpaceFromAttributes() const;
};

PXR_NAMESPACE_CLOSE_SCOPE

#endif
