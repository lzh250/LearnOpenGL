#include "lzhopenglwidget.h"

#include <QKeyEvent>
#include <QTime>
// #include <QElapsedTimer>

QVector3D cubePositions[] = {
    QVector3D( 0.0f,  0.0f,   0.0f),
    QVector3D( 2.0f,  5.0f, -15.0f),
    QVector3D(-1.5f, -2.2f,  -2.5f),
    QVector3D(-3.8f, -2.0f, -12.3f),
    QVector3D( 2.4f, -0.4f,  -3.5f),
    QVector3D(-1.7f,  3.0f,  -7.5f),
    QVector3D( 1.3f, -2.0f,  -2.5f),
    QVector3D( 1.5f,  2.0f,  -2.5f),
    QVector3D( 1.5f,  0.2f,  -1.5f),
    QVector3D(-1.3f,  1.0f,  -1.5f)
};

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
};

unsigned int VBO, VAO;
unsigned int texture1, texture2;
int texture_location;
unsigned int shader_program;
unsigned int vertex_shader, fragment_shader;

// QElapsedTimer elapsed_timer;

// const char *vertex_shader_source_old_1 = R"(
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
const char *vertex_shader_source_old_2 = R"(
    #version 450 core
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec2 aTexCoord;

    out vec2 ourTexCoord;

    uniform float radian;
    uniform float width;
    uniform float height;
    uniform vec3 rotationAxis;
    uniform mat4 perspective;
    uniform vec3 cubePosition;

    void main()
    {
        float length = sqrt(rotationAxis[0]*rotationAxis[0] + rotationAxis[1]*rotationAxis[1] + rotationAxis[2]*rotationAxis[2]);
        if (length < 1e-7) length = 1;
        float inverseNorm = 1.0f / length;
        float sinhalfangle = sin(0.5 * radian);
        float coshalfangle = cos(0.5 * radian);
        float x = rotationAxis[0] * sinhalfangle * inverseNorm;
        float y = rotationAxis[1] * sinhalfangle * inverseNorm;
        float z = rotationAxis[2] * sinhalfangle * inverseNorm;
        float w = coshalfangle;
        vec3 quaternion = vec3(x, y, z);
        vec3 uv = cross(quaternion, aPos);
        vec3 uuv = cross(quaternion, uv);
        uv *= (2.0f * w);
        uuv *= 2.0f;
        vec3 model = aPos + uv + uuv;

        // 正方形不透视投影
        // gl_Position = vec4(model + vec3(0.5f, -0.5f, -0.0f), 1.0f);

        // 自己算的透视投影
        // 应该在CPU方计算，因为r和t，因为r和t不需要每次都计算。
        // 之前纹理有问题，插值后纹理怪怪的，后找到原因，x和y值算错了，错写成x = view[0] * 1 / r / view[2];y = view[1] * 1 / t / view[2];
        // vec3 view = model + vec3(0.0f, 0.0f, -3.0f);
        // float imgAspectRatio = width / height;
        // float scale = 0.4142135623730950488016887242097; // tan(angle_of_view * 0.5 * Pi / 180) * n
        // float r = imgAspectRatio * scale;
        // float t = scale;
        // x = view[0] * 1 / r;
        // y = view[1] * 1 / t;
        // z = -view[2] * (100 + 1) / (100 - 1) - 2 * 100 * 1 / (100 - 1);
        // gl_Position = vec4(x, y, z, -view[2]);

        // 透视投影矩阵
        // 之前顶点有问题，正方形变成长方形，找到原因，Qt方面，在初始化成功之前获取的窗口宽高，永远100x30导致。
        vec4 view = vec4(model + vec3(0.0f, 0.0f, -3.0f) + cubePosition, 1.0f);
        gl_Position = perspective * view;

         ourTexCoord = aTexCoord;
})";

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
    uniform vec3 rotationAxis;
    uniform mat4 view;
    uniform mat4 perspective;
    uniform vec3 cubePosition;

    void main()
    {
        float length = sqrt(rotationAxis[0]*rotationAxis[0] + rotationAxis[1]*rotationAxis[1] + rotationAxis[2]*rotationAxis[2]);
        if (length < 1e-7) length = 1;
        float inverseNorm = 1.0f / length;
        float sinhalfangle = sin(0.5 * radian);
        float coshalfangle = cos(0.5 * radian);
        float x = rotationAxis[0] * sinhalfangle * inverseNorm;
        float y = rotationAxis[1] * sinhalfangle * inverseNorm;
        float z = rotationAxis[2] * sinhalfangle * inverseNorm;
        float w = coshalfangle;
        vec3 quaternion = vec3(x, y, z);
        vec3 uv = cross(quaternion, aPos);
        vec3 uuv = cross(quaternion, uv);
        uv *= (2.0f * w);
        uuv *= 2.0f;
        vec3 model = aPos + uv + uuv;

        // 透视投影矩阵
        // 之前顶点有问题，正方形变成长方形，找到原因，Qt方面，在初始化成功之前获取的窗口宽高，永远100x30导致。
        vec4 m = vec4(model + cubePosition, 1.0f);
        gl_Position = perspective * view * m;

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
    setFocusPolicy(Qt::StrongFocus);
    setMouseTracking(true);

    cam_pos   = QVector3D(0.0f, 0.0f,  3.0f);
    cam_front = QVector3D(0.0f, 0.0f, -1.0f);
    cam_up    = QVector3D(0.0f, 1.0f,  0.0f);

    timer.setInterval(10);
    connect(&timer, &QTimer::timeout, this, [&]() {
        makeCurrent();
        radian += 1.0/360*M_PI;

        //静态欧拉角方式,x轴旋转：

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

        doneCurrent();
        update();
    });
    timer.start();
    // elapsed_timer.start();
}

