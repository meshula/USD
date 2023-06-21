
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
    nanoexr_Reader_t         _exrReader;
    SourceColorSpace         _sourceColorSpace;
    int                      _subimage = 0;
    int                      _mip = 0;
    bool                     _suppressErrors = false;

public:
    Hio_OpenEXRImage() = default;
    virtual ~Hio_OpenEXRImage() override = default;

    using Base = HioImage;
    bool      Read(StorageSpec const &storage) override {
                  return ReadCropped(0, 0, 0, 0, storage); }
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
    std::string const &GetFilename() const override;

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

template<typename T>
class ImageProcessor
{
public:
    // Flip the image in-place.
    static void FlipImage(T* buffer, int width, int height, int channelCount)
    {
        for (int y = 0; y < height / 2; ++y) {
            for (int x = 0; x < width; ++x) {
                for (int c = 0; c < channelCount; ++c) {
                    std::swap(buffer[(y * width + x) * channelCount + c],
                              buffer[((height - y - 1) * width + x) * channelCount + c]);
                }
            }
        }
    }

    // Crop the image in-place.
    static void CropImage(T* buffer, 
                          int width, int height, int channelCount,
                          int cropTop, int cropBottom, int cropLeft, int cropRight)
    {
        int newWidth = width - cropLeft - cropRight;
        int newHeight = height - cropTop - cropBottom;

        if (newWidth <= 0 || newHeight <= 0 || (newWidth == width && newHeight == height))
            return;

        for (int y = 0; y < newHeight; ++y) {
            for (int x = 0; x < newWidth; ++x) {
                for (int c = 0; c < channelCount; ++c) {
                    buffer[(y * newWidth + x) * channelCount + c] =
                        buffer[((y + cropTop) * width + x + cropLeft) * channelCount + c];
                }
            }
        }
    }

    static void HalfToFloat(GfHalf* buffer, float* outBuffer, int width, int height, int channelCount)
    {
        if (!buffer || !outBuffer)
            return;
        
        for (int i = 0; i < width * height * channelCount; ++i) {
            outBuffer[i] = buffer[i];
        }
    }

    static void FloatToHalf(float* buffer, GfHalf* outBuffer, int width, int height, int channelCount)
    {
        if (!buffer || !outBuffer)
            return;
        
        for (int i = 0; i < width * height * channelCount; ++i) {
            outBuffer[i] = buffer[i];
        }
    }
};

