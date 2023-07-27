set(PXR_PREFIX pxr/usd)
set(PXR_PACKAGE usd)

find_package(glfw3 QUIET)
if (TARGET glfw OR TARGET glfw3)
    message(STATUS "Found glfw")
else()
    message(STATUS "Installing glfw")

    include(FetchContent)
    FetchContent_Declare(glfw
        GIT_REPOSITORY "https://github.com/glfw/glfw.git"
        GIT_TAG "master"
        GIT_SHALLOW ON)

    FetchContent_GetProperties(glfw)
    if(NOT glfw_POPULATED)
        FetchContent_Populate(glfw)
        set(BUILD_SHARED_LIBS OFF CACHE INTERNAL "shared")
        set(GLFW_BUILD_EXAMPLES OFF CACHE INTERNAL "glfw examples")
        set(GLFW_BUILD_TESTS OFF CACHE INTERNAL "glfw tests")
        set(GLFW_BUILD_DOCS OFF CACHE INTERNAL "glfw docs")
        set(GLFW_INSTALL ON CACHE INTERNAL "glfw install")
        set(GLFW_VULKAN_STATIC OFF CACHE INTERNAL "glfw vulkan") # "Assume the Vulkan loader is linked with the application"
        add_subdirectory(${glfw_SOURCE_DIR} ${glfw_BINARY_DIR})
    endif()
endif()

find_library(METAL_LIBRARY Metal REQUIRED)
find_library(METALKIT_LIBRARY MetalKit REQUIRED)
set(CORE_IMAGE "-framework CoreImage")
set(IMAGEIO "-framework ImageIO")
set(CORE_GRAPHICS "-framework CoreGraphics")
set(UNIFORMTYPEIDENTIFIERS_LIBRARY "-framework UniformTypeIdentifiers")

# QuartzCore is needed on mac only, but not on iOS
if (NOT IOS)
    set(QUARTZCORE "-framework QuartzCore")
else()
    set(QUARTZCORE "")
endif()

pxr_cpp_bin(usdhddemo
    INCLUDE_DIRS
 
    LIBRARIES
        arch
        glfw
        hd
        hdSt
        hgi
        hio
        sdf
        tf
        usd
        usdGeom
        usdImagingGL
        ${Boost_PYTHON_LIBRARY}
        ${Python3_LIBRARY}
        ${METAL_LIBRARY}
        ${METALKIT_LIBRARY}
        ${UNIFORMTYPEIDENTIFIERS_LIBRARY}
        ${CORE_IMAGE}
        ${CORE_GRAPHICS}
        ${IMAGEIO}
        ${QUARTZCORE}
)

#set(METAL_SOURCE BlitShaders.metal)
#set_source_files_properties(${METAL_SOURCE} PROPERTIES LANGUAGE METAL)

target_sources(usdhddemo
    PUBLIC
        main.mm
        ${METAL_SOURCE}
)
