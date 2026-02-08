#include "lzhopenglwidget.h"

#include <QKeyEvent>
#include <QDateTime>
#include <QOpenGLFramebufferObjectFormat>


LzhOpenGLWidget::LzhOpenGLWidget(QWidget *parent) :
    QOpenGLWidget(parent),
    light_positions {
        QVector3D(0.0f, 0.0f, 10.0f)
    },
    light_colors {
        QVector3D(150.0f, 150.0f, 150.0f)
    }
{
    setFocusPolicy(Qt::StrongFocus);
    setMouseTracking(true);
}

LzhOpenGLWidget::~LzhOpenGLWidget()
{
    makeCurrent();
    // Delete...
    doneCurrent();
}

void LzhOpenGLWidget::initializeGL()
{
    initializeOpenGLFunctions();

    glEnable(GL_DEPTH_TEST);

    cam_pos   = QVector3D(0.0f, 0.0f,  3.0f);
    cam_front = QVector3D(0.0f, 0.0f, -1.0f);
    cam_up    = QVector3D(0.0f, 1.0f,  0.0f);

    shader.Init(":/shader/1.1.pbr.vs", ":/shader/1.1.pbr.fs");

    shader.Use();
    shader.SetInt("albedoMap", 0);
    shader.SetInt("normalMap", 1);
    shader.SetInt("metallicMap", 2);
    shader.SetInt("roughnessMap", 3);
    shader.SetInt("aoMap", 4);

    // load PBR material textures
    // --------------------------
    albedo    = LoadTexture(":/res/rusted_iron/albedo.png");
    normal    = LoadTexture(":/res/rusted_iron/normal.png");
    metallic  = LoadTexture(":/res/rusted_iron/metallic.png");
    roughness = LoadTexture(":/res/rusted_iron/roughness.png");
    ao        = LoadTexture(":/res/rusted_iron/ao.png");

}

void LzhOpenGLWidget::resizeGL(int w, int h)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    // glViewport(0, 0, w, h);

    QMatrix4x4 projection = Perspective(fov, (float)w / h, near, far);
    shader.Use();
    shader.SetMat4("projection", projection);
}

void LzhOpenGLWidget::paintGL()
{
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    QMatrix4x4 view = LookAt(cam_pos, cam_pos + cam_front, cam_up);

    shader.Use();
    shader.SetMat4("view", view);
    shader.SetVec3("camPos", cam_pos);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, albedo);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, normal);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, metallic);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, roughness);
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, ao);

    // render rows*column number of spheres with varying metallic/roughness values scaled by rows and columns respectively
    for (int row = 0; row < nr_rows; ++row)
    {
        for (int col = 0; col < nr_columns; ++col)
        {
            QMatrix4x4 model;
            model.translate(
                (col - (nr_columns / 2)) * spacing,
                (row - (nr_rows / 2)) * spacing,
                0.0f
            );
            shader.SetMat4("model", model);
            TmpMatrix mat3(3, 3);
            for (int i = 0; i < 3; ++i)
            {
                for (int j = 0; j < 3; ++j)
                {
                    mat3(i, j) = model(i, j);
                }
            }
            mat3 = GaussJordanInverse(mat3);
            QMatrix3x3 normal_matrix;
            for (int i = 0; i < 3; ++i)
            {
                for (int j = 0; j < 3; ++j)
                {
                    model(i, j) = mat3(i, j);
                }
            }

            shader.SetMat3("normalMatrix", normal_matrix.transposed());
            RenderSphere();
        }
    }

    // render light source (simply re-render sphere at light positions)
    // this looks a bit off as we use the same shader, but it'll make their positions obvious and
    // keeps the codeprint small.
    for (unsigned int i = 0; i < sizeof(light_positions) / sizeof(light_positions[0]); ++i)
    {
        QVector3D newPos = light_positions[i] + QVector3D(sin(i * 0.1) * 5.0, 0.0, 0.0);
        newPos = light_positions[i];
        shader.SetVec3("lightPositions[" + std::to_string(i) + "]", newPos);
        shader.SetVec3("lightColors[" + std::to_string(i) + "]", light_colors[i]);

        QMatrix4x4 model;
        model.translate(newPos);
        model.scale(0.5f);
        shader.SetMat4("model", model);

        TmpMatrix mat3(3, 3);
        for (int i = 0; i < 3; ++i)
        {
            for (int j = 0; j < 3; ++j)
            {
                mat3(i, j) = model(i, j);
            }
        }
        mat3 = GaussJordanInverse(mat3);
        QMatrix3x3 normal_matrix;
        for (int i = 0; i < 3; ++i)
        {
            for (int j = 0; j < 3; ++j)
            {
                model(i, j) = mat3(i, j);
            }
        }

        shader.SetMat3("normalMatrix", normal_matrix.transposed());
        RenderSphere();
    }
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

    QMatrix4x4 projection = Perspective(fov, (float)width() / height(), near, far);
    shader.Use();
    shader.SetMat4("projection", projection);

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
    //             bloom = !bloom;
    //         }
    //         b_key_released = false;
    //         update();
    //     }
    // }
    //     break;
    // case Qt::Key_Q:
    //     if (exposure > 0.0f)
    //     {
    //         exposure -= 0.1f;
    //     }
    //     else
    //     {
    //         exposure = 0.0f;
    //     }
    //     update();
    //     break;
    // case Qt::Key_E:
    //     exposure += 0.1f;
    //     update();
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

