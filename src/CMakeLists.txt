collect_group_all_target_files(${CMAKE_CURRENT_SOURCE_DIR} SOURCEFILES)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_library(tint_wasm STATIC ${SOURCEFILES})

# copy from:
# https://github.com/kainino0x/webgpu-cross-platform-demo/blob/main/CMakeLists.txt

if(EMSCRIPTEN)
    # set(PORT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../third_party/dawn/src/emdawnwebgpu/pkg)
    set(PORT_DIR ${CMAKE_CURRENT_LIST_DIR}/../third_party/emdawnwebgpu_pkg)
    # target_link_directories(tint_wasm PRIVATE ${CMAKE_CURRENT_LIST_DIR}/../third_party/emdawnwebgpu_pkg/webgpu/include/)
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
        emdawnwebgpu_config
        emdawnwebgpu_c
        tint_lang_wgsl_inspector 
        tint_lang_wgsl_reader
    )
elseif(WIN32)
    target_link_libraries(tint_wasm PUBLIC
        webgpu_c
        glfw
        dawn_internal_config
        dawn::dawn_common
        dawn::dawn_system_utils
        dawn::dawn_wgpu_utils
        dawn::dawn_proc
        dawn::dawn_glfw
        dawn::dawn_native
        tint_lang_wgsl_inspector 
        tint_lang_wgsl_reader
    )
endif()


