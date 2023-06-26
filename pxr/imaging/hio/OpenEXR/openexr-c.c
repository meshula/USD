
// configuration settings for building OpenEXRCore are in the CMakeLists.txt file

#include "OpenEXRCore/openexr.h"

#if defined(__clang__)
#pragma clang diagnostic ignored "-Wunused-function"
#elif defined(__GNUC__)
#pragma GCC diagnostic ignored "-Wunused-function"
#endif
#ifdef _MSC_VER
// suppress MSVC warning about preferring _strdup.
#define _strdup strdup
#endif

#include "OpenEXRCoreUnity.h"

#include <ctype.h>
#include <math.h>

// re-export the statically hidden exr_ functions as required
// for visibility from C++

exr_result_t nanoexr_get_attribute_by_index(
    exr_const_context_t     ctxt,
    int                     part_index,
    int                     i,
    const exr_attribute_t** outattr)
{
    return exr_get_attribute_by_index(ctxt, part_index, EXR_ATTR_LIST_SORTED_ORDER, i, outattr);
}

int nanoexr_get_attribute_count(exr_const_context_t ctxt, int part_index) {
    int count = 0;
    exr_get_attribute_count(ctxt, part_index, &count);
    return count;
}

const char* nanoexr_get_error_code_as_string (exr_result_t code)
{
    return exr_get_error_code_as_string(code);
}

static float integrate_gaussian(float x, float sigma)
{
  float p1 = erf((x - 0.5f) / sigma * sqrtf(0.5f));
  float p2 = erf((x + 0.5f) / sigma * sqrtf(0.5f));
  return (p2-p1) * 0.5f;
}

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
        memcpy(dst->data, src->data, src->channelCount * srcWidth * srcHeight * sizeof(float));
        return true;
    }
    
    float* srcData = (float*)src->data;
    float* dstData = (float*)dst->data;

    // two pass image resize

    // Create a Gaussian filter, per:
    // https://bartwronski.com/2021/10/31/practical-gaussian-filter-binomial-filter-and-small-sigma-gaussians
    // chose sigma to suppress high frequencies that can't be represented in the downsampled image
    const float ratio = (float)dstWidth / (float)srcWidth;
    const float sigma = 1.f / 2.f * ratio;
    const float support = 0.995f;
    float radius = ceilf(sqrtf(-2.0f * sigma * sigma * logf(1.0f - support)));
    int filterSize = (int)radius;
    if (!filterSize)
        return false;
    
    float* filter = (float*) malloc(sizeof(float) * 1 + filterSize * 2);
    float sum = 0.0f;
    for (int i = 0; i <= filterSize; i++) {
        int idx = i + filterSize;
        filter[idx] = integrate_gaussian((float) i, sigma);
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
    free(filter);
    free(firstPass);
    return true;
}

static void
err_cb (exr_const_context_t f, int code, const char* msg)
{
    fprintf(stderr, "err_cb ERROR %d: %s\n", code, msg);
}

void nanoexr_new(const char* filename, nanoexr_Reader_t* reader) {
    exr_get_library_version (&reader->exrSDKVersionMajor,
                             &reader->exrSDKVersionMinor,
                             &reader->exrSDKVersionPatch,
                             &reader->exrSDKExtraInfo);

    reader->filename = strdup(filename);
    reader->width = 0;
    reader->height = 0;
    reader->channelCount = 0;
    reader->pixelType = EXR_PIXEL_LAST_TYPE;
    reader->partIndex = 0;
    reader->mipLevels.level = 0;
    reader->isScanline = false;
    reader->tileLevelCount = 0;
    reader->tileLevelInfo = NULL;
    reader->wrapMode = nanoexr_WrapModeClampToEdge;
}


void nanoexr_delete(nanoexr_Reader_t* reader) {
    if (!reader)
        return;
    free(reader->filename);
    free(reader->tileLevelInfo);
    free(reader);
}

