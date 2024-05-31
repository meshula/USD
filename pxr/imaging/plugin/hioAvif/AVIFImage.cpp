//
// Copyright 2024 Pixar
//
// Licensed under the terms set forth in the LICENSE.txt file available at
// https://openusd.org/license.
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


    static float integrate_gaussian(float x, float sigma)
    {
        float p1 = erf((x - 0.5f) / sigma * sqrtf(0.5f));
        float p2 = erf((x + 0.5f) / sigma * sqrtf(0.5f));
        return (p2-p1) * 0.5f;
    }

    // Enum capturing the underlying data type on a channel.
    typedef enum
    {
        EXR_PIXEL_UINT  = 0,
        EXR_PIXEL_HALF  = 1,
        EXR_PIXEL_FLOAT = 2,
        EXR_PIXEL_LAST_TYPE
    } exr_pixel_type_t;

    // structure to hold image data that is read from an EXR file
    typedef struct {
        uint8_t* data;
        size_t dataSize;
        exr_pixel_type_t pixelType;
        int channelCount; // 1 for luminance, 3 for RGB, 4 for RGBA
        int width, height;
        int dataWindowMinY, dataWindowMaxY;
    } nanoexr_ImageData_t;

    bool nanoexr_Gaussian_resample(const nanoexr_ImageData_t* src,
                                nanoexr_ImageData_t* dst)
    {
        if (src->pixelType != EXR_PIXEL_FLOAT && dst->pixelType != EXR_PIXEL_FLOAT)
            return false;
        if (src->channelCount != dst->channelCount)
            return false;
        
        const int srcWidth  = src->width;
        const int dstWidth  = dst->width;
        const int srcHeight = src->height;
        const int dstHeight = dst->height;
        if (srcWidth == dstWidth && srcHeight == dstHeight) {
            memcpy(dst->data, src->data, 
                src->channelCount * srcWidth * srcHeight * sizeof(float));
            return true;
        }
        
        float* srcData = (float*)src->data;
        float* dstData = (float*)dst->data;

        // two pass image resize using a Gaussian filter per:
        // https://bartwronski.com/2021/10/31/practical-gaussian-filter-binomial-filter-and-small-sigma-gaussians
        // chose sigma to suppress high frequencies that can't be represented 
        // in the downsampled image
        const float ratio_w = (float)dstWidth / (float)srcWidth;
        const float ratio_h = (float)dstHeight / (float)srcHeight;
        const float sigma_w = 1.f / 2.f * ratio_w;
        const float sigma_h = 1.f / 2.f * ratio_h;
        const float support = 0.995f;
        float radius = ceilf(sqrtf(-2.0f * sigma_w * sigma_w * logf(1.0f - support)));
        int filterSize_w = (int)radius;
        if (!filterSize_w)
            return false;
        
        float* filter_w = (float*) malloc(sizeof(float) * (filterSize_w + 1) * 2);
        float sum = 0.0f;
        for (int i = 0; i <= filterSize_w; i++) {
            int idx = i + filterSize_w;
            filter_w[idx] = integrate_gaussian((float) i, sigma_w);
            if (i > 0)
                sum += 2 * filter_w[idx];
            else
                sum = filter_w[idx];
        }
        for (int i = 0; i <= filterSize_w; ++i) {
            filter_w[i + filterSize_w] /= sum;
        }
        for (int i = 0; i < filterSize_w; ++i) {
            filter_w[filterSize_w - i - 1] = filter_w[i + filterSize_w + 1];
        }
        int fullFilterSize_w = filterSize_w * 2 + 1;

        // again for height
        radius = ceilf(sqrtf(-2.0f * sigma_h * sigma_h * logf(1.0f - support)));
        int filterSize_h = (int)radius;
        if (!filterSize_h)
            return false;
        
        float* filter_h = (float*) malloc(sizeof(float) * (1 + filterSize_h) * 2);
        sum = 0.0f;
        for (int i = 0; i <= filterSize_h; i++) {
            int idx = i + filterSize_h;
            filter_h[idx] = integrate_gaussian((float) i, sigma_h);
            if (i > 0)
                sum += 2 * filter_h[idx];
            else
                sum = filter_h[idx];
        }
        for (int i = 0; i <= filterSize_h; ++i) {
            filter_h[i + filterSize_h] /= sum;
        }
        for (int i = 0; i < filterSize_h; ++i) {
            filter_h[filterSize_h - i - 1] = filter_h[i + filterSize_h + 1];
        }
        int fullFilterSize_h = filterSize_h * 2 + 1;
        
        // first pass: resize horizontally
        int srcFloatsPerLine = src->channelCount * srcWidth;
        int dstFloatsPerLine = src->channelCount * dstWidth;
        float* firstPass = (float*)malloc(dstWidth * src->channelCount * srcHeight * sizeof(float));
        for (int y = 0; y < srcHeight; ++y) {
            for (int x = 0; x < dstWidth; ++x) {
                for (int c = 0; c < src->channelCount; ++c) {
                    float sum = 0.0f;
                    for (int i = 0; i < fullFilterSize_w; ++i) {
                        int srcX = (int)((x + 0.5f) / ratio_w - 0.5f) + i - filterSize_w;
                        if (srcX < 0 || srcX >= srcWidth)
                            continue;
                        int idx = y * srcFloatsPerLine + (srcX * src->channelCount) + c;
                        sum += srcData[idx] * filter_w[i];
                    }
                    firstPass[y * dstFloatsPerLine + (x * src->channelCount) + c] = sum;
                }
            }
        }

        // second pass: resize vertically
        float* secondPass = dstData;
        for (int y = 0; y < dstHeight; ++y) {
            for (int x = 0; x < dstWidth; ++x) {
                for (int c = 0; c < src->channelCount; ++c) {
                    float sum = 0.0f;
                    for (int i = 0; i < fullFilterSize_h; ++i) {
                        int srcY = (int)((y + 0.5f) / ratio_h - 0.5f) + i - filterSize_h;
                        if (srcY < 0 || srcY >= srcHeight)
                            continue;
                        int idx = src->channelCount * srcY * dstWidth + (x * src->channelCount) + c;
                        sum += firstPass[idx] * filter_h[i];
                    }
                    secondPass[dst->channelCount * y * dstWidth + (x * dst->channelCount) + c] = sum;
                }
            }
        }
        free(filter_h);
        free(filter_w);
        free(firstPass);
        return true;
    }

    
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
