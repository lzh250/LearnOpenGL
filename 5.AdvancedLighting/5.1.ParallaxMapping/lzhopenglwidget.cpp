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
    glDeleteVertexArrays(1, &quad_vao);
    glDeleteBuffers(1, &quad_vbo);
    glDeleteTextures(1, &diffuse_mapping);
    glDeleteTextures(1, &normal_mapping);
    doneCurrent();
}

void LzhOpenGLWidget::initializeGL()
{
    initializeOpenGLFunctions();

    glEnable(GL_DEPTH_TEST);

    cam_pos   = QVector3D(0.0f, 0.0f,  3.0f);
    cam_front = QVector3D(0.0f, 0.0f, -1.0f);
    cam_up    = QVector3D(0.0f, 1.0f,  0.0f);

    shader.Init(":/shader/4.normal_mapping.vs", ":/shader/4.normal_mapping.fs");

    diffuse_mapping = LoadTexture(":/res/bricks2.jpg");
    normal_mapping  = LoadTexture(":/res/bricks2_normal.jpg");
    depth_mapping   = LoadTexture(":/res/bricks2_disp.jpg");

    shader.Use();
    shader.SetInt("diffuseMap", 0);
    shader.SetInt("normalMap" , 1);
    shader.SetInt("depthMap" , 1);

    light_pos = QVector3D(0.5f, 1.0f, 0.3f);


    // E1=ΔU1T+ΔV1B
    // E2=ΔU2T+ΔV2B
    // 我们也可以写成这样：
    // (E1x,E1y,E1z)=ΔU1(Tx,Ty,Tz)+ΔV1(Bx,By,Bz)
    // (E2x,E2y,E2z)=ΔU2(Tx,Ty,Tz)+ΔV2(Bx,By,Bz)
    // 你可能想起你的代数课了，这是让我们去解 T 和 B 。
    // | E1x E1y E1z | = | ΔU1 ΔV1 | | Tx Ty Tz |
    // | E2x E2y E2z |   | ΔU2 ΔV2 | | Bx By Bz |
    // 两边都乘以 ΔUΔV 的逆矩阵
    // | ΔU1 ΔV1 |−1 | E1x E1y E1z | = | Tx Ty Tz |
    // | ΔU2 ΔV2 |   | E2x E2y E2z |   | Bx By Bz |
    // 这样我们就可以解出T和B了。这需要我们计算出delta纹理坐标矩阵的逆矩阵。我不打算讲解计算逆矩阵的细节，但大致是把它变化为，1除以矩阵的行列式，再乘以它的伴随矩阵(Adjugate Matrix)。
    // | Tx Ty Tz | = 1 / (ΔU1ΔV2−ΔU2ΔV1) |  ΔV2 −ΔV1 | | E1x E1y E1z |
    // | Bx By Bz |                       | −ΔU2  ΔU1 | | E2x E2y E2z |

    // positions
    QVector3D pos1(-1.0f,  1.0f, 0.0f);
    QVector3D pos2(-1.0f, -1.0f, 0.0f);
    QVector3D pos3( 1.0f, -1.0f, 0.0f);
    QVector3D pos4( 1.0f,  1.0f, 0.0f);
    // texture coordinates
    QVector2D uv1(0.0f, 1.0f);
    QVector2D uv2(0.0f, 0.0f);
    QVector2D uv3(1.0f, 0.0f);
    QVector2D uv4(1.0f, 1.0f);
    // normal vector
    QVector3D nm(0.0f, 0.0f, 1.0f);

    // calculate tangent/bitangent vectors of both triangles
    QVector3D tangent1, bitangent1;
    QVector3D tangent2, bitangent2;
    // triangle 1
    // ----------
    QVector3D edge1 = pos2 - pos1;
    QVector3D edge2 = pos3 - pos1;
    QVector2D deltaUV1 = uv2 - uv1;
    QVector2D deltaUV2 = uv3 - uv1;

    float f = 1.0f / (deltaUV1.x() * deltaUV2.y() - deltaUV2.x() * deltaUV1.y());

    tangent1[0] = f * (deltaUV2.y() * edge1.x() - deltaUV1.y() * edge2.x());
    tangent1[1] = f * (deltaUV2.y() * edge1.y() - deltaUV1.y() * edge2.y());
    tangent1[2] = f * (deltaUV2.y() * edge1.z() - deltaUV1.y() * edge2.z());

    bitangent1[0] = f * (-deltaUV2.x() * edge1.x() + deltaUV1.x() * edge2.x());
    bitangent1[1] = f * (-deltaUV2.x() * edge1.y() + deltaUV1.x() * edge2.y());
    bitangent1[2] = f * (-deltaUV2.x() * edge1.z() + deltaUV1.x() * edge2.z());

    // triangle 2
    // ----------
    edge1 = pos3 - pos1;
    edge2 = pos4 - pos1;
    deltaUV1 = uv3 - uv1;
    deltaUV2 = uv4 - uv1;

    f = 1.0f / (deltaUV1.x() * deltaUV2.y() - deltaUV2.x() * deltaUV1.y());

    tangent2[0] = f * (deltaUV2.y() * edge1.x() - deltaUV1.y() * edge2.x());
    tangent2[1] = f * (deltaUV2.y() * edge1.y() - deltaUV1.y() * edge2.y());
    tangent2[2] = f * (deltaUV2.y() * edge1.z() - deltaUV1.y() * edge2.z());


    bitangent2[0] = f * (-deltaUV2.x() * edge1.x() + deltaUV1.x() * edge2.x());
    bitangent2[1] = f * (-deltaUV2.x() * edge1.y() + deltaUV1.x() * edge2.y());
    bitangent2[2] = f * (-deltaUV2.x() * edge1.z() + deltaUV1.x() * edge2.z());


    float quadVertices[] = {
        // positions                  // normal               // texcoords      // tangent                                // bitangent
        pos1.x(), pos1.y(), pos1.z(), nm.x(), nm.y(), nm.z(), uv1.x(), uv1.y(), tangent1.x(), tangent1.y(), tangent1.z(), bitangent1.x(), bitangent1.y(), bitangent1.z(),
        pos2.x(), pos2.y(), pos2.z(), nm.x(), nm.y(), nm.z(), uv2.x(), uv2.y(), tangent1.x(), tangent1.y(), tangent1.z(), bitangent1.x(), bitangent1.y(), bitangent1.z(),
        pos3.x(), pos3.y(), pos3.z(), nm.x(), nm.y(), nm.z(), uv3.x(), uv3.y(), tangent1.x(), tangent1.y(), tangent1.z(), bitangent1.x(), bitangent1.y(), bitangent1.z(),

        pos1.x(), pos1.y(), pos1.z(), nm.x(), nm.y(), nm.z(), uv1.x(), uv1.y(), tangent2.x(), tangent2.y(), tangent2.z(), bitangent2.x(), bitangent2.y(), bitangent2.z(),
        pos3.x(), pos3.y(), pos3.z(), nm.x(), nm.y(), nm.z(), uv3.x(), uv3.y(), tangent2.x(), tangent2.y(), tangent2.z(), bitangent2.x(), bitangent2.y(), bitangent2.z(),
        pos4.x(), pos4.y(), pos4.z(), nm.x(), nm.y(), nm.z(), uv4.x(), uv4.y(), tangent2.x(), tangent2.y(), tangent2.z(), bitangent2.x(), bitangent2.y(), bitangent2.z()
    };
    // configure plane VAO
    glGenVertexArrays(1, &quad_vao);
    glGenBuffers(1, &quad_vbo);
    glBindVertexArray(quad_vao);
    glBindBuffer(GL_ARRAY_BUFFER, quad_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(8 * sizeof(float)));
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(11 * sizeof(float)));
}

