
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

#include "pxr/imaging/hio/image.h"
#include "pxr/imaging/hio/types.h"
#include "openexr-c.h"
#include "OpenEXRCore/internal_coding.h"

#include "pxr/usd/ar/asset.h"
#include "pxr/usd/ar/resolvedPath.h"
#include "pxr/usd/ar/resolver.h"
#include "pxr/usd/ar/writableAsset.h"

// use gf types to read and write metadata
#include "pxr/base/gf/matrix4d.h"
#include "pxr/base/gf/matrix4f.h"

#include "pxr/base/arch/defines.h"
#include "pxr/base/arch/pragmas.h"
#include "pxr/base/tf/diagnostic.h"
#include "pxr/base/tf/stringUtils.h"
#include "pxr/base/tf/type.h"

#define STB_IMAGE_RESIZE_STATIC
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "pxr/imaging/hio/stb/stb_image_resize.h"


PXR_NAMESPACE_OPEN_SCOPE

#if 0
notice below how the arwriteableasset is transfmogrified step by step into an
    std_textoutput

    // Helper class for writing out strings for the text file format.
class Sdf_TextOutput
{
public:
    explicit Sdf_TextOutput(std::ostream &out)
        : Sdf_TextOutput(std::make_shared<Sdf_StreamWritableAsset>(out)) {}

    explicit Sdf_TextOutput(std::shared_ptr<ArWritableAsset> &&asset)
        : _asset(std::move(asset)), _offset(0), _buffer(new char[BUFFER_SIZE]),
          _bufferPos(0) {}

    ~Sdf_TextOutput()
    {
        if (_asset)
        {
            Close();
        }
    }

    Sdf_TextOutput(const Sdf_TextOutput &) = delete;
    const Sdf_TextOutput &operator=(const Sdf_TextOutput &) = delete;

    // Close the output, flushing contents to destination.
    bool Close()
    {
        if (!_asset)
        {
            return true;
        }

        const bool ok = _FlushBuffer() && _asset->Close();
        _asset.reset();
        return ok;
    }

    // Write given \p str to output.
    bool Write(const std::string &str)
    {
        return _Write(str.c_str(), str.length());
    }

    // Write NUL-terminated character string \p str to output.
    bool Write(const char *str) { return _Write(str, strlen(str)); }

private:
    bool _Write(const char *str, size_t strLength)
    {
        // Much of the text format writing code writes small number of
        // characters at a time. Buffer writes to batch writes into larger
        // chunks.
        while (strLength != 0)
        {
            const size_t numAvail = BUFFER_SIZE - _bufferPos;
            const size_t numToCopy = std::min(numAvail, strLength);
            memcpy(_buffer.get() + _bufferPos, str, numToCopy);

            _bufferPos += numToCopy;
            str += numToCopy;
            strLength -= numToCopy;

            if (_bufferPos == BUFFER_SIZE)
            {
                if (!_FlushBuffer())
                {
                    return false;
                }
            }
        }

        return true;
    }

    bool _FlushBuffer()
    {
        if (_bufferPos == 0)
        {
            return true;
        }

        const size_t nWritten = _asset->Write(_buffer.get(), _bufferPos, _offset);

        if (nWritten != _bufferPos)
        {
            TF_RUNTIME_ERROR("Failed to write bytes");
            return false;
        }
        _offset += nWritten;
        _bufferPos = 0;
        return true;
    };

    std::shared_ptr<ArWritableAsset> _asset;
    size_t _offset;

    const size_t BUFFER_SIZE = 4096;
    std::unique_ptr<char[]> _buffer;
    size_t _bufferPos;
};