void LzhOpenGLWidget::RenderCube()
{
    if (cube_vao == 0)
    {
        float vertices[] = {
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
        glGenBuffers(1, &cube_vbo);
        // fill buffer
        glBindBuffer(GL_ARRAY_BUFFER, cube_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        // link vertex attributes
        glBindVertexArray(cube_vao);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
    glBindVertexArray(cube_vao);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}

void LzhOpenGLWidget::RenderQuad()
{
    if (quad_vao == 0)
    {
        float quadVertices[] = {
            // positions        // texture Coords
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // setup plane VAO
        glGenVertexArrays(1, &quad_vao);
        glGenBuffers(1, &quad_vbo);
        glBindVertexArray(quad_vao);
        glBindBuffer(GL_ARRAY_BUFFER, quad_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    glBindVertexArray(quad_vao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

void LzhOpenGLWidget::RenderSphere()
{
    if (sphere_vao == 0)
    {
        glGenVertexArrays(1, &sphere_vao);

        unsigned int vbo, ebo;
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);

        QVector<QVector3D> positions;
        QVector<QVector2D> uv;
        QVector<QVector3D> normals;
        QVector<unsigned int> indices;

        const unsigned int X_SEGMENTS = 64;
        const unsigned int Y_SEGMENTS = 64;
        const float PI = 3.14159265359f;
        for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
        {
            for (unsigned int y = 0; y <= Y_SEGMENTS; ++y)
            {
                float xSegment = (float)x / (float)X_SEGMENTS;
                float ySegment = (float)y / (float)Y_SEGMENTS;
                float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
                float yPos = std::cos(ySegment * PI);
                float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

                positions.push_back(QVector3D(xPos, yPos, zPos));
                uv.push_back(QVector2D(xSegment, ySegment));
                normals.push_back(QVector3D(xPos, yPos, zPos));
            }
        }

        bool oddRow = false;
        for (unsigned int y = 0; y < Y_SEGMENTS; ++y)
        {
            if (!oddRow) // even rows: y == 0, y == 2; and so on
            {
                for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
                {
                    indices.push_back(y       * (X_SEGMENTS + 1) + x);
                    indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
                }
            }
            else
            {
                for (int x = X_SEGMENTS; x >= 0; --x)
                {
                    indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
                    indices.push_back(y       * (X_SEGMENTS + 1) + x);
                }
            }
            oddRow = !oddRow;
        }
        index_count = static_cast<unsigned int>(indices.size());

        QVector<float> data;
        for (unsigned int i = 0; i < positions.size(); ++i)
        {
            data.push_back(positions[i].x());
            data.push_back(positions[i].y());
            data.push_back(positions[i].z());
            if (normals.size() > 0)
            {
                data.push_back(normals[i].x());
                data.push_back(normals[i].y());
                data.push_back(normals[i].z());
            }
            if (uv.size() > 0)
            {
                data.push_back(uv[i].x());
                data.push_back(uv[i].y());
            }
        }
        glBindVertexArray(sphere_vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
        unsigned int stride = (3 + 2 + 3) * sizeof(float);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
    }

    glBindVertexArray(sphere_vao);
    glDrawElements(GL_TRIANGLE_STRIP, index_count, GL_UNSIGNED_INT, 0);
}

float LzhOpenGLWidget::OurLerp(float a, float b, float f)
{
    return a + f * (b - a);
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
    GLint internalformat = GL_RGBA;
    GLenum format = GL_BGRA;
    if (image.format() == QImage::Format_Grayscale8)
    {
        internalformat = GL_RED;
        format = GL_RED;
    }
    unsigned int texture_id;
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexImage2D(GL_TEXTURE_2D, 0, internalformat, image.width(), image.height(), 0, format, GL_UNSIGNED_BYTE, image.mirrored().constBits());
    glGenerateMipmap(GL_TEXTURE_2D);

    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT); // for this tutorial: use GL_CLAMP_TO_EDGE to prevent semi-transparent borders. Due to interpolation it takes texels from next repeat
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    return texture_id;
}


// 高斯消元法求矩阵逆
TmpMatrix LzhOpenGLWidget::GaussJordanInverse(TmpMatrix matrix, double epsilon) {
    int size = matrix.get_rows();
    TmpMatrix inverse(size, size);

    // 初始化逆矩阵为单位矩阵
    for (int i = 0; i < size; ++i) {
        inverse(i, i) = 1;
    }

    // 增广原矩阵和逆矩阵
    for (int i = 0; i < size; ++i) {
        matrix(i, i) += epsilon; // 防止出现零主元，通过添加一个小量epsilon
    }

    // 高斯-约当消元法求逆
    for (int i = 0; i < size; ++i) {
        // 寻找主元
        double maxEl = std::abs(matrix(i, i));
        int maxRow = i;
        for (int k = i + 1; k < size; k++) {
            if (std::abs(matrix(k, i)) > maxEl) {
                maxEl = std::abs(matrix(k, i));
                maxRow = k;
            }
        }

        // 交换行
        for (int k = 0; k < size; k++) {
            std::swap(matrix(maxRow, k), matrix(i, k));
            std::swap(inverse(maxRow, k), inverse(i, k));
        }

        // 归一化当前行，并将其他行变为0
        for (int j = 0; j < size; j++) {
            if (j != i) {
                double factor = matrix(j, i) / matrix(i, i);
                for (int k = 0; k < size; k++) {
                    matrix(j, k) -= factor * matrix(i, k);
                    inverse(j, k) -= factor * inverse(i, k);
                }
                matrix(j, i) = 0;
            }
        }

        // 将当前行的对角线元素变为1
        for (int k = 0; k < size; k++) {
            inverse(i, k) /= matrix(i, i);
        }
        matrix(i, i) = 1;
    }

    return inverse;
}
