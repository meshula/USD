//
// Copyright 2024 Pixar
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
#ifndef PXR_BASE_GF_COLORSPACE_H
#define PXR_BASE_GF_COLORSPACE_H

/// \file gf/color.h
/// \ingroup group_gf_BasicGeometry

#include "pxr/pxr.h"
#include "pxr/base/gf/vec2f.h"
#include "pxr/base/gf/matrix3f.h"
#include "pxr/base/gf/api.h"
#include "pxr/base/tf/span.h"
#include "pxr/base/tf/staticTokens.h"
#include "pxr/base/tf/hash.h"

#include <iostream>

PXR_NAMESPACE_OPEN_SCOPE

/// \enum GfColorSpace
/// \ingroup group_gf_BasicGeometry
///
/// Color spaces natively supported by Gf.
/// The token names correspond to the canonical names defined
/// by the OpenColorIO Nanocolor project
///
/// ACEScg:           The Academy Color Encoding System, a color space designed
///                     for cinematic content creation and exchange, with AP1 primaries
/// Linear_AP1:       Alias for ACEScg. A linearized version of the ACEScg color space
/// G18_AP1:          A color space with a 1.8 gamma and an AP1 primaries color gamut
/// G22_AP1:          A color space with a 2.2 gamma and an AP1 primaries color gamut
/// G18_Rec709:       A color space with a 1.8 gamma, and primaries per the Rec. 709
///                     standard, commonly used in HDTV
/// G22_Rec709:       A color space with a 2.2 gamma, and primaries per the Rec. 709
///                     standard, commonly used in HDTV
/// Linear_Rec709:    A linearized version of the Rec. 709 color space.
/// AdobeRGB:         A color space developed by Adobe Systems. It has a wider gamut
///                     than sRGB and is suitable for photography and printing
/// Linear_AdobeRGB:  The AdobeRGB gamut, and linear gamma
/// Linear_DisplayP3: DisplayP3 gamut, and linear gamma
/// Linear_sRGB:      sRGB gamut, linear gamma
/// sRGB_Texture:     An sRGB color space optimized for texture mapping.
/// sRGB:             A synonym for sRGB_Texture
/// sRGB_DisplayP3:   sRGB color space adapted to the Display P3 primaries
/// Linear_Rec2020:   Rec2020 gamut, and linear gamma
/// CIEXYZ:           The CIE 1931 XYZ color space, a standard color space used
///                     in color science
///
/// identity and raw are a utility color space that does not tranform the color
///
/// Colorspaces outside of the set may be defined.
#define GF_COLORSPACE_CANONICAL_NAME_TOKENS  \
    ((Identity, "identity"))                 \
    ((Raw, "raw"))                           \
    ((ACEScg, "acescg"))                     \
    ((AdobeRGB, "adobergb"))                 \
    ((LinearAdobeRGB, "lin_adobergb"))       \
    ((CIEXYZ, "CIEXYZ"))                     \
    ((LinearAP0, "lin_ap0"))                 \
    ((LinearAP1, "lin_ap1"))                 \
    ((G18AP1, "g18_ap1"))                    \
    ((G22AP1, "g22_ap1"))                    \
    ((LinearRec2020, "lin_rec2020"))         \
    ((LinearRec709, "lin_rec709"))           \
    ((G18Rec709, "g18_rec709"))              \
    ((G22Rec709, "g18_rec709"))              \
    ((LinearDisplayP3, "lin_displayp3"))     \
    ((LinearSRGB, "lin_srgb"))               \
    ((SRGBTexture, "srgb_texture"))          \
    ((SRGB, "sRGB"))                         \
    ((SRGBDisplayP3, "srgb_displayp3"))      \

TF_DECLARE_PUBLIC_TOKENS(GfColorSpaceCanonicalName, GF_API, 
                                        GF_COLORSPACE_CANONICAL_NAME_TOKENS);

/// \class GfColorSpace
/// \ingroup group_gf_Color
///
/// Basic type: ColorSpace
///
/// This class represents a colorspace. Color spaces may be created by
/// name, parameterization, or by a 3x3 matrix and a gamma operator.
///
/// The parameters used to construct the color space are not available for
/// introspection ~ the color space object is intended for color conversion
/// operations on a GfColor.
///
/// The color spaces natively recognized by GfColorSpace are listed in 
/// GfColorspaceCanonicalName.