LzhOpenGLWidget::~LzhOpenGLWidget()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
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

    glEnable(GL_DEPTH_TEST);

    vertex_shader   = glCreateShader(GL_VERTEX_SHADER);
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

    glUniform3f(glGetUniformLocation(shader_program, "rotationAxis"), 1.0f, 0.3f, 0.5f);

    /*
     * 关于 glBindTexture(GL_TEXTURE_2D, texture1) 函数，
     * initializeGL 中的 glBindTexture 只是为了设置属性，并不关心最新的活动单元是谁，所以没必要在之前调用 glActiveTexture。
     * paintGL 中的 glBindTexture 是渲染时用，需要先通过 glActiveTexture 指定纹理单元，再通过 glBindTexture 使之前设定好的这个纹理去影响最新指定的纹理单元。
     * 估计 glActiveTexture glBindTexture 这种麻烦的设计，是为了实现着色器中的单元与纹理设定的灵活性。可以方便地控制一个着色器代码变更不同的纹理。
     * 其实单元是单元，纹理是纹理，不是一一对应的，可以灵活更换，这个很多人可能被网上泛滥的有毒蠢文给搞得不清晰了。
     * glBindTexture 只能说设置属性时需要bind，渲染时也需要bind，但需要先指定要影响哪个纹理单元，然后再bind。设置属性时的bind不需要关心影响了哪个纹理单元，所以不需要明确指定。
     * 另外 glActiveTexture 参数 GL_TEXTURE<0 - 32> 对应 作色器 中 SamplerX 变量的值 <0 - 32> 。
     */

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

    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2);
    glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    QImage image2(":/res/images/awesomeface.png");
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image2.width(), image2.height(), 0, GL_BGRA, GL_UNSIGNED_BYTE, image2.mirrored().constBits());
    glGenerateMipmap(GL_TEXTURE_2D);

    glUniform1i(glGetUniformLocation(shader_program, "container"), 0);
    glUniform1i(glGetUniformLocation(shader_program, "awesomeface"), 1);

    glCreateBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glCreateVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (const void *)0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (const void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
}

void LzhOpenGLWidget::resizeGL(int w, int h)
{
    Perspective();
    glUniformMatrix4fv(glGetUniformLocation(shader_program, "perspective"), 1, GL_FALSE, perspective.data());
}

void LzhOpenGLWidget::paintGL()
{
    glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(shader_program);           // [must]

    // float elapsed_time = (float)elapsed_timer.elapsed();
    // QVector3D eye(sin(elapsed_time / 1000) * 15, 0.0f, cos(elapsed_time / 1000) * 15);
    // QVector3D center;
    // QVector3D up(0.0f, 1.0f, 0.0f);
    // QMatrix4x4 view = LookAt(eye, center, up);
    QMatrix4x4 view = LookAt(cam_pos, cam_pos + cam_front, cam_up);
    glUniformMatrix4fv(glGetUniformLocation(shader_program, "view"), 1, GL_FALSE, view.constData());

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture2);

    //glBindVertexArray(VAO);
    //glDrawElements(shape_type, 6, GL_UNSIGNED_INT, (const void *)0);
    //glDrawArrays(GL_QUADS, 0, 8);

    for (auto &item : cubePositions) {
        glUniform3f(glGetUniformLocation(shader_program, "cubePosition"), item[0], item[1], item[2]);
        glDrawArrays(shape_type, 0, 36);
    }
}

