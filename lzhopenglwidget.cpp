#include "lzhopenglwidget.h"

#include <QTime>
#include <QMatrix4x4>

float vertices[] = {
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
    0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
    0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

    0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
    0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f


    ,1.0f,  0.5f, 0.0f,  0.0f, 1.0f,
    2.0f,  0.5f, 0.0f,  1.0f, 1.0f,
    2.0f,  -0.5f, 0.0f,  1.0f, 0.0f,
    1.0f,  -0.5f, 0.0f,  0.0f, 0.0f,
    1.0f,  0.5f, 0.0f,  0.0f, 1.0f,
    2.0f,  -0.5f, 0.0f,  1.0f, 0.0f
};

unsigned int VBO, VAO;
unsigned int texture1, texture2;
int texture_location;
unsigned int shader_program;
unsigned int vertex_shader, fragment_shader;

// const char *vertex_shader_source_old = R"(
//     #version 450 core
//     layout (location = 0) in vec3 aPos;
//     layout (location = 1) in vec2 aTexCoord;

//     out vec2 ourTexCoord;

//     uniform mat4 transform;

//     void main()
//     {
//         gl_Position = transform * vec4(aPos, 1.0f);
//         ourTexCoord = aTexCoord;
// })";

//https://blog.csdn.net/csxiaoshui/article/details/65445633
//四元数
const char *vertex_shader_source = R"(
    #version 450 core
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec2 aTexCoord;

    out vec2 ourTexCoord;

    uniform float radian;
    uniform float width;
    uniform float height;
    uniform mat4 transform;

    void main()
    {
        float sinhalfangle = sin(0.5 * radian);
        float coshalfangle = cos(0.5 * radian);
        float x = sinhalfangle;
        float y = 0.0f;
        float z = 0.0f;
        float w = coshalfangle;
        vec3 quaternion = vec3(x, y, z);
        vec3 uv = cross(quaternion, aPos);
        vec3 uuv = cross(quaternion, uv);
        uv *= (2.0f * w);
        uuv *= 2.0f;
        vec3 model = aPos + uv + uuv;

        // 正方形不投影透视
        // gl_Position = vec4(model + vec3(0.5f, -0.5f, -0.0f), 1.0f);

        // 自己算的投影透视
        // 应该在CPU方计算，因为r和t，因为r和t不需要每次都计算。
        // 之前纹理有问题，插值后纹理怪怪的，后找到原因，x和y值算错了
        vec3 view = model + vec3(0.0f, 0.0f, -3.0f);
        float imgAspectRatio = width / height;
        float scale = 0.4142135623730950488016887242097; // tan(angle_of_view * 0.5 * Pi / 180) * n
        float r = imgAspectRatio * scale;
        float t = scale;
        x = view[0] * 1 / r;
        y = view[1] * 1 / t;
        z =0;// -view[2] * (100 + 1) / (100 - 1) - 2 * 100 * 1 / (100 - 1);
        gl_Position = vec4(x, y, z, -view[2]);

        // 网上找的投影透视矩阵
        // 之前顶点有问题，正方形变成长方形，找到原因，Qt方面，在初始化成功之前获取的窗口宽高，永远100x30导致。
        // vec4 view = vec4(model + vec3(0.0f, 0.0f, -3.0f), 1.0f);
        // vec4 ok = transform * view;
        // gl_Position = ok;

         ourTexCoord = aTexCoord;
})";

const char *frament_shader_source = R"(
#version 450 core

in vec2 ourTexCoord;
uniform sampler2D containner;
uniform sampler2D awesomeface;

void main()
{
    gl_FragColor = mix(texture(containner, ourTexCoord), texture(awesomeface, ourTexCoord), 0.5);
})";

