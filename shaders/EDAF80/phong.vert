#version 410

layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec3 text_coord;
layout (location = 3) in vec3 tangents;
layout (location = 4) in vec3 binormal;

uniform mat4 vertex_model_to_world;
uniform mat4 normal_model_to_world;
uniform mat4 vertex_world_to_clip;

out VS_OUT {
    vec3 vertex;
    vec3 normal;
    vec2 text_coord;
    mat3 TBN;
} vs_out;


void main()
{
    vs_out.vertex = vec3(vertex_model_to_world * vec4(vertex, 1.0));
    vs_out.normal = (normal_model_to_world * vec4(normal, 0.0)).xyz;
    vs_out.text_coord = text_coord.xy;

    vec3 T = normalize(vec3(vertex_model_to_world * vec4(tangents, 0.0)));
    vec3 B = normalize(vec3(vertex_model_to_world * vec4(binormal, 0.0)));
    vec3 N = normalize(vec3(normal_model_to_world * vec4(normal, 0.0)));
    vs_out.TBN = mat3(T, B, N);

    gl_Position = vertex_world_to_clip * vertex_model_to_world * vec4(vertex, 1.0);
}
