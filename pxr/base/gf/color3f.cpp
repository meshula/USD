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
#include "pxr/base/gf/color3f.h"
#include "nc/Nanocolor.h"
#include "nc/NanocolorUtils.h"

PXR_NAMESPACE_OPEN_SCOPE

TF_REGISTRY_FUNCTION(TfType) {
    TfType::Define<GfColor3f>();
}

/// return a color space's enumeration from its name
GfColorSpace GetGfColorSpaceFromName(const std::string& sname)
{
    const char* name = sname.c_str();
    if (!strnicmp(name, "ACESCg", 6)) {
        return GfColorSpace::ACEScg;
    }
    if (!strnicmp(name, "Linear_AP1", 10)) {
        return GfColorSpace::LinearAP1;
    }
    if (!strnicmp(name, "G18_AP1", 7)) {
        return GfColorSpace::G18AP1;
    }
    if (!strnicmp(name, "G22_AP1", 7)) {
        return GfColorSpace::G22AP1;
    }
    if (!strnicmp(name, "G18_Rec709", 10)) {
        return GfColorSpace::G18Rec709;
    }
    if (!strnicmp(name, "G22_Rec709", 10)) {
        return GfColorSpace::G22Rec709;
    }
    if (!strnicmp(name, "Linear_Rec709", 13)) {
        return GfColorSpace::LinearRec709;
    }
    if (!strnicmp(name, "AdobeRGB", 8)) {
        return GfColorSpace::AdobeRGB;
    }
    if (!strnicmp(name, "Linear_AdobeRGB", 15)) {
        return GfColorSpace::LinearAdobeRGB;
    }
    if (!strnicmp(name, "Linear_DisplayP3", 16)) {
        return GfColorSpace::LinearDisplayP3;
    }
    if (!strnicmp(name, "Linear_sRGB", 11)) {
        return GfColorSpace::LinearSRGB;
    }
    if (!strnicmp(name, "sRGB_Texture", 13)) {
        return GfColorSpace::SRGBTexture;
    }
    if (!strnicmp(name, "sRGB_DisplayP3", 15)) {
        return GfColorSpace::SRGBDisplayP3;
    }
    if (!strnicmp(name, "Linear_Rec2020", 14)) {
        return GfColorSpace::LinearRec2020;
    }
    return GfColorSpace::Custom;
}

/// return a color space's name from its enumeration
std::string GetGfColorSpaceName(GfColorSpace colorSpace)
{
    switch (colorSpace) {
        case GfColorSpace::ACEScg:
            return "ACEScg";
        case GfColorSpace::LinearAP1:
            return "Linear_AP1";
        case GfColorSpace::G18AP1:
            return "G18_AP1";
        case GfColorSpace::G22AP1:
            return "G22_AP1";
        case GfColorSpace::G18Rec709:
            return "G18_Rec709";
        case GfColorSpace::G22Rec709:
            return "G22_Rec709";
        case GfColorSpace::LinearRec709:
            return "Linear_Rec709";
        case GfColorSpace::AdobeRGB:
            return "AdobeRGB";
        case GfColorSpace::LinearAdobeRGB:
            return "Linear_AdobeRGB";
        case GfColorSpace::LinearDisplayP3:
            return "Linear_DisplayP3";
        case GfColorSpace::LinearSRGB:
            return "Linear_sRGB";
        case GfColorSpace::SRGBTexture:
            return "sRGB_Texture";
        case GfColorSpace::SRGBDisplayP3:
            return "sRGB_DisplayP3";
        case GfColorSpace::LinearRec2020:
            return "Linear_Rec2020";
        default:
            return "Custom";
    }
}

struct GfColor3f::Data
{
    NcColorSpace colorSpace;

    Data()
    {
        memset(&colorSpace, 0, sizeof(colorSpace));
    }

    void SetColorSpace(GfColorSpace cs)
    {
        colorSpace = NcGetNamedColorSpace(GetGfColorSpaceName(cs).c_str());
    }
};

GfColor3f::GfColor3f()
{
    _data = new Data;
}

