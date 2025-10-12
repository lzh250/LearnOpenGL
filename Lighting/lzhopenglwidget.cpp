#include "lzhopenglwidget.h"

#include <QKeyEvent>

float vertices[] = {
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
     0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
     0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
};

const char *object_vertex_shader_source = R"(
    #version 450 core

    layout (location = 0) in vec3 pos;
    layout (location = 1) in vec3 aNormal;

    out vec3 fragPos;
    out vec3 normal;

    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 perspective;

    void main()
    {
        //fragPos = pos;
        fragPos = vec3(model * vec4(pos, 1.0));
        normal = aNormal;
        //gl_Position = perspective * view * vec4(pos + vec3(0.0f, 0.0f, 0.0f), 1.0f);
        gl_Position = perspective * view * vec4(fragPos, 1.0f);
    }
)";

const char *object_fragment_shader_source = R"(
    #version 450 core

    struct Material {
        vec3 ambient;
        vec3 diffuse;
        vec3 specular;
        float shininess;
    };

    in vec3 fragPos;
    in vec3 normal;

    uniform vec3 lightPos;
    uniform vec3 viewPos;
    uniform vec3 lightColor;
    uniform vec3 objectColor;
    uniform Material material;

    void main()
    {
        // ambient
        vec3 ambient = lightColor * material.ambient;

        // diffuse
        vec3 norm = normalize(normal);
        vec3 lightDir = normalize(lightPos - fragPos);
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = lightColor * (diff * material.diffuse);

        // specular
        vec3 viewDir = normalize(viewPos - fragPos);
        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
        vec3 specular = lightColor * (spec * material.specular);

        // 通过环境光照、漫反射关照、镜面光照得出结果
        vec3 result = ambient + diffuse + specular;
        gl_FragColor = vec4(result, 1.0);
    }
)";

const char *light_vertex_shader_source = R"(
    #version 450 core

    layout (location = 0) in vec3 pos;

    uniform vec3 lightPos;
    uniform mat4 view;
    uniform mat4 perspective;

    void main()
    {
        gl_Position = perspective * view * vec4(pos * 0.2f + lightPos, 1.0f);
    }
)";

const char *light_fragment_shader_source = R"(
    #version 450 core

    uniform vec4 lightColor;

    void main()
    {
        gl_FragColor = lightColor;
    }
)";

LzhOpenGLWidget::LzhOpenGLWidget(QWidget *parent) :
    QOpenGLWidget(parent),
    light_pos(1.2f, 1.0f, 2.0f)
{
    setFocusPolicy(Qt::StrongFocus);
    setMouseTracking(true);

    cam_pos   = QVector3D(0.0f, 0.0f,  3.0f);
    cam_front = QVector3D(0.0f, 0.0f, -1.0f);
    cam_up    = QVector3D(0.0f, 1.0f,  0.0f);
}

LzhOpenGLWidget::~LzhOpenGLWidget()
{
    glDeleteVertexArrays(1, &object_VAO);
    glDeleteVertexArrays(1, &light_VAO);
    glDeleteBuffers(1, &VBO);
}

