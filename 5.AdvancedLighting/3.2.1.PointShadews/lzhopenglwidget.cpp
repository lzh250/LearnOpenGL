#include "lzhopenglwidget.h"

#include <QKeyEvent>
#include <QDateTime>
#include <QOpenGLFramebufferObjectFormat>

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
    glDeleteBuffers(1, &cube_vbo);
    glDeleteTextures(1, &depth_map_texture);
    glDeleteFramebuffers(1, &depth_map_fbo);
    glDeleteTextures(1, &wood_texture);
    doneCurrent();
}

void LzhOpenGLWidget::initializeGL()
{
    initializeOpenGLFunctions();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    cam_pos   = QVector3D(0.0f, 0.0f,  3.0f);
    cam_front = QVector3D(0.0f, 0.0f, -1.0f);
    cam_up    = QVector3D(0.0f, 1.0f,  0.0f);

    shadow_perspective = Perspective(90.0f, (float)SHADOW_WIDTH / SHADOW_HEIGHT, near_plane, far_plane);
    //shadow_perspective.perspective(90.0f, (float)SHADOW_WIDTH / SHADOW_HEIGHT, near_plane, far_plane);

    simple_depth_shader.Init(":/shader/3.2.1.point_shadows_depth.vs", ":/shader/3.2.1.point_shadows_depth.fs", ":/shader/3.2.1.point_shadows_depth.gs");
    shader.Init(":/shader/3.1.2.shadow_mapping.vs", ":/shader/3.1.2.shadow_mapping.fs");

    float cube_vertices[] = {
        // back face
        -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
         1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
         1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right
         1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
        -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
        -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
        // front face
        -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
         1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
         1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
         1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
        -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
        -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
        // left face
        -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
        -1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
        -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
        -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
        -1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
        -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
        // right face
         1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
         1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
         1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right
         1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
         1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
         1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left
        // bottom face
        -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
         1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
         1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
         1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
        -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
        -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
        // top face
        -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
         1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
         1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right
         1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
        -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
        -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left
    };

    glGenVertexArrays(1, &cube_vao);
    glBindVertexArray(cube_vao);
    glGenBuffers(1, &cube_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, cube_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void *)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void *)(sizeof(float) * 3));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void *)(sizeof(float) * 6));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    wood_texture = LoadTexture(":/res/wood.png");

    glGenTextures(1, &depth_map_texture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, depth_map_texture);
    for (int i = 0; i < 6; ++i)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glGenFramebuffers(1, &depth_map_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, depth_map_fbo);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depth_map_texture, 0);
    glReadBuffer(GL_NONE);  // [MUST] 当生成一个深度立方体贴图时我们只关心深度值，所以我们必须显式告诉OpenGL这个帧缓冲对象不会渲染到一个颜色缓冲里
    glDrawBuffer(GL_NONE);  // [MUST] 当生成一个深度立方体贴图时我们只关心深度值，所以我们必须显式告诉OpenGL这个帧缓冲对象不会渲染到一个颜色缓冲里
    glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebufferObject());

    shader.Use();
    shader.SetInt("diffuseTexture", 0);
    shader.SetInt("depthMap", 1);

    light_pos = QVector3D(0.0f, 0.0f, 0.0f);
}

void LzhOpenGLWidget::resizeGL(int w, int h)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, w, h);
    shader.Use();

    shader.SetMat4("projection", Perspective(fov, (float)w / h, near, far));
    // QMatrix4x4 a;
    // a.perspective(fov, (float)w / h, near, far);
    // shader.SetMat4("projection", a);
}