class GfColorSpace {
    friend class GfColor;
public:    
    /// Construct a GfColorSpace from a canonical name token.
    ///
    /// \param name The canonical name token of the color space.
    GF_API explicit GfColorSpace(const TfToken& name);
    
    /// Construct a custom color space from raw values.
    ///
    /// \param name The name token of the color space.
    /// \param redChroma The red chromaticity coordinates.
    /// \param greenChroma The green chromaticity coordinates.
    /// \param blueChroma The blue chromaticity coordinates.
    /// \param whitePoint The white point chromaticity coordinates.
    /// \param gamma The gamma value of the log section.
    /// \param linearBias The linear bias of the log section.
    /// \param K0 The linear break point.
    /// \param phi The slope of the linear section.
    GF_API explicit GfColorSpace(const TfToken& name,
                                 const GfVec2f &redChroma,
                                 const GfVec2f &greenChroma,
                                 const GfVec2f &blueChroma,
                                 const GfVec2f &whitePoint,
                                 float gamma,
                                 float linearBias);
    
    /// Construct a color space from a 3x3 matrix and linearization parameters.
    ///
    /// \param name The name token of the color space.
    /// \param rgbToXYZ The RGB to XYZ conversion matrix.
    /// \param gamma The gamma value of the log section.
    /// \param linearBias The linear bias of the log section.
    /// \param K0 The linear break point.
    /// \param phi The slope of the linear section.
    GF_API explicit GfColorSpace(const TfToken& name,
                                 const GfMatrix3f &rgbToXYZ,
                                 float gamma,
                                 float linearBias);

    /// Destructor.
    ~GfColorSpace() = default;
    
    /// Get the name of the color space.
    ///
    /// \return The name of the color space.
    GF_API TfToken GetName() const;

    /// Check if two color spaces are equal.
    ///
    /// \param lh The left-hand side color space.
    /// \return True if the color spaces are equal, false otherwise.
    GF_API bool operator ==(const GfColorSpace &lh) const;
    
    /// Check if two color spaces are not equal.
    ///
    /// \param rh The rigt-hand side color space.
    /// \return True if the color spaces are not equal, false otherwise.
    bool operator !=(const GfColorSpace &rh) const { return !(*this == rh); }

    /// Convert in place a packed array of RGB values from one color space to another.
    ///
    /// \param to The target color space.
    /// \param rgb The packed array of RGB values to convert.
    GF_API void ConvertRGB(const GfColorSpace& to, TfSpan<float> rgb);

    /// Convert in place a packed array of RGBA values from one color space to another.
    ///
    /// \param to The target color space.
    /// \param rgba The packed array of RGBA values to convert.
    GF_API void ConvertRGBA(const GfColorSpace& to, TfSpan<float> rgba);

    /// Get the RGB to CIEXYZ conversion matrix.
    ///
    /// \return The RGB to CIEXYZ conversion matrix.
    GF_API GfMatrix3f GetRGBToXYZ() const;

    /// Get the gamma value of the color space.
    ///
    /// \return The gamma value of the color space.
    GF_API float GetGamma() const;

    /// Get the linear bias of the color space.
    ///
    /// \return The linear bias of the color space.
    GF_API float GetLinearBias() const;

    /// Get the computed K0 and Phi values for use in the transfer function.
    ///
    GF_API std::pair<float, float> GetTransferFunctionParams() const;

    /// Indicate if the color space was constructed from primaries.
    ///
    /// \return True if the color space was constructed from primaries, false otherwise.
    GF_API bool IsConstructedFromPrimaries() const;

    /// Get the chromaticity coordinates and white point if the color space
    /// was constructed from primaries.
    ///
    /// \return The chromaticity coordinates and white point; 
    /// an empty optional if the color space was not constructed from primaries.
    GF_API std::optional<std::tuple<GfVec2f, GfVec2f, GfVec2f, GfVec2f>> GetPrimaries() const;

    /// TfHash function for GfColor.
    /// \param vec The GfColor object to hash.
    /// \return The hash value.
    template <class HashState>
    friend void
    TfHashAppend(HashState &h, GfColorSpace const &cs) {
        h.Append(HashData(cs._data.get()));
    }

private:
    struct Data;
    std::shared_ptr<Data> _data;

    // Hash function for the private data
    GF_API static size_t HashData(Data*);
};


PXR_NAMESPACE_CLOSE_SCOPE

#endif // PXR_BASE_GF_COLORSPACE_H
