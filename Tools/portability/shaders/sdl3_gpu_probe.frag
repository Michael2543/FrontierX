#version 450

layout(location = 0) in vec4 vertex_color;
layout(location = 1) in vec2 texture_coordinate;
layout(location = 0) out vec4 out_color;

layout(set = 2, binding = 0) uniform sampler2D color_texture;
layout(set = 3, binding = 0) uniform TextureStageState
{
    uvec4 operations;
    uvec4 arguments;
} stage;

vec4 argument(uint selector, vec4 texel)
{
    return selector == 2u ? texel : vertex_color;
}

vec4 combine(uint operation, vec4 first, vec4 second)
{
    if (operation == 2u) return first;
    if (operation == 3u) return second;
    return first * second;
}

void main()
{
    vec4 texel = texture(color_texture, texture_coordinate);
    vec4 color = combine(stage.operations.x,
        argument(stage.arguments.x, texel), argument(stage.arguments.y, texel));
    vec4 alpha = combine(stage.operations.y,
        argument(stage.arguments.z, texel), argument(stage.arguments.w, texel));
    out_color = vec4(color.rgb, alpha.a);
}
