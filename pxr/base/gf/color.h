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
#include "pxr/base/gf/colorSpace.h"
#include "pxr/base/gf/vec2f.h"
#include "pxr/base/gf/matrix3f.h"
#include "pxr/base/gf/api.h"
#include "pxr/base/tf/span.h"
#include "pxr/base/tf/staticTokens.h"
#include "pxr/base/tf/hash.h"

#include <iostream>

PXR_NAMESPACE_OPEN_SCOPE

/// \class GfColor
/// \brief Represents a color in a specific color space.
/// \ingroup group_gf_Color
/// 
/// Basic type: Color
///
/// The GfColor class represents a color in a specific color space. It provides
/// various methods for constructing, manipulating, and retrieving color values.
///
/// The color values are stored as an RGB tuple and are associated with a color
/// space. The color space determines the interpretation of the RGB values.
///
/// This class provides methods for setting and getting color values, converting
/// between color spaces, normalizing luminance, and comparing colors.

class GfColor {
public:
    /// The default constructor creates white, in the "lin_rec709" color space.
    GF_API GfColor();

    /// Construct a color from another color.
    /// \param color The color to copy.
    GfColor(const GfColor& color) = default;

    /// Construct a color from an RGB tuple and color space.
    /// \param rgb The RGB tuple.
    /// \param colorSpace The color space.
    GF_API
    GfColor(const GfVec3f &rgb, const GfColorSpace& colorSpace);

    /// Construct a color from another color into the specified color space.
    /// \param color The color to copy.
    /// \param colorSpace The color space.
    GF_API
    GfColor(const GfColor &color, const GfColorSpace& colorSpace);

    /// Move constructor.
    /// \param color The color to move.
    GfColor(GfColor&& color) noexcept = default;

    /// Copy assignment operator.
    /// \param color The color to copy.
    /// \return A reference to the assigned color.
    GfColor& operator=(const GfColor& color) noexcept = default;

    /// Move assignment operator.
    /// \param color The color to move.
    /// \return A reference to the assigned color.
    GfColor& operator=(GfColor&& color) noexcept = default;

    /// TfHash function for GfColor.
    /// \param vec The GfColor object to hash.
    /// \return The hash value.
    template <class HashState>
    friend void
    TfHashAppend(HashState &h, GfColor const &color) {
        h.Append(color._rgb, color._colorSpace);
    }

    /// Set the color from a CIEXYZ coordinate, adapting to the existing color space.
    /// \param xyz The CIEXYZ coordinate.
    GF_API
    void SetFromCIEXYZ(const GfVec3f& xyz);

    /// Set the color from blackbody temperature in Kelvin, in the existing color space.
    /// \param kelvin The blackbody temperature in Kelvin.
    /// \param luminosity The luminosity.
    GF_API
    void SetFromBlackbodyKelvin(float kelvin, float luminosity);

    /// Set the color from a wavelength in nanometers, in the existing color space.
    /// \param nm The wavelength in nanometers.
    GF_API
    void SetFromWavelengthNM(float nm);

    /// Get the RGB tuple.
    /// \return The RGB tuple.
    GfVec3f GetRGB() const { return _rgb; }

    /// Get the color space.
    /// \return The color space.
    GfColorSpace GetColorSpace() const { return _colorSpace; }

    /// Get the CIEXYZ coordinate of the color.
    /// \return The CIEXYZ coordinate.
    GF_API
    GfVec3f GetCIEXYZ() const;

    /// Return the color's RGB values, normalized to a specified luminance.
    /// \param luminance The specified luminance.
    /// \return The normalized color.
    GF_API
    GfColor NormalizedLuminance(float luminance) const;

    /// Normalize the color to a specified luminance.
    /// \param luminance The specified luminance.
    /// \return A reference to the normalized color.
    GF_API
    GfColor& NormalizeLuminance(float luminance);

    /// Equality operator.
    /// \param l The left-hand side color.
    /// \return True if the colors are equal, false otherwise.
    bool operator ==(const GfColor &l) const;

    /// Inequality operator.
    /// \param r The right-hand side color.
    /// \return True if the colors are not equal, false otherwise.
    bool operator !=(const GfColor &r) const;

private:
    GfColorSpace _colorSpace; ///< The color space.
    GfVec3f _rgb; ///< The RGB tuple.
};

/// Output a GfColor.
/// \ingroup group_gf_DebuggingOutput
/// @brief Stream insertion operator for outputting GfColor objects to an output stream.
///
/// This operator allows GfColor objects to be written to an output stream.
///
/// @param os The output stream to write to.
/// @param color The GfColor object to be outputted.
/// @return The output stream after writing the GfColor object.
std::ostream& operator<<(std::ostream &, GfColor const &);

PXR_NAMESPACE_CLOSE_SCOPE

#endif // PXR_BASE_GF_COLOR_H
