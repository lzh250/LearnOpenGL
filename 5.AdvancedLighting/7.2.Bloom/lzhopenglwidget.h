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
    void keyReleaseEvent(QKeyEvent *event);

private:
    void RenderCube();
    void RenderQuad();

    QMatrix4x4 LookAt( QVector3D &eye, const QVector3D &center, const QVector3D &up);
    QMatrix4x4 Perspective(float fov, float aspect, float near, float far);
    QVector4D MakeQuaternion(QVector3D rotation_axis, double radian);

    unsigned int LoadTexture(const char *path);

private:
    Shader       shader, shader_light, shader_blur, shader_bloom_final;
    unsigned int wood_texture, container_exture;
    unsigned int hdr_fbo, pingpong_fbos[2];
    unsigned int color_buffers[2];
    unsigned int pingpong_color_buffers[2];
    unsigned int rbo_depth;
    QVector<QVector3D> light_positions;
    QVector<QVector3D> light_colors;
    unsigned int cube_vao, cube_vbo;
    unsigned int quad_vao, quad_vbo;

    // bool         b_key_released = true;
    float        exposure = 1.0f;

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
