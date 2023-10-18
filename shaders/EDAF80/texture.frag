#version 410

in VS_OUT {
    vec2 vertex;
} fs_in;

uniform sampler2D image;
out vec4 frag_color;

void main()
{
    frag_color = texture(image, fs_in.vertex);
}
