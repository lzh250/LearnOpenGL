#ifndef LZHOPENGLWIDGET_H
#define LZHOPENGLWIDGET_H

#include <QOpenGLWidget>
#include <QWidget>
#include <QOpenGLFunctions_4_5_Core>
#include <QTimer>
#include <QMatrix4x4>

class LzhOpenGLWidget : public QOpenGLWidget, QOpenGLFunctions_4_5_Core
{
public:
    LzhOpenGLWidget(QWidget *parent);
    virtual ~LzhOpenGLWidget();

public:
    void DrawRectangle();
    void Clear();
    void SetPolygonMode(bool is_wire);

    // QOpenGLWidget interface
protected:
    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();

private:
    void Perspective();

private:
    QTimer timer;
    float radian = 0.0f;

    bool is_draw = false;
    int  shape_type = GL_TRIANGLES;

    QMatrix4x4 perspective;

};

#endif // LZHOPENGLWIDGET_H
