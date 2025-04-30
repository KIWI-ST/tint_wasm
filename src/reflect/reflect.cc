#include "reflect.h"

#include <tint/tint.h>
#include <webgpu/webgpu.h>

WGSLReflector::WGSLReflector() 
{
}

void WGSLReflector::ReflectShaderAttributes( 
    const std::string& code,
    const std::string& entry_point,
    uint8_t& attribute_count,
    std::unordered_map<std::string, WGPUVertexAttribute>& attribute_map,
    std::vector<WGPUVertexAttribute>& att_ordered,
    std::unordered_map<uint32_t, std::string>& location_name_map,
    const tint::wgsl::reader::Options& parse_option
) const
{
    tint::Source::File file("", code);
    tint::Program program = tint::wgsl::reader::Parse(&file, parse_option);
    if (!program.IsValid()) {
        return;
    }
    tint::inspector::Inspector inspector(program);

    const auto& entry = inspector.GetEntryPoint(entry_point);
    if (entry.stage != tint::inspector::PipelineStage::kVertex) {
        LOGE("[E][Util][ReflectShaderAttribute] attribute reflect take effect only at stage: %s.", entry.stage);
        return;
    }

    attribute_count = entry.input_variables.size();

    for (uint8_t i = 0; i < attribute_count; i++) {
        tint::inspector::StageVariable variable = entry.input_variables[i];
        WGPUVertexAttribute attr = { };
        attr.shaderLocation = variable.attributes.location.value();
        attr.format =
            Util::GetInstance().GetWGPUVertexFormatByCompositionComponent(variable.composition_type, variable.component_type);
        // Wait Emit Stage
        attr.offset = 0;
        attribute_map.emplace(variable.name, attr);
        att_ordered.emplace_back(attr);
        location_name_map.emplace(attr.shaderLocation, variable.name);
    }

    auto compareAttribute = [ ] (const WGPUVertexAttribute& att1, const WGPUVertexAttribute& att2)
        {
            return att1.shaderLocation < att2.shaderLocation;
        };

    std::sort(att_ordered.begin(), att_ordered.end(), compareAttribute);
}

