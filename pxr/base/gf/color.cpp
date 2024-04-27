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
#include "pxr/pxr.h"
#include "pxr/base/gf/color.h"
#include "pxr/base/gf/colorSpace.h"
#include "pxr/base/gf/ostreamHelpers.h"
#include "pxr/base/gf/vec3f.h"
#include "pxr/base/tf/enum.h"
#include "pxr/base/tf/hash.h"
#include "pxr/base/tf/registryManager.h"
#include "pxr/base/tf/type.h"
#include "nc/nanocolor.h"
#include "nc/nanocolorUtils.h"
#include "colorSpace_data.h"
#include <mutex>

PXR_NAMESPACE_OPEN_SCOPE

TF_REGISTRY_FUNCTION(TfType) {
    TfType::Define<GfColor>();
}

std::ostream& 
operator<<(std::ostream &out, GfColor const &v)
{
    GfVec3f rgb = v.GetRGB();
    return out << '(' 
        << Gf_OstreamHelperP(rgb[0]) << ", " 
        << Gf_OstreamHelperP(rgb[1]) << ", " 
        << Gf_OstreamHelperP(rgb[2]) << ", "
        << Gf_OstreamHelperP(v.GetColorSpace().GetName().GetString()) << ')';
}

// The default constructor creates white, in the "lin_rec709" space
GfColor::GfColor()
: _colorSpace(GfColorSpaceCanonicalName->LinearRec709)
, _rgb(1.0f, 1.0f, 1.0f)
{
}


// Construct from an rgb tuple and colorspace
GfColor::GfColor(const GfVec3f &rgb, const GfColorSpace& colorSpace)
: _colorSpace(colorSpace)
, _rgb(rgb)
{
}

// Construct a color from another color into the specified color space
GfColor::GfColor(const GfColor &color, const GfColorSpace& colorSpace)
: _colorSpace(colorSpace)
{
    NcRGB src = {color._rgb[0], color._rgb[1], color._rgb[2]};
    NcRGB dst = NcTransformColor(_colorSpace._data->colorSpace,
                                 colorSpace._data->colorSpace, src);
    _rgb = GfVec3f(dst.r, dst.g, dst.b);
}

// Set the color from a CIEXYZ coordinate, adapting to the existing color space
void GfColor::SetFromCIEXYZ(const GfVec3f& xyz)
{
    NcCIEXYZ ncxyz = { xyz[0], xyz[1], xyz[2] };
    NcRGB dst = NcXYZToRGB(_colorSpace._data->colorSpace, ncxyz);
    _rgb = GfVec3f(dst.r, dst.g, dst.b);
}

// Set the color from blackbody temperature in Kelvin, 
// adapting to the existing color space
void GfColor::SetFromBlackbodyKelvin(float kelvin, float luminosity)
{
    NcCIEXYZ xyz = NcKelvinToXYZ(kelvin, luminosity);
    NcRGB dst = NcXYZToRGB(_colorSpace._data->colorSpace, xyz);
    _rgb = GfVec3f(dst.r, dst.g, dst.b);
}

// Set the color from a wavelength in nanometers, 
// adapting to the existing color space
void GfColor::SetFromWavelengthNM(float nm)
{
    NcCIEXYZ xyz = NcCIE1931ColorFromWavelength(nm, false);
    NcRGB dst = NcXYZToRGB(_colorSpace._data->colorSpace, xyz);
    _rgb = GfVec3f(dst.r, dst.g, dst.b);
}

// Get the CIEXYZ coordinate of the color
GfVec3f GfColor::GetCIEXYZ() const
{
    NcRGB src = {_rgb[0], _rgb[1], _rgb[2]};
    NcCIEXYZ dst = NcRGBToXYZ(_colorSpace._data->colorSpace, src);
    return GfVec3f(dst.x, dst.y, dst.z);
}

// Return the color's RGB values, normalized to a specified luminance
GfColor GfColor::NormalizedLuminance(float luminance) const
{
    GfColor ret = *this;
    ret.NormalizeLuminance(luminance);
    return ret;
}

// Normalize the color to a specified luminance
GfColor& GfColor::NormalizeLuminance(float luminance)
{
    NcRGB src = {_rgb[0], _rgb[1], _rgb[2]};
    NcCIEXYZ dst = NcRGBToXYZ(_colorSpace._data->colorSpace, src);
    dst = NcNormalizeXYZ(dst);
    _rgb = GfVec3f(dst.x * luminance, dst.y * luminance, dst.z * luminance);
    return *this;
}

PXR_NAMESPACE_CLOSE_SCOPE