int nanoexr_open(nanoexr_Reader_t* reader, int partIndex, 
                 nanoexr_attrRead attrRead, void* attrRead_userData) {
    if (!reader)
        return EXR_ERR_INVALID_ARGUMENT;
    
    exr_context_t exr;
    exr_context_initializer_t init = EXR_DEFAULT_CONTEXT_INITIALIZER;
    int rv = exr_start_read(&exr, reader->filename, &init);
    if (rv != EXR_ERR_SUCCESS) {
        exr_finish(&exr);
        return rv;
    }

    exr_attr_box2i_t datawin;
    rv = exr_get_data_window(exr, partIndex, &datawin);
    if (rv != EXR_ERR_SUCCESS) {
        exr_finish(&exr);
        return rv;
    }
    reader->partIndex = partIndex;
    reader->width = datawin.max.x - datawin.min.x + 1;
    reader->height = datawin.max.y - datawin.min.y + 1;

    exr_storage_t storage;
    rv = exr_get_storage(exr, partIndex, &storage);
    if (rv != EXR_ERR_SUCCESS) {
        exr_finish(&exr);
        return rv;
    }
    reader->isScanline = (storage == EXR_STORAGE_SCANLINE);

    int numMipLevelsX = 1, numMipLevelsY = 1;
    if (reader->isScanline) {
        numMipLevelsX = 1;
        numMipLevelsY = 1;
    } else {
        rv = exr_get_tile_levels(exr, partIndex, &numMipLevelsX, &numMipLevelsY);
        if (rv != EXR_ERR_SUCCESS) {
            exr_finish(&exr);
            return rv;
        }
    }
    if (numMipLevelsX != numMipLevelsY) {
        // current assumption is that mips are only supproted for square images
        // should this be flagged?
        numMipLevelsX = 1;
        numMipLevelsY = 1;
    }
    reader->mipLevels.level = numMipLevelsX;

    if (!reader->isScanline) {
        reader->tileLevelCount = numMipLevelsX;
        reader->tileLevelInfo = (nanoexr_TileMipInfo_t*) calloc(sizeof(nanoexr_TileMipInfo_t), numMipLevelsX);
        for (int i = 0; i < numMipLevelsX; i++) {
            int tileWidth, tileHeight, levelWidth, levelHeight;
            rv = exr_get_tile_sizes(exr, partIndex, i, i, &tileWidth, &tileHeight);
            if (rv != EXR_ERR_SUCCESS) {
                exr_finish(&exr);
                return rv;
            }
            rv = exr_get_level_sizes(exr, partIndex, i, i, &levelWidth, &levelHeight);
            if (rv != EXR_ERR_SUCCESS) {
                exr_finish(&exr);
                return rv;
            }
            reader->tileLevelInfo[i].tileWidth = tileWidth;
            reader->tileLevelInfo[i].tileHeight = tileHeight;
            reader->tileLevelInfo[i].levelWidth = levelWidth;
            reader->tileLevelInfo[i].levelHeight = levelHeight;
        }
    }

    const exr_attr_chlist_t* chlist = NULL;
    rv = exr_get_channels(exr, partIndex, &chlist);
    if (rv != EXR_ERR_SUCCESS) {
        exr_finish(&exr);
        return rv;
    }
    reader->channelCount = chlist->num_channels;
    reader->pixelType = chlist->entries[0].pixel_type;

    const exr_attribute_t* attr;
    exr_result_t wrap_rv = exr_get_attribute_by_name(exr, partIndex, "wrapmodes", &attr);
    if (wrap_rv == EXR_ERR_SUCCESS) {
        if (!strncmp("black", attr->string->str, 5))
            reader->wrapMode = nanoexr_WrapModeClampToBorderColor;
        else if (!strncmp("clamp", attr->string->str, 5))
            reader->wrapMode = nanoexr_WrapModeClampToEdge;
        else if (!strncmp("periodic", attr->string->str, 8))
            reader->wrapMode = nanoexr_WrapModeRepeat;
        else if (!strncmp("mirror", attr->string->str, 6))
            reader->wrapMode = nanoexr_WrapModeMirrorRepeat;
    }

    if (attrRead)
        attrRead(attrRead_userData, exr);

    exr_finish(&exr);
    return rv;
}