LzhOpenGLWidget::LzhOpenGLWidget(QWidget *parent) : QOpenGLWidget(parent)
{
    timer.setInterval(10);
    connect(&timer, &QTimer::timeout, this, [&]() {
        Perspective();
        makeCurrent();
        radian += 1.0/360*M_PI;

        //静态欧拉角方式：

        // QMatrix4x4 matrix;
        // // matrix.translate(0.5f, -0.5f);
        // // matrix.rotate(angle, 0.0f, 0.0f, 1.0f);
        // QMatrix4x4 matrix1, matrixx, matrixy, matrixz;
        // matrix1(0, 0) = 1.0f; matrix1(0, 1) = 0.0f; matrix1(0, 2) = 0.0f; matrix1(0, 3) = 0.5f;
        // matrix1(1, 0) = 0.0f; matrix1(1, 1) = 1.0f; matrix1(1, 2) = 0.0f; matrix1(1, 3) = -0.5f;
        // matrix1(2, 0) = 0.0f; matrix1(2, 1) = 0.0f; matrix1(2, 2) = 1.0f; matrix1(2, 3) = 0.0f;
        // matrix1(3, 0) = 0.0f; matrix1(3, 1) = 0.0f; matrix1(3, 2) = 0.0f; matrix1(3, 3) = 1.0f;

        // // 如果绕x轴旋转，用这个
        // // matrixx(0, 0) = 1.0f; matrixx(0, 1) = 0.0f; matrixx(0, 2) = 0.0f; matrixx(0, 3) = 0.0f;
        // // matrixx(1, 0) = 0.0f; matrixx(1, 1) = cos(radian); matrixx(1, 2) = -sin(radian); matrixx(1, 3) = 0.0f;
        // // matrixx(2, 0) = 0.0f; matrixx(2, 1) = sin(radian); matrixx(2, 2) = cos(radian); matrixx(2, 3) = 0.0f;
        // // matrixx(3, 0) = 0.0f; matrixx(3, 1) = 0.0f; matrixx(3, 2) = 0.0f; matrixx(3, 3) = 1.0f;

        // // 如果绕y轴旋转，用这个
        // // matrixy(0, 0) = cos(radian); matrixy(0, 1) = 0.0f; matrixy(0, 2) = sin(radian); matrixy(0, 3) = 0.0f;
        // // matrixy(1, 0) = 0.0f; matrixy(1, 1) =  1.0f; matrixy(1, 2) = 0.0f; matrixy(1, 3) = 0.0f;
        // // matrixy(2, 0) = -sin(radian); matrixy(2, 1) = 0.0f; matrixy(2, 2) = cos(radian); matrixy(2, 3) = 0.0f;
        // // matrixy(3, 0) = 0.0f; matrixy(3, 1) = 0.0f; matrixy(3, 2) = 0.0f; matrixy(3, 3) = 1.0f;

        // // 如果绕z轴旋转，用这个
        // matrixz(0, 0) = cos(radian); matrixz(0, 1) = -sin(radian); matrixz(0, 2) = 0.0f; matrixz(0, 3) = 0.0f;
        // matrixz(1, 0) = sin(radian); matrixz(1, 1) = cos(radian); matrixz(1, 2) = 0.0f; matrixz(1, 3) = 0.0f;
        // matrixz(2, 0) = 0.0f; matrixz(2, 1) = 0.0f; matrixz(2, 2) = 1.0f; matrixz(2, 3) = 0.0f;
        // matrixz(3, 0) = 0.0f; matrixz(3, 1) = 0.0f; matrixz(3, 2) = 0.0f; matrixz(3, 3) = 1.0f;

        // // 如果绕任意轴旋转，需要先算出欧拉角，具体还没研究

        // matrix = matrix1 *  matrixz;
        // glUniformMatrix4fv(glGetUniformLocation(shader_program, "transform"), 1, GL_FALSE, matrix.data());
        // doneCurrent();
        // update();


        // 动态欧拉角方式，还没研究

        // 四元数方式
        // 细节没研究，按照网上给定的结论直接拿来用的：
        /*
         *  1. 确定旋转轴和旋转角度：假设旋转轴为单位向量 [x,y,z]，旋转角度为 θ（弧度）。
         *  2. 计算四元数的实部 w 和虚部 [x,y,z]：
         *     实部 w=cos(θ/2)。
         *     虚部 x=x⋅sin(θ/2)，y=y⋅sin((θ/2)，z=z⋅sin(θ/2)。
         *  3. 将计算得到的 w,x,y,z 组合成四元数 q=[w,x,y,z]。
        */
        glUniform1f(glGetUniformLocation(shader_program, "radian"), radian);
        glUniform1f(glGetUniformLocation(shader_program, "width"), width());
        glUniform1f(glGetUniformLocation(shader_program, "height"), height());

        glUniformMatrix4fv(glGetUniformLocation(shader_program, "transform"), 1, GL_FALSE, perspective.data());
        doneCurrent();
        update();
    });
    timer.start();
}

