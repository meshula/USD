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
/// UsdColorSpaceAPI is an API schema that introduces a property for
/// authoring color space opinions.
/// 
/// If an attribute lacks authored color space information, the `colorSpace`
/// property on a prim specifies the color space to use for that attribute.
/// This also applies to any attributes on descendant prims that do not have
/// their own authored color space, unless those prims explicitly specify one.
/// 
/// This schema may be applied to any prim to introduce a color space at any
/// point in a compositional hierarchy.
/// 
/// ComputeColorSpaceName is supplied to calculate the effective color space of
/// a prim, as defined by its most local color space opinion, if any exists.
/// If none is found, the default is lin_rec709.
/// 
/// Color space conversions can be done by first creating a `GfColorSpace` 
/// using the token returned by `ComputeColorSpaceName`. Then, assign that 
/// color space and the relevant color property values to a `GfColor`. Finally, 
/// create a new `GfColor` using the original `GfColor` and the target
/// `GfColorSpace`.
/// 
/// It is recommended that in situations where performance is a concern, an 
/// application should perform conversions infrequently and cache results
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

    /// Computes the color space name for the given attribute on this prim. If the
    /// attribute does not belong to this prim, a `TF_CODING_ERROR` is issued. 
    /// Otherwise,the color space is determined by checking for a `colorSpace`
    /// property on the attribute itself. If no such property is authored, the
    /// prim's `colorSpace` property is checked. If neither the attribute nor
    /// the prim defines a `colorSpace`, the search continues up the parent
    /// hierarchy until a `colorSpace` property is found or the root prim is
    /// reached. If no `colorSpace` property is found, the fallback color space,
    /// `lin_rec709`, is returned.
    ///
    /// If a `colorSpace` name is found, but it is not one of the standard color
    /// spaces, and does not correspond to a user defined color space, the value
    /// `raw` is returned.
    ///
    /// This function should be considered a reference implementation for
    /// determining the color space of an attribute. Since the algorithm is
    /// implemented as an exhaustive search performed through the prim 
    /// hierarchy, applications may want to implement a caching mechanism to
    /// avoid redundant searches.
    ///
    /// \sa UsdColorSpaceAPI::Apply
    /// \sa UsdColorSpaceAPI::GetColorSpaceAttr
    /// \param attribute The attribute to compute the color space for.
    USD_API
    TfToken ComputeColorSpaceName(const UsdAttribute& attribute) const;

    /// Computes the color space for the given attribute on this prim, using the
    /// same algorithm as `ComputeColorSpaceName`. The same performancecaveat
    /// applies.
    /// \param attribute The attribute to compute the color space for.
    USD_API
    GfColorSpace ComputeColorSpace(const UsdAttribute& attribute) const;

    /// Computes the color space name for this prim.
    /// The color space is determined by checking this prim for a
    /// `colorSpace` property. If no `colorSpace `property is authored,
    /// the search continues up the prim's hierarchyuntil a `colorSpace `
    /// property is found or the root prim is reached. If no `colorSpace` 
    /// property is found, the fallback color space, `lin_rec709`, is returned.
    ///
    /// If a `colorSpace` name is found, but does not match one of the standard 
    /// color spaces or a user defined color space, the value `raw` is returned.
    ///
    /// This function should be considered as a reference implementation, and
    /// applications may want to implement a caching mechanism for performance.
    ///
    /// \sa UsdColorSpaceAPI::Apply
    /// \sa UsdColorSpaceAPI::GetColorSpaceAttr
    USD_API
    TfToken ComputeColorSpaceName() const;

    /// Computes the color space for this prim, using the same algorithm as
    /// `ComputeColorSpaceName`. The same performance caveat applies.
    USD_API
    GfColorSpace ComputeColorSpace() const;
};

PXR_NAMESPACE_CLOSE_SCOPE

#endif
