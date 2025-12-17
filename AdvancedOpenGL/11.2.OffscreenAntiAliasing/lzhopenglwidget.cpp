#include "lzhopenglwidget.h"

#include <QKeyEvent>
#include <QDateTime>
#include <QOpenGLFramebufferObjectFormat>

float cube_vertices[] = {
    // positions
    -0.5f, -0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
    -0.5f,  0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,

    -0.5f, -0.5f,  0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,
    -0.5f, -0.5f,  0.5f,

    -0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,

     0.5f,  0.5f,  0.5f,
     0.5f,  0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,

    -0.5f, -0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f, -0.5f,  0.5f,
    -0.5f, -0.5f,  0.5f,
    -0.5f, -0.5f, -0.5f,

    -0.5f,  0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
     0.5f,  0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f, -0.5f
};

float quad_vertices[] = {   // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
    // positions   // texCoords
    -1.0f,  1.0f,  0.0f, 1.0f,
    -1.0f, -1.0f,  0.0f, 0.0f,
     1.0f, -1.0f,  1.0f, 0.0f,

    -1.0f,  1.0f,  0.0f, 1.0f,
     1.0f, -1.0f,  1.0f, 0.0f,
     1.0f,  1.0f,  1.0f, 1.0f
};

LzhOpenGLWidget::LzhOpenGLWidget(QWidget *parent) :
    QOpenGLWidget(parent)
{
    setFocusPolicy(Qt::StrongFocus);
    setMouseTracking(true);
}

LzhOpenGLWidget::~LzhOpenGLWidget()
{
    makeCurrent();
    glDeleteVertexArrays(1, &cube_vao);
    glDeleteVertexArrays(1, &quad_vao);
    glDeleteBuffers(1, &cube_vbo);
    glDeleteBuffers(1, &quad_vbo);
    glDeleteFramebuffers(1, &framebuffer);
    glDeleteFramebuffers(1, &intermediate_fbo);
    glDeleteTextures(1, &texture_color_buffer_multi_sampled);
    glDeleteTextures(1, &screen_texture);
    doneCurrent();
}

void LzhOpenGLWidget::initializeGL()
{
    initializeOpenGLFunctions();

    glEnable(GL_DEPTH_TEST);

    cam_pos   = QVector3D(0.0f, 0.0f,  3.0f);
    cam_front = QVector3D(0.0f, 0.0f, -1.0f);
    cam_up    = QVector3D(0.0f, 1.0f,  0.0f);

    shader.Init(":/shader/11.1.anti_aliasing.vs", ":/shader/11.1.anti_aliasing.fs");
    screen_shader.Init(":/shader/11.2.aa_post.vs", ":/shader/11.2.aa_post.fs");

    glGenVertexArrays(1, &cube_vao);
    glBindVertexArray(cube_vao);
    glGenBuffers(1, &cube_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, cube_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3,GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void *)0);
    glBindVertexArray(0);

    glGenVertexArrays(1, &quad_vao);
    glBindVertexArray(quad_vao);
    glGenBuffers(1, &quad_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, quad_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), quad_vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (void *)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (void *)(sizeof(float) * 2));

    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glGenTextures(1, &texture_color_buffer_multi_sampled);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, texture_color_buffer_multi_sampled);
    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGB, 800, 600, GL_TRUE);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, texture_color_buffer_multi_sampled, 0);
    unsigned int rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, 800, 600);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_FRAMEBUFFER, rbo);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        qDebug() << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!";
    }
    glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebufferObject());

    //**************************************************************************************
    //
    // 因为多重采样缓冲有一点特别，我们不能直接将它们的缓冲图像用于其他运算，
    // 比如不能直接拿来当普通纹理在默认着色器中进行采样，而是需要有一个把它内容还原成普通纹理的过程。
    // 这个还原过程通过glBlitFramebuffer来完成，这个函数可将一个缓冲数据块传输到另一个帧缓冲，
    // 对于多重采样帧缓冲，这个函数将多重采样帧缓冲内容的图像还原数据传输到另外一个普通帧缓冲。
    //
    //**************************************************************************************
    glGenFramebuffers(1, &intermediate_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, intermediate_fbo);
    glGenTextures(1, &screen_texture);
    glBindTexture(GL_TEXTURE_2D, screen_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 800, 600, 0, GL_BGRA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, screen_texture, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        cout << "ERROR::FRAMEBUFFER:: Intermediate framebuffer is not complete!" << endl;
    glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebufferObject());

    screen_shader.Use();
    screen_shader.SetInt("screenTexture", 0);

}

void LzhOpenGLWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
    Perspective();
    shader.Use();
    shader.SetMat4("projection", perspective);
}

