#version 450

layout(location = 0) in vec4 vertex_color;
layout(location = 1) in vec2 texture_coordinate;
layout(location = 2) in vec4 vertex_specular;
layout(location = 3) in vec2 second_texture_coordinate;
layout(location = 0) out vec4 out_color;

layout(set = 2, binding = 0) uniform sampler2D color_texture;
layout(set = 2, binding = 1) uniform sampler2D second_texture;
layout(set = 3, binding = 0) uniform TextureStageState
{
    uvec4 operations;
    uvec4 arguments;
    vec4 factor;
    uvec4 alpha_test;
    uvec4 second_operations;
    uvec4 second_arguments;
} stage;

vec4 argument(uint selector, vec4 texel, vec4 current)
{
    return selector == 2u ? texel : selector == 3u ? stage.factor :
        selector == 1u ? current : vertex_color;
}

vec4 combine(uint operation, vec4 first, vec4 second)
{
    if (operation == 7u) return clamp(first + second, 0.0, 1.0);
    if (operation == 2u) return first;
    if (operation == 3u) return second;
    return first * second;
}

bool alpha_passes(float alpha)
{
    float reference = float(stage.alpha_test.z) / 255.0;
    uint function = stage.alpha_test.y;
    if (function == 1u) return false;
    if (function == 2u) return alpha < reference;
    if (function == 3u) return alpha == reference;
    if (function == 4u) return alpha <= reference;
    if (function == 5u) return alpha > reference;
    if (function == 6u) return alpha != reference;
    if (function == 7u) return alpha >= reference;
    return true;
}

void main()
{
    vec2 selected_coordinate = stage.operations.w == 1u ?
        second_texture_coordinate : texture_coordinate;
    vec4 texel = texture(color_texture, selected_coordinate);
    vec4 color = combine(stage.operations.x,
        argument(stage.arguments.x, texel, vertex_color), argument(stage.arguments.y, texel, vertex_color));
    vec4 alpha = combine(stage.operations.y,
        argument(stage.arguments.z, texel, vertex_color), argument(stage.arguments.w, texel, vertex_color));
    if (stage.operations.x == 1u) color = vertex_color;
    if (stage.operations.y == 1u) alpha = vertex_color;
    if (stage.operations.x != 1u && stage.second_operations.x != 1u)
    {
        vec4 current = vec4(color.rgb, alpha.a);
        vec2 coordinate = stage.second_operations.w == 1u ?
            second_texture_coordinate : texture_coordinate;
        vec4 second_texel = texture(second_texture, coordinate);
        color = combine(stage.second_operations.x,
            argument(stage.second_arguments.x, second_texel, current),
            argument(stage.second_arguments.y, second_texel, current));
        if (stage.second_operations.y != 1u)
            alpha = combine(stage.second_operations.y,
                argument(stage.second_arguments.z, second_texel, current),
                argument(stage.second_arguments.w, second_texel, current));
    }
    if (stage.alpha_test.x == 1u && !alpha_passes(alpha.a)) discard;
    vec3 specular = stage.operations.z == 1u ? vertex_specular.rgb : vec3(0.0);
    out_color = vec4(clamp(color.rgb + specular, 0.0, 1.0), alpha.a);
}