void LzhOpenGLWidget::initializeGL()
{
    initializeOpenGLFunctions();

    glEnable(GL_DEPTH_TEST);

    glCreateBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    //==================================================================================
    // object

    glCreateVertexArrays(1, &object_VAO);
    glBindVertexArray(object_VAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (const void *)0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (const void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    object_vertex_shader   = glCreateShader(GL_VERTEX_SHADER);
    object_fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(object_vertex_shader, 1, &object_vertex_shader_source, NULL);
    glCompileShader(object_vertex_shader);
    glShaderSource(object_fragment_shader, 1, &object_fragment_shader_source, NULL);
    glCompileShader(object_fragment_shader);


    object_program = glCreateProgram();

    glAttachShader(object_program, object_vertex_shader);
    glAttachShader(object_program, object_fragment_shader);
    glLinkProgram(object_program);
    int success = true;
    glGetProgramiv(object_program, GL_LINK_STATUS, &success);
    if (success == 0) {
        char info_log[512] = {0};
        glGetProgramInfoLog(object_program, 512, NULL, info_log);
        qDebug() << info_log;
    }

    glUseProgram(object_program);
    glUniform3f(glGetUniformLocation(object_program, "objectColor"), 1.0f, 0.5f, 0.31f);
    glUniform3f(glGetUniformLocation(object_program, "lightColor"), 1.0f, 1.0f, 1.0f);
    glUniform3f(glGetUniformLocation(object_program, "lightPos"), light_pos.x(), light_pos.y(), light_pos.z());
    glUniform3f(glGetUniformLocation(object_program, "material.ambient"), 1.0f, 0.5f, 0.31f);
    glUniform3f(glGetUniformLocation(object_program, "material.diffuse"), 1.0f, 0.5f, 0.31f);
    glUniform3f(glGetUniformLocation(object_program, "material.specular"), 0.5f, 0.5f, 0.5f); // specular lighting doesn't have full effect on this object's material
    glUniform1f(glGetUniformLocation(object_program, "material.shininess"), 32.0f);

    QMatrix4x4 model;
    //model.setToIdentity();
    glUniformMatrix4fv(glGetUniformLocation(object_program, "model"), 1, GL_FALSE, model.constData());

    //==================================================================================
    // light
    glCreateVertexArrays(1, &light_VAO);
    glBindVertexArray(light_VAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (const void *)0);
    glEnableVertexAttribArray(0);

    light_vertex_shader   = glCreateShader(GL_VERTEX_SHADER);
    light_fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(light_vertex_shader, 1, &light_vertex_shader_source, NULL);
    glCompileShader(light_vertex_shader);
    glShaderSource(light_fragment_shader, 1, &light_fragment_shader_source, NULL);
    glCompileShader(light_fragment_shader);

    light_program = glCreateProgram();
    glAttachShader(light_program, light_vertex_shader);
    glAttachShader(light_program, light_fragment_shader);
    glLinkProgram(light_program);
    success = true;
    glGetProgramiv(light_program, GL_LINK_STATUS, &success);
    if (success == 0) {
        char info_log[512] = {0};
        glGetProgramInfoLog(light_program, 512, NULL, info_log);
        qDebug() << info_log;
    }

    glUseProgram(light_program);
    glUniform3f(glGetUniformLocation(object_program, "lightPos"), light_pos.x(), light_pos.y(), light_pos.z());
    glUniform4f(glGetUniformLocation(light_program, "lightColor"), 1.0f, 1.0f, 1.0f, 1.0f);
}

void LzhOpenGLWidget::resizeGL(int w, int h)
{
    Perspective();
    glUseProgram(object_program);
    glUniformMatrix4fv(glGetUniformLocation(object_program, "perspective"), 1, GL_FALSE, perspective.constData());
    glUseProgram(light_program);
    glUniformMatrix4fv(glGetUniformLocation(light_program, "perspective"), 1, GL_FALSE, perspective.constData());
}

void LzhOpenGLWidget::paintGL()
{
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    QMatrix4x4 view = LookAt(cam_pos, cam_pos + cam_front, cam_up);

    glUseProgram(object_program);
    glUniformMatrix4fv(glGetUniformLocation(object_program, "view"), 1, GL_FALSE, view.constData());
    glUniform3f(glGetUniformLocation(object_program, "viewPos"), cam_pos.x(), cam_pos.y(), cam_pos.z());
    glBindVertexArray(object_VAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    glUseProgram(light_program);
    glUniformMatrix4fv(glGetUniformLocation(light_program, "view"), 1, GL_FALSE, view.constData());
    glBindVertexArray(light_VAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
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

    //makeCurrent();
    Perspective();
    glUseProgram(object_program);
    glUniformMatrix4fv(glGetUniformLocation(object_program, "perspective"), 1, GL_FALSE, perspective.data());
    glUseProgram(light_program);
    glUniformMatrix4fv(glGetUniformLocation(light_program, "perspective"), 1, GL_FALSE, perspective.data());
    update();
    //doneCurrent();

    event->accept();
}

void LzhOpenGLWidget::keyPressEvent(QKeyEvent *event)
{
    switch (event->key())
    {
    case Qt::Key_W:
        cam_pos += cam_front * 0.05f;
        update();
        break;
    case Qt::Key_S:
        cam_pos -= cam_front * 0.05f;
        update();
        break;
    case Qt::Key_A:
        cam_pos -= QVector3D::crossProduct(cam_front, cam_up) * 0.05f;
        update();
        break;
    case Qt::Key_D:
        cam_pos += QVector3D::crossProduct(cam_front, cam_up) * 0.05f;
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
    perspective(2, 2) = -(100.0f + 1.0f) / (100.0f - 1.0f);
    perspective(2, 3) = -2 * 100.0 * 1.0 / (100.0f - 1.0f);

    perspective(3, 0) = 0.0f;
    perspective(3, 1) = 0.0f;
    perspective(3, 2) = -1;
    perspective(3, 3) = 0.0f;
}
