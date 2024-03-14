//
// Copyright 2023 Pixar
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

#include "pxr/base/arch/pragmas.h"

// Not all functions in the OpenEXR library are used by Hio, and the OpenEXR
// symbols themselves are declared static for inclusion within Hio.
// Therefore, the unused function warning is suppressed as the messages are
// not useful for development, as it is expected that many functions are
// defined but not referenced or exported.
ARCH_PRAGMA_UNUSED_FUNCTION

#include "pxr/imaging/hio/image.h"
#include "pxr/imaging/hio/types.h"

#include "pxr/usd/ar/asset.h"
#include "pxr/usd/ar/resolvedPath.h"
#include "pxr/usd/ar/resolver.h"
#include "pxr/usd/ar/writableAsset.h"

#include "pxr/base/gf/matrix3d.h"
#include "pxr/base/gf/matrix3f.h"
#include "pxr/base/gf/matrix4d.h"
#include "pxr/base/gf/matrix4f.h"
#include "pxr/base/gf/range2d.h"
#include "pxr/base/gf/vec2f.h"

#include "pxr/base/arch/defines.h"
#include "pxr/base/arch/pragmas.h"
#include "pxr/base/tf/diagnostic.h"
#include "pxr/base/tf/stringUtils.h"
#include "pxr/base/tf/type.h"

#include "AVIF/src/avif/avif.h"
#include "OpenEXR/openexr-c.h"

PXR_NAMESPACE_OPEN_SCOPE

class Hio_AVIFImage final : public HioImage
{
    std::shared_ptr<ArAsset> _asset;
    std::string              _filename;
    SourceColorSpace         _sourceColorSpace = SourceColorSpace::Raw;
    int                      _subimage = 0;
    int                      _mip = 0;
    
    // mutable because GetMetadata is const, yet it doesn't make sense
    // to cache the dictionary unless metadata is requested.
    mutable VtDictionary     _metadata;

    // The callback dictionary is a pointer to the dictionary that was
    // passed to the Write method.  It is valid only during the
    // Write call.
    VtDictionary const*      _callbackDict = nullptr;

public:
    Hio_AVIFImage() = default;
    ~Hio_AVIFImage() override {
    }
    
    const std::shared_ptr<ArAsset> Asset() const { return _asset; }

    using Base = HioImage;
    bool Read(StorageSpec const &storage) override {
        return ReadCropped(0, 0, 0, 0, storage); 
    }
    bool ReadCropped(int const cropTop,  int const cropBottom,
                     int const cropLeft, int const cropRight,
                     StorageSpec const &storage) override;
    bool Write(StorageSpec const &storage,
               VtDictionary const &metadata) override;

    // IsColorSpaceSRGB asks if the color space is SRGB, but
    // what Hydra really wants to know is whether the pixels are gamma pixels.
    // OpenEXR images are always linear, so just return false.
    bool IsColorSpaceSRGB() const override { return false; }
    HioFormat GetFormat() const override;
    int  GetWidth() const override { return 0; }
    int  GetHeight() const override { return 0; }
    int  GetBytesPerPixel() const override;
    int  GetNumMipLevels() const override;
    bool GetMetadata(TfToken const &key, VtValue *value) const override;
    bool GetSamplerMetadata(HioAddressDimension dim,
                            HioAddressMode *param) const override;
    std::string const &GetFilename() const override { return _filename; }

    const VtDictionary &GetMetadata() const { return _metadata; }

protected:
    bool _OpenForReading(std::string const &filename, int subimage, int mip,
                         SourceColorSpace sourceColorSpace,
                         bool suppressErrors) override;
    bool _OpenForWriting(std::string const &filename) override;
};

TF_REGISTRY_FUNCTION(TfType)
{
    using Image = Hio_AVIFImage;
    TfType t = TfType::Define<Image, TfType::Bases<Image::Base>>();
    t.SetFactory<HioImageFactory<Image>>();
}

HioFormat Hio_AVIFImage::GetFormat() const
{
    return HioFormatInvalid;
}

int Hio_AVIFImage::GetBytesPerPixel() const
{
    return 0;
}

int Hio_AVIFImage::GetNumMipLevels() const
{
    return 0;
}

namespace {
    
    template<typename T>
    class ImageProcessor
    {
    public:
        // Flip the image in-place.
        static void FlipImage(T* buffer, int width, int height, int channelCount)
        {
            // use std::swap_ranges to flip the image in-place
            for (int y = 0; y < height / 2; ++y) {
                std::swap_ranges(
                                 buffer + y * width * channelCount,
                                 buffer + (y + 1) * width * channelCount,
                                 buffer + (height - y - 1) * width * channelCount);
            }
        }
        
        // Crop the image in-place.
        static void CropImage(T* buffer, 
                              int width, int height, int channelCount,
                              int cropTop, int cropBottom,
                              int cropLeft, int cropRight)
        {
            int newWidth = width - cropLeft - cropRight;
            int newHeight = height - cropTop - cropBottom;
            
            if (newWidth <= 0 || newHeight <= 0
                || (newWidth == width && newHeight == height))
                return;
            
            for (int y = 0; y < newHeight; ++y) {
                for (int x = 0; x < newWidth; ++x) {
                    for (int c = 0; c < channelCount; ++c) {
                        buffer[(y * newWidth + x) * channelCount + c] =
                        buffer[((y + cropTop) * width + x + cropLeft)
                               * channelCount + c];
                    }
                }
            }
        }
        
