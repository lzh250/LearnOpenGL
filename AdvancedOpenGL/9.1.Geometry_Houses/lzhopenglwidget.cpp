#include "lzhopenglwidget.h"

#include <QKeyEvent>

const float points[] = {
    -0.5f,  0.5f, 1.0f, 0.0f, 0.0f, // 左上
     0.5f,  0.5f, 0.0f, 1.0f, 0.0f, // 右上
     0.5f, -0.5f, 0.0f, 0.0f, 1.0f, // 右下
    -0.5f, -0.5f, 1.0f, 1.0f, 0.0f  // 左下
};

LzhOpenGLWidget::LzhOpenGLWidget(QWidget *parent) :
    QOpenGLWidget(parent)
{
    // setFocusPolicy(Qt::StrongFocus);
    // setMouseTracking(true);
}

LzhOpenGLWidget::~LzhOpenGLWidget()
{
    makeCurrent();
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    doneCurrent();
}

void LzhOpenGLWidget::initializeGL()
{
    initializeOpenGLFunctions();

    glEnable(GL_DEPTH_TEST);

    shader.Init(":/shader/9.1.geometry_shader.vs", ":/shader/9.1.geometry_shader.fs", ":/shader/9.1.geometry_shader.gs");

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(2 * sizeof(float)));
    glBindVertexArray(0);
}

void LzhOpenGLWidget::resizeGL(int w, int h)
{
}

void LzhOpenGLWidget::paintGL()
{
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shader.Use();
    glBindVertexArray(vao);
    glDrawArrays(GL_POINTS, 0, 4);
}