/// @TODO Pass in the part name
exr_result_t nanoexr_write_exr(
    const char* filename,
    int width, int height,
    uint8_t* red,   int32_t redPixelStride,   int32_t redLineStride,
    uint8_t* green, int32_t greenPixelStride, int32_t greenLineStride,
    uint8_t* blue,  int32_t bluePixelStride,  int32_t blueLineStride)
{
    int partidx = 0;
    exr_context_t exr;
    exr_context_initializer_t init = EXR_DEFAULT_CONTEXT_INITIALIZER;

    // switch to write mode after everything is set up
    /// @TODO use EXR_INTERMEDIATE_TEMP_FILE after this code works
    exr_result_t result = exr_start_write(
                                &exr, filename, EXR_WRITE_FILE_DIRECTLY, &init);
    if (result != EXR_ERR_SUCCESS) {
        return result;
    }

    result = exr_add_part(exr, "beauty", EXR_STORAGE_SCANLINE, &partidx);
    if (result != EXR_ERR_SUCCESS) {
        return result;
    }

    // modern exr should support long names
    exr_set_longname_support(exr, 1);

    /// @TODO allow other compression levels
    result = exr_set_zip_compression_level(exr, 0, 4);
    if (result != EXR_ERR_SUCCESS) {
        return result;
    }

    exr_attr_box2i_t dataw = {0, 0, width - 1, height - 1};
    exr_attr_box2i_t dispw = dataw;
    exr_attr_v2f_t   swc   = {0.5f, 0.5f}; // center of the screen window
    result = exr_initialize_required_attr (
        exr,
        partidx,
        &dataw,
        &dispw,
        1.f,    // pixel aspect ratio
        &swc,
        1.f,    // screen window width corresponding to swc
        EXR_LINEORDER_INCREASING_Y,
        EXR_COMPRESSION_ZIPS); // one line per chunk, ZIP is 16
    if (result != EXR_ERR_SUCCESS) {
        return result;
    }

    result = exr_add_channel(
        exr,
        partidx,
        "R",
        EXR_PIXEL_HALF,
        EXR_PERCEPTUALLY_LOGARITHMIC, // hint to compression that pixels are an image (as opposed to data of some sort)
        1, 1); // x & y sampling rate
    if (result != EXR_ERR_SUCCESS) {
        return result;
    }
    
    result = exr_add_channel(
        exr,
        partidx,
        "G",
        EXR_PIXEL_HALF,
        EXR_PERCEPTUALLY_LOGARITHMIC, // hint to compression that pixels are an image (as opposed to data of some sort)
        1, 1); // x & y sampling rate
    if (result != EXR_ERR_SUCCESS) {
        return result;
    }
    
    result = exr_add_channel(
        exr,
        partidx,
        "B",
        EXR_PIXEL_HALF,
        EXR_PERCEPTUALLY_LOGARITHMIC, // hint to compression that pixels are an image (as opposed to data of some sort)
        1, 1); // x & y sampling rate
    if (result != EXR_ERR_SUCCESS) {
        return result;
    }

    result = exr_set_version(exr, partidx, 1); // 1 is the latest version

    // set chromaticities to Rec. ITU-R BT.709-3
    exr_attr_chromaticities_t chroma = {
        0.6400f, 0.3300f, // red
        0.3000f, 0.6000f, // green
        0.1500f, 0.0600f, // blue
        0.3127f, 0.3290f};  // white
    result = exr_attr_set_chromaticities(exr, partidx, "chromaticities", &chroma);
    if (result != EXR_ERR_SUCCESS) {
        return result;
    }

    result = exr_write_header(exr);
    if (result != EXR_ERR_SUCCESS) {
        return result;
    }

#if 0
    for (int i = 0; i < metadataCount; ++i) {
        switch (metadata[i].type) {
            case EXR_ATTR_INT:
                result = exr_attr_set_int(exr, partidx, metadata[i].name, metadata[i].value.i[0]);
                break;
            case EXR_ATTR_FLOAT:
                result = exr_attr_set_float(exr, partidx, metadata[i].name, metadata[i].value.f[0]);
                break;
            case EXR_ATTR_STRING:
                result = exr_attr_set_string(exr, partidx, metadata[i].name, metadata[i].value.s);
                break;
            case EXR_ATTR_M44F:
                result = exr_attr_set_m44f(exr, partidx, metadata[i].name, (exr_attr_m44f_t*) metadata[i].value.m44f);
                break;
            case EXR_ATTR_M44D:
                result = exr_attr_set_m44d(exr, partidx, metadata[i].name, (exr_attr_m44d_t*) metadata[i].value.m44d);
                break;
            default:
                break;
        }
    }
#endif
    
    exr_encode_pipeline_t encoder;
    exr_chunk_info_t cinfo;
    int32_t               scansperchunk = 0;
    exr_get_scanlines_per_chunk(exr, partidx, &scansperchunk);
    bool                  first = true;

    uint8_t* pRed = red + (height - 1) * redLineStride;
    uint8_t* pGreen = green + (height - 1) * greenLineStride;
    uint8_t* pBlue = blue + (height - 1) * blueLineStride;
    
    int chunkInfoIndex = 0;
    for (int y = dataw.min.y; y <= dataw.max.y; y += scansperchunk, ++chunkInfoIndex) {
        result = exr_write_scanline_chunk_info(exr, partidx, y, &cinfo);
        if (result != EXR_ERR_SUCCESS) {
            return result;
        }

        if (first)
        {
            result = exr_encoding_initialize(exr, partidx, &cinfo, &encoder);
            if (result != EXR_ERR_SUCCESS) {
                return result;
            }
        }
        else
        {
            result = exr_encoding_update(exr, partidx, &cinfo, &encoder);
        }
        
        encoder.channel_count = 3;
        encoder.channels[0].user_pixel_stride = redPixelStride;
        encoder.channels[0].user_line_stride  = redLineStride;
        encoder.channels[0].encode_from_ptr   = pRed;
        encoder.channels[0].height            = scansperchunk; // chunk height
        encoder.channels[0].width             = dataw.max.x - dataw.min.y + 1;
        encoder.channels[1].user_pixel_stride = greenPixelStride;
        encoder.channels[1].user_line_stride  = greenLineStride;
        encoder.channels[1].height            = scansperchunk; // chunk height
        encoder.channels[1].width             = dataw.max.x - dataw.min.y + 1;
        encoder.channels[1].encode_from_ptr   = pGreen;
        encoder.channels[2].user_pixel_stride = bluePixelStride;
        encoder.channels[2].user_line_stride  = blueLineStride;
        encoder.channels[2].height            = scansperchunk; // chunk height
        encoder.channels[2].width             = dataw.max.x - dataw.min.y + 1;
        encoder.channels[2].encode_from_ptr   = pBlue;

        if (first) {
            result = exr_encoding_choose_default_routines(exr, partidx, &encoder);
            if (result != EXR_ERR_SUCCESS) {
                return result;
            }
        }

        result = exr_encoding_run(exr, partidx, &encoder);
        if (result != EXR_ERR_SUCCESS) {
            return result;
        }

        first = false;
        pRed -= redLineStride;
        pGreen -= greenLineStride;
        pBlue -= blueLineStride;
    }

    result = exr_encoding_destroy(exr, &encoder);
    if (result != EXR_ERR_SUCCESS) {
        return result;
    }

    result = exr_finish(&exr);
    return result;
}

