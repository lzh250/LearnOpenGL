#include "lzhopenglwidget.h"

#include <QKeyEvent>
#include <QDateTime>

float vertices[] = {
    // positions          // normals           // texture coords
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
     0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
};

    // positions all containers
    QVector3D cubePositions[] = {
    QVector3D( 0.0f,  0.0f,  0.0f),
    QVector3D( 2.0f,  5.0f, -15.0f),
    QVector3D(-1.5f, -2.2f, -2.5f),
    QVector3D(-3.8f, -2.0f, -12.3f),
    QVector3D( 2.4f, -0.4f, -3.5f),
    QVector3D(-1.7f,  3.0f, -7.5f),
    QVector3D( 1.3f, -2.0f, -2.5f),
    QVector3D( 1.5f,  2.0f, -2.5f),
    QVector3D( 1.5f,  0.2f, -1.5f),
    QVector3D(-1.3f,  1.0f, -1.5f)
};

const char *object_vertex_shader_source = R"(
    #version 450 core

    layout (location = 0) in vec3 pos;
    layout (location = 1) in vec3 aNormal;
    layout (location = 2) in vec2 aTexCoords;

    out vec3 fragPos;
    out vec3 normal;
    out vec2 texCoords;

    uniform vec3 cubePosition;
    uniform vec4 quaternion;
    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 perspective;

    void main()
    {
        vec3 q_xyz = vec3(quaternion);

        vec3 uv = cross(q_xyz, pos);
        vec3 uuv = cross(q_xyz, uv);
        uv *= (2.0f * quaternion.w);
        uuv *= 2.0f;
        vec3 rotatePos = pos + uv + uuv;
        fragPos = rotatePos + cubePosition;

        uv = cross(q_xyz, aNormal);
        uuv = cross(q_xyz, uv);
        uv *= (2.0f * quaternion.w);
        uuv *= 2.0f;
        normal = aNormal + uv + uuv;

        texCoords = aTexCoords;

        gl_Position = perspective * view * vec4(fragPos, 1.0f);
    }
)";

