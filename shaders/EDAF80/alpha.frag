#version 410

in VS_OUT {
    vec2 vertex;
} fs_in;

uniform sampler2D image;
out vec4 frag_color;

void main()
{
    float a = texture(image, fs_in.vertex).a;
    frag_color.rgb = texture(image, fs_in.vertex).rgb;
    frag_color.a = 1;
}