int nanoexr_getPixelTypeSize(exr_pixel_type_t t)
{
    switch (t) {
        case EXR_PIXEL_HALF: return 2;
        case EXR_PIXEL_UINT: return 4;
        case EXR_PIXEL_FLOAT: return 4;
        default: return 0;
    }
}




static bool strIsRed(const char* layerName, const char* str) {
    if (layerName && (strncmp(layerName, str, strlen(layerName)) != 0))
        return false;

    // check if the case folded string is R or RED, or if it ends in .R or .RED
    char* folded = strdup(str);
    for (int i = 0; folded[i]; ++i) {
        folded[i] = tolower(folded[i]);
    }
    if (strcmp(folded, "r") == 0 || strcmp(folded, "red") == 0)
        return true;
    size_t l = strlen(folded);
    if ((l > 2) && (folded[l - 2] == '.') && (folded[l - 1] == 'r'))
        return true;
    if (l < 4)
        return false;
    return strcmp(folded + l - 4, ".red");
}

static bool strIsGreen(const char* layerName, const char* str) {
    if (layerName && (strncmp(layerName, str, strlen(layerName)) != 0))
        return false;

    // check if the case folded string is G or GREEN, or if it ends in .G or .GREEN
    char* folded = strdup(str);
    for (int i = 0; folded[i]; ++i) {
        folded[i] = tolower(folded[i]);
    }
    if (strcmp(folded, "g") == 0 || strcmp(folded, "green") == 0)
        return true;
    size_t l = strlen(folded);
    if ((l > 2) && (folded[l - 2] == '.') && (folded[l - 1] == 'g'))
        return true;
    if (l < 6)
        return false;
    return strcmp(folded + l - 6, ".green");
}

