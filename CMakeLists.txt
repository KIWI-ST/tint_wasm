cmake_minimum_required(VERSION 3.21)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# set(CMAKE_POLICY_DEFAULT_CMP0079 NEW)
# set(CMAKE_BUILD_TYPE "Debug")
# set(PRODUCT_PATH "${CMAKE_CURRENT_BINARY_DIR}/bin")

set(PROJECT_NAME "TINT_WASM")
project(
    ${PROJECT_NAME}
    DESCRIPTION "wgsl parser"
    LANGUAGES C CXX
    HOMEPAGE_URL "https://github.com/KIWI-ST/tint_wasm"
    VERSION 0.0.0
)

# Tint: C++ 20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(cmake/options.cmake)

# helper

if(EMSCRIPTEN)
    include(cmake/options.cmake)
    include(cmake/cond.wasm.cmake)
else()
    message("tint_wasm only supports wasm build")
endif()

# include nano renderer gems    
add_subdirectory(src)
