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

exr_result_t nanoexr_read_tiled_exr(const char* filename,
                                    nanoexr_ImageData_t* img,
                                    const char* layerName,
                                    int partIndex,
                                    int level);

void nanoexr_release_image_data(nanoexr_ImageData_t* imageData);

#ifdef __cplusplus
}
#endif


#endif

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
    if (exr) {
        exr_finish(&exr);
        if (decoder)
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

exr_result_t nanoexr_read_tiled_exr(const char* filename,
                                    nanoexr_ImageData_t* img,
                                    const char* layerName,
                                    int partIndex,
                                    int level)
{
    exr_context_t exr = NULL;
    exr_result_t rv = EXR_ERR_SUCCESS;
    exr_context_initializer_t cinit = EXR_DEFAULT_CONTEXT_INITIALIZER;
    exr_decode_pipeline_t decoder = EXR_DECODE_PIPELINE_INITIALIZER;
    cinit.error_handler_fn = tiled_exr_err_cb;
    cinit.read_fn = NULL;    // nanoexr_read_func;
    cinit.size_fn = NULL;    // nanoexr_query_size_func;
    cinit.user_data = NULL;  // data for read and size functions
    rv = exr_test_file_header(filename, &cinit);
    CHECK_RV(rv);

    rv = exr_start_read(&exr, filename, &cinit);
    CHECK_RV(rv);

    exr_storage_t storage;
    rv = exr_get_storage(exr, partIndex, &storage);
    CHECK_RV(rv);
    if (storage != EXR_STORAGE_TILED) {
        fprintf(stderr, "nanoexr error: file is not tiled\n");
        CHECK_RV(EXR_ERR_INVALID_ARGUMENT);
    }

    int num_sub_images = 0;
    rv = exr_get_count(exr, &num_sub_images);
    CHECK_RV(rv);
    if (partIndex >= num_sub_images) {
        fprintf(stderr, "nanoexr error: part index %d out of range\n", partIndex);
        CHECK_RV(EXR_ERR_INVALID_ARGUMENT);
    }

    exr_attr_box2i_t datawin;
    exr_attr_box2i_t displaywin;

    rv = exr_get_data_window(exr, partIndex, &datawin);
    CHECK_RV(rv);
    rv = exr_get_display_window(exr, partIndex, &displaywin);
    CHECK_RV(rv);

    int width = datawin.max.x - datawin.min.x + 1;
    int height = datawin.max.y - datawin.min.y + 1;

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

    exr_compression_t compression;
    rv = exr_get_compression(exr, partIndex, &compression);
    CHECK_RV(rv);

    const exr_attr_chlist_t* chlist = NULL;
    rv = exr_get_channels(exr, partIndex, &chlist);
    CHECK_RV(rv);

    exr_pixel_type_t pixelType = chlist->entries[0].pixel_type;
    int bytesPerChannel = nanoexr_getPixelTypeSize(pixelType);
    if (bytesPerChannel == 0) {
        fprintf(stderr, "nanoexr error: unsupported pixel type\n");
        CHECK_RV(EXR_ERR_INVALID_ARGUMENT);
    }

    //img->channelCount = 3; externally supplied
    img->width = width;
    img->height = height;
    img->dataSize = width * height * img->channelCount * bytesPerChannel;
    img->data = (unsigned char*) malloc(img->dataSize);
    img->pixelType = EXR_PIXEL_HALF;

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

    int rgbaIndex[4] = {-1, -1, -1, -1};
    
    int32_t xTiles = (width + tilew - 1) / tilew;
    int32_t yTiles = (height + tileh - 1) / tileh;

    for (int tileY = 0; tileY < yTiles; ++tileY) {
        for (int tileX = 0; tileX < xTiles; ++tileX) {
            exr_chunk_info_t cinfo;
            rv = exr_read_tile_chunk_info(exr, partIndex, tileX, tileY, mipLevel, mipLevel, &cinfo);
            CHECK_RV(rv);
            memset(&decoder, 0, sizeof(decoder));

            if (decoder.channels == NULL) {
                rv = exr_decoding_initialize(exr, partIndex, &cinfo, &decoder);
                CHECK_RV(rv);
                int bytesPerElement = decoder.channels[0].bytes_per_element;
                int pixelbytes = bytesPerElement * img->channelCount;
                
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
                    
                    if (decoder.channels[c].bytes_per_element != bytesPerElement) {
                        CHECK_RV(EXR_ERR_INVALID_ARGUMENT);
                    }
                    
                    //if (decoder.channels[c].data_type != reader->pixelType) {
                    //    CHECK_RV(EXR_ERR_INVALID_ARGUMENT);
                    //}
                    
                    // start writing at the address which is offset by the tileX position
                    // and also the tileY position
                    uint8_t* curtilestart = img->data + tileX * pixelbytes * tilew;
                    curtilestart += tileY * tileh * pixelbytes * width;
                    
                    decoder.channels[c].decode_to_ptr = curtilestart + channelIndex * bytesPerChannel;
                    decoder.channels[c].user_pixel_stride = img->channelCount * bytesPerElement;
                    decoder.channels[c].user_line_stride = decoder.channels[c].user_pixel_stride * width;
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

#endif