void LzhOpenGLWidget::paintGL()
{
    // move light position over time
    //light_pos[2] = sin((unsigned short)QDateTime::currentMSecsSinceEpoch() * 0.5) * 3.0;

    glClearColor(0.1f, 0.1f, 1.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // 0. create depth cubemap transformation matrices
    // -----------------------------------------------
    QVector<QMatrix4x4> shadow_transforms;
    shadow_transforms.push_back(shadow_perspective * LookAt(light_pos, light_pos + QVector3D( 1.0f,  0.0f,  0.0f), QVector3D(0.0f, -1.0f,  0.0f)));
    shadow_transforms.push_back(shadow_perspective * LookAt(light_pos, light_pos + QVector3D(-1.0f,  0.0f,  0.0f), QVector3D(0.0f, -1.0f,  0.0f)));
    shadow_transforms.push_back(shadow_perspective * LookAt(light_pos, light_pos + QVector3D( 0.0f,  1.0f,  0.0f), QVector3D(0.0f,  0.0f,  1.0f)));
    shadow_transforms.push_back(shadow_perspective * LookAt(light_pos, light_pos + QVector3D( 0.0f, -1.0f,  0.0f), QVector3D(0.0f,  0.0f, -1.0f)));
    shadow_transforms.push_back(shadow_perspective * LookAt(light_pos, light_pos + QVector3D( 0.0f,  0.0f,  1.0f), QVector3D(0.0f, -1.0f,  0.0f)));
    shadow_transforms.push_back(shadow_perspective * LookAt(light_pos, light_pos + QVector3D( 0.0f,  0.0f, -1.0f), QVector3D(0.0f, -1.0f,  0.0f)));
    // QMatrix4x4 a;
    // a.lookAt(light_pos, light_pos + QVector3D( 1.0f,  0.0f,  0.0f), QVector3D(0.0f, -1.0f,  0.0f));
    // shadow_transforms.push_back(shadow_perspective * a);
    // a.setToIdentity();
    // a.lookAt(light_pos, light_pos + QVector3D(-1.0f,  0.0f,  0.0f), QVector3D(0.0f, -1.0f,  0.0f));
    // shadow_transforms.push_back(shadow_perspective * a);
    // a.setToIdentity();
    // a.lookAt(light_pos, light_pos + QVector3D( 0.0f,  1.0f,  0.0f), QVector3D(0.0f,  0.0f,  1.0f));
    // shadow_transforms.push_back(shadow_perspective * a);
    // a.setToIdentity();
    // a.lookAt(light_pos, light_pos + QVector3D( 0.0f, -1.0f,  0.0f), QVector3D(0.0f,  0.0f, -1.0f));
    // shadow_transforms.push_back(shadow_perspective * a);
    // a.setToIdentity();
    // a.lookAt(light_pos, light_pos + QVector3D( 0.0f,  0.0f,  1.0f), QVector3D(0.0f, -1.0f,  0.0f));
    // shadow_transforms.push_back(shadow_perspective * a);
    // a.setToIdentity();
    // a.lookAt(light_pos, light_pos + QVector3D( 0.0f,  0.0f, -1.0f), QVector3D(0.0f, -1.0f,  0.0f));
    // shadow_transforms.push_back(shadow_perspective * a);

    // 1. render scene to depth cubemap
    // --------------------------------
    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, depth_map_fbo);
    glClear(GL_DEPTH_BUFFER_BIT);
    simple_depth_shader.Use();
    for (unsigned int i = 0; i < 6; ++i)
    {
        simple_depth_shader.SetMat4("shadowMatrices[" + std::to_string(i) + "]", shadow_transforms[i]);
    }
    simple_depth_shader.SetFloat("far_plane", far_plane);
    simple_depth_shader.SetVec3("lightPos", light_pos);
    RenderScene(simple_depth_shader);
    glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebufferObject());

    // 2. render scene as normal
    // -------------------------
    glViewport(0, 0, width(), height());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shader.Use();
    //QMatrix4x4 view = LookAt(cam_pos, cam_pos + cam_front, cam_up);
    QMatrix4x4 view; view.lookAt(cam_pos, cam_pos + cam_front, cam_up);
    shader.SetMat4("view", view);
    shader.SetVec3("viewPos", cam_pos);
    shader.SetVec3("lightPos", light_pos);
    shader.SetInt("shadows", shadows);
    shader.SetFloat("far_plane", far_plane);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, wood_texture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, depth_map_texture);
    RenderScene(shader);
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

    shader.Use();
    shader.SetMat4("projection", Perspective(fov, (float)width() / height(), near, far));
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
    // case Qt::Key_B:
    // {
    //     //*************************************************************
    //     // Qt长期按住一个键再释放时的事件特性：
    //     // 1. 触发KeyPressEvent，isAutoRepeat()返回false
    //     // 2. 停顿一会儿
    //     // 3. 触发KeyPressEvent，isAutoRepeat()返回true
    //     // 4. 触发KeyReleaseEvent，isAutoRepeat()返回true
    //     // 5. 3 4 循环往复
    //     // 6. 释放按键，触发KeyReleaseEvent，isAutoRepeat()返回false
    //     //*************************************************************
    //     if (!event->isAutoRepeat())
    //     {
    //         if (b_key_released)
    //         {
    //             gamma_enabled = !gamma_enabled;
    //         }
    //         b_key_released = false;
    //         update();
    //     }
    // }
    //     break;
    default:
        break;
    }
}