void WGSLReflector::ReflectShaderUniforms(
    const std::string& code,
    const std::string& entry_point,
    const tint::inspector::PipelineStage& pipeline_stage,
    uint8_t& bind_group_count,
    std::unordered_map<uint32_t, std::vector<WGPUBindGroupLayoutEntry>>& group_id_with_binding_layout_map,
    std::unordered_map<uint32_t, std::vector<tint::inspector::ResourceBinding>>& group_id_with_resource_bindings_map,
    const tint::wgsl::reader::Options& parse_option
) const
{
    auto getBindGroupLayoutEntries = [&group_id_with_binding_layout_map] (const uint32_t& group_id) -> std::vector<WGPUBindGroupLayoutEntry>&
    {
        if (group_id_with_binding_layout_map.find(group_id) == group_id_with_binding_layout_map.end()) {
            std::vector<WGPUBindGroupLayoutEntry> bind_group_entries;
            group_id_with_binding_layout_map.emplace(group_id, bind_group_entries);
        }
        return group_id_with_binding_layout_map.find(group_id)->second;
    };

    auto getResourceBindingsByGroupID = [&group_id_with_resource_bindings_map] (const uint32_t& group_id) -> std::vector<tint::inspector::ResourceBinding>&
    {
        if (group_id_with_resource_bindings_map.find(group_id) == group_id_with_resource_bindings_map.end()) {
            std::vector<tint::inspector::ResourceBinding> resource_bindings;
            group_id_with_resource_bindings_map.emplace(group_id, resource_bindings);
        }
        return group_id_with_resource_bindings_map.find(group_id)->second;
    };

    tint::Source::File file("", code);
    tint::Program program = tint::wgsl::reader::Parse(&file, parse_option);
    if (!program.IsValid()) {
        LOGE("[E][Util][ReflectShaderUniforms] shader vaild failed. error: %s.", program.Diagnostics().Str().c_str());
        return;
    }

    tint::inspector::Inspector inspector(program);

    const auto& entry = inspector.GetEntryPoint(entry_point);

    if (entry.stage != pipeline_stage) {
        uint8_t stage_code = static_cast<uint8_t>(entry.stage);
        LOGE("[E][Util][ReflectShaderUniforms] unsupported pipeline stage, stage code: %d.", stage_code);
        return;
    }

    WGPUShaderStage shader_stage = WGPUShaderStage_None;
    switch (pipeline_stage) 
    {
        case tint::inspector::PipelineStage::kVertex:
        {
            // DEBUG, support shader stage vertex
            shader_stage = WGPUShaderStage_Vertex;
            break;
        }
        case tint::inspector::PipelineStage::kFragment:
        {
            shader_stage = WGPUShaderStage_Fragment;
            break;
        }
        case tint::inspector::PipelineStage::kCompute:
        {
            shader_stage = WGPUShaderStage_Compute;
            break;
        }
        default:
        {
            LOGE("[E][Util][ReflectShaderUniforms] convert WGPUShaderStageFlags visibility error.");
            break;
        }
    }

    const std::vector<tint::inspector::ResourceBinding>& bindings = inspector.GetResourceBindings(entry_point);

    // check texture use with sampler or not ?
    // textureLoad needs sampleType UnfilterableFloat
    // otherwise Float
    auto sampler_texture_uses = inspector.GetSamplerTextureUses(entry_point);
    const auto CheckTextureUsesSampler = [ &sampler_texture_uses, &shader_stage ] (const uint32_t group, const uint32_t binding)
        {
            // WARNING:: compute shader not allow built-in function 'textureSampler', must without sampler use.
            if (WGPUShaderStage_Compute == shader_stage) {
                return false;
            }
            if (sampler_texture_uses.empty()) {
                return false;
            }
            for (const auto& use : sampler_texture_uses) {
                if (use.texture_binding_point.group == group && use.texture_binding_point.binding == binding) {
                    return true;
                }
            }
            return false;
        };

    for (const tint::inspector::ResourceBinding& binding : bindings) {
        auto& group = getBindGroupLayoutEntries(binding.bind_group);
        auto& resource_bindings = getResourceBindingsByGroupID(binding.bind_group);
        WGPUBindGroupLayoutEntry bind_group_layout_entry { };
        bind_group_layout_entry.binding = binding.binding;
        bind_group_layout_entry.visibility = shader_stage;
        tint::inspector::ResourceBinding::ResourceType t = binding.resource_type;
        switch (t) 
        {
            // https://github.com/gpuweb/gpuweb/issues/4132, read-only storage buffer access from vertex shader should be allowed.
            // https://github.com/gpuweb/gpuweb/pull/4133, allow reading storage buffers in vertex shaders.
            case tint::inspector::ResourceBinding::ResourceType::kReadOnlyStorageBuffer:
            case tint::inspector::ResourceBinding::ResourceType::kStorageBuffer:
            case tint::inspector::ResourceBinding::ResourceType::kUniformBuffer:
            {
                bind_group_layout_entry.buffer.type = GetBufferBindingTypeByResourceType(binding.resource_type);
                // byte length of component type
                bind_group_layout_entry.buffer.minBindingSize = binding.size;
                group.emplace_back(bind_group_layout_entry);
                resource_bindings.emplace_back(binding);
                break;
            }
            case tint::inspector::ResourceBinding::ResourceType::kDepthTexture:
            case tint::inspector::ResourceBinding::ResourceType::kSampledTexture:
            {
                bind_group_layout_entry.texture.viewDimension =
                    GetTextureViewDimensionByTextureDimFormat(static_cast<TEXTURE_DIMENSION_FORMAT>(binding.dim));
                // FIXED:: https://github.com/KIWI-ST/issues/2
                const bool is_sampler_use = CheckTextureUsesSampler(binding.bind_group, binding.binding);
                if (tint::inspector::ResourceBinding::ResourceType::kDepthTexture == t) {
                    bind_group_layout_entry.texture.sampleType = WGPUTextureSampleType_Depth;
                } else {
                    bind_group_layout_entry.texture.sampleType = GetTextureSampleType(is_sampler_use, binding.sampled_kind, binding.image_format);
                }
                group.emplace_back(bind_group_layout_entry);
                resource_bindings.emplace_back(binding);
                break;
            }
            case tint::inspector::ResourceBinding::ResourceType::kWriteOnlyStorageTexture:
            case tint::inspector::ResourceBinding::ResourceType::kReadOnlyStorageTexture:
            case tint::inspector::ResourceBinding::ResourceType::kReadWriteStorageTexture:
            {
                if (tint::inspector::ResourceBinding::ResourceType::kWriteOnlyStorageTexture == t) {
                    bind_group_layout_entry.storageTexture.access = WGPUStorageTextureAccess_WriteOnly;
                } else if (tint::inspector::ResourceBinding::ResourceType::kReadOnlyStorageTexture == t) {
                    bind_group_layout_entry.storageTexture.access = WGPUStorageTextureAccess_ReadOnly;
                } else if (tint::inspector::ResourceBinding::ResourceType::kReadWriteStorageTexture == t) {
                    bind_group_layout_entry.storageTexture.access = WGPUStorageTextureAccess_ReadWrite;
                } else {
                    uint32_t type_code = static_cast<uint32_t>(t);
                    LOGE("[E][Util][ReflectShaderUniforms] unsupported resourcebinding type, type code: %d.", type_code);
                }
                bind_group_layout_entry.storageTexture.format = GetTextureFormatByTintTexelFormat(binding.image_format);
                bind_group_layout_entry.storageTexture.viewDimension =
                    GetTextureViewDimensionByTextureDimFormat(static_cast<TEXTURE_DIMENSION_FORMAT>(binding.dim));
                group.emplace_back(bind_group_layout_entry);
                resource_bindings.emplace_back(binding);
                break;
            }
            case tint::inspector::ResourceBinding::ResourceType::kSampler:
            {
                if (WGPUShaderStage_Compute == shader_stage) {
                    bind_group_layout_entry.sampler.type = WGPUSamplerBindingType_NonFiltering;
                } else {
                    bind_group_layout_entry.sampler.type = WGPUSamplerBindingType_Filtering;
                }
                bind_group_layout_entry.binding = binding.binding;
                bind_group_layout_entry.visibility = shader_stage;
                // TODO:: other type of sampler binding type
            
                group.emplace_back(bind_group_layout_entry);
                resource_bindings.emplace_back(binding);
                break;
            }
            default:
            {
                uint8_t type_code = static_cast<uint8_t>(binding.resource_type);
                LOGE("[E][Util][ReflectShaderUniforms] unsupported tint resource type %d.", type_code);
                break;
            }
        }
    }

    // multi bind group layouts recomposite accroding to vector index
    bind_group_count = group_id_with_binding_layout_map.size();
}