/// Construct a color from a GfVec3f, chromaticities, whitepoint, gamma, 
/// linear bias for the gamma section, a break point for a linear section,
/// and the slope of the linear section.

GfColor3f::GfColor3f(const GfVec3f &color,
                     const GfVec2f &redChroma,
                     const GfVec2f &greenChroma,
                     const GfVec2f &blueChroma,
                     const GfVec2f &whitePoint,
                     float gamma,
                     float linearBias,
                     float K0,
                     float phi)
{
    _data = new Data;
    _data->colorSpace.redPrimary.x = redChroma[0];
    _data->colorSpace.redPrimary.y = redChroma[1];
    _data->colorSpace.greenPrimary.x = greenChroma[0];
    _data->colorSpace.greenPrimary.y = greenChroma[1];
    _data->colorSpace.bluePrimary.x = blueChroma[0];
    _data->colorSpace.bluePrimary.y = blueChroma[1];
    _data->colorSpace.whitePoint.x = whitePoint[0];
    _data->colorSpace.whitePoint.y = whitePoint[1];
    _data->colorSpace.gamma = gamma;
    _data->colorSpace.linearBias = linearBias;
    _data->colorSpace.colorTransform.transfer.gamma = gamma;
    _data->colorSpace.colorTransform.transfer.linearBias = linearBias;
    _data->colorSpace.colorTransform.transfer.K0 = K0;
    _data->colorSpace.colorTransform.transfer.phi = phi;
    Set(color);
}

void GfColor3f::Set(const GfVec3f &color, GfColorSpace colorSpace)
{
    _data->SetColorSpace(colorSpace);
    _color = color;
    _colorSpace = colorSpace;
}

/// Set the color from the input color, adapting to the existing color space
void GfColor3f::SetWithAdaptation(const GfColor3f& c)
{
    NcRGB src = {c[0], c[1], c[2]};
    NcRGB dst = NcTransformColor(&data->colorSpace, &c.data->colorSpace, c);
    _color = GfVec3f(dst.r, dst.g, dst.b);
}

/// Set the color from a CIEXYZ coordinate, adapting to the existing color space
GfColor3f GfColor3f::SetFromCIEXYZ(const GfVec3f& xyz)
{
    NcRGB dst = NcTransformXYZToRGB(&data->colorSpace, xyz);
    _color = GfVec3f(dst.r, dst.g, dst.b);
    return _color;
}

/// Set the color from blackbody temperature in Kelvin, adapting to the existing color space
GfColor3f GfColor3f::SetFromBlackbodyKelvin(float kelvin)
{
    NcRGB dst = NcTransformBlackbodyToRGB(&data->colorSpace, kelvin);
    _color = GfVec3f(dst.r, dst.g, dst.b);
    return _color;
}

// Set the color from a wavelength in nanometers, adapting to the existing color space
GfColor3f GfColor3f::SetFromWavelengthNM(float nm)
{
    NcRGB dst = NcTransformWavelengthToRGB(&data->colorSpace, nm);
    _color = GfVec3f(dst.r, dst.g, dst.b);
    return _color;
}

/// Get the CIEXYZ coordinate of the color
GfVec3f GfColor3f::GetCIEXYZ() const
{
    NcRGB src = {c[0], c[1], c[2]};
    NcRGB dst = NcTransformRGBToXYZ(&data->colorSpace, src);
    return GfVec3f(dst.r, dst.g, dst.b);
}

/// Return the color's RGB values, normalized to a specified luminance
GfVec3f GfColor3f::NormalizedLuminance(float luminance) const
{
    NcRGB src = {c[0], c[1], c[2]};
    NcRGB dst = NcNormalizeLuminance(&data->colorSpace, src, luminance);
    return GfVec3f(dst.r, dst.g, dst.b);
}

/// Normalize the color to a specified luminance, and return the RGB value
GfVec3f GfColor3f::NormalizeLuminance(float luminance)
{
    NcRGB src = {c[0], c[1], c[2]};
    NcRGB dst = NcNormalizeLuminance(&data->colorSpace, src, luminance);
    _color = GfVec3f(dst.r, dst.g, dst.b);
    return _color;
}

PXR_NAMESPACE_CLOSE_SCOPE