static bool strIsBlue(const char* layerName, const char* str) {
    if (layerName && (strncmp(layerName, str, strlen(layerName)) != 0))
        return false;

    // check if the case folded string is B or BLUE, or if it ends in .B or .BLUE
    char* folded = strdup(str);
    for (int i = 0; folded[i]; ++i) {
        folded[i] = tolower(folded[i]);
    }
    if (strcmp(folded, "b") == 0 || strcmp(folded, "blue") == 0)
        return true;
    size_t l = strlen(folded);
    if ((l > 2) && (folded[l - 2] == '.') && (folded[l - 1] == 'b'))
        return true;
    if (l < 5)
        return false;
    return strcmp(folded + l - 5, ".blue");
}

static bool strIsAlpha(const char* layerName, const char* str) {
    if (layerName && (strncmp(layerName, str, strlen(layerName)) != 0))
        return false;

    // check if the case folded string is A or ALPHA, or if it ends in .A or .ALPHA
    char* folded = strdup(str);
    for (int i = 0; folded[i]; ++i) {
        folded[i] = tolower(folded[i]);
    }
    if (strcmp(folded, "a") == 0 || strcmp(folded, "alpha") == 0)
        return true;
    size_t l = strlen(folded);
    if ((l > 2) && (folded[l - 2] == '.') && (folded[l - 1] == 'a'))
        return true;
    if (l < 6)
        return false;
    return strcmp(folded + l - 6, ".alpha");
}

void nanoexr_release_image_data(nanoexr_ImageData_t* imageData)
{
    if (imageData->data) {
        free(imageData->data);
        imageData->data = NULL;
    }
}

static void nanoexr_cleanup(exr_context_t exr, 
                             exr_decode_pipeline_t* decoder)
{
    if (exr && decoder) {
        exr_decoding_destroy(exr, decoder);
    }
}

static void
tiled_exr_err_cb (exr_const_context_t f, int code, const char* msg)
{
    fprintf(stderr, "err_cb ERROR %d: %s\n", code, msg);
}

static bool nanoexr_check_rv(exr_result_t rv, exr_context_t exr, 
                             exr_decode_pipeline_t* decoder, 
                             nanoexr_ImageData_t* img)
{
    if (rv != EXR_ERR_SUCCESS) {
        fprintf(stderr, "nanoexr error: %s\n", exr_get_default_error_message(rv));
        nanoexr_release_image_data(img);
        nanoexr_cleanup(exr, decoder);
        return false;
    }
    return true;
}

#define CHECK_RV(rv) if (!nanoexr_check_rv(rv, exr, &decoder, img)) return rv;

static exr_result_t _nanoexr_rgba_decoding_initialize(
    exr_context_t exr,
    nanoexr_ImageData_t* img,
    const char* layerName,
    int partIndex, exr_chunk_info_t* cinfo, exr_decode_pipeline_t* decoder,
    int* rgba)
{
    exr_result_t rv = EXR_ERR_SUCCESS;
    rv = exr_decoding_initialize(exr, partIndex, cinfo, decoder);
    if (rv != EXR_ERR_SUCCESS) {
        fprintf(stderr, "exr_decoding_initialize failed: %s\n", exr_get_default_error_message(rv));
        return rv;
    }
    int bytesPerChannel = nanoexr_getPixelTypeSize(img->pixelType);
    for (int i = 0; i < img->channelCount; ++i) {
        rgba[i] = -1;
    }
    for (int c = 0; c < decoder->channel_count; ++c) {
        int channelIndex = -1;
        if (strIsRed(layerName, decoder->channels[c].channel_name)) {
            rgba[0] = c;
            channelIndex = 0;
        }
        else if (strIsGreen(layerName, decoder->channels[c].channel_name)) {
            rgba[1] = c;
            channelIndex = 1;
        }
        else if (strIsBlue(layerName, decoder->channels[c].channel_name)) {
            rgba[2] = c;
            channelIndex = 2;
        }
        else if (strIsAlpha(layerName, decoder->channels[c].channel_name)) {
            rgba[3] = c;
            channelIndex = 3;
        }
        if (channelIndex == -1 || channelIndex >= img->channelCount) {
            decoder->channels[c].decode_to_ptr = 0;
            continue;
        }
        // precompute pixel channel byte offset
        decoder->channels[c].decode_to_ptr = (uint8_t*) (ptrdiff_t) (channelIndex * bytesPerChannel);
    }
    return rv;
}