bool SdfTextFileFormat::WriteToFile(const SdfLayer &layer,
                                    const std::string &filePath,
                                    const std::string &comment,
                                    const FileFormatArguments &args) const
{
    std::shared_ptr<ArWritableAsset> asset = ArGetResolver().OpenAssetForWrite(
        ArResolvedPath(filePath), ArResolver::WriteMode::Replace);
    if (!asset)
    {
        TF_RUNTIME_ERROR("Unable to open %s for write", filePath.c_str());
        return false;
    }

    Sdf_TextOutput out(std::move(asset));

    const bool ok =
        _WriteLayer(&layer, out, GetFileCookie(), GetVersionString(), comment);

    if (ok && !out.Close())
    {
        TF_RUNTIME_ERROR("Could not close %s", filePath.c_str());
        return false;
    }

    return ok;
}

class OpenEXR_WritableAsset : public ArWritableAsset
{
public:
    explicit OpenEXR_WritableAsset(std::ostream &out) : _out(out) {}

    virtual ~OpenEXR_WritableAsset();

    bool Close() override
    {
        _out.flush();
        return true;
    }

    size_t Write(const void *buffer, size_t count, size_t offset) override
    {
        // The offset is ignored as we assume this object will only be
        // used for sequential writes. This is a performance optimization,
        // since calling tellp repeatedly can be expensive.
        // if (_out.tellp() != static_cast<ssize_t>(offset)) {
        //     _out.seekp(offset);
        // }

        _out.write(static_cast<const char *>(buffer), count);
        return count;
    }

private:
    std::ostream &_out;
};

#endif

class Hio_OpenEXRImage final : public HioImage
{
    std::shared_ptr<ArAsset> _asset;
    std::string              _filename;
    nanoexr_Reader_t*        _exrReader = nullptr;
    SourceColorSpace         _sourceColorSpace;
    int                      _subimage = 0;
    int                      _mip = 0;
    bool                     _suppressErrors = false;

public:
    Hio_OpenEXRImage() = default;
    ~Hio_OpenEXRImage() override;

    using Base = HioImage;
    std::string const &GetFilename() const override;
    bool      Read(StorageSpec const &storage) override;
    bool      ReadCropped(int const cropTop, int const cropBottom, int const cropLeft,
                          int const cropRight, StorageSpec const &storage) override;
    bool      Write(StorageSpec const &storage, VtDictionary const &metadata) override;
    int       GetWidth() const override;
    int       GetHeight() const override;
    HioFormat GetFormat() const override;
    int       GetBytesPerPixel() const override;
    int       GetNumMipLevels() const override;
    bool      IsColorSpaceSRGB() const override;
    bool      GetMetadata(TfToken const &key, VtValue *value) const override;
    bool      GetSamplerMetadata(HioAddressDimension dim,
                                 HioAddressMode *param) const override;

protected:
    bool _OpenForReading(std::string const &filename, int subimage, int mip,
                         SourceColorSpace sourceColorSpace,
                         bool suppressErrors) override;
    bool _OpenForWriting(std::string const &filename) override;
    
    bool _CropAndResize(void const *sourceData, int const cropTop,
                        int const cropBottom,
                        int const cropLeft,
                        int const cropRight,
                        bool const resizeNeeded,
                        StorageSpec const & storage);
};

TF_REGISTRY_FUNCTION(TfType)
{
    using Image = Hio_OpenEXRImage;
    TfType t = TfType::Define<Image, TfType::Bases<Image::Base>>();
    t.SetFactory<HioImageFactory<Image>>();
}

HioFormat HioFormatFromExrPixelType(exr_pixel_type_t pixelType, int numChannels)
{
    switch (pixelType)
    {
    case EXR_PIXEL_UINT:
        switch (numChannels)
        {
        case 1: return HioFormatInt32;
        case 2: return HioFormatInt32Vec2;
        case 3: return HioFormatInt32Vec3;
        default: return HioFormatInt32Vec4;
        }

    case EXR_PIXEL_HALF:
        switch (numChannels)
        {
        case 1: return HioFormatFloat16;
        case 2: return HioFormatFloat16Vec2;
        case 3: return HioFormatFloat16Vec3;
        default: return HioFormatFloat16Vec4;
        }

    case EXR_PIXEL_FLOAT:
        switch (numChannels)
        {
        case 1: return HioFormatFloat32;
        case 2: return HioFormatFloat32Vec2;
        case 3: return HioFormatFloat32Vec3;
        default: return HioFormatFloat32Vec4;
        }

    default:
        return HioFormatInvalid;
    }
}

