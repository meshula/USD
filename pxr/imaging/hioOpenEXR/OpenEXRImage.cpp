
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

/*

Open Questions:
- Hydra may ask for downsampled images (constructing mips)
- Hydra won't ask for upsampled images (I supplied a simple Gaussian upsample anyway)
- What role does cropping play? (maybe there's a crop node in shadegraphs?)
- how are int32 textures used? Are they in named layers? (id pixels, single layer exr files only)
- Hydra will never ask Hio for float to int8 tonemapping, right? (never)
- Hydra will never ask Hio for exr files expecting gamma pixels back? (because exr are linear only) (never)
- how are layers piped from MaterialX to Hio? (Seems to throw away layer info?)
- where are the extended tests for Hio? (I think there are some tests I can explicitly run)
- is there test data to check the permutations of Hydra with MaterialX, UDIMs, and MatX with UDIMs? (I think there are sample files somewhere)
- 4:2:0 chroma support needed? How about YUV? (No to both)
    
 Remaining:

 - inter miniz
 - hook it to stb_image/png  https://github.com/nothings/stb/issues/113
 - switch OpenEXR to using miniz
 - cropped reading
 - populate attributes dictionary
 - test writing via UsdRecord
 - move HioOpenEXR directly into Hio
 - final code clean up and checkin
 - start submitting patches to OpenEXR project (doesn't gate the task)

 Maybe:
 - should be able to do "stapled AOVs" from usd record unless usd record is too inflexible (USD-7846)
 - can't find extant usages of tiled images, is it a requirement?
 - implement concurrent reads since EXR allows it (is there a demonstrable performance benefit?)

 Future:
 - ArWriteableAsset
 - SdfFileFormat plugin
 - Image Plane implementation that can leverage the SdfFileFormat plugin
 
 */



PXR_NAMESPACE_OPEN_SCOPE

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

bool Hio_OpenEXRImage::Read(StorageSpec const &storage)
{
    return ReadCropped(27777, 0, 0, 0, storage);
}

#if 0
should be read image (tiled or scanline)
  read a tile, if scanline treat it as if there's just one tile
  read an image, croppped (tiled or scanline)
#endif


bool Hio_OpenEXRImage::ReadCropped(
                int const cropTop,  int const cropBottom,
                int const cropLeft, int const cropRight, 
                StorageSpec const &storage)
{
    if (!_exrReader)
        return false;

    if (cropTop < 0 || cropBottom < 0 || cropLeft < 0 || cropRight < 0)
        return false;

    // cache values and
    // determine what needs to happen as a consequence of the input parameters

    int fileWidth =  _exrReader->width;
    int fileHeight = _exrReader->height;
    int fileChannelCount = _exrReader->channelCount;
    exr_pixel_type_t filePixelType = _exrReader->pixelType;

    int outWidth =  storage.width;
    int outHeight = storage.height;
    int outChannelCount = HioGetComponentCount(storage.format);

    bool inputIsHalf =   filePixelType == EXR_PIXEL_HALF;
    bool inputIsFloat =  filePixelType == EXR_PIXEL_FLOAT;
    bool outputIsFloat = HioGetHioType(storage.format) == HioTypeFloat;
    bool outputIsHalf =  HioGetHioType(storage.format) == HioTypeHalfFloat;

    bool cropping = cropTop || cropBottom || cropLeft || cropRight;
    bool resizing = (fileWidth != outWidth) || (fileHeight != outHeight);

    int readHeight = fileHeight - cropTop - cropBottom;
    if (readHeight < 0) {
        memset(storage.data, 0, GetWidth() * GetHeight() * GetBytesPerPixel());
        return true;
    }

    std::vector<uint16_t> halfInputBuffer;
    if (inputIsHalf) {
        halfInputBuffer.resize(fileWidth * readHeight * fileChannelCount);
    }
    std::vector<float> floatInputBuffer;
    if (inputIsHalf && (resizing || outputIsFloat)) {
        floatInputBuffer.resize(fileWidth * readHeight * fileChannelCount);
    }
    
    if (cropping) {
        if (nanoexr_isTiled(_exrReader)) {
            // ... @TODO read tiled cropped
            return false;
        }
        else {
            // read only requested scan line data
            nanoexr_ImageData_t img;
            
            if (inputIsHalf)
                img.data = reinterpret_cast<uint8_t*>(&halfInputBuffer[0]);
            else
                img.data = reinterpret_cast<uint8_t*>(&floatInputBuffer[0]);
            
            img.channelCount = outChannelCount;
            img.dataSize = fileWidth * readHeight * GetBytesPerPixel();
            img.width = fileWidth;
            img.height = readHeight;
            img.pixelType = filePixelType;
            exr_result_t rv = nanoexr_readScanlineData(_exrReader, &img, nullptr, cropTop);
            if (rv != EXR_ERR_SUCCESS) {
                return false;
            }
        }
    }
    else {
        if (nanoexr_isTiled(_exrReader)) {
            // ... @TODO read tiled, uncropped
            return false;
        }
        else {
            nanoexr_ImageData_t img;
            
            if (inputIsHalf)
                img.data = reinterpret_cast<uint8_t*>(&halfInputBuffer[0]);
            else
                img.data = reinterpret_cast<uint8_t*>(&floatInputBuffer[0]);
            
            img.channelCount = outChannelCount;
            img.dataSize = fileWidth * fileHeight * GetBytesPerPixel();
            img.width = fileWidth;
            img.height = fileHeight;
            img.pixelType = filePixelType;
            exr_result_t rv = nanoexr_readScanlineData(_exrReader, &img, nullptr, 0);
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
    nanoexr_Gaussian_resample(&src, &dst);
    
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
                                _exrReader->exr,
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
