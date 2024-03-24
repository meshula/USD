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
#ifndef PXR_BASE_GF_COLOR_H
#define PXR_BASE_GF_COLOR_H

/// \file gf/color.h
/// \ingroup group_gf_BasicGeometry

#include "pxr/pxr.h"
#include "pxr/base/gf/vec2d.h"
#include "pxr/base/gf/matrix3f.h"
#include "pxr/base/gf/api.h"
#include "pxr/base/tf/span.h"
#include "pxr/base/tf/staticTokens.h"
#include "pxr/base/tf/hash.h"

#include <float.h>

PXR_NAMESPACE_OPEN_SCOPE

/// \enum GfColorSpace
/// \ingroup group_gf_BasicGeometry
///
/// Color spaces supported by Gf.
///
/// ACEScg:           The Academy Color Encoding System, a color space designed
///                     for cinematic content creation and exchange, using AP1 primaries
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
/// sRGB_DisplayP3:   sRGB color space adapted to the Display P3 primaries
/// Linear_Rec2020:   Rec2020 gamut, and linear gamma

// Note that the quoted names here correspond to the canonical names defined
// by the OpenColorIO project

#define GF_COLORSPACE_CANONICAL_NAME_TOKENS  \
    ((Identity, "identity"))                 \
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
    ((SRGBDisplayP3, "srgb_displayp3"))      \
    ((Custom, "custom"))

TF_DECLARE_PUBLIC_TOKENS(GfColorspaceCanonicalName, GF_API, 
                                        GF_COLORSPACE_CANONICAL_NAME_TOKENS);


/// \class GfColorSpace
/// \ingroup group_gf_BasicGeometry
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
/// The color spaces supported by Gf are listed in GfColorspaceCanonicalName
/// For historical compatibility, "raw" is treated as a synonym for "identity",
/// and "sRGB" is treated as a synonym for "srgb_texture"

class GfColorSpace {
    friend class GfColor;
public:
    ~GfColorSpace() = default;
    
    // construct from a GfColorspaceCanonicalName token
    GF_API explicit GfColorSpace(TfToken name);
    
    // construct a custom colorspace from raw values
    GF_API explicit GfColorSpace(const std::string& name,
                                 const GfVec2f &redChroma,
                                 const GfVec2f &greenChroma,
                                 const GfVec2f &blueChroma,
                                 const GfVec2f &whitePoint,
                                 float gamma,
                                 float linearBias,
                                 float K0,
                                 float phi);
    
    // construct a colorspace from a 3x3 matrix and linearization parameters
    GF_API explicit GfColorSpace(const std::string& name,
                                 const GfMatrix3f &rgbToXYZ,
                                 float gamma,
                                 float linearBias,
                                 float K0,
                                 float phi);
    
    GF_API TfToken GetName() const;

    GF_API bool operator ==(const GfColorSpace &lh) const;
    bool operator !=(const GfColorSpace &lh) const { return !(*this == lh); }

private:
    struct Data;
    std::shared_ptr<Data> _data;
};

/// \class GfColor
/// \ingroup group_gf_BasicGeometry
///
/// Basic type: Color
///
/// This class represents a color, consisting of three components: red,
/// green, and blue.  It also contains a color space, which is used to
/// interpret the RGB values.
///
/// Various set and get methods are provided; these allow conversion
/// between RGB and other color spaces.
///
/// The color spaces supported by Gf are enumerated in GfColorSpace.

class GfColor {
public:
    /// The default constructor creates white, in the "lin_rec709" space
    GF_API GfColor();
    ~GfColor() = default;

    /// Construct from a color from another color
    GF_API
    GfColor(const GfColor&);

    /// Construct from an rgb tuple and colorspace
    GF_API
    GfColor(const GfVec3f &rgb, GfColorSpace colorSpace);

    /// Construct a color from another color into the specified color space
    GF_API
    GfColor(const GfColor &color, GfColorSpace colorSpace);

    /// Replace the color with the contents of the input
    GF_API
    GfColor(GfColor&&) noexcept = default;

    /// Replace the color with the contents of the input
    GF_API
    GfColor& operator=(const GfColor&);

    /// Replace the color with the contents of the input
    GF_API
    GfColor& operator=(GfColor&&) noexcept;

        /// Hash.
    friend inline size_t hash_value(GfColor const &vec) {
        return TfHash::Combine(vec._rgb[0], vec._rgb[1], vec._rgb[2], vec._colorSpace->GetName());
    }
    
    /// Set the color from a CIEXYZ coordinate, adapting to the existing color space
    GF_API
    void SetFromCIEXYZ(const GfVec3f& xyz);

    /// Set the color from blackbody temperature in Kelvin, adapting to the existing color space
    GF_API
    void SetFromBlackbodyKelvin(float kelvin, float luminosity);

    // Set the color from a wavelength in nanometers, adapting to the existing color space
    GF_API
    void SetFromWavelengthNM(float nm);

    /// Get the RGB tuple
    GF_API
    GfVec3f GetRGB() const { return _rgb; }

    /// Get the color space
    GF_API
    std::shared_ptr<GfColorSpace> GetColorSpace() const { return _colorSpace; }

    /// Get the CIEXYZ coordinate of the color
    GF_API
    GfVec3f GetCIEXYZ() const;

    /// Return the color's RGB values, normalized to a specified luminance
    GF_API
    GfVec3f NormalizedLuminance(float) const;

    /// Normalize the color to a specified luminance, and return the RGB value
    GF_API
    GfVec3f NormalizeLuminance(float);

    bool operator ==(const GfColor &l) const {
        return _rgb == l._rgb && *_colorSpace == *l._colorSpace;
    }
    bool operator !=(const GfColor &r) const {
	    return ! (*this == r);
    }
    
    /// Convert a packed array of RGB values from one color space to another
    GF_API static
    void ConvertRGB(const GfColorSpace& from, const GfColorSpace& to, 
                   TfSpan<const float> rgb);

    /// Convert a packed array of RGBA values from one color space to another
    GF_API static
    void ConvertRGBA(const GfColorSpace& from, const GfColorSpace& to, 
                     TfSpan<const float> rgba);

  private:
    GfVec3f _rgb;
    std::shared_ptr<GfColorSpace> _colorSpace;
};

/// Output a GfColor.
/// \ingroup group_gf_DebuggingOutput
GF_API std::ostream& operator<<(std::ostream &, GfColor const &);


PXR_NAMESPACE_CLOSE_SCOPE

#endif // PXR_BASE_GF_COLOR3F_H