bool Hio_OpenEXRImage::ReadCropped(
                int const cropTop,  int const cropBottom,
                int const cropLeft, int const cropRight, 
                StorageSpec const &storage)
{
    if (cropTop < 0 || cropBottom < 0 || cropLeft < 0 || cropRight < 0)
        return false;

    // cache values and
    // determine what needs to happen as a consequence of the input parameters

    int fileWidth =  _exrReader.width;
    int fileHeight = _exrReader.height;
    int fileChannelCount = _exrReader.channelCount;
    exr_pixel_type_t filePixelType = _exrReader.pixelType;

    int outWidth =  storage.width;
    int outHeight = storage.height;
    int outChannelCount = HioGetComponentCount(storage.format);

    bool inputIsHalf =   filePixelType == EXR_PIXEL_HALF;
    bool inputIsFloat =  filePixelType == EXR_PIXEL_FLOAT;
    bool outputIsFloat = HioGetHioType(storage.format) == HioTypeFloat;
    bool outputIsHalf =  HioGetHioType(storage.format) == HioTypeHalfFloat;

    if (!outputIsFloat && !outputIsHalf)
        return false;

    int outputBytesPerPixel = (outputIsFloat ? sizeof(float) : sizeof(GfHalf)) * outChannelCount;

    int readWidth = fileWidth - cropLeft - cropRight;
    int readHeight = fileHeight - cropTop - cropBottom;
    if (readHeight <= 0 || readWidth <= 0) {
        memset(storage.data, 0, outWidth * outHeight * outputBytesPerPixel);
        return true;
    }
    bool resizing = (readWidth != outWidth) || (readHeight != outHeight);

    // ensure there's enough memory for the greater of input and output channel count, for
    // in place conversions.
    int maxChannelCount = std::max(fileChannelCount, outChannelCount);
    std::vector<GfHalf> halfInputBuffer;
    if (inputIsHalf) {
        halfInputBuffer.resize(fileWidth * readHeight * maxChannelCount);
    }
    std::vector<float> floatInputBuffer;
    if (inputIsHalf && (resizing || outputIsFloat)) {
        floatInputBuffer.resize(fileWidth * readHeight * maxChannelCount);
    }

    {
        nanoexr_ImageData_t img;
        img.channelCount = outChannelCount;
        exr_result_t rv = nanoexr_read_exr(_exrReader.filename, &img, nullptr, 0, 0);
        if (rv != EXR_ERR_SUCCESS) {
            return false;
        }

        if (img.pixelType == EXR_PIXEL_HALF) {
            memcpy(&halfInputBuffer[0], img.data, img.dataSize);
        }
        else {
            memcpy(&floatInputBuffer[0], img.data, img.dataSize);
        }

        // flip and crop the image in place
        if (inputIsHalf) {
                       ImageProcessor<GfHalf>::FlipImage(&halfInputBuffer[0], fileWidth, fileHeight, img.channelCount);
            ImageProcessor<GfHalf>::CropImage(&halfInputBuffer[0],
                                              fileWidth, fileHeight, img.channelCount,
                                              cropTop, cropBottom, cropLeft, cropRight);
        }
        else {
            ImageProcessor<float>::FlipImage(&floatInputBuffer[0], fileWidth, fileHeight, fileChannelCount);
            ImageProcessor<float>::CropImage(&floatInputBuffer[0], 
                                            fileWidth, fileHeight, fileChannelCount,
                                            cropTop, cropBottom, cropLeft, cropRight);
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
            // output is half
            for (size_t i = 0; i < floatInputBuffer.size(); ++i)
                reinterpret_cast<uint16_t*>(storage.data)[i] = float_to_half(floatInputBuffer[i]);
        }
        return true;
    }

    // resize the image, so promote to float if necessary
    if (inputIsHalf) {
        ImageProcessor<uint16_t>::HalfToFloat(&halfInputBuffer[0], &floatInputBuffer[0],
                                              fileWidth, readHeight, fileChannelCount);
        inputIsFloat = true;
        inputIsHalf = false;
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
    if (outputIsFloat)
        return true;

    ImageProcessor<float>::FloatToHalf(&resizeOutputBuffer[0],
                                       reinterpret_cast<GfHalf*>(dst.data),
                                       outWidth, outHeight, outChannelCount);
    return true;
}

// XXX for consistency with other plugins, there is currently no ArAsset write function.
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

// For compatability with Ice/Imr we transmogrify some matrix metadata
/// XXX is this historical or is it still necessary?
static std::string
_TranslateMetadataKey(std::string const & metadataKey, bool *convertMatrixTypes)
{
    if (metadataKey == "NP") {
        *convertMatrixTypes = true;
        return "worldtoscreen";
    } else
    if (metadataKey == "Nl") {
        *convertMatrixTypes = true;
        return "worldtocamera";
    } else {
        return metadataKey;
    }
}

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

    //exr_context_initializer_t exrInit = EXR_DEFAULT_CONTEXT_INITIALIZER;
    //exrInit.read_fn = NULL;
    //exrInit.write_fn = NULL;
    //exrInit.user_data = (void*) _asset.get(); // Hio_OpenEXRImage will outlast the reader.
    nanoexr_Reader_t exrWriter = { 0 };
    nanoexr_new(_filename.c_str(), &exrWriter);

    std::vector<nanoexr_metadata_t> exrAttributes;
    exrAttributes.reserve(metadata.size());

    for (const std::pair<std::string, VtValue> m : metadata) {
        bool convertMatrixTypes = false;
        std::string key = _TranslateMetadataKey(m.first, &convertMatrixTypes);
        nanoexr_metadata_t attr;
        attr.name = strdup(key.c_str());

        if (m.second.IsHolding<std::string>()) {
            attr.type = EXR_ATTR_STRING;
            attr.value.s = strdup(m.second.Get<std::string>().c_str());
            exrAttributes.push_back(attr);
        } 
        else if (m.second.IsHolding<unsigned char>()) {
            attr.type = EXR_ATTR_INT;
            attr.value.i[0] = m.second.Get<unsigned char>();
            exrAttributes.push_back(attr);
        }
        else if (m.second.IsHolding<char>()) {
            attr.type = EXR_ATTR_INT;
            attr.value.i[0] = m.second.Get<char>();
            exrAttributes.push_back(attr);
        }
        else if (m.second.IsHolding<int>()) {
            attr.type = EXR_ATTR_INT;
            attr.value.i[0] = m.second.Get<int>();
            exrAttributes.push_back(attr);
        } 
        else if (m.second.IsHolding<unsigned int>()) {
            attr.type = EXR_ATTR_INT;
            attr.value.i[0] = static_cast<int>(m.second.Get<unsigned int>());
            exrAttributes.push_back(attr);
        } 
        else if (m.second.IsHolding<float>()) {
            attr.type = EXR_ATTR_FLOAT;
            attr.value.f[0] = m.second.Get<float>();
            exrAttributes.push_back(attr);
        } 
        else if (m.second.IsHolding<double>()) {
            attr.type = EXR_ATTR_DOUBLE;
            attr.value.f[0] = m.second.Get<double>();
            exrAttributes.push_back(attr);
        } 
        else if (m.second.IsHolding<GfMatrix4f>()) {
            attr.type = EXR_ATTR_M44F;
            memcpy(attr.value.m44f, m.second.Get<GfMatrix4f>().GetArray(), 16 * sizeof(float));
            exrAttributes.push_back(attr);
        }
        else if (m.second.IsHolding<GfMatrix4d>()) {
            // For compatibility with Ice/Imr write double matrix as float matrix
            if (convertMatrixTypes) {
                GfMatrix4f floatMatrix(m.second.Get<GfMatrix4d>());
                attr.type = EXR_ATTR_M44F;
                memcpy(attr.value.m44f, floatMatrix.GetArray(), 16 * sizeof(float));
                exrAttributes.push_back(attr);
            } 
            else {
                attr.type = EXR_ATTR_M44D;
                memcpy(attr.value.m44d, m.second.Get<GfMatrix4d>().GetArray(), 16 * sizeof(double));
                exrAttributes.push_back(attr);
            }
        }
    }

    if (storage.format == HioFormatFloat16Vec3 || storage.format == HioFormatFloat16Vec4) {
        int32_t size = 2;
        int32_t ch = storage.format == HioFormatFloat16Vec3 ? 3 : 4;
        uint8_t* pixels = reinterpret_cast<uint8_t*>(storage.data);
        int32_t lineStride = storage.width * size * ch;
        int32_t pixelStride = size * ch;
        exr_result_t rv = nanoexr_open_for_writing_fp16(
                                &exrWriter,
                                storage.width, storage.height,
                                pixels + (size * 2), pixelStride, lineStride, // red
                                pixels +  size,      pixelStride, lineStride, // green
                                pixels,              pixelStride, lineStride, // blue
                                exrAttributes.data(), exrAttributes.size());
        
        if (rv != EXR_ERR_SUCCESS) {
            TF_CODING_ERROR("Cannot open image \"%s\" for writing, %s",
                            _filename.c_str(), nanoexr_get_error_code_as_string(rv));
            return false;
        }
    }

    for (int i = 0; i < exrAttributes.size(); i++) {
        // free the memory allocated for each of the attributes created above
        free(exrAttributes[i].name);
        if (exrAttributes[i].type == EXR_ATTR_M44F) {
            free(exrAttributes[i].value.s);
        } 
    }
    return true;
}

