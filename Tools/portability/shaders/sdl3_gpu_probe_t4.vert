#version 450

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec4 in_diffuse_bgra;
layout(location = 2) in vec2 in_texture_coordinate;
layout(location = 3) in vec2 in_second_texture_coordinate;

layout(location = 0) out vec4 vertex_color;
layout(location = 1) out vec2 texture_coordinate;

void main()
{
    gl_Position = vec4(in_position, 1.0);
    vertex_color = in_diffuse_bgra.bgra;
    texture_coordinate = in_second_texture_coordinate;
}