const char *object_fragment_shader_source = R"(
    #version 450 core

    struct Material {
        sampler2D diffuse;  // 环境光也用这个漫反射的相同值
        sampler2D specular;
        float shininess;
    };

    struct Light {
        vec3 position;

        vec3 ambient;
        vec3 diffuse;
        vec3 specular;

        float constant;
        float linear;
        float quadratic;
    };

    in vec3 fragPos;
    in vec3 normal;
    in vec2 texCoords;

    uniform vec3 viewPos;
    uniform Material material;
    uniform Light light;

    void main()
    {
        // ambient
        vec3 ambient = light.ambient * texture(material.diffuse, texCoords).rgb;

        // diffuse
        vec3 norm = normalize(normal);
        vec3 lightDir = normalize(light.position - fragPos);
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = light.diffuse * diff * texture(material.diffuse, texCoords).rgb;

        // specular
        vec3 viewDir = normalize(viewPos - fragPos);
        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
        vec3 specular = light.specular * spec * texture(material.specular, texCoords).rgb;

        // attenuation
        float distance    = length(light.position - fragPos);
        float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

        ambient  *= attenuation;
        diffuse  *= attenuation;
        specular *= attenuation;

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

    timer.setInterval(1000);
    connect(&timer, &QTimer::timeout, this, [&]() {
        makeCurrent();

        int64_t current_m_epoch = QDateTime::currentMSecsSinceEpoch();

        light_color.setX(sin(current_m_epoch * 2.0));
        light_color.setY(sin(current_m_epoch * 0.7));
        light_color.setZ(sin(current_m_epoch * 1.3));

        doneCurrent();
        update();
    });

    timer.start();
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

    // VBO
    glCreateBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    //==================================================================================
    // object


    //   Textures
    glGenTextures(1, &texture_container2);
    // 上面一行也可换成 glCreateTextures(GL_TEXTURE_2D, 1, &texture_container2);
    glBindTexture(GL_TEXTURE_2D, texture_container2);
    glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    QImage image_container2(":/res/container2.png");
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image_container2.width(), image_container2.height(), 0, GL_BGRA, GL_UNSIGNED_BYTE, image_container2.mirrored().constBits());
    glGenerateMipmap(GL_TEXTURE_2D);

    glGenTextures(1, &texture_container2_specular);
    glBindTexture(GL_TEXTURE_2D, texture_container2_specular);
    glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    QImage image_container2_specular(":/res/image_container2_specular.png");
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image_container2_specular.width(), image_container2_specular.height(), 0, GL_BGRA, GL_UNSIGNED_BYTE, image_container2_specular.mirrored().constBits());
    glGenerateMipmap(GL_TEXTURE_2D);

    //   VAO
    glCreateVertexArrays(1, &object_VAO);
    glBindVertexArray(object_VAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (const void *)0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (const void *)(3 * sizeof(float)));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (const void *)(6 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    //    shaders
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

    glUniform1i(glGetUniformLocation(object_program, "meterial.diffuse"), 0);
    glUniform1i(glGetUniformLocation(object_program, "meterial.specular"), 1);

    // light properties
    glUniform3f(glGetUniformLocation(object_program, "light.position"), light_pos.x(), light_pos.y(), light_pos.z());
    glUniform3f(glGetUniformLocation(object_program, "light.ambient"), 0.2f, 0.2f, 0.2f);
    glUniform3f(glGetUniformLocation(object_program, "light.diffuse"), 0.5f, 0.5f, 0.5f);
    glUniform3f(glGetUniformLocation(object_program, "light.specular"), 1.0f, 1.0f, 1.0f);
    glUniform1f(glGetUniformLocation(object_program, "light.constant"), 1.0f);
    glUniform1f(glGetUniformLocation(object_program, "light.linear"), 0.09f);
    glUniform1f(glGetUniformLocation(object_program, "light.specular"), 0.032f);

    // material properties
    glUniform1f(glGetUniformLocation(object_program, "material.shininess"), 32.0f);


    QMatrix4x4 model;
    //model.setToIdentity();
    glUniformMatrix4fv(glGetUniformLocation(object_program, "model"), 1, GL_FALSE, model.constData());

    //==================================================================================
    // light
    glCreateVertexArrays(1, &light_VAO);
    glBindVertexArray(light_VAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (const void *)0);
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
    glUniform3f(glGetUniformLocation(light_program, "lightPos"), light_pos.x(), light_pos.y(), light_pos.z());

    // 灯光随着时间变化，就移到 paintGL() 中设置了
    //glUniform4f(glGetUniformLocation(light_program, "lightColor"), 1.0f, 1.0f, 1.0f, 1.0f);
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

    // QVector3D ambient_color = light_color * 0.1f;
    // QVector3D diffuse_color = light_color * 0.5f;
    // glUniform3f(glGetUniformLocation(object_program, "light.ambient"), ambient_color.x(), ambient_color.y(), ambient_color.z());
    // glUniform3f(glGetUniformLocation(object_program, "light.diffuse"), diffuse_color.x(), diffuse_color.y(), diffuse_color.z());

    glUniformMatrix4fv(glGetUniformLocation(object_program, "view"), 1, GL_FALSE, view.constData());
    glUniform3f(glGetUniformLocation(object_program, "viewPos"), cam_pos.x(), cam_pos.y(), cam_pos.z());
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_container2);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture_container2_specular);

    // 之前遇到问题10个立方体怎么也出不来，只出来一个。原因：uniform 误写成 out
    for (unsigned int i = 0; i < 10; ++i)
    {
        glUniform3f(glGetUniformLocation(object_program, "cubePosition"), cubePositions[i].x(), cubePositions[i].y(), cubePositions[i].z());
        float angle = 20.0f * i;
        double radians = angle / 180 * M_PI;
        QVector4D quaternion = MakeQuaternion(QVector3D(1.0f, 0.3f, 0.5f), radians);
        glUniform4f(glGetUniformLocation(object_program, "quaternion"), quaternion.x(), quaternion.y(), quaternion.z(), quaternion.w());

        glBindVertexArray(object_VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    glUseProgram(light_program);
    glUniformMatrix4fv(glGetUniformLocation(light_program, "view"), 1, GL_FALSE, view.constData());
    glUniform4f(glGetUniformLocation(light_program, "lightColor"), light_color.x(), light_color.y(), light_color.z(), 1.0);
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
