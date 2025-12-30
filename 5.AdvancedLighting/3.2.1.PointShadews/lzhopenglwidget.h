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
    void RenderScene(Shader &shader);
    void RanderCube();
    void RenderQuad();

    QMatrix4x4 LookAt( QVector3D &eye, const QVector3D &center, const QVector3D &up);
    QMatrix4x4 Perspective(float fov, float aspect, float near, float far);
    QVector4D MakeQuaternion(QVector3D rotation_axis, double radian);

    unsigned int LoadTexture(const char *path);

private:
    const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
    float near_plane = 1.0f;
    float far_plane  = 25.0f;

    Shader       shader, simple_depth_shader, debug_quad_shader;
    unsigned int cube_vao, cube_vbo;

    unsigned int wood_texture;

    unsigned int depth_map_fbo;
    unsigned int depth_map_texture;

    QVector3D light_pos;

    bool shadows = true;

    // for LookAt
    QVector3D cam_pos;
    QVector3D cam_front;
    QVector3D cam_up;

    QMatrix4x4 perspective;
    QMatrix4x4 shadow_perspective;
    float far = 100.0f;
    float near = 0.1f;
    float fov = 45.0f;

    bool first_mouse = true;
    float yaw = -90.0f;
    float pitch = 0.0f;
};

#endif // LZHOPENGLWIDGET_H

// gamma矫正
// 过去，大多数显示器是阴极射线管显示器（CRT）。这些显示器有一个物理特性就是两倍的输入电压并不能产生两倍的亮度。将输入电压加倍产生的亮度约为输入电压的2.2次幂，这叫做显示器Gamma。
// 理想状态是linear的，即gamma值为1，但现实是骨感的。
// 人类所感知的亮度恰好和CRT所显示出来相似的指数关系非常匹配。也就是说感知gamma也大概是2.2。
// 但物理亮度却不同，颜色值设计也是按照物理亮度来的，计算时也是在物理亮度空间计算更能找到规律。
// 2.2通常是是大多数显示设备的大概平均gamma值。基于gamma2.2的颜色空间叫做sRGB颜色空间。
// 纹理编辑者，所创建的所有纹理都是在sRGB空间中的纹理。
// 因为不是所有纹理都是在sRGB空间中的所以当你把纹理指定为sRGB纹理时要格外小心。
// 比如diffuse纹理，这种为物体上色的纹理几乎都是在sRGB空间中的。而为了获取光照参数的纹理，像specular贴图和法线贴图几乎都在线性空间中，所以如果你把它们也配置为sRGB纹理的话，光照就坏掉了。指定sRGB纹理时要当心。

// 本例中，因为只是试验，没有按照正常方式进行gamma矫正。
