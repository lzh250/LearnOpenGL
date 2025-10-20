#ifndef LZHOPENGLWIDGET_H
#define LZHOPENGLWIDGET_H

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

private:
    QTimer timer;

    unsigned int object_program, light_program;

    unsigned int VBO;

    unsigned int object_VAO;
    unsigned int object_vertex_shader, object_fragment_shader;

    unsigned int light_VAO;
    unsigned int light_vertex_shader, light_fragment_shader;

    unsigned int texture_container2, texture_container2_specular;

    QMatrix4x4 perspective;

    QVector3D light_pos;
    QVector3D light_color;

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
