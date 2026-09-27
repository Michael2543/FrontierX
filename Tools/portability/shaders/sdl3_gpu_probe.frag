#version 450

layout(location = 0) in vec4 vertex_color;
layout(location = 1) in vec2 texture_coordinate;
layout(location = 2) in vec4 vertex_specular;
layout(location = 3) in vec2 second_texture_coordinate;
layout(location = 0) out vec4 out_color;

layout(set = 2, binding = 0) uniform sampler2D color_texture;
layout(set = 3, binding = 0) uniform TextureStageState
{
    uvec4 operations;
    uvec4 arguments;
    vec4 factor;
} stage;

vec4 argument(uint selector, vec4 texel)
{
    return selector == 2u ? texel : selector == 3u ? stage.factor : vertex_color;
}

vec4 combine(uint operation, vec4 first, vec4 second)
{
    if (operation == 2u) return first;
    if (operation == 3u) return second;
    return first * second;
}

void main()
{
    vec2 selected_coordinate = stage.operations.w == 1u ?
        second_texture_coordinate : texture_coordinate;
    vec4 texel = texture(color_texture, selected_coordinate);
    vec4 color = combine(stage.operations.x,
        argument(stage.arguments.x, texel), argument(stage.arguments.y, texel));
    vec4 alpha = combine(stage.operations.y,
        argument(stage.arguments.z, texel), argument(stage.arguments.w, texel));
    vec3 specular = stage.operations.z == 1u ? vertex_specular.rgb : vec3(0.0);
    out_color = vec4(clamp(color.rgb + specular, 0.0, 1.0), alpha.a);
}
