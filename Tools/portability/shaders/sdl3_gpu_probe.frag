#version 450

layout(location = 0) in vec3 vertex_color;
layout(location = 1) in vec2 texture_coordinate;
layout(location = 0) out vec4 out_color;

layout(set = 2, binding = 0) uniform sampler2D color_texture;

void main()
{
    out_color = texture(color_texture, texture_coordinate) * vec4(vertex_color, 1.0);
}
