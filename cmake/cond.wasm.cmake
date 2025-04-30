include(cmake/utils.cmake)

add_definitions(-D__EMSCRIPTEN__)

set(CMAKE_POLICY_DEFAULT_CMP0091 NEW)
set(CMAKE_POLICY_DEFAULT_CMP0092 NEW)
set(DAWN_FETCH_DEPENDENCIES ON)

set(THIRD_PARTY_DPES
    third_party/dawn
)
foreach(THIRD_PARTY_DPE ${THIRD_PARTY_DPES})
    add_subdirectory(${THIRD_PARTY_DPE})
endforeach()