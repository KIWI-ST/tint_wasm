include(cmake/utils.cmake)

add_definitions(-D__EMSCRIPTEN__)

set(THIRD_PARTY_DPES
    third_party/dawn
)
foreach(THIRD_PARTY_DPE ${THIRD_PARTY_DPES})
    add_subdirectory(${THIRD_PARTY_DPE})
endforeach()