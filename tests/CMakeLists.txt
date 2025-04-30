collect_group_all_target_files(${CMAKE_CURRENT_SOURCE_DIR} SOURCEFILES)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(WIN32)
   add_executable(unit_test ${SOURCEFILES})
   target_link_libraries(unit_test PUBLIC tint_wasm)
elseif(EMSCRIPTEN)
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
    add_executable(unit_test ${SOURCEFILES})
    target_link_libraries(unit_test PUBLIC tint_wasm)
    add_compile_options(        
        "-Wall"
        "-Wextra"
    )
    target_link_options(unit_test PUBLIC
        "-sUSE_GLFW=3"
        "-sJSPI=1"
        "-sASYNCIFY=2"
        "-sUSE_WEBGPU=0"
        "-sEXIT_RUNTIME=0"
        "-sDEFAULT_LIBRARY_FUNCS_TO_INCLUDE=$getWasmTableEntry"
        # "--use-port=${PORT_DIR}/emdawnwebgpu.port.py"
        # "--closure-args=--externs=${PORT_DIR}/webgpu/src/webgpu-externs.js"
        # "$<$<CONFIG:Debug>:-sASSERTIONS=1>"
        # "$<$<CONFIG:Debug>:-Wno-limited-postlink-optimizations>"
        # "$<$<CONFIG:Release>:--closure=1>"
    )
endif()