        static void HalfToFloat(GfHalf* buffer, float* outBuffer,
                                int width, int height, int channelCount)
        {
            if (!buffer || !outBuffer)
                return;
            
            for (int i = 0; i < width * height * channelCount; ++i) {
                outBuffer[i] = buffer[i];
            }
        }
        
        static void FloatToHalf(float* buffer, GfHalf* outBuffer,
                                int width, int height, int channelCount)
        {
            if (!buffer || !outBuffer)
                return;
            
            for (int i = 0; i < width * height * channelCount; ++i) {
                outBuffer[i] = buffer[i];
            }
        }

        // return true for a successful resample
        static bool ResizeImage(const float* src, float* dst,
                                int width, int height, int channelCount)
        {
            nanoexr_ImageData_t srcImg = {
                (uint8_t*) src,
                channelCount * sizeof(float) * width * height,
                EXR_PIXEL_FLOAT,
                channelCount, width, height, 0, height - 1
            };
            nanoexr_ImageData_t dstImg = srcImg;
            dstImg.data = (uint8_t*) dst;
            return nanoexr_Gaussian_resample(&srcImg, &dstImg);
        }
    };
   
} // anon

bool Hio_AVIFImage::ReadCropped(
                int const cropTop,  int const cropBottom,
                int const cropLeft, int const cropRight, 
                StorageSpec const& storage)
{
    nanoexr_ImageData_t img;
    memset(&img, 0, sizeof(img));

    size_t sz = _asset->GetSize();
    uint8_t* data = (uint8_t*) malloc(sz);
    size_t offset = 0;
    size_t readSize = _asset->Read(data, sz, offset);
    if (!readSize)
        return false;

    // Initialize libavif
    avifImage *image = avifImageCreateEmpty();
    avifDecoder *decoder = avifDecoderCreate();
    avifResult result = avifDecoderReadMemory(decoder, image, data, readSize);
    if (result != AVIF_RESULT_OK) {
        printf("Error parsing AVIF file: %s\n", avifResultToString(result));
        avifDecoderDestroy(decoder);
        avifImageDestroy(image);
        return false;
    }

    // Convert to sRGB
    avifRGBImage rgb;
    const int bytesPerPixel = 2;
    const int channelCount = 4;
    memset(&rgb, 0, sizeof(rgb));
    avifRGBImageSetDefaults(&rgb, image);
    rgb.format = AVIF_RGB_FORMAT_RGBA; // Choose desired format (RGBA in this case)
    rgb.rowBytes = rgb.width * channelCount * bytesPerPixel;
    rgb.pixels = (uint8_t*) calloc(channelCount * bytesPerPixel * rgb.width * rgb.height, 1);
    rgb.depth = 8 * bytesPerPixel;
    rgb.isFloat = true;
    result = avifImageYUVToRGB(image, &rgb);
    if (result != AVIF_RESULT_OK) {
        printf("Error parsing AVIF file: %s\n", avifResultToString(result));
        avifDecoderDestroy(decoder);
        avifImageDestroy(image);
        return false;
    }

    printf("width: %d, height:%d\n", rgb.width, rgb.height);
    // rgba8 pixels are at rgb.pixels
    img.data = (uint8_t*) rgb.pixels;
    img.dataSize = channelCount * bytesPerPixel * rgb.width * rgb.height;
    if (bytesPerPixel == 1)
        img.pixelType = (exr_pixel_type_t) 3;  // A sentinel to indidate an 8 bit texture
    else
        img.pixelType = EXR_PIXEL_HALF;
    img.channelCount = channelCount;
    img.width = rgb.width;
    img.height = rgb.height;
    img.dataWindowMinY = 0;
    img.dataWindowMaxY = rgb.height - 1;

    // @TODO crop and resize and convert to desired storage spec
    
    memcpy(storage.data, img.data, img.dataSize);
    
    avifDecoderDestroy(decoder);
    avifImageDestroy(image);
    return true;
}


bool Hio_AVIFImage::GetMetadata(TfToken const &key, VtValue *value) const
{
    return false;
}

bool Hio_AVIFImage::GetSamplerMetadata(HioAddressDimension dim,
                                          HioAddressMode *param) const
{
    return false;
}

bool Hio_AVIFImage::_OpenForReading(std::string const &filename,
                                       int subimage, int mip,
                                       SourceColorSpace sourceColorSpace,
                                       bool /*suppressErrors*/)
{
    _filename = filename;
    _asset = ArGetResolver().OpenAsset(ArResolvedPath(filename));
    if (!_asset) {
        return false;
    }

    return false;
}

bool Hio_AVIFImage::Write(StorageSpec const &storage,
                             VtDictionary const &metadata)
{
    return false;
}

bool Hio_AVIFImage::_OpenForWriting(std::string const &filename)
{
    return false;
}

PXR_NAMESPACE_CLOSE_SCOPE
