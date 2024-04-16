//
// Copyright 2024 Pixar
//
// Licensed under the Apache License, Version 2.0 (the "Apache License")
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
#include "pxr/base/gf/ostreamHelpers.h"
#include "pxr/base/gf/vec3f.h"
#include "pxr/base/tf/enum.h"
#include "pxr/base/tf/registryManager.h"
#include "pxr/base/tf/type.h"
#include "nc/Nanocolor.h"
#include "nc/NanocolorUtils.h"
#include <mutex>

PXR_NAMESPACE_OPEN_SCOPE

TF_REGISTRY_FUNCTION(TfType) {
    TfType::Define<GfColor>();
}

TF_DEFINE_PUBLIC_TOKENS(GfColorspaceCanonicalName, GF_rgbSPACE_CANONICAL_NAME_TOKENS);

struct GfColorSpace::Data
{
    std::shared_ptr<NcColorSpace> colorSpace;
    bool constructedFromPrimaries;
};
GfColorSpace::GfColorSpace(TfToken name)
: _data(new Data())
{
    _data->colorSpace = std::shared_ptr<NcColorSpace>(new NcColorSpace());
    NcColorSpace nccs = NcGetNamedColorSpace(name.GetString().c_str());
    if (nccs.name == nullptr) {
        nccs = NcGetNamedColorSpace("identity");
    }
    *_data->colorSpace = nccs;
}

// construct a custom colorspace from raw values
GfColorSpace::GfColorSpace(const std::string& name,
                           const GfVec2f &redChroma,
                           const GfVec2f &greenChroma,
                           const GfVec2f &blueChroma,
                           const GfVec2f &whitePoint,
                           float gamma,
                           float linearBias,
                           float K0,
                           float phi)
: _data(new Data())
{
    _data->colorSpace = std::shared_ptr<NcColorSpace>(new NcColorSpace());
    _data->constructedFromPrimaries = true;
    auto cs = _data->colorSpace.get();
    cs->name = strdup(name.c_str());
    cs->redPrimary.x = redChroma[0];
    cs->redPrimary.y = redChroma[1];
    cs->greenPrimary.x = greenChroma[0];
    cs->greenPrimary.y = greenChroma[1];
    cs->bluePrimary.x = blueChroma[0];
    cs->bluePrimary.y = blueChroma[1];
    cs->whitePoint.x = whitePoint[0];
    cs->whitePoint.y = whitePoint[1];
    cs->gamma = gamma;
    cs->linearBias = linearBias;
    cs->colorTransform.transfer.gamma = gamma;
    cs->colorTransform.transfer.linearBias = linearBias;
    cs->colorTransform.transfer.K0 = K0;
    cs->colorTransform.transfer.phi = phi;
}

// construct a custom colorspace from a 3x3 matrix and linearization parameters
GfColorSpace::GfColorSpace(const std::string& name,
                           const GfMatrix3f &rgbToXYZ,
                           float gamma,
                           float linearBias,
                           float K0,
                           float phi)
: _data(new Data())
{
    _data->colorSpace = std::shared_ptr<NcColorSpace>(new NcColorSpace());
    _data->constructedFromPrimaries = false;
    auto cs = _data->colorSpace.get();
    cs->name = strdup(name.c_str());
    cs->gamma = gamma;
    cs->linearBias = linearBias;
    memcpy(cs->colorTransform.transform.m, rgbToXYZ.GetArray(), 9 * sizeof(float));
    cs->colorTransform.transfer.gamma = gamma;
    cs->colorTransform.transfer.linearBias = linearBias;
    cs->colorTransform.transfer.K0 = K0;
    cs->colorTransform.transfer.phi = phi;
}

bool GfColorSpace::operator==(const GfColorSpace &lh) const
{
    return NcColorSpaceEqual(_data->colorSpace.get(), lh._data->colorSpace.get());
}

/// Convert a packed array of RGB values from one color space to another
void GfColorSpace::ConvertRGB(const GfColorSpace& to, TfSpan<const float> rgb) {

}

/// Convert a packed array of RGBA values from one color space to another
void GfColorSpace::ConvertRGBA(const GfColorSpace& to, TfSpan<const float> rgba) {

}

TfToken GfColorSpace::GetName() const
{
    return TfToken(_data->colorSpace->name);
}