exr_result_t nanoexr_read_tiled_exr(exr_context_t exr,
                                    nanoexr_ImageData_t* img,
                                    const char* layerName,
                                    int partIndex,
                                    int level,
                                    int* rgbaIndex)
{
    exr_decode_pipeline_t decoder = EXR_DECODE_PIPELINE_INITIALIZER;
    exr_result_t rv = EXR_ERR_SUCCESS;
    int bytesPerChannel = nanoexr_getPixelTypeSize(img->pixelType);
    uint32_t tilew, tileh;
    exr_tile_level_mode_t levelMode;
    exr_tile_round_mode_t roundMode;
    rv = exr_get_tile_descriptor(exr, partIndex, &tilew, &tileh, &levelMode, &roundMode);
    CHECK_RV(rv);

    int mipLevelsX, mipLvelsY;
    rv = exr_get_tile_levels(exr, partIndex, &mipLevelsX, &mipLvelsY);
    CHECK_RV(rv);

    const int mipLevel = 0; // only reading mip level zero
    int levelWidth, levelHeight;
    rv = exr_get_level_sizes(exr, partIndex, mipLevel, mipLevel, &levelWidth, &levelHeight);
    CHECK_RV(rv);
    
    int32_t xTiles = (img->width + tilew - 1) / tilew;
    int32_t yTiles = (img->height + tileh - 1) / tileh;
    int pixelStride = img->channelCount * bytesPerChannel;
    int imageYStride = img->width * pixelStride;

    
#if 0
    // testing: fill with gradient
    uint16_t* d = (uint16_t*) img->data;
    uint16_t oneVal = float_to_half(1.0f);
    int out = 0;
    for (int y = 0; y < img->height; ++y) {
        for (int x = 0; x < img->width; ++x) {
            uint16_t val = float_to_half((float) x / (float) img->width);
            for (int c = 0; c < img->channelCount; ++ c) {
                d[out++] = c < 3 ? val : oneVal;
            }
        }
    }
    return rv;
#endif
    
    memset(&decoder, 0, sizeof(decoder));
    for (int tileY = 0; tileY < yTiles; ++tileY) {
        for (int tileX = 0; tileX < xTiles; ++tileX) {
            exr_chunk_info_t cinfo;
            rv = exr_read_tile_chunk_info(exr, partIndex, tileX, tileY, mipLevel, mipLevel, &cinfo);
            CHECK_RV(rv);
            if (decoder.channels == NULL) {
                rv = _nanoexr_rgba_decoding_initialize(exr, img, layerName, partIndex, &cinfo, &decoder, rgbaIndex);
                CHECK_RV(rv);
                
                rv = exr_decoding_choose_default_routines(exr, partIndex, &decoder);
                CHECK_RV(rv);
            }
            else {
                // Reuse existing pipeline
                rv = exr_decoding_update(exr, partIndex, &cinfo, &decoder);
                CHECK_RV(rv);
            }

            int x = tileX * tilew;
            int y = tileY * tileh;
            uint8_t* curtilestart = img->data;
            curtilestart += y * imageYStride;
            curtilestart += x * pixelStride;
            for (int c = 0; c < decoder.channel_count; ++c) {
                if (rgbaIndex[c] >= 0) {
                    decoder.channels[c].decode_to_ptr = curtilestart + rgbaIndex[c] * bytesPerChannel;
                }
                else {
                    decoder.channels[c].decode_to_ptr = NULL;
                }
                decoder.channels[c].user_pixel_stride = pixelStride;
                decoder.channels[c].user_line_stride = imageYStride;
                decoder.channels[c].user_bytes_per_element = bytesPerChannel;
            }

            rv = exr_decoding_run(exr, partIndex, &decoder);
            CHECK_RV(rv);
        }
    }

    nanoexr_cleanup(exr, &decoder);
    return rv;
}

