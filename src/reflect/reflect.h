#pragma once

#include <tint/tint.h>
#include <webgpu/webgpu.h>

class WGSLReflector 
{

public:
    explicit WGSLReflector();

    ~WGSLReflector() = default;
public:
    void ReflectShaderAttributes( 
        const std::string& code,
        const std::string& entry_point,
        uint8_t& attribute_count,
        std::unordered_map<std::string, WGPUVertexAttribute>& attribute_map,
        std::vector<WGPUVertexAttribute>& att_ordered,
        std::unordered_map<uint32_t, std::string>& location_name_map,
        const tint::wgsl::reader::Options& parse_option
    ) const;

    void ReflectShaderUniforms(
        const std::string& code,
        const std::string& entry_point,
        const tint::inspector::PipelineStage& pipeline_stage,
        uint8_t& bind_group_count,
        std::unordered_map<uint32_t, std::vector<WGPUBindGroupLayoutEntry>>& group_id_with_binding_layout_map,
        std::unordered_map<uint32_t, std::vector<tint::inspector::ResourceBinding>>& group_id_with_resource_bindings_map,
        const tint::wgsl::reader::Options& parse_option
    ) const;
private:

}

using WGSLReflectorPtr = std::shared_ptr<WGSLReflector>;