Hio_OpenEXRImage::~Hio_OpenEXRImage()
{
    nanoexr_close(_exrReader);
}

/// Copies the region of the source image defined by cropTop, cropBottom,
/// cropLeft, and cropRight into storage.data.  If needed, we resize
/// the incoming data to fit the dimensions defined in storage.  _width
/// and _height are updated to match those in storage
/// Bottom is at zero, top is some positive vlaue.
bool 
Hio_OpenEXRImage::_CropAndResize(
        void const *sourceData, int const cropTop,
        int  const cropBottom,
        int  const cropLeft,
        int  const cropRight,
        bool const resizeNeeded,
        StorageSpec const & storage)
{
    if (!TF_VERIFY(cropTop > cropBottom && cropRight > cropLeft,
        "Invalid crop parameters")) {
        return false;
    }
    const int bpp = GetBytesPerPixel();
    
    const int cropWidth = cropRight - cropLeft;
    const int cropHeight = cropTop - cropBottom;
    const int croppedStrideLength = cropWidth * bpp;

    const int strideLength = cropWidth * bpp;
    
    const int channels = HioGetComponentCount(storage.format);
    
    const HioType hioType = HioGetHioType(storage.format);
    switch (hioType) {
        case HioTypeUnsignedByte:
        case HioTypeUnsignedByteSRGB:
        case HioTypeSignedByte:
        case HioTypeUnsignedShort:
        case HioTypeSignedShort:
        case HioTypeUnsignedInt:
        case HioTypeInt:
        case HioTypeDouble:
        default:
            return false;
        case HioTypeHalfFloat:
        case HioTypeFloat:
            break;
    }

    // set destination
    // if resizing is needed, then copy into temporary memory
    // otherwise, copy straight into storage.data
    void* croppedData;
    std::unique_ptr<uint8_t[]>tempData;

    if (resizeNeeded) {
        int croppedImageSize = croppedStrideLength * cropHeight; 
        tempData.reset(new uint8_t[croppedImageSize]);
        croppedData = tempData.get();
    }
    else {
        croppedData = storage.data;
    }
    
    for (int i = 0; i < cropHeight; i ++)
    {
        unsigned char *src = (unsigned char*) sourceData + 
                             ((cropTop + i) * strideLength) + 
                             (cropLeft * bpp);
        unsigned char *dest = (unsigned char *) croppedData + 
                              (i * croppedStrideLength);

        //memcpy 1 row of data
        memcpy(dest, src, croppedStrideLength);
    }

    if (resizeNeeded) {
        //resize and copy data into storage
        if (IsColorSpaceSRGB()) {
            int alphaIndex = (channels == 3)?
                                 STBIR_ALPHA_CHANNEL_NONE : 3;
            stbir_resize_uint8_srgb((unsigned char *) croppedData, 
                                    cropWidth, cropHeight, 
                                    croppedStrideLength,
                                    (unsigned char *)storage.data, 
                                    storage.width,
                                    storage.height,
                                    storage.width * bpp,
                                    channels, alphaIndex, 0);
        } 
        else {
            switch (hioType) {
                case HioTypeFloat:
                    stbir_resize_float((float *) croppedData, 
                                       cropWidth, cropHeight, 
                                       croppedStrideLength,
                                       (float *)storage.data, 
                                       storage.width,
                                       storage.height,
                                       storage.width * bpp,
                                       channels);
                    break;
#if 0
                case HioTypeHalfFloat:
                    stbir_resize_float16(uint16_t *) croppedData, 
                                        cropWidth, cropHeight,
                                        croppedStrideLength,
                                        (unsigned char *)storage.data,
                                        storage.width,
                                        storage.height,
                                        storage.width * bpp,
                                        channels);
                    break;
                case HioTypeInt:
                    stbir_resize_uint32((unsigned char*) croppedData,
                                       cropWidth, cropHeight,
                                       croppedStrideLength,
                                       (unsigned char *)storage.data,
                                       storage.width,
                                       storage.height,
                                       storage.width * bpp,
                                       channels);
                    break;
#endif
                default:
                    TF_CODING_ERROR("Unsupported output type");
                    return false;
            }
        }
    }
    return true;
}

