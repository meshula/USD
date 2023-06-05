#ifndef PXR_IMAGING_HIO_EXRTILEREADER_H
#define PXR_IMAGING_HIO_EXRTILEREADER_H

#include "openexr-c.h"

#ifdef __cplusplus
extern "C" {
#endif

// reads an entire tiled image into memory
// returns any exr_result_t error code encountered upon reading
// if no error, returns EXR_ERR_SUCCESS
//
// imageData is a pointer to a nanoexr_ImageData_t struct supplied
// by the caller.  The data pointer in this struct will be set to
// point to the image data, and the dataSize field will be set to
// the size of the data in bytes.  The caller is responsible for
// freeing the data pointer when it is no longer needed.

void nanoexr_release_image_data(nanoexr_ImageData_t* imageData);

#ifdef __cplusplus
}
#endif
#endif // PXR_IMAGING_HIO_EXRTILEREADER_H

#define PXR_IMAGING_HIO_EXRTILEREADER_IMPL
#ifdef PXR_IMAGING_HIO_EXRTILEREADER_IMPL

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
        decoder->channels[c].decode_to_ptr = channelIndex * bytesPerChannel;
    }
    return rv;
}


#if 0
    // testing: fill with gradient
    uint16_t* d = (uint16_t*) img->data;
    uint16_t oneVal = float_to_half(1.0f);
    int out = 0;
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            uint16_t val = float_to_half((float) x / (float) width);
            for (int c = 0; c < img->channelCount; ++ c) {
                d[out++] = c < 3 ? val : oneVal;
            }
        }
    }
#endif


exr_result_t nanoexr_read_tiled_exr(exr_context_t exr,
                                    nanoexr_ImageData_t* img,
                                    const char* layerName,
                                    int partIndex,
                                    int level)
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

    int rgbaIndex[4] = {-1, -1, -1, -1};
    
    int32_t xTiles = (img->width + tilew - 1) / tilew;
    int32_t yTiles = (img->height + tileh - 1) / tileh;

    for (int tileY = 0; tileY < yTiles; ++tileY) {
        for (int tileX = 0; tileX < xTiles; ++tileX) {
            exr_chunk_info_t cinfo;
            rv = exr_read_tile_chunk_info(exr, partIndex, tileX, tileY, mipLevel, mipLevel, &cinfo);
            CHECK_RV(rv);
            memset(&decoder, 0, sizeof(decoder));

            if (decoder.channels == NULL) {
                rv = _nanoexr_rgba_decoding_initialize(exr, img, layerName, partIndex, &cinfo, &decoder, rgbaIndex);
                int bytesPerElement = decoder.channels[0].bytes_per_element;
                int pixelbytes = bytesPerElement * img->channelCount;
                
                for (int c = 0; c < decoder.channel_count; ++c) {
                    uint8_t* curtilestart = img->data + tileX * pixelbytes * tilew;
                    curtilestart += tileY * tileh * pixelbytes * img->width;
                    decoder.channels[c].decode_to_ptr = curtilestart + (ptrdiff_t) decoder.channels[c].decode_to_ptr;
                    decoder.channels[c].user_pixel_stride = img->channelCount * bytesPerElement;
                    decoder.channels[c].user_line_stride = decoder.channels[c].user_pixel_stride * img->width;
                    decoder.channels[c].user_bytes_per_element = bytesPerElement;
                }
                
                rv = exr_decoding_choose_default_routines(exr, partIndex, &decoder);
                CHECK_RV(rv);
            }
            else {
                // Reuse existing pipeline
                rv = exr_decoding_update(exr, partIndex, &cinfo, &decoder);
                CHECK_RV(rv);
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
                                       int partIndex)
{
    exr_decode_pipeline_t decoder = EXR_DECODE_PIPELINE_INITIALIZER;
    exr_result_t rv = EXR_ERR_SUCCESS;
    int rgbaIndex[4] = {-1, -1, -1, -1};

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
            rv = exr_decoding_initialize(exr, partIndex, &cinfo, &decoder);
            CHECK_RV(rv);
            bytesPerChannel = decoder.channels[0].bytes_per_element;
            pixelbytes = bytesPerChannel * img->channelCount;

            for (int c = 0; c < decoder.channel_count; ++c) {
                int channelIndex = -1;
                if (strIsRed(layerName, decoder.channels[c].channel_name)) {
                    rgbaIndex[0] = c;
                    channelIndex = 0;
                }
                else if (strIsGreen(layerName, decoder.channels[c].channel_name)) {
                    rgbaIndex[1] = c;
                    channelIndex = 1;
                }
                else if (strIsBlue(layerName, decoder.channels[c].channel_name)) {
                    rgbaIndex[2] = c;
                    channelIndex = 2;
                }
                else if (strIsAlpha(layerName, decoder.channels[c].channel_name)) {
                    rgbaIndex[3] = c;
                    channelIndex = 3;
                }
                else {
                    continue;   // skip this unknown channel
                }
                if (channelIndex >= img->channelCount) {
                    continue; // skip channels beyond what fits in the output buffer
                }
                
                if (decoder.channels[c].bytes_per_element != bytesPerChannel) {
                    CHECK_RV(EXR_ERR_INVALID_ARGUMENT);
                }
                
                decoder.channels[c].decode_to_ptr = NULL;
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

        uint16_t oneValue = float_to_half(1.0f);
        uint16_t zeroValue = float_to_half(0.0f);

        // fill in data for missing chunk channels
        for (int c = 0; c < img->channelCount; c++) {
            if (rgbaIndex[c] >= 0) {
                uint8_t* start = img->data + (chunky - img->dataWindowMinY) * img->width * pixelbytes;
                decoder.channels[c].decode_to_ptr = start + rgbaIndex[c] * bytesPerChannel;
            }
            else {
                uint8_t* curtilestart = img->data + (chunky - img->dataWindowMinY) * img->width * bytesPerChannel * img->channelCount;
                uint16_t fillValue = c == 3 ? oneValue : zeroValue;
                uint16_t* fillPtr = (uint16_t*) (curtilestart + c * bytesPerChannel);
                for (int y = 0; y < scanLinesPerChunk; ++y) {
                    for (int x = 0; x < img->width; ++x)
                        fillPtr[x * img->channelCount + c] = fillValue;
                    fillPtr += img->width * img->channelCount;
                }
            }
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

    if (storage == EXR_STORAGE_TILED) {
        rv = nanoexr_read_tiled_exr(exr, img, layerName, partIndex, level);
    }
    else {
        rv = nanoexr_read_scanline_exr(exr, img, layerName, partIndex);
    }

    rv = exr_finish(&exr);
    if (rv != EXR_ERR_SUCCESS) {
        fprintf(stderr, "nanoexr finish error: %s\n", exr_get_default_error_message(rv));
    }
    return rv;
}

#endif
