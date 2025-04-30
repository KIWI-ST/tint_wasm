#include "reflect.h"

#include <memory>
#include <vector>
#include <unordered_map>
#include <string>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <stdio.h>

#include <tint/tint.h>
#include <webgpu/webgpu.h>

void PrintLog(const char format [], ...)
{
    va_list args;
    va_start(args, format);
    vfprintf(stdout, format, args);
    va_end(args);
    fprintf(stdout, "\n");
}

void PrintError(const char format [], ...)
{
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    fprintf(stderr, "\n");
}

#define LOGI(...) PrintLog(__VA_ARGS__);
#define LOGE(...) PrintError(__VA_ARGS__);

WGSLReflector::WGSLReflector( )
{
}

const WGPUVertexFormat WGSLReflector::GetWGPUVertexFormatByCompositionComponent(
    const tint::inspector::CompositionType& composition_type,
    const tint::inspector::ComponentType& component_type
) const
{
    if (composition_type == tint::inspector::CompositionType::kScalar) {
        if (component_type == tint::inspector::ComponentType::kF32) {
            return WGPUVertexFormat_Float32;
        } else if (component_type == tint::inspector::ComponentType::kI32) {
            return WGPUVertexFormat_Sint32;
        } else if (component_type == tint::inspector::ComponentType::kU32) {
            return WGPUVertexFormat_Uint32;
        }
    } else if (composition_type == tint::inspector::CompositionType::kVec2) {
        if (component_type == tint::inspector::ComponentType::kF16) {
            return WGPUVertexFormat_Float16x2;
        } else if (component_type == tint::inspector::ComponentType::kF32) {  // invalid type
            return WGPUVertexFormat_Float32x2;
        } else if (component_type == tint::inspector::ComponentType::kU32) {
            return WGPUVertexFormat_Uint32x2;
        }
    } else if (composition_type == tint::inspector::CompositionType::kVec3) {
        if (component_type == tint::inspector::ComponentType::kF32) {  // invalid type
            return WGPUVertexFormat_Float32x3;
        } else if (component_type == tint::inspector::ComponentType::kI32) {
            return WGPUVertexFormat_Sint32x3;
        } else if (component_type == tint::inspector::ComponentType::kU32) {
            return WGPUVertexFormat_Uint32x3;
        }
    } else if (composition_type == tint::inspector::CompositionType::kVec4) {
        if (component_type == tint::inspector::ComponentType::kF16) {
            return WGPUVertexFormat_Float16x4;
        } else if (component_type == tint::inspector::ComponentType::kF32) {  // invalid type
            return WGPUVertexFormat_Float32x4;
        } else if (component_type == tint::inspector::ComponentType::kI32) {
            return WGPUVertexFormat_Sint32x4;
        } else if (component_type == tint::inspector::ComponentType::kU32) {
            return WGPUVertexFormat_Uint32x4;
        }
    }
    uint32_t composition_type_code = static_cast<uint32_t>(composition_type);
    uint32_t component_type_code = static_cast<uint32_t>(component_type);
    LOGE(
        "[E][Util][GetWGPUVertexFormatByCompositionComponent] unsupport webgpu component format, soruce composition type code:%d. compontent type code: %d.",
        composition_type_code,
        component_type_code
    );
    return WGPUVertexFormat::WGPUVertexFormat_Force32;
}