bool Hio_OpenEXRImage::Read(StorageSpec const &storage)
{
    return ReadCropped(0, 0, 0, 0, storage);
}

#if 0
should be read image (tiled or scanline)
  read a tile, if scanline treat it as if there's just one tile
  read an image, croppped (tiled or scanline)
#endif

/*
float erf(float x) {
  // save the sign of x
  float sign = (x >= 0) ? 1 : -1;
  x = fabs(x);

  // constants
  float a1 =  0.254829592;
  float a2 = -0.284496736;
  float a3 =  1.421413741;
  float a4 = -1.453152027;
  float a5 =  1.061405429;
  float p  =  0.3275911;

  // A&S formula 7.1.26
  float t = 1.0 / (1.0 + p*x);
  float y = 1.0 - (((((a5 * t + a4) * t) + a3) * t + a2) * t + a1) * t * expf(-x * x);
  return sign * y; // erf(-x) = -erf(x);
}
*/

float IntegrateGaussian(float x, float sigma)
{
  float p1 = erf((x - 0.5f) / sigma * sqrtf(0.5f));
  float p2 = erf((x + 0.5f) / sigma * sqrtf(0.5f));
  return (p2-p1) * 0.5f;
}


// downsample
bool downsample(nanoexr_ImageData_t* src, nanoexr_ImageData_t* dst)
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
        memcpy(dst->data, src->data, src->channelCount * srcWidth * srcHeight * sizeof(float));
        return true;
    }
    
    float* srcData = reinterpret_cast<float*>(src->data);
    float* dstData = reinterpret_cast<float*>(dst->data);

    // two pass image resize

    // Create a Gaussian filter, per:
    // https://bartwronski.com/2021/10/31/practical-gaussian-filter-binomial-filter-and-small-sigma-gaussians
    // chose sigma to suppress high frequencies that can't be represented in the downsampled image
    const float ratio = (float)dstWidth / (float)srcWidth;
    const float sigma = 1.f / 2.f * ratio;
    const float support = 0.995f;
    float radius = ceilf(sqrtf(-2.0f * sigma * sigma * logf(1.0f - support)));
    int filterSize = (int)radius;
    std::vector<float> filter;
    filter.resize(1 + filterSize * 2);
    float sum = 0.0f;
    for (int i = 0; i <= filterSize; i++) {
        int idx = i + filterSize;
        filter[idx] = IntegrateGaussian((float) i, sigma);
        if (i > 0)
            sum += 2 * filter[idx];
        else
            sum = filter[idx];
    }
    for (int i = 0; i <= filterSize; ++i) {
        filter[i + filterSize] /= sum;
    }
    for (int i = 0; i < filterSize; ++i) {
        filter[filterSize - i - 1] = filter[i + filterSize + 1];
    }
    int fullFilterSize = filterSize * 2 + 1;

    // first pass: resize horizontally
    int srcFloatsPerLine = src->channelCount * srcWidth;
    int dstFloatsPerLine = src->channelCount * dstWidth;
    float* firstPass = (float*)malloc(dstWidth * src->channelCount * srcHeight * sizeof(float));
    for (int y = 0; y < srcHeight; ++y) {
        for (int x = 0; x < dstWidth; ++x) {
            for (int c = 0; c < src->channelCount; ++c) {
                float sum = 0.0f;
                for (int i = 0; i < fullFilterSize; ++i) {
                    int srcX = (int)((x + 0.5f) / ratio - 0.5f) + i - filterSize;
                    if (srcX < 0 || srcX >= srcWidth)
                        continue;
                    int idx = y * srcFloatsPerLine + (srcX * src->channelCount) + c;
                    sum += srcData[idx] * filter[i];
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
                for (int i = 0; i < fullFilterSize; ++i) {
                    int srcY = (int)((y + 0.5f) / ratio - 0.5f) + i - filterSize;
                    if (srcY < 0 || srcY >= srcHeight)
                        continue;
                    int idx = src->channelCount * srcY * dstWidth + (x * src->channelCount) + c;
                    sum += firstPass[idx] * filter[i];
                }
                secondPass[src->channelCount * y * dstWidth + (x * src->channelCount) + c] = sum;
            }
        }
    }
    free(firstPass);
    return true;
}



