#include "lzhopenglwidget.h"

float vertices[] = {
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
    -0.5f,  0.5f, -0.5f,
};

const char *object_vertex_shader_source = R"(
    #version 450 core

    layout (location = 0) in vec3 pos;
    uniform mat4 perspective;

    void main()
    {
        gl_Position = perspective * vec4(pos + vec3(0.0f, 0.0f, -8.0f), 1.0f);
    }
)";

const char *object_fragment_shader_source = R"(
    #version 450 core
    uniform vec4 objectColor;

    void main()
    {
        gl_FragColor = objectColor;
    }
)";

const char *light_vertex_shader_source = R"(
    #version 450 core

    layout (location = 0) in vec3 pos;
    uniform mat4 perspective;

    void main()
    {
        gl_Position = perspective * vec4(pos * 0.2f + vec3(1.2f, 1.0f, -8.0f), 1.0f);
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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (const void *)0);
    glEnableVertexAttribArray(0);

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
    glUniform4f(glGetUniformLocation(object_program, "objectColor"), 1.0f, 0.5f, 0.31f, 1.0f);

    //==================================================================================
    // light
    glCreateVertexArrays(1, &light_VAO);
    glBindVertexArray(light_VAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (const void *)0);
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
    glClearColor(0.0f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(object_program);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glUseProgram(light_program);
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

void LzhOpenGLWidget::Perspective()
{
    float scale = tan(45 * 0.5f * M_PI / 180) * 1.0f;
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