void LzhOpenGLWidget::mouseMoveEvent(QMouseEvent *event)
{
    QPoint pos = event->pos();

    if (first_mouse)
    {
        last_x = pos.x();
        last_y = pos.y();
        first_mouse = false;
    }

    float x_offset = pos.x() - last_x;
    float y_offset = last_y - pos.y();
    last_x = pos.x();
    last_y = pos.y();

    x_offset *= 0.05f;
    y_offset *= 0.05f;

    yaw   += x_offset;
    pitch += y_offset;

    if (pitch >  89.0f) pitch =  89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    QVector3D front;
    // 默认朝向是世界X轴方向(不考虑俯仰)，yaw应该初始化为-90
    front[0] = cos(pitch / 180 * M_PI) * cos(yaw / 180 * M_PI);
    front[1] = sin(pitch / 180 * M_PI);
    front[2] = cos(pitch / 180 * M_PI) * sin(yaw / 180 * M_PI);
    // 默认朝向是世界Z轴负方向(不考虑俯仰)，yaw应该初始化为0
    // front[0] = cos(pitch / 180 * M_PI) * sin(yaw / 180 * M_PI);
    // front[1] = sin(pitch / 180 * M_PI);
    // front[2] = -cos(pitch / 180 * M_PI) * cos(yaw / 180 * M_PI);
    // 默认朝向是世界Z轴方向(不考虑俯仰)，yaw应该初始化为180
    // front[0] = -cos(pitch / 180 * M_PI) * sin(yaw / 180 * M_PI);
    // front[1] = sin(pitch / 180 * M_PI);
    // front[2] = cos(pitch / 180 * M_PI) * cos(yaw / 180 * M_PI);

    cam_front = front.normalized();
}

void LzhOpenGLWidget::wheelEvent(QWheelEvent *event)
{
    QPoint numPixels = event->pixelDelta();
    QPoint numDegrees = event->angleDelta() / 8;

    if (!numPixels.isNull())
    {
        fov -= numPixels.y();
    }
    else if (!numDegrees.isNull()) {
        QPoint numSteps = numDegrees / 15;
        fov -= numSteps.y();
    }
    if(fov <= 1.0f)
        fov = 1.0f;
    if(fov >= 45.0f)
        fov = 45.0f;

    makeCurrent();
    Perspective();
    glUniformMatrix4fv(glGetUniformLocation(shader_program, "perspective"), 1, GL_FALSE, perspective.data());
    doneCurrent();

    event->accept();
}

void LzhOpenGLWidget::keyPressEvent(QKeyEvent *event)
{
    switch (event->key())
    {
    case Qt::Key_W:
        cam_pos += cam_front * 0.05f;
        break;
    case Qt::Key_S:
        cam_pos -= cam_front * 0.05f;
        break;
    case Qt::Key_A:
        cam_pos -= QVector3D::crossProduct(cam_front, cam_up) * 0.05f;
        break;
    case Qt::Key_D:
        cam_pos += QVector3D::crossProduct(cam_front, cam_up) * 0.05f;
        break;
    default:
        break;
        makeCurrent();
        update();
        doneCurrent();
    }
}

QMatrix4x4 LzhOpenGLWidget::LookAt(QVector3D &eye, const QVector3D &center, const QVector3D &up)
{
    QVector3D f = (center - eye).normalized();
    QVector3D s = QVector3D::crossProduct(f, up).normalized();
    QVector3D u = QVector3D::crossProduct(s, f);

    QMatrix4x4 view;
    view(0, 0) = s.x();
    view(0, 1) = s.y();
    view(0, 2) = s.z();
    view(0, 3) = -QVector3D::dotProduct(s, eye);
    view(1, 0) = u.x();
    view(1, 1) = u.y();
    view(1, 2) = u.z();
    view(1, 3) = -QVector3D::dotProduct(u, eye);
    view(2, 0) = -f.x();
    view(2, 1) = -f.y();
    view(2, 2) = -f.z();
    view(2, 3) =  QVector3D::dotProduct(f, eye);
    view(3, 0) = 0.0f;
    view(3, 1) = 0.0f;
    view(3, 2) = 0.0f;
    view(3, 3) = 1.0f;

    return view;
}

void LzhOpenGLWidget::Perspective()
{
    float scale = tan(fov * 0.5f * M_PI / 180) * 1.0f;
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

    perspective(2, 0) = 0.0f;
    perspective(2, 1) = 0.0f;
    perspective(2, 2) = -(100.0f + 1.0f) / (100.0f - 1.0f);
    perspective(2, 3) = -2 * 100.0 * 1.0 / (100.0f - 1.0f);

    perspective(3, 0) = 0.0f;
    perspective(3, 1) = 0.0f;
    perspective(3, 2) = -1;
    perspective(3, 3) = 0.0f;
}
