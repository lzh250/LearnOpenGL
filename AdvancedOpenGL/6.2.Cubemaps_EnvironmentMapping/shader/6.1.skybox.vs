#version 450 core
layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    TexCoords = aPos;
    vec4 pos = projection * view * vec4(aPos, 1.0);
    gl_Position = pos.xyww; // z变成w值，欺骗深度缓冲，让它认为天空盒有着最大的深度值1.0（z分量会变为w / w = 1.0）
}  
