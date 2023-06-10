
// configuration settings for building OpenEXRCore are in the CMakeLists.txt file

#ifdef IMATH_HALF_SAFE_FOR_C
#undef IMATH_HALF_SAFE_FOR_C
#endif

#if defined(__clang__)
#pragma clang diagnostic ignored "-Wunused-function"
#elif defined(__GNUC__)
#pragma GCC diagnostic ignored "-Wunused-function"
#endif

#define EXR_INTERNAL static
#include "OpenEXRCoreUnity.h"

#include <ctype.h>
#include <math.h>

OPENEXR_CORE_INTERNAL_NAMESPACE_SOURCE_ENTER

#include "exrTileReader.h"

// re-export the statically hidden exr_ functions as required
// for visibility from C++

exr_result_t nanoexr_get_attribute_by_name (
    exr_const_context_t     ctxt,
    int                     part_index,
    const char*             name,
    const exr_attribute_t** outattr)
{
    return exr_get_attribute_by_name(ctxt, part_index, name, outattr);
}

const char* nanoexr_get_error_code_as_string (exr_result_t code)
{
    return exr_get_error_code_as_string(code);
}


static uint64_t gMaxTileBytes        = 8192 * 8192;


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

nanoexr_Reader_t* nanoexr_new(const char* filename,
                              exr_context_initializer_t* init) {
    nanoexr_Reader_t* reader = (nanoexr_Reader_t*) calloc(1, sizeof(nanoexr_Reader_t));
    exr_get_library_version (&reader->exrSDKVersionMajor,
                             &reader->exrSDKVersionMinor,
                             &reader->exrSDKVersionPatch,
                             &reader->exrSDKExtraInfo);

    reader->filename = strdup(filename);
    reader->exr = NULL;
    reader->width = 0;
    reader->height = 0;
    reader->channelCount = 0;
    reader->pixelType = EXR_PIXEL_LAST_TYPE;
    reader->partIndex = 0;
    reader->mipLevels.level = 0;
    reader->isScanline = false;
    reader->tileLevelCount = 0;
    reader->tileLevelInfo = NULL;

    if (init != NULL) {
        reader->init = *init;
    } else {
        reader->init = (exr_context_initializer_t) EXR_DEFAULT_CONTEXT_INITIALIZER;
        reader->init.error_handler_fn          = &err_cb;
    }
    return reader;
}

void nanoexr_close(nanoexr_Reader_t* reader) {
    if (reader && reader->exr) {
        exr_finish(&reader->exr);
        reader->exr = NULL;
    }
}

void nanoexr_delete(nanoexr_Reader_t* reader) {
    nanoexr_close(reader);
    free(reader->filename);
    free(reader->tileLevelInfo);
    free(reader);
}

int nanoexr_open(nanoexr_Reader_t* reader, int partIndex) {
    if (!reader)
        return EXR_ERR_INVALID_ARGUMENT;
    if (reader->exr) {
        nanoexr_close(reader);
    }
    int rv = exr_start_read(&reader->exr, reader->filename, &reader->init);
    if (rv != EXR_ERR_SUCCESS) {
        nanoexr_close(reader);
        return rv;
    }

    exr_attr_box2i_t datawin;
    rv = exr_get_data_window(reader->exr, partIndex, &datawin);
    if (rv != EXR_ERR_SUCCESS) {
        nanoexr_close(reader);
        return rv;
    }
    reader->partIndex = partIndex;
    reader->width = datawin.max.x - datawin.min.x + 1;
    reader->height = datawin.max.y - datawin.min.y + 1;

    exr_storage_t storage;
    rv = exr_get_storage(reader->exr, partIndex, &storage);
    if (rv != EXR_ERR_SUCCESS) {
        nanoexr_close(reader);
        return rv;
    }
    reader->isScanline = (storage == EXR_STORAGE_SCANLINE);

    int numMipLevelsX = 1, numMipLevelsY = 1;
    if (reader->isScanline) {
        numMipLevelsX = 1;
        numMipLevelsY = 1;
    } else {
        rv = exr_get_tile_levels(reader->exr, partIndex, &numMipLevelsX, &numMipLevelsY);
        if (rv != EXR_ERR_SUCCESS) {
            nanoexr_close(reader);
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
            rv = exr_get_tile_sizes(reader->exr, partIndex, i, i, &tileWidth, &tileHeight);
            if (rv != EXR_ERR_SUCCESS) {
                nanoexr_close(reader);
                return rv;
            }
            rv = exr_get_level_sizes(reader->exr, partIndex, i, i, &levelWidth, &levelHeight);
            if (rv != EXR_ERR_SUCCESS) {
                nanoexr_close(reader);
                return rv;
            }
            reader->tileLevelInfo[i].tileWidth = tileWidth;
            reader->tileLevelInfo[i].tileHeight = tileHeight;
            reader->tileLevelInfo[i].levelWidth = levelWidth;
            reader->tileLevelInfo[i].levelHeight = levelHeight;
        }
    }

    const exr_attr_chlist_t* chlist = NULL;
    rv = exr_get_channels(reader->exr, partIndex, &chlist);
    if (rv != EXR_ERR_SUCCESS) {
        nanoexr_close(reader);
        return rv;
    }
    reader->channelCount = chlist->num_channels;
    reader->pixelType = chlist->entries[0].pixel_type;

    return rv;
}

