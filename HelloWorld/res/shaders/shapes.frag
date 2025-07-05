#version 450 core

in vec3 ourColor;
in vec2 ourTexCoord;
uniform sampler2D wall;

void main()
{
    //gl_FragColor = vec4(ourColor, 1.0f);
    gl_FragColor = texture(wall, ourTexCoord) * vec4(ourColor, 0.1f);
}
