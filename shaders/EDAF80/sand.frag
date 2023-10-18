#version 410

uniform vec3 light_position;
uniform vec3 camera_position;

uniform vec3 ambient_colour;
uniform vec3 diffuse_colour;
uniform vec3 specular_colour;
uniform float shininess_value;
uniform mat4 normal_model_to_world;

uniform sampler2D diffuse_map;
uniform sampler2D normal_map;
uniform sampler2D specular_map;

vec3 ka;
vec3 kd;
vec3 ks;
vec3 normal;
vec3 N;

in VS_OUT {
    vec3 vertex;
    vec3 normal;
    vec2 text_coord;
    vec3 tangents;
    vec3 binormal;
    mat3 TBN;
} fs_in;

out vec4 frag_color;

void main()
{
    /*normal = texture(normal_map, fs_in.text_coord).rgb;
    normal = normal*2.0 - 1.0;
    N = normalize(fs_in.TBN * normal);*/
    vec3 n = normalize(texture(normal_map,fs_in.text_coord).xyz*2.0-1.0);
    N = (normal_model_to_world * vec4(fs_in.TBN * n,0)).xyz;
    
    kd = texture(diffuse_map, fs_in.text_coord).rgb;
    ks = texture(specular_map, fs_in.text_coord).rgb;
    
    vec3 L = normalize(light_position - fs_in.vertex);
    vec3 V = normalize(camera_position - fs_in.vertex);
    
    vec3 diffuse = kd  * max(dot(N, L), 0.0);
    vec3 specular = ks * pow(max(dot(reflect(-L,N),V),0.0),shininess_value);
    
    frag_color.xyz = ambient_colour + diffuse + specular;
    frag_color.w = 1.0;
}
