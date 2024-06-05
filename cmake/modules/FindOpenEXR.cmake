#
# Copyright 2016 Pixar
#
# Licensed under the terms set forth in the LICENSE.txt file available at
# https://openusd.org/license.
#

find_path(OPENEXR_INCLUDE_DIR
    OpenEXR/half.h
HINTS
    "${OPENEXR_LOCATION}"
    "$ENV{OPENEXR_LOCATION}"
PATH_SUFFIXES
    include/
DOC
    "OpenEXR headers path"
)

if(OPENEXR_INCLUDE_DIR)
  set(openexr_config_file "${OPENEXR_INCLUDE_DIR}/OpenEXR/OpenEXRConfig.h")
  if(EXISTS ${openexr_config_file})
      file(STRINGS
           ${openexr_config_file}
           TMP
           REGEX "#define OPENEXR_VERSION_STRING.*$")
      string(REGEX MATCHALL "[0-9.]+" OPENEXR_VERSION ${TMP})

      file(STRINGS
           ${openexr_config_file}
           TMP
           REGEX "#define OPENEXR_VERSION_MAJOR.*$")
      string(REGEX MATCHALL "[0-9]" OPENEXR_MAJOR_VERSION ${TMP})

      file(STRINGS
           ${openexr_config_file}
           TMP
           REGEX "#define OPENEXR_VERSION_MINOR.*$")
      string(REGEX MATCHALL "[0-9]" OPENEXR_MINOR_VERSION ${TMP})
  endif()
endif()

foreach(OPENEXR_LIB
    Half
    Iex
    Imath
    IlmImf
    IlmThread
    IlmImfUtil
    IexMath
    )

    # OpenEXR libraries may be suffixed with the version number, so we search
    # using both versioned and unversioned names.
    set(DEBUG_POSTFIX )
    if(DEFINED PXR_USE_DEBUG_BUILD)
        if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin" AND ${PXR_USE_DEBUG_BUILD} MATCHES ON)
            set(DEBUG_POSTFIX _d)
        endif()
    endif()
    find_library(OPENEXR_${OPENEXR_LIB}_LIBRARY
        NAMES
            ${OPENEXR_LIB}-${OPENEXR_MAJOR_VERSION}_${OPENEXR_MINOR_VERSION}${DEBUG_POSTFIX}
            ${OPENEXR_LIB}{DEBUG_POSTFIX}
        HINTS
            "${OPENEXR_LOCATION}"
            "$ENV{OPENEXR_LOCATION}"
        PATH_SUFFIXES
            lib/
        DOC
            "OPENEXR's ${OPENEXR_LIB} library path"
    )

    if(OPENEXR_${OPENEXR_LIB}_LIBRARY)
        list(APPEND OPENEXR_LIBRARIES ${OPENEXR_${OPENEXR_LIB}_LIBRARY})
    endif()
endforeach(OPENEXR_LIB)

# So #include <half.h> works
list(APPEND OPENEXR_INCLUDE_DIRS ${OPENEXR_INCLUDE_DIR})
list(APPEND OPENEXR_INCLUDE_DIRS ${OPENEXR_INCLUDE_DIR}/OpenEXR)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(OpenEXR
    REQUIRED_VARS
        OPENEXR_INCLUDE_DIRS
        OPENEXR_LIBRARIES
    VERSION_VAR
        OPENEXR_VERSION
)