void LzhOpenGLWidget::keyReleaseEvent(QKeyEvent *event)
{
    // if (event->key() == Qt::Key_B && !event->isAutoRepeat())
    // {
    //     b_key_released = true;
    // }
}

void LzhOpenGLWidget::RenderScene(Shader &shader)
{
    // floor
    QMatrix4x4 model;
    model.scale(5.0f);
    shader.SetMat4("model", model);
    glDisable(GL_CULL_FACE); // note that we disable culling here since we render 'inside' the cube instead of the usual 'outside' which throws off the normal culling methods.
    shader.SetInt("reverse_normals", 1); // A small little hack to invert normals when drawing cube from the inside so lighting still works.
    RanderCube();
    shader.SetInt("reverse_normals", 0); // and of course disable it
    glEnable(GL_CULL_FACE);
    // cubes
    model.setToIdentity();
    model.translate(QVector3D(4.0f, -3.5f, 0.0));
    model.scale(0.5f);
    shader.SetMat4("model", model);
    RanderCube();
    model.setToIdentity();
    model.translate(QVector3D(2.0f, 3.0f, 1.0));
    model.scale(0.75f);
    shader.SetMat4("model", model);
    RanderCube();
    model.setToIdentity();
    model.translate(QVector3D(-3.0f, -1.0f, 0.0));
    model.scale(0.5f);
    shader.SetMat4("model", model);
    RanderCube();
    model.setToIdentity();
    model.translate(QVector3D(-1.5f, 1.0f, 1.5));
    model.scale(0.5f);
    shader.SetMat4("model", model);
    RanderCube();
    model.setToIdentity();
    model.translate(QVector3D(-1.5f, 2.0f, -3.0));
    model.rotate(60.0f, QVector3D(1.0, 0.0, 1.0));
    model.scale(0.75f);
    shader.SetMat4("model", model);
    RanderCube();
}

void LzhOpenGLWidget::RanderCube()
{
    glBindVertexArray(cube_vao);
    glDrawArrays(GL_TRIANGLES, 0, 36);
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

QMatrix4x4 LzhOpenGLWidget::Perspective(float fov, float aspect, float near, float far)
{
    float scale = tan(fov * 0.5f * M_PI / 180) * 1.0f;
    float r = aspect * scale;
    float t = scale;

    QMatrix4x4 perspective;
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

    return perspective;
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

unsigned int LzhOpenGLWidget::LoadTexture(const char *path)
{
    QImage image(path);
    if (image.isNull())
    {
        qDebug() << "Failed to open the image: " << path;
        return -1;
    }

    unsigned int texture_id;
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width(), image.height(), 0, GL_BGRA, GL_UNSIGNED_BYTE, image.mirrored().constBits());
    glGenerateMipmap(GL_TEXTURE_2D);

    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT); // for this tutorial: use GL_CLAMP_TO_EDGE to prevent semi-transparent borders. Due to interpolation it takes texels from next repeat
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    return texture_id;
}