exr_result_t nanoexr_read_scanline_exr(exr_context_t exr,
                                       nanoexr_ImageData_t* img,
                                       const char* layerName,
                                       int partIndex,
                                       int* rgbaIndex)
{
    exr_decode_pipeline_t decoder = EXR_DECODE_PIPELINE_INITIALIZER;
    exr_result_t rv = EXR_ERR_SUCCESS;

    int scanLinesPerChunk;
    rv = exr_get_scanlines_per_chunk(exr, partIndex, &scanLinesPerChunk);
    CHECK_RV(rv);
    
    int bytesPerChannel = nanoexr_getPixelTypeSize(img->pixelType);
    int pixelbytes = bytesPerChannel * img->channelCount;

    for (int chunky = img->dataWindowMinY; chunky < img->dataWindowMaxY; chunky += scanLinesPerChunk) {
        exr_chunk_info_t cinfo;
        rv = exr_read_scanline_chunk_info(exr, partIndex, chunky, &cinfo);
        CHECK_RV(rv);
        if (decoder.channels == NULL) {
            rv = _nanoexr_rgba_decoding_initialize(exr, img, layerName, partIndex, &cinfo, &decoder, rgbaIndex);
            CHECK_RV(rv);
            if (decoder.channels == NULL) {
                return EXR_ERR_INCORRECT_CHUNK;
            }
            bytesPerChannel = decoder.channels[0].bytes_per_element;
            pixelbytes = bytesPerChannel * img->channelCount;

            for (int c = 0; c < decoder.channel_count; ++c) {                
                decoder.channels[c].user_pixel_stride = img->channelCount * bytesPerChannel;
                decoder.channels[c].user_line_stride = decoder.channels[c].user_pixel_stride * img->width;
                decoder.channels[c].user_bytes_per_element = bytesPerChannel;
            }
            
            rv = exr_decoding_choose_default_routines(exr, partIndex, &decoder);
            CHECK_RV(rv);
        }
        else {
            // Reuse existing pipeline
            rv = exr_decoding_update(exr, partIndex, &cinfo, &decoder);
            CHECK_RV(rv);
        }
        uint8_t* start = img->data + (chunky - img->dataWindowMinY) * img->width * pixelbytes;
        for (int c = 0; c < decoder.channel_count; ++c) {                
            decoder.channels[c].decode_to_ptr = start + rgbaIndex[c] * bytesPerChannel;
        }

        rv = exr_decoding_run(exr, partIndex, &decoder);
        CHECK_RV(rv);
    }

    nanoexr_cleanup(exr, &decoder);
    return rv;
}