std::ostream& 
operator<<(std::ostream &out, GfColor const &v)
{
    GfVec3f rgb = v.GetRGB();
    return out << '(' 
        << Gf_OstreamHelperP(rgb[0]) << ", " 
        << Gf_OstreamHelperP(rgb[1]) << ", " 
        << Gf_OstreamHelperP(rgb[2]) << ", "
        << Gf_OstreamHelperP(v.GetColorSpace()->GetName().GetString()) << ')';
}

// The default constructor creates white, in the "lin_rec709" space
GfColor::GfColor()
{
    _rgb = GfVec3f(1.0f, 1.0f, 1.0f);
    _colorSpace = std::make_shared<GfColorSpace>(
                             GfColorspaceCanonicalName->LinearRec709);
}

// Construct from an rgb tuple and colorspace
GfColor::GfColor(const GfVec3f &rgb, GfColorSpace colorSpace)
{
    _rgb = rgb;
    _colorSpace = std::make_shared<GfColorSpace>(colorSpace);
}

// Construct from a color from the input color,
GfColor::GfColor(const GfColor& color)
{
    _rgb = color._rgb;
    _colorSpace = color._colorSpace;
}

// Construct a color from another color into the specified color space
GfColor::GfColor(const GfColor &color, GfColorSpace colorSpace)
{
    NcRGB src = {color._rgb[0], color._rgb[1], color._rgb[2]};
    NcRGB dst = NcTransformColor(_colorSpace->_data->colorSpace.get(),
                                 colorSpace._data->colorSpace.get(), src);
    _rgb = GfVec3f(dst.r, dst.g, dst.b);
    _colorSpace = std::make_shared<GfColorSpace>(colorSpace);
}

// Replace the color with the contents of the input
GfColor& GfColor::operator=(const GfColor& color)
{
    _rgb = color._rgb;
    _colorSpace = color._colorSpace;
    return *this;
}

// Replace the color with the contents of the input
GfColor& GfColor::operator=(GfColor&& color) noexcept
{
    _rgb = std::move(color._rgb);
    _colorSpace = std::move(color._colorSpace);
    return *this;
}

// Set the color from a CIEXYZ coordinate, adapting to the existing color space
void GfColor::SetFromCIEXYZ(const GfVec3f& xyz)
{
    NcCIEXYZ ncxyz = { xyz[0], xyz[1], xyz[2] };
    NcRGB dst = NcXYZToRGB(_colorSpace->_data->colorSpace.get(), ncxyz);
    _rgb = GfVec3f(dst.r, dst.g, dst.b);
}

// Set the color from blackbody temperature in Kelvin, 
// adapting to the existing color space
void GfColor::SetFromBlackbodyKelvin(float kelvin, float luminosity)
{
    NcCIEXYZ xyz = NcKelvinToXYZ(kelvin, luminosity);
    NcRGB dst = NcXYZToRGB(_colorSpace->_data->colorSpace.get(), xyz);
    _rgb = GfVec3f(dst.r, dst.g, dst.b);
}

// Set the color from a wavelength in nanometers, 
// adapting to the existing color space
void GfColor::SetFromWavelengthNM(float nm)
{
    NcCIEXYZ xyz = NcCIE1931ColorFromWavelength(nm, false);
    NcRGB dst = NcXYZToRGB(_colorSpace->_data->colorSpace.get(), xyz);
    _rgb = GfVec3f(dst.r, dst.g, dst.b);
}

// Get the CIEXYZ coordinate of the color
GfVec3f GfColor::GetCIEXYZ() const
{
    NcRGB src = {_rgb[0], _rgb[1], _rgb[2]};
    NcCIEXYZ dst = NcRGBToXYZ(_colorSpace->_data->colorSpace.get(), src);
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
void GfColor::NormalizeLuminance(float luminance)
{
    NcRGB src = {_rgb[0], _rgb[1], _rgb[2]};
    NcCIEXYZ dst = NcRGBToXYZ(_colorSpace->_data->colorSpace.get(), src);
    dst = NcNormalizeXYZ(dst);
    _rgb = GfVec3f(dst.x * luminance, dst.y * luminance, dst.z * luminance);
}

PXR_NAMESPACE_CLOSE_SCOPE