/// @TODO Pass in the part name
exr_result_t nanoexr_open_for_writing_fp16(nanoexr_Reader_t* nexr,
    int width, int height,
    uint8_t* red,   int32_t redPixelStride,   int32_t redLineStride,
    uint8_t* green, int32_t greenPixelStride, int32_t greenLineStride,
    uint8_t* blue,  int32_t bluePixelStride,  int32_t blueLineStride)
{
    int partidx = 0;
    // switch to write mode after everything is set up
    /// @TODO use EXR_INTERMEDIATE_TEMP_FILE after this code works
    exr_result_t result = exr_start_write(&nexr->exr, nexr->filename, EXR_WRITE_FILE_DIRECTLY, &nexr->init);
    if (result != EXR_ERR_SUCCESS) {
        return result;
    }

    result = exr_add_part(nexr->exr, "beauty", EXR_STORAGE_SCANLINE, &partidx);
    if (result != EXR_ERR_SUCCESS) {
        return result;
    }

    // modern exr should support long names
    exr_set_longname_support(nexr->exr, 1);


    /// @TODO allow other compression levels
    result = exr_set_zip_compression_level(nexr->exr, 0, 4);
    if (result != EXR_ERR_SUCCESS) {
        return result;
    }

    exr_attr_box2i_t dataw = {0, 0, width - 1, height - 1};
    exr_attr_box2i_t dispw = dataw;
    exr_attr_v2f_t   swc   = {0.5f, 0.5f}; // center of the screen window
    result = exr_initialize_required_attr (
        nexr->exr,
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
        nexr->exr,
        partidx,
        "R",
        EXR_PIXEL_HALF,
        EXR_PERCEPTUALLY_LOGARITHMIC, // hint to compression that pixels are an image (as opposed to data of some sort)
        1, 1); // x & y sampling rate
    if (result != EXR_ERR_SUCCESS) {
        return result;
    }
    
    result = exr_add_channel(
        nexr->exr,
        partidx,
        "G",
        EXR_PIXEL_HALF,
        EXR_PERCEPTUALLY_LOGARITHMIC, // hint to compression that pixels are an image (as opposed to data of some sort)
        1, 1); // x & y sampling rate
    if (result != EXR_ERR_SUCCESS) {
        return result;
    }
    
    result = exr_add_channel(
        nexr->exr,
        partidx,
        "B",
        EXR_PIXEL_HALF,
        EXR_PERCEPTUALLY_LOGARITHMIC, // hint to compression that pixels are an image (as opposed to data of some sort)
        1, 1); // x & y sampling rate
    if (result != EXR_ERR_SUCCESS) {
        return result;
    }

    result = exr_set_version(nexr->exr, partidx, 1); // 1 is the latest version

    // set chromaticities to Rec. ITU-R BT.709-3
    exr_attr_chromaticities_t chroma = {
        0.6400f, 0.3300f, // red
        0.3000f, 0.6000f, // green
        0.1500f, 0.0600f, // blue
        0.3127f, 0.3290f};  // white
    result = exr_attr_set_chromaticities(nexr->exr, partidx, "chromaticities", &chroma);
    if (result != EXR_ERR_SUCCESS) {
        return result;
    }

    result = exr_write_header(nexr->exr);
    if (result != EXR_ERR_SUCCESS) {
        return result;
    }

    exr_encode_pipeline_t encoder;


    exr_chunk_info_t cinfo;
    int32_t               scansperchunk = 0;
    exr_get_scanlines_per_chunk(nexr->exr, partidx, &scansperchunk);
    bool                  first = true;

    uint8_t* pRed = red + (height - 1) * redLineStride;
    uint8_t* pGreen = green + (height - 1) * greenLineStride;
    uint8_t* pBlue = blue + (height - 1) * blueLineStride;
    
    int chunkInfoIndex = 0;
    for (int y = dataw.min.y; y <= dataw.max.y; y += scansperchunk, ++chunkInfoIndex) {
        result = exr_write_scanline_chunk_info(nexr->exr, partidx, y, &cinfo);
        if (result != EXR_ERR_SUCCESS) {
            return result;
        }

        if (first)
        {
            result = exr_encoding_initialize(nexr->exr, partidx, &cinfo, &encoder);
            if (result != EXR_ERR_SUCCESS) {
                return result;
            }
        }
        else
        {
            result = exr_encoding_update(nexr->exr, partidx, &cinfo, &encoder);
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
            result = exr_encoding_choose_default_routines(nexr->exr, partidx, &encoder);
            if (result != EXR_ERR_SUCCESS) {
                return result;
            }
        }

        result = exr_encoding_run(nexr->exr, partidx, &encoder);
        if (result != EXR_ERR_SUCCESS) {
            return result;
        }

        first = false;
        pRed -= redLineStride;
        pGreen -= greenLineStride;
        pBlue -= blueLineStride;
    }

    result = exr_encoding_destroy(nexr->exr, &encoder);
    if (result != EXR_ERR_SUCCESS) {
        return result;
    }

    result = exr_finish(&nexr->exr);
    return result;
}

exr_pixel_type_t nanoexr_getPixelType(nanoexr_Reader_t* reader) {
    if (!reader || !reader->exr)
        return EXR_PIXEL_LAST_TYPE;
    return reader->pixelType;
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

OPENEXR_CORE_INTERNAL_NAMESPACE_SOURCE_EXIT
