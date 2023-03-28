set(PXR_PREFIX pxr/imaging)
set(PXR_PACKAGE hioOpenEXR)

if(NOT TARGET ZLIB::ZLIB)
    find_package(ZLIB REQUIRED)
endif()

pxr_library(hioOpenEXR
    LIBRARIES
        ar
        gf
        hio
        tf
        usd
        ZLIB::ZLIB

    PUBLIC_CLASSES

    PUBLIC_HEADERS
        api.h

    PRIVATE_HEADERS
        openexr-c.h

    PRIVATE_CLASSES

    CPPFILES
        OpenEXRImage.cpp
        openexr-c.c

    RESOURCE_FILES
        plugInfo.json
)
