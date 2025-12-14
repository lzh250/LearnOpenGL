#include "lzhopenglwidget.h"

#include "model.h"
#include <QKeyEvent>
#include <QDateTime>

LzhOpenGLWidget::LzhOpenGLWidget(QWidget *parent) :
    QOpenGLWidget(parent)
{
    setFocusPolicy(Qt::StrongFocus);
    setMouseTracking(true);
}

LzhOpenGLWidget::~LzhOpenGLWidget()
{
    makeCurrent();
    glDeleteBuffers(1, &asteroids_vbo);
    doneCurrent();
}

void LzhOpenGLWidget::initializeGL()
{
    initializeOpenGLFunctions();

    glEnable(GL_DEPTH_TEST);

    cam_pos   = QVector3D(0.0f, 0.0f,  155.0f);
    cam_front = QVector3D(0.0f, 0.0f, -1.0f);
    cam_up    = QVector3D(0.0f, 1.0f,  0.0f);

    shader.Init(":/shader/1.model_loading.vs", ":/shader/1.model_loading.fs");
    asteroids_shader.Init(":/shader/10.3.asteroids.vs", ":/shader/10.3.asteroids.fs");

    QString rock_path = QCoreApplication::applicationDirPath() + "/res/rock/rock.obj";
    QString planet_path = QCoreApplication::applicationDirPath() + "/res/planet/planet.obj";
    rock.LoadModel(rock_path.toStdString());
    planet.LoadModel(planet_path.toStdString());

    srand((unsigned int)QDateTime::currentMSecsSinceEpoch());
    float radius = 150.0f;
    float offset = 25.0f;

    float *models = new float[amount * 16];
    for (unsigned int i = 0; i < amount; ++i)
    {
        QMatrix4x4 model;
        model.setToIdentity();
        // 1. 位移：分布在半径为 'radius' 的圆形上，偏移的范围是 [-offset, offset]
        float angle = (float)i / (float)amount * 360.0f;
        float displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float x = sin(angle) * radius + displacement;
        displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float y = displacement * 0.4f; // 让行星带的高度比x和z的宽度要小
        displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float z = cos(angle) * radius + displacement;
        model.translate(QVector3D(x, y, z));

        // 2. 缩放：在 0.05 和 0.25f 之间缩放
        float scale = (rand() % 20) / 100.0f + 0.05;
        model.scale(scale);

        // 3. 旋转：绕着一个（半）随机选择的旋转轴向量进行随机的旋转
        float rotate_angle = (rand() % 360);
        model.rotate(rotate_angle, QVector3D(0.4f, 0.6f, 0.8f));

        // 4. 添加到矩阵的数组中
        memcpy_s(&models[i * 16], 16 * sizeof(float), model.constData(), 16 * sizeof(float));
    }

    glGenBuffers(1, &asteroids_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, asteroids_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 16 * amount, models, GL_STATIC_DRAW);

    delete[] models;

    //******************************************************************************
    // 顶点属性最大允许的数据大小等于一个vec4
    // 对于mat4,需要多次传入。可以每次传vec4长度的数据，多次传输。
    // 因为一个mat4本质上是4个vec4，我们需要为这个矩阵预留4个顶点属性。
    // 因为我们将它的位置值设置为3，矩阵每一列的顶点属性位置值就是3、4、5和6。
    //******************************************************************************

    for (unsigned int i = 0; i < rock.meshes.size(); ++i)
    {
        unsigned int VAO = rock.meshes.at(i).VAO;
        glBindVertexArray(VAO);
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 16, (void *)0);
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 16, (void *)(4 * sizeof(float)));
        glEnableVertexAttribArray(5);
        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 16, (void *)(8 * sizeof(float)));
        glEnableVertexAttribArray(6);
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 16, (void *)(12 * sizeof(float)));

        glVertexAttribDivisor(3, 1);
        glVertexAttribDivisor(4, 1);
        glVertexAttribDivisor(5, 1);
        glVertexAttribDivisor(6, 1);
    }

    glBindVertexArray(0);
}

void LzhOpenGLWidget::resizeGL(int w, int h)
{
    Perspective();
    shader.Use();
    shader.SetMat4("projection", perspective);
    asteroids_shader.Use();
    asteroids_shader.SetMat4("projection", perspective);
}

void LzhOpenGLWidget::paintGL()
{
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    QMatrix4x4 view = LookAt(cam_pos, cam_pos + cam_front, cam_up);

    // 绘制行星
    shader.Use();
    QMatrix4x4 model;
    model.translate(QVector3D(0.0f, -3.0f, 0.0f));
    model.scale(QVector3D(4.0f, 4.0f, 4.0f));
    shader.SetMat4("model", model);
    shader.SetMat4("view", view);
    planet.Draw(shader);

    // 绘制小行星
    asteroids_shader.Use();
    asteroids_shader.SetMat4("view", view);
    asteroids_shader.SetInt("texture_diffuse1", 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, rock.textures_loaded[0].id);
    for(unsigned int i = 0; i < rock.meshes.size(); i++)
    {
        glBindVertexArray(rock.meshes[i].VAO);
        glDrawElementsInstanced(GL_TRIANGLES, rock.meshes[i].indices.size(), GL_UNSIGNED_INT, (void *)0, amount);
        rock.Draw(shader);
    }
    glBindVertexArray(0);
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
        cam_pos += cam_front * 2.2f;    // 之前是0.2f
        update();
        break;
    case Qt::Key_S:
        cam_pos -= cam_front * 2.2f;
        update();
        break;
    case Qt::Key_A:
        cam_pos -= QVector3D::crossProduct(cam_front, cam_up) * 2.2f;
        update();
        break;
    case Qt::Key_D:
        cam_pos += QVector3D::crossProduct(cam_front, cam_up) * 2.2f;
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
