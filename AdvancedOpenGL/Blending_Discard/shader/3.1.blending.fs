#version 450 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture1;

void main()
{
    vec4 texColor = texture(texture1, TexCoords);
    if(texColor.a < 0.1)
        discard;
    if (texColor.r > 0.5 && texColor.g > 0.5 && texColor.b > 0.5)
        discard;
    FragColor = texColor;
}