const WGPUTextureFormat WGSLReflector::GetTextureFormatByTintTexelFormat(
    const tint::inspector::ResourceBinding::TexelFormat& tint_texel_format) const
{
    switch (tint_texel_format) {
    case tint::inspector::ResourceBinding::TexelFormat::kBgra8Unorm:
    return WGPUTextureFormat_BGRA8Unorm;
    case tint::inspector::ResourceBinding::TexelFormat::kRgba8Unorm:
    return WGPUTextureFormat_RGBA8Unorm;
    case tint::inspector::ResourceBinding::TexelFormat::kRgba8Snorm:
    return WGPUTextureFormat_RGBA8Snorm;
    case tint::inspector::ResourceBinding::TexelFormat::kRgba8Uint:
    return WGPUTextureFormat_RGBA8Uint;
    case tint::inspector::ResourceBinding::TexelFormat::kRgba8Sint:
    return WGPUTextureFormat_RGBA8Sint;
    case tint::inspector::ResourceBinding::TexelFormat::kRgba16Uint:
    return WGPUTextureFormat_RGBA16Uint;
    case tint::inspector::ResourceBinding::TexelFormat::kRgba16Sint:
    return WGPUTextureFormat_RGBA16Sint;
    case tint::inspector::ResourceBinding::TexelFormat::kRgba16Float:
    return WGPUTextureFormat_RGBA16Float;
    case tint::inspector::ResourceBinding::TexelFormat::kR32Uint:
    return WGPUTextureFormat_R32Uint;
    case tint::inspector::ResourceBinding::TexelFormat::kR32Sint:
    return WGPUTextureFormat_R32Sint;
    case tint::inspector::ResourceBinding::TexelFormat::kR32Float:
    return WGPUTextureFormat_R32Float;
    case tint::inspector::ResourceBinding::TexelFormat::kRg32Uint:
    return WGPUTextureFormat_R32Uint;
    case tint::inspector::ResourceBinding::TexelFormat::kRg32Sint:
    return WGPUTextureFormat_RG32Sint;
    case tint::inspector::ResourceBinding::TexelFormat::kRg32Float:
    return WGPUTextureFormat_RG32Float;
    case tint::inspector::ResourceBinding::TexelFormat::kRgba32Uint:
    return WGPUTextureFormat_RGBA32Uint;
    case tint::inspector::ResourceBinding::TexelFormat::kRgba32Sint:
    return WGPUTextureFormat_RGBA32Sint;
    case tint::inspector::ResourceBinding::TexelFormat::kRgba32Float:
    return WGPUTextureFormat_RGBA32Float;
    default:
    uint32_t type_code = static_cast<uint32_t>(tint_texel_format);
    LOGE("[E][Util][GetTextureViewDimensionByTintTexelFormat] unsupport texture dimension. error code: %d.", type_code);
    return WGPUTextureFormat_RGBA8Unorm;
    }
}

const WGPUBufferBindingType WGSLReflector::GetBufferBindingTypeByResourceType(
    const tint::inspector::ResourceBinding::ResourceType& resource_type
) const
{
         // read only storage buffer, e.g: var<storage, read>
    if (tint::inspector::ResourceBinding::ResourceType::kReadOnlyStorageBuffer == resource_type) {
        return WGPUBufferBindingType_ReadOnlyStorage;
    }
    // read_write storage buffer, e.g: var<storage, read_write>
    else if (tint::inspector::ResourceBinding::ResourceType::kStorageBuffer == resource_type) {
        return WGPUBufferBindingType_Storage;
    }
    // read buffer, e.g: var<uniform>
    else if (resource_type == tint::inspector::ResourceBinding::ResourceType::kUniformBuffer) {
        return WGPUBufferBindingType_Uniform;
    } else {
        uint32_t type_code = static_cast<uint32_t>(resource_type);
        LOGE("[E][Util][GetBufferBindingTypeByResourceType] unsupport tint::inspector::ResourceBinding::ResourceType type, type code: %d.", type_code);
        return WGPUBufferBindingType_BindingNotUsed;
    }
}

const WGPUTextureSampleType WGSLReflector::GetTextureSampleType(
    const bool is_sampler_use,
    const tint::inspector::ResourceBinding::SampledKind& tint_sampler_kind,
    const tint::inspector::ResourceBinding::TexelFormat& texel_format
) const
{
    switch (tint_sampler_kind) {
    case tint::inspector::ResourceBinding::SampledKind::kUInt:
    return WGPUTextureSampleType_Uint;
    case tint::inspector::ResourceBinding::SampledKind::kSInt:
    return WGPUTextureSampleType_Sint;
    default:
    return is_sampler_use ? WGPUTextureSampleType_Float : WGPUTextureSampleType_UnfilterableFloat;
    }
}

