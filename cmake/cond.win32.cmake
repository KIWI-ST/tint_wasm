include(cmake/utils.cmake)

add_definitions(-D__WIN32__)

set(CMAKE_POLICY_DEFAULT_CMP0091 NEW)
set(CMAKE_POLICY_DEFAULT_CMP0092 NEW)
# set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Od")

# set(gtest_force_shared_crt on)
set(DAWN_FETCH_DEPENDENCIES ON)
set(ABSL_MSVC_STATIC_RUNTIME OFF)
set(DAWN_ENABLE_VULKAN ON)
set(DAWN_ENABLE_D3D12 OFF)
set(DAWN_ENABLE_D3D11 OFF)
set(DAWN_ENABLE_INSTALL OFF)
set(TINT_ENABLE_INSTALL OFF)
set(DAWN_BUILD_MONOLITHIC_LIBRARY ON)
# set(ASSIMP_BUILD_ZLIB ON)
# set(ASSIMP_WARNINGS_AS_ERRORS OFF)

# add thrid_party dependencies
set(THIRD_PARTY_DPES
    third_party/dawn
)
foreach(THIRD_PARTY_DPE ${THIRD_PARTY_DPES})
    add_subdirectory(${THIRD_PARTY_DPE})
endforeach()

# regroup project into named floder
foreach(THIRD_PARTY_DPE ${THIRD_PARTY_DPES})
    do_collect_all_targets_recursive(TARGET_DEPS ${THIRD_PARTY_DPE})
    foreach(TARGET_DEP ${TARGET_DEPS})
        # set_output_location("${TARGET_DEP}" "${PRODUCT_PATH}")
        if(WIN32 AND MSVC AND CMAKE_GENERATOR MATCHES "Visual Studio")
            # disable empty sturct limit
            add_compile_options(/Zc:trigraphs-)
            # pack projects to folder in visual studio
            set_property(TARGET ${TARGET_DEP} PROPERTY FOLDER "External")
        elseif(APPLE AND CMAKE_GENERATOR MATCHES "Xcode")
            # pack projects to folder in Xcode
            set_target_properties(${TARGET_DEP} PROPERTIES FOLDER "External")
        endif()
    endforeach()
endforeach()

