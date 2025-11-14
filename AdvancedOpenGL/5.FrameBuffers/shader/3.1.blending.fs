#version 450 core

in vec2 TexCoords;

uniform sampler2D texture1;

const float offset = 1.0 / 300.0;

void main()
{
    //============================================================================================
    // 相反色
    //
    // vec4 texColor = texture(texture1, TexCoords);
    // gl_FragColor = vec4(vec3(1.0 - texture(texture1, TexCoords)), 1.0);

    //============================================================================================
    // 灰度图
    // 人眼会对绿色更加敏感一些，而对蓝色不那么敏感，所以为了获取物理上更精确的效果，我们需要使用加权的(Weighted)通道
    //
    // vec4 texColor = texture(texture1, TexCoords);
    // // if(texColor.a < 0.1)
    // //     discard;
    // FragColor = texColor;
    // float average = 0.2126 * FragColor.r + 0.7152 * FragColor.g + 0.0722 * FragColor.b;
    // FragColor = vec4(average, average, average, 1.0);


    //============================================================================================
    // 核效果
    vec2 offsets[9] = vec2[](
        vec2(-offset,  offset), // 左上
        vec2( 0.0f,    offset), // 正上
        vec2( offset,  offset), // 右上
        vec2(-offset,  0.0f),   // 左
        vec2( 0.0f,    0.0f),   // 中
        vec2( offset,  0.0f),   // 右
        vec2(-offset, -offset), // 左下
        vec2( 0.0f,   -offset), // 正下
        vec2( offset, -offset)  // 右下
    );

    // 锐化
    // float kernel[9] = {
    //     -1, -1, -1,
    //     -1,  9, -1,
    //     -1, -1, -1
    // };
    // 锐化
    // float kernel[9] = {
    //     2,   2, 2,
    //     2, -15, 2,
    //     2,   2, 2
    // };
    // 模糊
    //float kernel[9] = float[](
    //    1.0 / 16, 2.0 / 16, 1.0 / 16,
    //    2.0 / 16, 4.0 / 16, 2.0 / 16,
    //    1.0 / 16, 2.0 / 16, 1.0 / 16
    //);
    // 边缘检测
    float kernel[9] = {
        2,   2, 2,
        2, -15, 2,
        2,   2, 2
    };

    vec3 sampleTex[9];
    for(int i = 0; i < 9; i++)
    {
        sampleTex[i] = vec3(texture(texture1, TexCoords.st + offsets[i]));
    }
    vec3 col = vec3(0.0);
    for(int i = 0; i < 9; i++)
        col += sampleTex[i] * kernel[i];

    gl_FragColor = vec4(col, 1.0);
}
