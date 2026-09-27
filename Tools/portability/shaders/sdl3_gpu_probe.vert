#version 450

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec4 in_diffuse_bgra;
layout(location = 2) in vec2 in_texture_coordinate;

layout(location = 0) out vec4 vertex_color;
layout(location = 1) out vec2 texture_coordinate;
layout(location = 2) out vec4 vertex_specular;
layout(location = 3) out vec2 second_texture_coordinate;

layout(set = 1, binding = 0) uniform VertexTransform
{
    mat4 clip_from_object;
    vec4 uv_transform;
    vec4 color_modulation;
} transform_state;

void main()
{
    gl_Position = transform_state.clip_from_object * vec4(in_position, 1.0);
    vertex_color = in_diffuse_bgra.bgra;
    texture_coordinate = in_texture_coordinate;
    vertex_specular = vec4(0.0);
    second_texture_coordinate = in_texture_coordinate;
}
