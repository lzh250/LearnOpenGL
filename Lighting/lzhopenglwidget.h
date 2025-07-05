#ifndef LZHOPENGLWIDGET_H
#define LZHOPENGLWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions_4_5_Core>
#include <QMatrix4x4>

class LzhOpenGLWidget : public QOpenGLWidget, QOpenGLFunctions_4_5_Core
{
public:
    LzhOpenGLWidget(QWidget *parent);
    virtual ~LzhOpenGLWidget();

protected:
    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();

private:
    QMatrix4x4 LookAt( QVector3D &eye, const QVector3D &center, const QVector3D &up);
    void Perspective();

private:
    unsigned int object_program, light_program;

    unsigned int VBO;
    unsigned int object_VAO;
    unsigned int object_vertex_shader, object_fragment_shader;

    unsigned int light_VAO, light_VBO;
    unsigned int light_vertex_shader, light_fragment_shader;

    QMatrix4x4 perspective;

    QVector3D light_pos;
};

#endif // LZHOPENGLWIDGET_H
