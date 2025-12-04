#ifndef LZHOPENGLWIDGET_H
#define LZHOPENGLWIDGET_H

#include "shader.h"

#include <QTimer>
#include <QOpenGLWidget>
#include <QOpenGLFunctions_4_5_Core>
#include <QMatrix4x4>

class LzhOpenGLWidget : public QOpenGLWidget, QOpenGLFunctions_4_5_Core
{
public:
    LzhOpenGLWidget(QWidget *parent);
    virtual ~LzhOpenGLWidget();

    // QOpenGLWidget interface
protected:
    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();

    // QWidget interface
protected:
    void mouseMoveEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);
    void keyPressEvent(QKeyEvent *event);

private:
    QMatrix4x4 LookAt( QVector3D &eye, const QVector3D &center, const QVector3D &up);
    void Perspective();
    QVector4D MakeQuaternion(QVector3D rotation_axis, double radian);

    unsigned int LoadTexture(char const *path);
    unsigned int LoadCubemap(vector<std::string> faces);

private:
    unsigned int cube_VAO, cube_VBO;

    unsigned int ubo_matrices;

    Shader shader_red, shader_green, shader_blue, shader_yellow;
    QMatrix4x4 model_red, model_green, model_blue, model_yellow;

    QMatrix4x4 perspective;

    // for LookAt
    QVector3D cam_pos;
    QVector3D cam_front;
    QVector3D cam_up;

    float fov = 45.0f;

    bool first_mouse = true;
    float yaw = -90.0f;
    float pitch = 0.0f;
};

#endif // LZHOPENGLWIDGET_H

/*
布局规则的原文可以在OpenGL的Uniform缓冲规范这里找到，但我们将会在下面列出最常见的规则。GLSL中的每个变量，比如说int、float和bool，都被定义为4字节量。每4个字节将会用一个N来表示。

类型	布局规则
标量，比如int和bool	每个标量的基准对齐量为N。
向量	2N或者4N。这意味着vec3的基准对齐量为4N。
标量或向量的数组	每个元素的基准对齐量与vec4的相同。
矩阵	储存为列向量的数组，每个向量的基准对齐量与vec4的相同。
结构体	等于所有元素根据规则计算后的大小，但会填充到vec4大小的倍数。
和OpenGL大多数的规范一样，使用例子就能更容易地理解。我们会使用之前引入的那个叫做ExampleBlock的Uniform块，并使用std140布局计算出每个成员的对齐偏移量：

layout (std140) uniform ExampleBlock
{
                     // 基准对齐量        // 对齐偏移量
    float value;     // 4               // 0
    vec3 vector;     // 16              // 16  (必须是16的倍数，所以 4->16)
    mat4 matrix;     // 16              // 32  (列 0)
                     // 16              // 48  (列 1)
                     // 16              // 64  (列 2)
                     // 16              // 80  (列 3)
    float values[3]; // 16              // 96  (values[0])
                     // 16              // 112 (values[1])
                     // 16              // 128 (values[2])
    bool boolean;    // 4               // 144
    int integer;     // 4               // 148
};
作为练习，尝试去自己计算一下偏移量，并和表格进行对比。使用计算后的偏移量值，根据std140布局的规则，我们就能使用像是glBufferSubData的函数将变量数据按照偏移量填充进缓冲中了。虽然std140布局不是最高效的布局，但它保证了内存布局在每个声明了这个Uniform块的程序中是一致的。
*/
