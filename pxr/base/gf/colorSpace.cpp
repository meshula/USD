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
    TfType::Define<GfColorSpace>();
}

TF_DEFINE_PUBLIC_TOKENS(GfColorSpaceCanonicalName, GF_COLORSPACE_CANONICAL_NAME_TOKENS);


size_t GfColorSpace::HashData(GfColorSpace::Data* d) {
    if (!d) {
        return 0;
    }
    NcColorSpaceM33Descriptor desc;
    if (!NcGetColorSpaceM33Descriptor(d->colorSpace, &desc)) {
        return 0;
    }

    return TfHash::Combine(TfHashCString()(desc.name),
                           d->constructedFromPrimaries,
                           desc.rgbToXYZ.m[0], desc.rgbToXYZ.m[1], desc.rgbToXYZ.m[2],
                           desc.rgbToXYZ.m[3], desc.rgbToXYZ.m[4], desc.rgbToXYZ.m[5],
                           desc.rgbToXYZ.m[6], desc.rgbToXYZ.m[7], desc.rgbToXYZ.m[8],
                           desc.gamma, desc.linearBias);
}

GfColorSpace::GfColorSpace(const TfToken& name)
: _data(new Data())
{
    _data->colorSpace = NcGetNamedColorSpace(name.GetString().c_str());
    if (_data->colorSpace == nullptr) {
        // A color space constructed with a name that is not a registered name
        // should function like an identity color space; the only reason to do
        // this is to have a sentinel color space meant for comparison and
        // hashing.
        NcColorSpaceM33Descriptor identity;
        identity.name = strdup(name.GetString().c_str());
        identity.rgbToXYZ = { 1.0f, 0.0f, 0.0f,
                              0.0f, 1.0f, 0.0f,
                              0.0f, 0.0f, 1.0f };
        identity.gamma = 1.0f;
        identity.linearBias = 0.0f;
        _data->colorSpace = NcCreateColorSpaceM33(&identity);
        _data->constructedFromPrimaries = false;
    }
}

// construct a custom colorspace from raw values
GfColorSpace::GfColorSpace(const TfToken& name,
                           const GfVec2f &redChroma,
                           const GfVec2f &greenChroma,
                           const GfVec2f &blueChroma,
                           const GfVec2f &whitePoint,
                           float gamma,
                           float linearBias)
: _data(new Data())
{
    NcColorSpaceDescriptor desc;
    desc.name = strdup(name.GetString().c_str());
    desc.redPrimary.x = redChroma[0];
    desc.redPrimary.y = redChroma[1];
    desc.greenPrimary.x = greenChroma[0];
    desc.greenPrimary.y = greenChroma[1];
    desc.bluePrimary.x = blueChroma[0];
    desc.bluePrimary.y = blueChroma[1];
    desc.whitePoint.x = whitePoint[0];
    desc.whitePoint.y = whitePoint[1];
    desc.gamma = gamma;
    desc.linearBias = linearBias;
    _data->colorSpace = NcCreateColorSpace(&desc);
    _data->constructedFromPrimaries = true;
}

// construct a custom colorspace from a 3x3 matrix and linearization parameters
GfColorSpace::GfColorSpace(const TfToken& name,
                           const GfMatrix3f &rgbToXYZ,
                           float gamma,
                           float linearBias)
: _data(new Data())
{
    NcColorSpaceM33Descriptor desc;
    desc.name = strdup(name.GetString().c_str());
    desc.rgbToXYZ.m[0] = rgbToXYZ[0][0];
    desc.rgbToXYZ.m[1] = rgbToXYZ[0][1];
    desc.rgbToXYZ.m[2] = rgbToXYZ[0][2];
    desc.rgbToXYZ.m[3] = rgbToXYZ[1][0];
    desc.rgbToXYZ.m[4] = rgbToXYZ[1][1];
    desc.rgbToXYZ.m[5] = rgbToXYZ[1][2];
    desc.rgbToXYZ.m[6] = rgbToXYZ[2][0];
    desc.rgbToXYZ.m[7] = rgbToXYZ[2][1];
    desc.rgbToXYZ.m[8] = rgbToXYZ[2][2];
    desc.gamma = gamma;
    desc.linearBias = linearBias;
    _data->colorSpace = NcCreateColorSpaceM33(&desc);
    _data->constructedFromPrimaries = false;
}

bool GfColorSpace::operator==(const GfColorSpace &lh) const
{
    return NcColorSpaceEqual(_data->colorSpace, lh._data->colorSpace);
}

/// Convert a packed array of RGB values from one color space to another
void GfColorSpace::ConvertRGB(const GfColorSpace& to, TfSpan<float> rgb) {
    // Convert the RGB values in place
    size_t count = rgb.size() / 3;
    if (!count || (count * 3 != rgb.size())) {
        TF_CODING_ERROR("RGB array size must be a multiple of 3");
        return;
    }
    NcTransformColors(to._data->colorSpace, _data->colorSpace, 
                      (NcRGB*) rgb.data(), (int) count);
}

/// Convert a packed array of RGBA values from one color space to another
void GfColorSpace::ConvertRGBA(const GfColorSpace& to, TfSpan<float> rgba) {
    // Convert the RGBA values in place
    size_t count = rgba.size() / 4;
    if (!count || (count * 4 != rgba.size())) {
        TF_CODING_ERROR("RGBA array size must be a multiple of 4");
        return;
    }
    NcTransformColorsWithAlpha(to._data->colorSpace, _data->colorSpace, 
                               rgba.data(), (int) count);
}

TfToken GfColorSpace::GetName() const
{
    NcColorSpaceM33Descriptor desc;
    if (!NcGetColorSpaceM33Descriptor(_data->colorSpace, &desc)) {
        return TfToken();
    }
    return TfToken(desc.name);
}


PXR_NAMESPACE_CLOSE_SCOPE
