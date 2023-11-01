
# A very minimal cmake script that demonstrates finding an OpenUSD installation,
# and building and installing a simple application program.

cmake_minimum_required(VERSION 3.26)
project(UsdFindPackageExample CXX)

# the location OpenUSD was installed to can be passed to cmake via the
# CMAKE_PREFIX_PATH variable. The path will be the one that includes
# cmake/pxrTargets.cmake

find_package(pxr REQUIRED)
find_package(MaterialX)

# work around code signing issues per https://gitlab.kitware.com/cmake/cmake/-/issues/21854
set(CMAKE_XCODE_ATTRIBUTE_OTHER_CODE_SIGN_FLAGS "-o linker-signed")

add_executable(UsdFindPackageExample "UsdAppEXample.cpp")

# In order to compile, the c++ standard must be set to c++17 or greater.
target_compile_features(UsdFindPackageExample PRIVATE cxx_std_17)

target_link_libraries(UsdFindPackageExample ${PXR_LIBRARIES} ${MATERIALX_LIBRARIES})

set_target_properties(UsdFindPackageExample
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_INSTALL_PREFIX}/bin"
)

install(
    TARGETS UsdFindPackageExample
    RUNTIME DESTINATION bin)