bool Hio_OpenEXRImage::ReadCropped(
                int const cropTop,  int const cropBottom,
                int const cropLeft, int const cropRight, 
                StorageSpec const &storage)
{
    if (!_exrReader)
        return false;

    // cache values and
    // determine what needs to happen as a consequence of the input parameters
    int fileWidth = _exrReader->width;
    int fileHeight = _exrReader->height;
    int fileChannelCount = _exrReader->channelCount;
    exr_pixel_type_t filePixelType = _exrReader->pixelType;
    bool cropping = cropTop || cropBottom || cropLeft || cropRight;
    int outWidth = storage.width - cropLeft - cropRight;
    int outHeight = storage.height - cropTop - cropBottom;
    bool resizing = (fileWidth != outWidth) || (fileHeight != outHeight);
    int outChannelCount = HioGetComponentCount(storage.format);
    
    bool inputIsHalf = filePixelType == EXR_PIXEL_HALF;
    bool inputIsFloat = filePixelType == EXR_PIXEL_FLOAT;
    bool outputIsFloat = HioGetHioType(storage.format) == HioTypeFloat;
    bool outputIsHalf = HioGetHioType(storage.format) == HioTypeHalfFloat;

    std::vector<uint16_t> halfInputBuffer;
    if (inputIsHalf) {
        halfInputBuffer.resize(fileWidth * fileHeight * fileChannelCount);
    }
    std::vector<float> floatInputBuffer;
    if (inputIsHalf && (resizing || outputIsFloat)) {
        floatInputBuffer.resize(fileWidth * fileHeight * fileChannelCount);
    }
    
    if (cropping) {
        if (nanoexr_isTiled(_exrReader)) {
            // ... @TODO read tiled cropped
        }
        else {
            // read only relevant scan line data
            // ... @TODO
        }
    }
    else {
        if (nanoexr_isTiled(_exrReader)) {
            // ... @TODO read tiled, uncropped
        }
        else {
            nanoexr_ImageData_t img;
            
            if (inputIsHalf)
                img.data = reinterpret_cast<uint8_t*>(&halfInputBuffer[0]);
            else
                img.data = reinterpret_cast<uint8_t*>(&floatInputBuffer[0]);
            
            img.channelCount = outChannelCount;
            img.dataSize = fileWidth * fileHeight * GetBytesPerPixel();
            img.pixelType = filePixelType;
            exr_result_t rv = nanoexr_readScanlineData(_exrReader, &img);
            if (rv != EXR_ERR_SUCCESS) {
                return false;
            }
        }
    }

    if (!resizing) {
        if (inputIsHalf && outputIsHalf) {
            memcpy(reinterpret_cast<void*>(storage.data),
                   &halfInputBuffer[0], sizeof(uint16_t) * halfInputBuffer.size());
        }
        else if (inputIsFloat && outputIsFloat) {
            memcpy(reinterpret_cast<void*>(storage.data),
                   &floatInputBuffer[0], sizeof(float) * floatInputBuffer.size());
        }
        else if (outputIsFloat) {
            for (int i = 0; i < halfInputBuffer.size(); ++i)
                reinterpret_cast<float*>(storage.data)[i] = half_to_float(halfInputBuffer[i]);
        }
        else {
            for (int i = 0; i < floatInputBuffer.size(); ++i)
                reinterpret_cast<uint16_t*>(storage.data)[i] = float_to_half(floatInputBuffer[i]);
        }
        return true;
    }
    
    // resizing
    if (inputIsHalf) {
        floatInputBuffer.resize(fileWidth * fileHeight * fileChannelCount);
        for (int i = 0; i < floatInputBuffer.size(); ++i)
            floatInputBuffer[i] = half_to_float(halfInputBuffer[i]);
    }

    std::vector<float> resizeOutputBuffer;

    nanoexr_ImageData_t src;
    src.data = reinterpret_cast<uint8_t*>(&floatInputBuffer[0]);
    src.channelCount = fileChannelCount;
    src.dataSize = fileWidth * fileHeight * GetBytesPerPixel();
    src.pixelType = EXR_PIXEL_FLOAT;
    src.width = fileWidth;
    src.height = fileHeight;

    nanoexr_ImageData_t dst;
    dst.channelCount = outChannelCount;
    dst.dataSize = outWidth * outHeight * outChannelCount * sizeof(float);
    dst.pixelType = EXR_PIXEL_FLOAT;
    dst.width = outWidth;
    dst.height = outHeight;

    if (outputIsFloat) {
        dst.data = reinterpret_cast<uint8_t*>(storage.data);
    }
    else {
        resizeOutputBuffer.resize(outWidth * outHeight * outChannelCount);
        dst.data = reinterpret_cast<uint8_t*>(&resizeOutputBuffer[0]);
    }
    downsample(&src, &dst);
    
    if (outputIsHalf) {
        for (int i = 0; i < resizeOutputBuffer.size(); ++i)
            reinterpret_cast<uint16_t*>(storage.data)[i] =
            float_to_half(resizeOutputBuffer[i]);
    }
    return true;
}


