#version 450 core

in vec2 texCoords;

uniform sampler2D tex;

void main()
{
    gl_FragColor = texture(tex, texCoords);
}
