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

    CPPFILES
        OpenEXRImage.cpp
        stbImage.cpp
        OpenEXR/openexr-c.c

    RESOURCE_FILES
        plugInfo.json

    DOXYGEN_FILES
        overview.dox
)

# Not all functions compiled within the OpenEXR library are used, suppress
# warnings about them, as the warnings are not informative.
target_compile_options(hio PRIVATE
     $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:
     -Wno-unused-function>)