exr_result_t nanoexr_read_exr(const char* filename, 
                              nanoexr_ImageData_t* img,
                              const char* layerName,
                              int partIndex,
                              int level) {
    exr_context_t exr = NULL;
    exr_result_t rv = EXR_ERR_SUCCESS;
    exr_context_initializer_t cinit = EXR_DEFAULT_CONTEXT_INITIALIZER;
    cinit.error_handler_fn = tiled_exr_err_cb;
    cinit.read_fn = NULL;    // nanoexr_read_func;
    cinit.size_fn = NULL;    // nanoexr_query_size_func;
    cinit.user_data = NULL;  // data for read and size functions
    rv = exr_test_file_header(filename, &cinit);
    if (rv != EXR_ERR_SUCCESS) {
        fprintf(stderr, "nanoexr header error: %s\n", exr_get_default_error_message(rv));
        return rv;
    }

    rv = exr_start_read(&exr, filename, &cinit);
    if (rv != EXR_ERR_SUCCESS) {
        fprintf(stderr, "nanoexr start error: %s\n", exr_get_default_error_message(rv));
        exr_finish(&exr);
        return rv;
    }
    exr_storage_t storage;
    rv = exr_get_storage(exr, partIndex, &storage);
    if (rv != EXR_ERR_SUCCESS) {
        fprintf(stderr, "nanoexr storage error: %s\n", exr_get_default_error_message(rv));
        exr_finish(&exr);
        return rv;
    }

    int num_sub_images = 0;
    rv = exr_get_count(exr, &num_sub_images);
    if (rv != EXR_ERR_SUCCESS || partIndex >= num_sub_images) {
        fprintf(stderr, "nanoexr error: part index %d out of range\n", partIndex);
        exr_finish(&exr);
        return rv;
    }

    // check that compression type is understood
    exr_compression_t compression;
    rv = exr_get_compression(exr, partIndex, &compression);
    if (rv != EXR_ERR_SUCCESS) {
        fprintf(stderr, "nanoexr compression error: %s\n", exr_get_default_error_message(rv));
        exr_finish(&exr);
        return rv;
    }

    exr_attr_box2i_t datawin;
    exr_attr_box2i_t displaywin;
    rv = exr_get_data_window(exr, partIndex, &datawin);
    if (rv != EXR_ERR_SUCCESS) {
        fprintf(stderr, "nanoexr data window error: %s\n", exr_get_default_error_message(rv));
        exr_finish(&exr);
        return rv;
    }
    rv = exr_get_display_window(exr, partIndex, &displaywin);
    if (rv != EXR_ERR_SUCCESS) {
        fprintf(stderr, "nanoexr display window error: %s\n", exr_get_default_error_message(rv));
        exr_finish(&exr);
        return rv;
    }

    int width = datawin.max.x - datawin.min.x + 1;
    int height = datawin.max.y - datawin.min.y + 1;

    const exr_attr_chlist_t* chlist = NULL;
    rv = exr_get_channels(exr, partIndex, &chlist);
    if (rv != EXR_ERR_SUCCESS) {
        fprintf(stderr, "nanoexr channels error: %s\n", exr_get_default_error_message(rv));
        exr_finish(&exr);
        return rv;
    }

    exr_pixel_type_t pixelType = chlist->entries[0].pixel_type;
    int bytesPerChannel = nanoexr_getPixelTypeSize(pixelType);
    if (bytesPerChannel == 0) {
        fprintf(stderr, "nanoexr error: unsupported pixel type\n");
        exr_finish(&exr);
        return rv;
    }

    //img->channelCount = 3; externally supplied
    img->width = width;
    img->height = height;
    img->dataSize = width * height * img->channelCount * bytesPerChannel;
    img->pixelType = pixelType;
    img->dataWindowMinY = datawin.min.y;
    img->dataWindowMaxY = datawin.max.y;
    img->data = (unsigned char*) malloc(img->dataSize);
    if (img->data == NULL) {
        fprintf(stderr, "nanoexr error: could not allocate memory for image data\n");
        exr_finish(&exr);
        return rv;
    }

    int rgbaIndex[4] = {-1, -1, -1, -1};

    if (storage == EXR_STORAGE_TILED) {
        rv = nanoexr_read_tiled_exr(exr, img, layerName, partIndex, level, rgbaIndex);
    }
    else {
        rv = nanoexr_read_scanline_exr(exr, img, layerName, partIndex, rgbaIndex);
    }

    // xxx specialize for float, uint16 and half.
    uint16_t oneValue = float_to_half(1.0f);
    uint16_t zeroValue = float_to_half(0.0f);

    // fill in data for missing channels
    int pixelStride = img->channelCount * bytesPerChannel;
    int lineStride = img->width * pixelStride;
    for (int c = 0; c < img->channelCount; c++) {
        if (rgbaIndex[c] < 0) {
            for (int y = 0; y < img->height; ++y) {
                for (int x = 0; x < img->width; ++x) {
                    uint8_t* curpixel = img->data + y * lineStride + x * pixelStride + c * bytesPerChannel;
                    if (img->pixelType == EXR_PIXEL_HALF) {
                        *((uint16_t*) curpixel) = c == 3 ? oneValue : zeroValue;
                    }
                    else {
                        *((float*) curpixel) = c == 3 ? 1.f : 0.f;
                    }
                }
            }
        }
    }

    rv = exr_finish(&exr);
    if (rv != EXR_ERR_SUCCESS) {
        fprintf(stderr, "nanoexr finish error: %s\n", exr_get_default_error_message(rv));
    }
    return rv;
}
