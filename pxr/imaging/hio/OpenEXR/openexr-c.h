
// openexr_c.h and openexr_c.c
// are a "single file" standalone version of the OpenEXRCore library.
// It's not part of the regular OpenEXR build. These two files are meant
// to be added to your project, either as a library with just the single
// standalone files, or added directly to your project.

#ifndef openexr_c_h
#define openexr_c_h

#if defined(__clang__)
#pragma clang diagnostic ignored "-Wunused-function"
#elif defined(__GNUC__)
#pragma GCC diagnostic ignored "-Wunused-function"
#endif

#include "OpenEXRCore/openexr_attr.h"
#include "OpenEXRCore/openexr_context.h"
#include "OpenEXRCore/openexr_part.h"
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

OPENEXR_CORE_INTERNAL_NAMESPACE_SOURCE_ENTER

exr_result_t nanoexr_get_attribute_by_name (
    exr_const_context_t     ctxt,
    int                     part_index,
    const char*             name,
    const exr_attribute_t** outattr);

typedef struct {
    int tileWidth,  tileHeight;
    int levelWidth, levelHeight;
} nanoexr_TileMipInfo_t;

typedef struct {
    uint8_t* data;
    size_t dataSize;
    exr_pixel_type_t pixelType;
    int channelCount; // 1 for luminance, 3 for RGB, 4 for RGBA
    int width, height;
    int dataWindowMinY, dataWindowMaxY;
} nanoexr_ImageData_t;

// simple struct to force type safety on interface
typedef struct {
    int level;
} nanoexr_MipLevel_t;

typedef struct {
    exr_context_t exr;
    exr_context_initializer_t init;
    char* filename;
    bool isScanline;
    int partIndex;
    exr_pixel_type_t pixelType;
    int channelCount;
    int width, height;
    int tileLevelCount;
    nanoexr_MipLevel_t mipLevels;
    nanoexr_TileMipInfo_t* tileLevelInfo;
    int exrSDKVersionMajor;
    int exrSDKVersionMinor;
    int exrSDKVersionPatch;
    const char* exrSDKExtraInfo;
} nanoexr_Reader_t;

nanoexr_Reader_t* nanoexr_new(const char* filename, 
                              exr_context_initializer_t*);

const char* nanoexr_get_error_code_as_string (exr_result_t code);

exr_result_t nanoexr_open(nanoexr_Reader_t* reader, int partIndex);
exr_result_t nanoexr_open_for_writing_fp16(nanoexr_Reader_t* nexr,
    int width, int height,
    uint8_t* red, int32_t redPixelStride, int32_t redLineStride,
    uint8_t* green, int32_t greenPixelStride, int32_t greenLineStride,
    uint8_t* blue, int32_t bluePixelStride, int32_t blueLineStride);
void         nanoexr_close(nanoexr_Reader_t* reader);
void         nanoexr_delete(nanoexr_Reader_t* reader);
exr_pixel_type_t   nanoexr_getPixelType(nanoexr_Reader_t* reader);
int                nanoexr_getPixelTypeSize(exr_pixel_type_t t);

bool nanoexr_Gaussian_resample(const nanoexr_ImageData_t* src,
                               nanoexr_ImageData_t* dst);

exr_result_t nanoexr_read_exr(const char* filename,
                              nanoexr_ImageData_t* img,
                              const char* layerName,
                              int partIndex,
                              int level);

OPENEXR_CORE_INTERNAL_NAMESPACE_SOURCE_EXIT

#ifdef __cplusplus
}
#endif

#endif