bool HioConvertFloatToFormat(HioFormat format, void *dst, const float *src, size_t numPixels)
{
    int c = HioGetComponentCount(format);
    switch (HioGetHioType(format)) {
    case HioTypeFloat:
        for (size_t i = 0; i < numPixels * c; ++i)
            ((float *)dst)[i] = src[i];
        return true;
    case HioTypeHalfFloat:
        for (size_t i = 0; i < numPixels * c; ++i)
            ((GfHalf *)dst)[i] = src[i];
        return true;

    default:
        return false;
    }
}

bool Hio_OpenEXRImage::Write(StorageSpec const &storage, VtDictionary const &metadata)
{
    return false;
}

std::string const& Hio_OpenEXRImage::GetFilename() const
{
    return _filename;
}

int Hio_OpenEXRImage::GetWidth() const
{
    if (!_exrReader)
        return 0;
    
    return _exrReader->width;
}
int Hio_OpenEXRImage::GetHeight() const
{
    if (!_exrReader)
        return 0;
    
    return _exrReader->height;
}

HioFormat Hio_OpenEXRImage::GetFormat() const
{
    if (!_exrReader)
        return HioFormatInvalid;

    return HioFormatFromExrPixelType(nanoexr_getPixelType(_exrReader), _exrReader->channelCount);
}

int Hio_OpenEXRImage::GetBytesPerPixel() const
{
    if (!_exrReader)
        return 0;

    switch (nanoexr_getPixelType(_exrReader))
    {
    case EXR_PIXEL_UINT:
        return _exrReader->channelCount * 4;
    case EXR_PIXEL_HALF:
        return _exrReader->channelCount * 2;
    case EXR_PIXEL_FLOAT:
        return _exrReader->channelCount * 4;
    default:
        return 0;
    }
}

int Hio_OpenEXRImage::GetNumMipLevels() const
{
    if (!_exrReader)
        return 0;
    return _exrReader->mipLevels.level;
}

