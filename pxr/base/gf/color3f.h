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
#include "pxr/base/gf/api.h"

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

enum GfColorSpace {
    GfColorSpaceIdentity,
    GfColorSpaceCIEXYZ,
    GfColorSpaceACEScg,
    GfColorSpaceLinearAP1,
    GfColorSpaceG18AP1,
    GfColorSpaceG22AP1,
    GfColorSpaceG18Rec709,
    GfColorSpaceG22Rec709,
    GfColorSpaceLinearRec709,
    GfColorSpaceAdobeRGB,
    GfColorSpaceLinearAdobeRGB,
    GfColorSpaceLinearDisplayP3,
    GfColorSpaceLinearSRGB,
    GfColorSpaceSRGBTexture,
    GfColorSpaceSRGBDisplayP3,
    GfColorSpaceLinearRec2020,
    GfColorSpaceCustom,
    GfColorSpaceLast=GfColorSpaceLinearRec2020;
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

class GfColor3f {
  public:

    /// The default constructor leaves the color undefined
    GF_API GfColor3f();

    GF_API ~GfColor3f() = default;

    /// Construct a color from a GfVec3f, and a color space
    GfColor3f(const GfVec3f &color, GfColorSpace colorSpace = GfColorSpaceLinearSRGB) {
        Set( color, colorSpace );
    }

    /// Construct a color from another color into the specified color space
    GfColor3f(const GfColor3f &color, GfColorSpace colorSpace) {
        Set( {0,0,0}, colorSpace );
        SetWithAdaptation(color);
    }

    /// Construct a color from a GfVec3f, chromaticities, whitepoint, gamma, 
    /// linear bias for the gamma section, a break point for a linear section,
    /// and the slope of the linear section.
    GF_API
    GfColor3f(const GfVec3f &color,
              const GfVec2f &redChroma,
              const GfVec2f &greenChroma,
              const GfVec2f &blueChroma,
              const GfVec2f &whitePoint,
              float gamma,
              float linearBias,
              float K0,
              float phi);

    GF_API
    void Set(const GfVec3f &color, GfColorSpace colorSpace);

    /// Set the color from the input color, adapting to the existing color space
    GF_API
    void SetWithAdaptation(const GfColor3f&);

    /// Set the color from a CIEXYZ coordinate, adapting to the existing color space
    GF_API
    void SetFromCIEXYZ(const GfVec3f&);

    /// Set the color from blackbody temperature in Kelvin, adapting to the existing color space
    GF_API
    void SetFromBlackbodyKelvin(float);

    // Set the color from a wavelength in nanometers, adapting to the existing color space
    GF_API
    void SetFromWavelengthNM(float);

    /// Get the RGB tuple
    GfVec3f GetRGB() const { return _rgb; }

    /// Get the color space
    GfColorSpace GetColorSpace() const { return _colorSpace; }

    /// Get the CIEXYZ coordinate of the color
    GF_API
    GfVec3f GetCIEXYZ() const;

    /// Return the color's RGB values, normalized to a specified luminance
    GF_API
    GfVec3f NormalizedLuminance(float) const;

    /// Normalize the color to a specified luminance, and return the RGB value
    GF_API
    GfVec3f NormalizeLuminance(float);

    bool operator ==(const GfColor3f &l) const {
        return _color == l._color && _colorSpace == l._colorSpace;
    }
    bool operator !=(const GfColor3f &r) const {
	    return ! (*this == r);
    }

  private:
    GfVec3f _rgb;
    GfColorSpace _colorSpace;
    struct Data;
    const std::shared_ptr<Data> _data;
};

/// return a color space's enumeration from its name
GF_API
GfColorSpace GetGfColorSpaceFromName(const std::string& name);

/// return a color space's name from its enumeration
GF_API
std::string GetGfColorSpaceName(GfColorSpace colorSpace);

PXR_NAMESPACE_CLOSE_SCOPE

#endif // PXR_BASE_GF_COLOR3F_H
