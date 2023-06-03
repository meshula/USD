
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

#include "OpenEXR/OpenEXRCoreNamespaces.h"
#include "OpenEXR/openexr-c.h"
#include "OpenEXR/OpenEXRCore/internal_coding.h"

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


/*

Open Questions:
- Hydra may ask for downsampled images (constructing mips)
- Hydra won't ask for upsampled images (I supplied a simple Gaussian upsample anyway)
- What role does cropping play? (maybe there's a crop node in shadegraphs?)

Answered Questions:

- 4:2:0 chroma support needed? How about YUV? (No to both)
- Hydra will never ask Hio for float to int8 tonemapping, right? (never)
- Hydra will never ask Hio for exr files expecting gamma pixels back? (because exr are linear only) (never)
- how are int32 textures used? Are they in named layers? (id pixels, single layer exr files only)
- where are the extended tests for Hio? (not explicitly tested)
- is there test data to check the permutations of Hydra with MaterialX, UDIMs, and MatX with UDIMs? (no)
- how are layers piped from MaterialX to Hio? (Hio doesn't know about layers)

 Remaining:

 - cropped reading
 - populate attributes dictionary
 - test writing via UsdRecord
 - final code clean up and checkin
 - test int32 textures

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

#ifdef OPENEXR_USE_NAMESPACES
using namespace OPENEXR_INTERNAL_NS;
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
        case 3: //return HioFormatInt32Vec3;
        default: return HioFormatInt32Vec4;  // 3 will be promoted to 4 during the read
        }

    case EXR_PIXEL_HALF:
        switch (numChannels)
        {
        case 1: return HioFormatFloat16;
        case 2: return HioFormatFloat16Vec2;
        case 3: //return HioFormatFloat16Vec3;
        default: return HioFormatFloat16Vec4;  // 3 will be promoted to 4 during the read
        }

    case EXR_PIXEL_FLOAT:
        switch (numChannels)
        {
        case 1: return HioFormatFloat32;
        case 2: return HioFormatFloat32Vec2;
        case 3: //return HioFormatFloat32Vec3;
        default: return HioFormatFloat32Vec4;  // 3 will be promoted to 4 during the read
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
    return ReadCropped(0, 0, 0, 0, storage);
}

//should be read image (tiled or scanline)
//  read a tile, if scanline treat it as if there's just one tile
//  read an image, croppped (tiled or scanline)


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
    int readWidth = fileWidth - cropLeft - cropRight;
    if (readWidth < 0) {
        memset(storage.data, 0, GetWidth() * GetHeight() * GetBytesPerPixel());
        return true;
    }

    // ensure there's enough memory for the greater of input and output channel count, for
    // in place conversions.
    int maxChannelCount = std::max(fileChannelCount, outChannelCount);
    std::vector<uint16_t> halfInputBuffer;
    if (inputIsHalf) {
        halfInputBuffer.resize(fileWidth * readHeight * maxChannelCount);
    }
    std::vector<float> floatInputBuffer;
    if (inputIsHalf && (resizing || outputIsFloat)) {
        floatInputBuffer.resize(fileWidth * readHeight * maxChannelCount);
    }

    if (nanoexr_isTiled(_exrReader)) {        
        nanoexr_ImageData_t img;
        img.channelCount = outChannelCount;
        exr_result_t rv = nanoexr_read_tiled_exr(_exrReader->filename, &img, nullptr, 0, 0);
        if (rv != EXR_ERR_SUCCESS) {
            return false;
        }

        memcpy(&halfInputBuffer[0], img.data, img.dataSize);
    }
    else {
        // read requested scan line data
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
        exr_result_t rv = nanoexr_readScanlineData(_exrReader, &img,
                                                   nullptr, cropTop);
        if (rv != EXR_ERR_SUCCESS) {
            return false;
        }
    }

    // flip the image
    if (inputIsHalf) {
        for (int y = 0; y < readHeight / 2; ++y) {
            for (int x = 0; x < fileWidth; ++x) {
                for (int c = 0; c < outChannelCount; ++c) {
                    std::swap(halfInputBuffer[(y * fileWidth + x) * outChannelCount + c],
                              halfInputBuffer[((readHeight - y - 1) * fileWidth + x) * outChannelCount + c]);
                }
            }
        }
    }
    else {
        for (int y = 0; y < readHeight / 2; ++y) {
            for (int x = 0; x < fileWidth; ++x) {
                for (int c = 0; c < outChannelCount; ++c) {
                    std::swap(floatInputBuffer[(y * fileWidth + x) * outChannelCount + c],
                              floatInputBuffer[((readHeight - y - 1) * fileWidth + x) * outChannelCount + c]);
                }
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
            for (size_t i = 0; i < halfInputBuffer.size(); ++i)
                reinterpret_cast<float*>(storage.data)[i] = half_to_float(halfInputBuffer[i]);
        }
        else {
            for (size_t i = 0; i < floatInputBuffer.size(); ++i)
                reinterpret_cast<uint16_t*>(storage.data)[i] = float_to_half(floatInputBuffer[i]);
        }
        return true;
    }
    
    // resizing
    if (inputIsHalf) {
        floatInputBuffer.resize(readWidth * readHeight * fileChannelCount);
        for (int y = 0; y < readHeight; ++y) {
            int srcAddr = fileWidth * fileChannelCount * y;
            for (int i = 0; i < readWidth * fileChannelCount; ++i) {
                floatInputBuffer[i] = half_to_float(halfInputBuffer[srcAddr + i + (cropLeft * fileChannelCount)]);
            }
        }
    }

    std::vector<float> resizeOutputBuffer;

    nanoexr_ImageData_t src;
    src.data = reinterpret_cast<uint8_t*>(&floatInputBuffer[0]);
    src.channelCount = fileChannelCount;
    src.dataSize = readWidth * readHeight * GetBytesPerPixel();
    src.pixelType = EXR_PIXEL_FLOAT;
    src.width = readWidth;
    src.height = readHeight;

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
        for (size_t i = 0; i < resizeOutputBuffer.size(); ++i)
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


int64_t exr_AssetRead_Func(
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
        return -1;
    }
    return asset->Read(buffer, sz, offset);
}

#if 0
/// @TODO in the future Hio could use asset writing featuers
int64_t exr_AssetWrite_Func(
    exr_const_context_t         ctxt,
    void*                       userdata,
    const void*                 buffer,
    uint64_t                    sz,
    uint64_t                    offset,
    exr_stream_error_func_ptr_t error_cb)
{
    ArAsset* asset = (ArAsset*)userdata;
    if (!asset || !buffer || !sz) {
        if (error_cb)
            error_cb(ctxt, EXR_ERR_INVALID_ARGUMENT,
                           "%s", "Invalid arguments to write callback");
        return -1;
    }
    return asset->Write(buffer, sz, offset);
}
#endif



bool Hio_OpenEXRImage::Write(StorageSpec const &storage, VtDictionary const &metadata)
{
    const HioType type = HioGetHioType(storage.format);
    if (type != HioTypeFloat && type != HioTypeHalfFloat) {
        TF_CODING_ERROR("Unsupported pixel type %d", type);
        return false;
    }
    if (storage.format != HioFormatFloat16 &&
        storage.format != HioFormatFloat16Vec2 &&
        storage.format != HioFormatFloat16Vec3 &&
        storage.format != HioFormatFloat16Vec4 &&
        storage.format != HioFormatFloat32 &&
        storage.format != HioFormatFloat32Vec2 &&
        storage.format != HioFormatFloat32Vec3 &&
        storage.format != HioFormatFloat32Vec4) {
        TF_CODING_ERROR("Unsupported pixel format %d", storage.format);
        return false;
    }

    exr_context_initializer_t exrInit = EXR_DEFAULT_CONTEXT_INITIALIZER;
    //exrInit.read_fn = exr_AssetRead_Func;
    //exrInit.write_fn = exr_AssetWrite_Func;
    exrInit.read_fn = NULL; // If read is set, and write is not, things fail in write because user_data doesn't get allocated
                            // and then when we try to set fh->fd = -1 that's a seg fault
    exrInit.write_fn = NULL; // Use the default file system write, not ArAsset 
                             // at the moment.
                             // The reason is that none of the existing Hio write
                             // routines use ArAsset.
    exrInit.user_data = (void*) _asset.get(); // Hio_OpenEXRImage will outlast the reader.
    nanoexr_Reader_t* exrWriter = nanoexr_new(_filename.c_str(), &exrInit);
    if (!exrWriter) {
        TF_CODING_ERROR("Cannot create image \"%s\" for writing",
                        _filename.c_str());
        return false;
    }

    if (storage.format == HioFormatFloat16Vec3 || storage.format == HioFormatFloat16Vec4) {
        int32_t size = 2;
        int32_t ch = storage.format == HioFormatFloat16Vec3 ? 3 : 4;
        uint8_t* pixels = reinterpret_cast<uint8_t*>(storage.data);
        int32_t lineStride = storage.width * size * ch;
        int32_t pixelStride = size * ch;
        exr_result_t rv = nanoexr_open_for_writing_fp16(
                                exrWriter,
                                storage.width, storage.height,
                                pixels + (size * 2), pixelStride, lineStride,
                                pixels +  size,      pixelStride, lineStride,
                                pixels,              pixelStride, lineStride
                            );
        
        if (rv != EXR_ERR_SUCCESS) {
            TF_CODING_ERROR("Cannot open image \"%s\" for writing, %s",
                            _filename.c_str(), nanoexr_get_error_code_as_string(rv));
            return false;
        }
    }
    
    //set info to match storage
    int width = storage.width;
    int height = storage.height;
    HioFormat format = storage.format;
    HioType outputType = HioGetHioType(storage.format);
    int nchannels = HioGetComponentCount(storage.format);

    //Again, how to store metadata?
    //for (const std::pair<std::string, VtValue>& m : metadata) {
    //    _SetAttribute(&spec, m.first, m.second);
    //}
    
    //Part definition (required attributes and additional metadata)

    
    
    // Transition to writing data (this “commits” the part definitions, any changes requested after will result in an error)

    // Write part data in sequential order of parts (part0 -> partN-1).

    // Within each part, multiple threads can be encoding and writing data concurrently. For some EXR part definitions, this may be able to write data concurrently when it can predict the chunk sizes, or data is allowed to be padded. For others, it may need to temporarily cache chunks until the data is received to flush in order. The concurrency around this is handled by the library

    // Once finished writing data, use exr_finish() to clean up the context, which will flush any unwritten data such as the final chunk offset tables, and handle the temporary file flags.
    nanoexr_close(exrWriter);

    return true;
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
    return false;    
}

bool Hio_OpenEXRImage::GetSamplerMetadata(HioAddressDimension dim,
                        HioAddressMode *param) const
{
    const exr_attribute_t* attr;
    if (EXR_ERR_SUCCESS != nanoexr_get_attribute_by_name(
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
    //exrInit.write_fn = exr_AssetWrite_Func;
    exrInit.write_fn = NULL; // Use the default file system write, not ArAsset 
                             // at the moment.
                             // The reason is that none of the existing Hio write
                             // routines use ArAsset.
    exrInit.user_data = (void*) _asset.get(); // Hio_OpenEXRImage will outlast the reader.
    _exrReader = nanoexr_new(filename.c_str(), &exrInit);

    int rv = nanoexr_open(_exrReader, 0);
    if (rv != 0) {
        TF_CODING_ERROR("Cannot open image \"%s\" for reading, %s",
                        filename.c_str(), nanoexr_get_error_code_as_string(rv));
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
    _filename = filename;
    return true;
}

PXR_NAMESPACE_CLOSE_SCOPE