const WGPUTextureViewDimension WGSLReflector::GetTextureViewDimensionByTextureDimFormat(
    const tint::inspector::ResourceBinding::TextureDimension& texture_dim) const
{
    switch (texture_dim) {
    case tint::inspector::ResourceBinding::TextureDimension::k1d:
    return WGPUTextureViewDimension_1D;
    case tint::inspector::ResourceBinding::TextureDimension::k2d:
    return WGPUTextureViewDimension_2D;
    case tint::inspector::ResourceBinding::TextureDimension::k2dArray:
    return WGPUTextureViewDimension_2DArray;
    case tint::inspector::ResourceBinding::TextureDimension::k3d:
    return WGPUTextureViewDimension_3D;
    case tint::inspector::ResourceBinding::TextureDimension::kCube:
    return WGPUTextureViewDimension_Cube;
    case tint::inspector::ResourceBinding::TextureDimension::kCubeArray:
    return WGPUTextureViewDimension_CubeArray;
    default:
    return WGPUTextureViewDimension_2D;
    }
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
    if (!program.IsValid( )) {
        return;
    }
    tint::inspector::Inspector inspector(program);

    const auto& entry = inspector.GetEntryPoint(entry_point);
    if (entry.stage != tint::inspector::PipelineStage::kVertex) {
        LOGE("[E][Util][ReflectShaderAttribute] attribute reflect take effect only at stage: %s.", entry.stage);
        return;
    }

    attribute_count = entry.input_variables.size( );

    for (uint8_t i = 0; i < attribute_count; i++) {
        tint::inspector::StageVariable variable = entry.input_variables[i];
        WGPUVertexAttribute attr = { };
        attr.shaderLocation = variable.attributes.location.value( );
        attr.format = GetWGPUVertexFormatByCompositionComponent(variable.composition_type, variable.component_type);
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

    std::sort(att_ordered.begin( ), att_ordered.end( ), compareAttribute);
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
    auto getBindGroupLayoutEntries = [ &group_id_with_binding_layout_map ] (const uint32_t& group_id) -> std::vector<WGPUBindGroupLayoutEntry>&
    {
        if (group_id_with_binding_layout_map.find(group_id) == group_id_with_binding_layout_map.end( )) {
            std::vector<WGPUBindGroupLayoutEntry> bind_group_entries;
            group_id_with_binding_layout_map.emplace(group_id, bind_group_entries);
        }
        return group_id_with_binding_layout_map.find(group_id)->second;
    };

    auto getResourceBindingsByGroupID = [ &group_id_with_resource_bindings_map ] (const uint32_t& group_id) -> std::vector<tint::inspector::ResourceBinding>&
    {
        if (group_id_with_resource_bindings_map.find(group_id) == group_id_with_resource_bindings_map.end( )) {
            std::vector<tint::inspector::ResourceBinding> resource_bindings;
            group_id_with_resource_bindings_map.emplace(group_id, resource_bindings);
        }
        return group_id_with_resource_bindings_map.find(group_id)->second;
    };

    tint::Source::File file("", code);
    tint::Program program = tint::wgsl::reader::Parse(&file, parse_option);
    if (!program.IsValid( )) {
        LOGE("[E][Util][ReflectShaderUniforms] shader vaild failed. error: %s.", program.Diagnostics( ).Str( ).c_str( ));
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
    switch (pipeline_stage) {
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
        if (sampler_texture_uses.empty( )) {
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
        switch (t) {
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
                GetTextureViewDimensionByTextureDimFormat(binding.dim);
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
                GetTextureViewDimensionByTextureDimFormat(binding.dim);
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
    bind_group_count = group_id_with_binding_layout_map.size( );
}

