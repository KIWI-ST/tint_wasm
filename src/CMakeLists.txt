collect_group_all_target_files(${CMAKE_CURRENT_SOURCE_DIR} SOURCEFILES)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_library(tint_wasm STATIC ${SOURCEFILES})

# copy from:
# https://github.com/kainino0x/webgpu-cross-platform-demo/blob/main/CMakeLists.txt

set(PORT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../third_party/dawn/src/emdawnwebgpu/pkg)

add_compile_options(
    "-fno-exceptions"
    "$<$<CONFIG:Release>:-flto>"
    "--use-port=${PORT_DIR}/emdawnwebgpu.port.py"
    "-Wall"
    "-Wextra"
)
add_link_options(
    "-fno-exceptions"
    "$<$<CONFIG:Release>:-flto>"
)

target_link_options(tint_wasm PRIVATE
    # "-sUSE_GLFW=3"
    "-sJSPI=1"
    "-sASYNCIFY=2"
    "-sUSE_WEBGPU=0"
    "--use-port=${PORT_DIR}/emdawnwebgpu.port.py"
    "--closure-args=--externs=${PORT_DIR}/webgpu/src/webgpu-externs.js"
    "-sEXIT_RUNTIME=0"
    "$<$<CONFIG:Debug>:-sASSERTIONS=1>"
    "$<$<CONFIG:Debug>:-Wno-limited-postlink-optimizations>"
    "$<$<CONFIG:Release>:--closure=1>"
)

#TODO:: remove tint
target_link_libraries(tint_wasm PRIVATE
    tint_lang_wgsl_inspector 
    tint_lang_wgsl_reader
)