#ifndef LZHOPENGLWIDGET_H
#define LZHOPENGLWIDGET_H

#include "model.h"

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

    // 加速插值
    float OurLerp(float a, float b, float f);

    QMatrix4x4 LookAt( QVector3D &eye, const QVector3D &center, const QVector3D &up);
    QMatrix4x4 Perspective(float fov, float aspect, float near, float far);
    QVector4D MakeQuaternion(QVector3D rotation_axis, double radian);

    unsigned int LoadTexture(const char *path);

private:
    Model backpack;

    Shader shader_geometry_pass;
    unsigned int g_buffer;
    unsigned int g_position, g_normal, g_albedo;
    unsigned int rbo_depth;

    Shader shader_ssao;
    unsigned int ssao_fbo;
    unsigned int ssao_color_buffer;
    QVector<QVector3D> ssao_kernel;
    QVector<QVector3D> ssao_noise;
    unsigned int noise_texture;

    Shader shader_ssao_blur;
    unsigned int ssao_blur_fbo;
    unsigned int ssao_color_buffer_flur;

    QVector<QVector3D> light_positions;
    QVector<QVector3D> light_colors;

    Shader shader_debug;

    unsigned int quad_vao = 0, quad_vbo = 0;
    unsigned int cube_vao = 0, cube_vbo = 0;

    // bool         b_key_released = true;
    // bool         bloom = true;
    // float        exposure = 1.0f;

    // for LookAt
    QVector3D cam_pos;
    QVector3D cam_front;
    QVector3D cam_up;

    QMatrix4x4 perspective;
    float far = 50.0f;
    float near = 0.1f;
    float fov = 45.0f;

    bool first_mouse = true;
    float yaw = -90.0f;
    float pitch = 0.0f;
};

#endif // LZHOPENGLWIDGET_H
