#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/html5.h>
#endif

#ifdef __WIN32__
#include <wtypes.h>
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include <reflect/reflect.h>

#ifdef __WIN32__
// int APIENTRY WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd) 
int main(int argc, char** argv)
{
#elif __EMSCRIPTEN__
EMSCRIPTEN_KEEPALIVE int main(int argc, char** argv)
{
#endif

#ifdef __WIN32__
    _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE);
    _CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDERR);
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

    std::string shader_code = R"(
            
            struct Vertex
            {
                px:f32,
                py:f32,
                pz:f32,
                nx:f32,
                ny:f32,
                nz:f32,
                tx:f32,
                ty:f32
            };

            struct ModelViewProjectMatrices
	        {
	            project: mat4x4<f32>,
	            view: mat4x4<f32>,
	            model: mat4x4<f32>,
	        };

            struct FRAGMENT_INPUT
            {
                @builtin(position) position:vec4<f32>,
                @interpolate(flat) @location(0) traingle_id:u32,
            };

            @group(0) @binding(0)
            var<uniform> modelViewProjectMatrix:ModelViewProjectMatrices;

            @group(0) @binding(1)
            var<storage, read> vertices: array<Vertex>;

            @vertex
            fn vs_main(@builtin(vertex_index) vertexIndex:u32, @builtin(instance_index) instanceIndex:u32) -> FRAGMENT_INPUT
            {
                var f: FRAGMENT_INPUT;
                let mat = modelViewProjectMatrix.project * modelViewProjectMatrix.view * modelViewProjectMatrix.model;
                let position = vec4<f32>(vertices[vertexIndex].px, vertices[vertexIndex].py, vertices[vertexIndex].pz, 1.0);
                f.position = mat * position;
                f.traingle_id = vertexIndex;
                return f;
            }
        
        )";

    uint8_t attribute_count;
    std::unordered_map<std::string, WGPUVertexAttribute> attribute_map;
    std::vector<WGPUVertexAttribute> att_ordered;
    std::unordered_map<uint32_t, std::string> location_name_map;
    tint::wgsl::reader::Options parse_option {};

    reflect::WGSLReflectorPtr reflector = std::make_shared<reflect::WGSLReflector>( );
    reflector->ReflectShaderAttributes(
        shader_code, 
        "vs_main", 
        attribute_count, 
        attribute_map, 
        att_ordered, 
        location_name_map, 
        parse_option
    );

    std::unordered_map<uint32_t, std::vector<WGPUBindGroupLayoutEntry>> group_id_with_binding_layout_map;
    std::unordered_map<uint32_t, std::vector<tint::inspector::ResourceBinding>> group_id_with_resource_bindings_map;
    reflector->ReflectShaderUniforms(
        shader_code, 
        "vs_main",
        tint::inspector::PipelineStage::kVertex,
        attribute_count,
        group_id_with_binding_layout_map,
        group_id_with_resource_bindings_map,
        parse_option
    );

    return 0;
}