std::string const& Hio_OpenEXRImage::GetFilename() const
{
    return _filename;
}

int Hio_OpenEXRImage::GetWidth() const
{
    return _exrReader.width;
}
int Hio_OpenEXRImage::GetHeight() const
{
    return _exrReader.height;
}

HioFormat Hio_OpenEXRImage::GetFormat() const
{
    return HioFormatFromExrPixelType(_exrReader.pixelType, _exrReader.channelCount);
}

int Hio_OpenEXRImage::GetBytesPerPixel() const
{
    return _exrReader.channelCount * nanoexr_getPixelTypeSize(_exrReader.pixelType);
}

int Hio_OpenEXRImage::GetNumMipLevels() const
{
    return _exrReader.mipLevels.level;
}

bool Hio_OpenEXRImage::IsColorSpaceSRGB() const
{
    // the function asks if the color space is SRGB, but fundamentally
    // what Hydra reall wants to know is whether the pixels are gamma pixels.
    // OpenEXR images are always linear, so we can just return false.
    return false;
}

bool Hio_OpenEXRImage::GetMetadata(TfToken const &key, VtValue *value) const
{
    return false;    
}

bool Hio_OpenEXRImage::GetSamplerMetadata(HioAddressDimension dim,
                        HioAddressMode *param) const
{
    if (!param)
        return false;
    
    switch (_exrReader.wrapMode) {
        case nanoexr_WrapModeClampToEdge: *param = HioAddressModeClampToEdge; break;
        case nanoexr_WrapModeMirrorClampToEdge: *param = HioAddressModeClampToEdge; break;
        case nanoexr_WrapModeRepeat: *param = HioAddressModeRepeat; break;
        case nanoexr_WrapModeMirrorRepeat: *param = HioAddressModeMirrorRepeat; break;
        case nanoexr_WrapModeClampToBorderColor: *param = HioAddressModeClampToBorderColor;
    }
    return true;
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

/*    exr_context_initializer_t exrInit = EXR_DEFAULT_CONTEXT_INITIALIZER;
    exrInit.read_fn = exr_AssetRead_Func;
    exrInit.write_fn = NULL;
    exrInit.write_fn = NULL; // Use the default file system write, not ArAsset
                             // at the moment.
                             // The reason is that none of the existing Hio write
                             // routines use ArAsset.
    exrInit.user_data = (void*) _asset.get(); // Hio_OpenEXRImage will outlast the reader. */
    nanoexr_new(filename.c_str(), &_exrReader);

    int rv = nanoexr_open(&_exrReader, 0);
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
