//
// Copyright 2020 Pixar
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

#ifndef PXR_IMAGING_HIO_TEXTUREUTILS_H
#define PXR_IMAGING_HIO_TEXTUREUTILS_H

#include "pxr/imaging/hio/types.h"
#include "pxr/base/gf/half.h"
#include "pxr/base/gf/math.h"
#include "pxr/base/trace/trace.h"

PXR_NAMESPACE_OPEN_SCOPE

namespace Hio {
    /// Converts given number of texels.
    ///
    /// Conversion can be in-place if the ends of the source and destination
    /// buffers align.
    using ConversionFunction =
        void(*)(const void * src,
                size_t numTexels,
                void * dst);

    std::pair<HioFormat, ConversionFunction>
    GetConversionFunction(const HioFormat hioFormat,
                          const bool performPremultiplyAlpha);
}

#if 0

bool
HdStTextureUtils::ReadAndConvertImage(
    HioImageSharedPtr const &image,
    const bool flipped,
    const bool premultiplyAlpha,
    const HgiMipInfo &mipInfo,
    const size_t layer,
    void * const bufferStart)
{
    TRACE_FUNCTION();

    const ConversionFunction conversionFunction =
        GetHioToHgiConversion(image->GetFormat(),
                              premultiplyAlpha);

    // Given the start of the buffer containing all mips
    // and layers, compute where the desired mip and layer
    // starts.
    unsigned char * const mipLayerStart =
        static_cast<unsigned char*>(bufferStart)
        + mipInfo.byteOffset + layer * mipInfo.byteSizePerLayer;

    HioImage::StorageSpec spec;
    spec.width  = mipInfo.dimensions[0];
    spec.height = mipInfo.dimensions[1];
    spec.format = image->GetFormat();
    spec.flipped = flipped;
    if (conversionFunction) {
        // This part is a bit tricky: the RGB to RGBA conversion
        // is in place. To make sure we do not write over data
        // that have not been read yet, we need to align the ends.
        const size_t hioSize =
            HioGetDataSize(image->GetFormat(), mipInfo.dimensions);
        spec.data = mipLayerStart + mipInfo.byteSizePerLayer - hioSize;
    } else {
        spec.data = mipLayerStart;
    }

    if (!image->Read(spec)) {
        return false;
    }

    if (conversionFunction) {
        conversionFunction(spec.data, spec.width * spec.height, mipLayerStart);
    }

    return true;
}
#endif

PXR_NAMESPACE_CLOSE_SCOPE

#endif
