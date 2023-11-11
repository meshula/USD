set(PXR_PREFIX pxr/imaging)
set(PXR_PACKAGE hio)

pxr_library(hio
    LIBRARIES
        arch
        js
        plug
        tf
        vt
        trace
        ar
        hf

    INCLUDE_DIRS
        ${Boost_INCLUDE_DIRS}

    PUBLIC_CLASSES
        fieldTextureData
        glslfx
        glslfxConfig
        glslfxResourceLayout
        image
        imageRegistry
        types

    PRIVATE_CLASSES
        debugCodes
        dictionary

    PUBLIC_HEADERS
        api.h

    PRIVATE_HEADERS
        OpenEXR/openexr-c.h
        rankedTypeMap.h
        stb/stb_image.h
        stb/stb_image_resize.h
        stb/stb_image_write.h
        AVIF/src/avif/avif_cxx.h
        AVIF/src/avif/avif.h
        AVIF/src/avif/internal.h

    CPPFILES
        OpenEXRImage.cpp
        stbImage.cpp
        OpenEXR/openexr-c.c
        AVIF/src/alpha.c
        AVIF/src/avif.c
        AVIF/src/colr.c
        AVIF/src/diag.c
        AVIF/src/exif.c
        AVIF/src/io.c
        AVIF/src/mem.c
        AVIF/src/obu.c
        AVIF/src/rawdata.c
        AVIF/src/read.c
        AVIF/src/reformat.c
        AVIF/src/reformat_libsharpyuv.c
        AVIF/src/reformat_libyuv.c
        AVIF/src/avif-scale.c
        AVIF/src/stream.c
        AVIF/src/utils.c
        AVIF/src/write.c
        AVIF/src/src-libyuv/planar_functions.c
        AVIF/src/src-libyuv/row_common.c
        AVIF/src/src-libyuv/scale.c
        AVIF/src/src-libyuv/scale_any.c
        AVIF/src/src-libyuv/scale_common.c


    RESOURCE_FILES
        plugInfo.json

    DOXYGEN_FILES
        overview.dox
)