void LzhOpenGLWidget::resizeGL(int w, int h)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, w, h);
    shader.Use();

    shader.SetMat4("projection", Perspective(fov, (float)w / h, near, far));
}

void LzhOpenGLWidget::paintGL()
{
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shader.Use();

    // render normal-mapped quad
    QMatrix4x4 model;
    model.rotate((unsigned short)QDateTime::currentMSecsSinceEpoch() * -0.1, QVector3D(1.0f, 0.0f, 1.0f));
    QMatrix4x4 view = LookAt(cam_pos, cam_pos + cam_front, cam_up);
    shader.SetMat4("model", model);
    shader.SetMat4("view", view);
    shader.SetVec3("viewPos", cam_pos);
    shader.SetVec3("lightPos", light_pos);
    shader.SetFloat("depthScale", depth_scale);
    qDebug() << depth_scale;
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, diffuse_mapping);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, normal_mapping);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, depth_mapping);
    RenderQuad();

    // render light source (simply re-renders a smaller plane at the light's position for debugging/visualization)
    model.setToIdentity();
    model.translate(light_pos);
    model.scale(0.1f);
    shader.SetMat4("model", model);
    RenderQuad();
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
    case Qt::Key_Q:
        if (depth_scale > 0.0f)
        {
            depth_scale -= 0.0005f;
        }
        else
        {
            depth_scale = 0.0f;
        }
        update();
        break;
    case Qt::Key_E:
        if (depth_scale < 1.0f)
        {
            depth_scale += 0.0005f;
        }
        else
        {
            depth_scale = 1.0f;
        }
        update();
        break;
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

void LzhOpenGLWidget::RenderQuad()
{
    glBindVertexArray(quad_vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
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