bool Hio_OpenEXRImage::IsColorSpaceSRGB() const
{
    // the code below tests for an SRGB color space, but
    // what Hydra really wants to know is if it's linear or not.
    // OpenEXR images are always linear, so we can just return false.
    return false;
       /*
    exr_attr_chromaticities_t chr {};
    if (EXR_ERR_SUCCESS != exr_attr_get_chromaticities(
                                _exrReader->f,
                                _exrReader->partIndex,
                               "chromaticities", &chr)) {
        return true; // actually REC709, but the chromaticities are the same
    }

    auto approx = [](float a, float b) -> bool {
        return (std::abs(a-b) < 0.01f);
    };
    
    // test chromaticities against Rec709.
    return approx(chr.red_x, 0.64f) && approx(chr.red_y, 0.33f) &&
           approx(chr.green_x, 0.30f) && approx(chr.green_y, 0.60f) &&
           approx(chr.blue_x, 0.15f) && approx(chr.blue_y, 0.06f) &&
           approx(chr.white_x, 0.3127f) && approx(chr.white_y, 0.3290f);
*/
}

bool Hio_OpenEXRImage::GetMetadata(TfToken const &key, VtValue *value) const
{
    
}

bool Hio_OpenEXRImage::GetSamplerMetadata(HioAddressDimension dim,
                        HioAddressMode *param) const
{
    const exr_attribute_t* attr;
    if (EXR_ERR_SUCCESS != exr_get_attribute_by_name(
                                _exrReader->f,
                                _exrReader->partIndex,
                                "wrapmodes", &attr)) {
        return HioAddressModeClampToEdge;
    }
    
    if (attr->type != EXR_ATTR_STRING) {
        return HioAddressModeClampToEdge;
    }
    
    std::string wrapmodes(attr->string->str, attr->string->length);
    if (wrapmodes.find("black") != std::string::npos)
        return HioAddressModeClampToBorderColor;    // hydra only has HdWrapBlack
    if (wrapmodes.find("clamp") != std::string::npos)
        return HioAddressModeClampToEdge;
    if (wrapmodes.find("periodic") != std::string::npos)
        return HioAddressModeRepeat;
    if (wrapmodes.find("mirror") != std::string::npos)
        return HioAddressModeMirrorRepeat;
    // Hio has no "no opinion" enum, match the OIIO plugin.
    return HioAddressModeClampToEdge;
}

uint64_t exr_AssetRead_Func(
    exr_const_context_t         ctxt,
    void*                       userdata,
    void*                       buffer,
    uint64_t                    sz,
    uint64_t                    offset,
    exr_stream_error_func_ptr_t error_cb)
{
    ArAsset* asset = (ArAsset*)userdata;
    if (!asset || !buffer || !sz) {
        if (error_cb)
            error_cb(ctxt, EXR_ERR_INVALID_ARGUMENT,
                           "%s", "Invalid arguments to read callback");
        return 0;
    }
    return asset->Read(buffer, sz, offset);
}

// set inits.read_fn to exr_AssetRead_Func
// called by HioImage::OpenForReading
//
// subimage will be interpreted as a "part" in OpenEXR parlance.
bool Hio_OpenEXRImage::_OpenForReading(std::string const &filename,
                                       int subimage, int mip,
                                       SourceColorSpace sourceColorSpace,
                                       bool suppressErrors)
{
    _asset = ArGetResolver().OpenAsset(ArResolvedPath(filename));
    if (!_asset) {
        return false;
    }

    exr_context_initializer_t exrInit = EXR_DEFAULT_CONTEXT_INITIALIZER;
    exrInit.read_fn = exr_AssetRead_Func;
    exrInit.user_data = (void*) _asset.get(); // Hio_OpenEXRImage will outlast the reader.
    _exrReader = nanoexr_new(filename.c_str(), &exrInit);

    int rv = nanoexr_open(_exrReader, 0);
    if (rv != 0) {
        TF_CODING_ERROR("Cannot open image \"%s\" for reading, %s",
                        filename.c_str(), exr_get_error_code_as_string(rv));
        return false;
    }
    
    _filename = filename;
    _subimage = subimage;
    _mip = mip;
    _sourceColorSpace = sourceColorSpace;
    _suppressErrors = suppressErrors;
    return true;
}

bool Hio_OpenEXRImage::_OpenForWriting(std::string const &filename)
{
    
}


PXR_NAMESPACE_CLOSE_SCOPE