void LzhOpenGLWidget::paintGL()
{
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    QMatrix4x4 view = LookAt(cam_pos, cam_pos + cam_front, cam_up);

    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    shader.Use();
    QMatrix4x4 model;
    shader.SetMat4("model", model);
    shader.SetMat4("view", view);
    shader.SetMat4("projection", perspective);
    glBindVertexArray(cube_vao);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, intermediate_fbo);
    glBlitFramebuffer(0, 0, 800, 600, 0, 0, 800, 600, GL_COLOR_BUFFER_BIT, GL_NEAREST);

    glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebufferObject());
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    screen_shader.Use();
    glBindVertexArray(quad_vao);
    glBindTexture(GL_TEXTURE_2D, screen_texture);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void LzhOpenGLWidget::mouseMoveEvent(QMouseEvent *event)
{
    QPoint pos = event->pos();

    if (first_mouse)
    {
        QCursor::setPos(mapToGlobal(QPoint(width() / 2, height() / 2)));
        first_mouse = false;
        return;
    }

    float x_offset = pos.x() - width()/2;
    float y_offset = height()/2 - pos.y();

    x_offset *= 0.05f;
    y_offset *= 0.05f;

    yaw   += x_offset;
    pitch += y_offset;

    if (pitch >  89.0f) pitch =  89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    QVector3D front;
    // 默认朝向是世界X轴方向(不考虑俯仰)，yaw应该初始化为-90
    front[0] = cos(pitch / 180 * M_PI) * cos(yaw / 180 * M_PI);
    front[1] = sin(pitch / 180 * M_PI);
    front[2] = cos(pitch / 180 * M_PI) * sin(yaw / 180 * M_PI);
    // 默认朝向是世界Z轴负方向(不考虑俯仰)，yaw应该初始化为0
    // front[0] = cos(pitch / 180 * M_PI) * sin(yaw / 180 * M_PI);
    // front[1] = sin(pitch / 180 * M_PI);
    // front[2] = -cos(pitch / 180 * M_PI) * cos(yaw / 180 * M_PI);
    // 默认朝向是世界Z轴方向(不考虑俯仰)，yaw应该初始化为180
    // front[0] = -cos(pitch / 180 * M_PI) * sin(yaw / 180 * M_PI);
    // front[1] = sin(pitch / 180 * M_PI);
    // front[2] = cos(pitch / 180 * M_PI) * cos(yaw / 180 * M_PI);

    cam_front = front.normalized();

    QCursor::setPos(mapToGlobal(QPoint(width() / 2, height() / 2)));

    update();
}

void LzhOpenGLWidget::wheelEvent(QWheelEvent *event)
{
    QPoint numPixels = event->pixelDelta();
    QPoint numDegrees = event->angleDelta() / 8;

    if (!numPixels.isNull())
    {
        fov -= numPixels.y();
    }
    else if (!numDegrees.isNull()) {
        QPoint numSteps = numDegrees / 15;
        fov -= numSteps.y();
    }
    if(fov <= 1.0f)
        fov = 1.0f;
    if(fov >= 45.0f)
        fov = 45.0f;

    Perspective();
    shader.Use();
    shader.SetMat4("projection", perspective);
    update();

    event->accept();
}

void LzhOpenGLWidget::keyPressEvent(QKeyEvent *event)
{
    switch (event->key())
    {
    case Qt::Key_W:
        cam_pos += cam_front * 0.2f;
        update();
        break;
    case Qt::Key_S:
        cam_pos -= cam_front * 0.2f;
        update();
        break;
    case Qt::Key_A:
        cam_pos -= QVector3D::crossProduct(cam_front, cam_up) * 0.2f;
        update();
        break;
    case Qt::Key_D:
        cam_pos += QVector3D::crossProduct(cam_front, cam_up) * 0.2f;
        update();
        break;
    default:
        break;
    }
}

QMatrix4x4 LzhOpenGLWidget::LookAt(QVector3D &eye, const QVector3D &center, const QVector3D &up)
{
    QVector3D f = (center - eye).normalized();
    QVector3D s = QVector3D::crossProduct(f, up).normalized();
    QVector3D u = QVector3D::crossProduct(s, f);

    QMatrix4x4 view;
    view(0, 0) = s.x();
    view(0, 1) = s.y();
    view(0, 2) = s.z();
    view(0, 3) = -QVector3D::dotProduct(s, eye);
    view(1, 0) = u.x();
    view(1, 1) = u.y();
    view(1, 2) = u.z();
    view(1, 3) = -QVector3D::dotProduct(u, eye);
    view(2, 0) = -f.x();
    view(2, 1) = -f.y();
    view(2, 2) = -f.z();
    view(2, 3) =  QVector3D::dotProduct(f, eye);
    view(3, 0) = 0.0f;
    view(3, 1) = 0.0f;
    view(3, 2) = 0.0f;
    view(3, 3) = 1.0f;

    return view;
}

void LzhOpenGLWidget::Perspective()
{
    float scale = tan(fov * 0.5f * M_PI / 180) * 1.0f;
    float imgAspectRatio = (float)width() / height();
    float r = imgAspectRatio * scale;
    float t = scale;

    perspective(0, 0) = 1.0f / r;
    perspective(0, 1) = 0.0f;
    perspective(0, 2) = 0.0f;
    perspective(0, 3) = 0.0f;

    perspective(1, 0) = 0.0f;
    perspective(1, 1) = 1.0f / t;
    perspective(1, 2) = 0;
    perspective(1, 3) = 0;

    perspective(2, 0) = 0.0f;
    perspective(2, 1) = 0.0f;
    perspective(2, 2) = -(far + near) / (far - near);
    perspective(2, 3) = -2 * far * near / (far - near);

    perspective(3, 0) = 0.0f;
    perspective(3, 1) = 0.0f;
    perspective(3, 2) = -1;
    perspective(3, 3) = 0.0f;
}

QVector4D LzhOpenGLWidget::MakeQuaternion(QVector3D rotation_axis, double radian)
{
    double length = sqrt(rotation_axis[0] * rotation_axis[0] +
                         rotation_axis[1] * rotation_axis[1] +
                         rotation_axis[2] * rotation_axis[2]);

    if (length < 1e-7) length = 1.0;

    double inverse_norm = 1.0 / length;
    double sin_half_angle = sin(0.5 * radian);
    double cos_half_angle = cos(0.5 * radian);
    double x = rotation_axis[0] * sin_half_angle * inverse_norm;
    double y = rotation_axis[1] * sin_half_angle * inverse_norm;
    double z = rotation_axis[2] * sin_half_angle * inverse_norm;
    double w = cos_half_angle;

    return QVector4D(x, y, z, w);
}
