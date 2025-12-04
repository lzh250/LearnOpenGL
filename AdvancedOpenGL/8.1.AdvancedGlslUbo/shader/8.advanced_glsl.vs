#version 450 core
layout (location = 0) in vec3 aPos;

layout (std140, binding = 0) uniform Matrices   // 绑定点不是必须从0开始
{
    mat4 projection;
    mat4 view;
};
uniform mat4 model;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}  
