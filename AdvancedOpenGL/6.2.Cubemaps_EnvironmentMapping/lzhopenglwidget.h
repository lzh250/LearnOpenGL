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

    unsigned int skybox_VAO, skybox_VBO;
    unsigned int cubemap_texture;

    Shader shader, skybox_shader;

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

// 点光源衰减

F_att = 1.0 / (K_c + K_i * d + K_q * d*d)

在这里d代表了片段距光源的距离。接下来为了计算衰减值，我们定义3个（可配置的）项：常数项Kc、一次项Kl和二次项Kq（constant，linear，quadratic）。
1. 常数项通常保持为1.0，它的主要作用是保证分母永远不会比1小，否则的话在某些距离上它反而会增加强度，这肯定不是我们想要的效果。
2. 一次项会与距离值相乘，以线性的方式减少强度。
3. 二次项会与距离的平方相乘，让光源以二次递减的方式减少强度。二次项在距离比较小的时候影响会比一次项小很多，但当距离值比较大的时候它就会比一次项更大了。

由于二次项的存在，光线会在大部分时候以线性的方式衰退，直到距离变得足够大，让二次项超过一次项，光的强度会以更快的速度下降。这样的结果就是，光在近距离时亮度很高，但随着距离变远亮度迅速降低，最后会以更慢的速度减少亮度。

但是，该对这三个项设置什么值呢？正确地设定它们的值取决于很多因素：环境、希望光覆盖的距离、光的类型等。在大多数情况下，这都是经验的问题，以及适量的调整。下面这个表格显示了模拟一个（大概）真实的，覆盖特定半径（距离）的光源时，这些项可能取的一些值。第一列指定的是在给定的三项时光所能覆盖的距离。这些值是大多数光源很好的起始点，它们由Ogre3D的Wiki所提供：
距离		常数项	一次项	二次项
7		1.0		0.7		1.8
13		1.0		0.35	0.44
20		1.0		0.22	0.20
32		1.0		0.14	0.07
50		1.0		0.09	0.032
65		1.0		0.07	0.017
100		1.0		0.045	0.0075
160		1.0		0.027	0.0028
200		1.0		0.022	0.0019
325		1.0		0.014	0.0007
600		1.0		0.007	0.0002
3250	1.0		0.0014	0.000007

*/

/*

// 聚光 - 软化边缘

我们可以用下面这个公式来计算这个值：
    I = (θ−γ) / ϵ
这里ϵ(Epsilon)是内（ϕ）和外圆锥（γ）之间的余弦值差（ϵ=ϕ−γ）。最终的I值就是在当前片段聚光的强度。

*/
