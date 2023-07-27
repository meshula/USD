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

pxr_build_test(testReadWriteEXR
    LIBRARIES
        hio
        ${Boost_PYTHON_LIBRARY}
        ${Python3_LIBRARY}
        
    CPPFILES
        testenv/testReadWriteEXR.cpp
)

#pxr_install_test_dir(
#    SRC testenv/testReadWriteEXR
#    DEST testReadWriteEXR
#)

pxr_register_test(testReadWriteEXR
    COMMAND "${CMAKE_INSTALL_PREFIX}/tests/testReadWriteEXR"
)