LzhOpenGLWidget::~LzhOpenGLWidget()
{
    // if (q_texture != nullptr)
    // {
    //     delete q_texture;
    //     q_texture = nullptr;
    // }
}

void LzhOpenGLWidget::DrawRectangle()
{
    shape_type = GL_TRIANGLES;
    is_draw = true;
    update();
}

void LzhOpenGLWidget::Clear()
{
    is_draw = false;
    update();
}

void LzhOpenGLWidget::SetPolygonMode(bool is_wire)
{
    makeCurrent();
    if (is_wire)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else
        glPolygonMode(GL_BACK, GL_FILL);
    doneCurrent();

    update();
}

void LzhOpenGLWidget::initializeGL()
{
    initializeOpenGLFunctions();

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL);
    glShaderSource(fragment_shader, 1, &frament_shader_source, NULL);
    glCompileShader(vertex_shader);
    glCompileShader(fragment_shader);

    shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);
    int success = true;
    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    if (success == 0) {
        char info_log[512] = {0};
        glGetProgramInfoLog(shader_program, 512, NULL, info_log);
        qDebug() << info_log;
    }

    glUseProgram(shader_program);   // [must] 不然mix无效果

    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);
    glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    QImage image(":/res/images/container.jpg");
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image.width(), image.height(), 0, GL_BGRA, GL_UNSIGNED_BYTE, image.mirrored().constBits());
    glGenerateMipmap(GL_TEXTURE_2D);
    glUniform1i(glGetUniformLocation(shader_program, "container"), 0);

    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2);
    glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    QImage image2(":/res/images/awesomeface.png");
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image2.width(), image2.height(), 0, GL_BGRA, GL_UNSIGNED_BYTE, image2.mirrored().constBits());
    glGenerateMipmap(GL_TEXTURE_2D);
    glUniform1i(glGetUniformLocation(shader_program, "awesomeface"), 1);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
}

void LzhOpenGLWidget::resizeGL(int w, int h)
{

}

void LzhOpenGLWidget::paintGL()
{
    glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shader_program);           // [must]
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture2);
    //glBindVertexArray(VAO);
    //glDrawElements(shape_type, 6, GL_UNSIGNED_INT, (const void *)0);
    //glDrawArrays(GL_QUADS, 0, 8);
    glDrawArrays(shape_type, 0, 42);
}

void LzhOpenGLWidget::Perspective()
{
    float scale = tan(45 * 0.5f * M_PI / 180) * 1.0f;
    float imgAspectRatio = (float)width() / height();
    float r = imgAspectRatio * scale;
    float t = scale;

    perspective(0, 0) = 1.0f / r;
    perspective(0, 1) = 0.0f;
    perspective(0, 2) = 0.0f;
    perspective(0, 3) = 0.0f;

    perspective(1, 0) = 0.0f;
    perspective(1, 1) = 1.0f / t;
    perspective(1, 2) = 0;
    perspective(1, 3) = 0;

    // perspective(2, 0) = (r + l) / (r - l);
    // perspective(2, 1) = (t + b) / (t - b);
    // perspective(2, 2) = -(100.0f + 1.0f) / (100.0f - 1.0f);
    // perspective(2, 3) = -1.0f;
    // perspective(3, 0) = 0.0f;
    // perspective(3, 1) = 0.0f;
    // perspective(3, 2) = -2 * 100.0 * 0.1 / (100.0f - 1.0f);
    // perspective(3, 3) = 0.0f;    perspective(2, 0) = (r + l) / (r - l);

    perspective(2, 0) = 0.0f;
    perspective(2, 1) = 0.0f;
    perspective(2, 2) = -(100.0f + 1.0f) / (100.0f - 1.0f);
    perspective(2, 3) = -2 * 100.0 * 1.0 / (100.0f - 1.0f);

    perspective(3, 0) = 0.0f;
    perspective(3, 1) = 0.0f;
    perspective(3, 2) = -1;
    perspective(3, 3) = 0.0f;
}
