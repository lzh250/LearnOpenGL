#ifndef LZHOPENGLWIDGET_H
#define LZHOPENGLWIDGET_H

#include "shader.h"

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

private:
    Shader       shader, screen_shader;
    unsigned int cube_vao, cube_vbo;
    unsigned int quad_vao, quad_vbo;
    unsigned int framebuffer;
    unsigned int intermediate_fbo;
    unsigned int texture_color_buffer_multi_sampled;
    unsigned int screen_texture;

    // for LookAt
    QVector3D cam_pos;
    QVector3D cam_front;
    QVector3D cam_up;

    QMatrix4x4 perspective;
    float far = 100.0f;
    float near = 0.1f;
    float fov = 45.0f;

    bool first_mouse = true;
    float yaw = -90.0f;
    float pitch = 0.0f;
};

#endif // LZHOPENGLWIDGET_H
