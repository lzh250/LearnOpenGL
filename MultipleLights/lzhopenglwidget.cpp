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
QVector3D cube_positions[] = {
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

// positions of the point lights
QVector3D point_light_positions[] = {
    QVector3D( 0.7f,  0.2f,  2.0f),
    QVector3D( 2.3f, -3.3f, -4.0f),
    QVector3D(-4.0f,  2.0f, -12.0f),
    QVector3D( 0.0f,  0.0f, -3.0f)
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

    struct DirLight {
        vec3 direction;

        vec3 ambient;
        vec3 diffuse;
        vec3 specular;
    };

    struct PointLight {
        vec3 position;

        float constant;
        float linear;
        float quadratic;

        vec3 ambient;
        vec3 diffuse;
        vec3 specular;
    };

    struct SpotLight {
        vec3 position;
        vec3 direction;
        float cutOff;
        float outerCutOff;

        vec3 ambient;
        vec3 diffuse;
        vec3 specular;

        float constant;
        float linear;
        float quadratic;
    };

    #define NR_POINT_LIGHTS 4

    in vec3 fragPos;
    in vec3 normal;
    in vec2 texCoords;

    uniform vec3 viewPos;
    uniform DirLight dirLight;
    uniform PointLight pointLights[NR_POINT_LIGHTS];
    uniform SpotLight spotLight;
    uniform Material material;

    // function prototypes
    vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
    vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
    vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

    void main()
    {
        // properties
        vec3 norm = normalize(normal);
        vec3 viewDir = normalize(viewPos - fragPos);

        // == =====================================================
        // Our lighting is set up in 3 phases: directional, point lights and an optional flashlight
        // For each phase, a calculate function is defined that calculates the corresponding color
        // per lamp. In the main() function we take all the calculated colors and sum them up for
        // this fragment's final color.
        // == =====================================================
        // phase 1: directional lighting
        vec3 result = CalcDirLight(dirLight, norm, viewDir);
        // phase 2: point lights
        for(int i = 0; i < NR_POINT_LIGHTS; i++)
            result += CalcPointLight(pointLights[i], norm, fragPos, viewDir);
        // phase 3: spot light
        result += CalcSpotLight(spotLight, norm, fragPos, viewDir);

        gl_FragColor = vec4(result, 1.0);
    }

    // calculates the color when using a directional light.
    vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
    {
        vec3 lightDir = normalize(-light.direction);
        // diffuse shading
        float diff = max(dot(normal, lightDir), 0.0);
        // specular shading
        vec3 reflectDir = reflect(-lightDir, normal);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
        // combine results
        vec3 ambient = light.ambient * vec3(texture(material.diffuse, texCoords));
        vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, texCoords));
        vec3 specular = light.specular * spec * vec3(texture(material.specular, texCoords));
        return (ambient + diffuse + specular);
    }

    // calculates the color when using a point light.
    vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
    {
        vec3 lightDir = normalize(light.position - fragPos);
        // diffuse shading
        float diff = max(dot(normal, lightDir), 0.0);
        // specular shading
        vec3 reflectDir = reflect(-lightDir, normal);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
        // attenuation
        float distance = length(light.position - fragPos);
        float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
        // combine results
        vec3 ambient = light.ambient * vec3(texture(material.diffuse, texCoords));
        vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, texCoords));
        vec3 specular = light.specular * spec * vec3(texture(material.specular, texCoords));
        ambient *= attenuation;
        diffuse *= attenuation;
        specular *= attenuation;
        return (ambient + diffuse + specular);
    }

    // calculates the color when using a spot light.
    vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
    {
        vec3 lightDir = normalize(light.position - fragPos);
        // diffuse shading
        float diff = max(dot(normal, lightDir), 0.0);
        // specular shading
        vec3 reflectDir = reflect(-lightDir, normal);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
        // attenuation
        float distance = length(light.position - fragPos);
        float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
        // spotlight intensity
        float theta = dot(lightDir, normalize(-light.direction));
        float epsilon = light.cutOff - light.outerCutOff;
        float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
        // combine results
        vec3 ambient = light.ambient * vec3(texture(material.diffuse, texCoords));
        vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, texCoords));
        vec3 specular = light.specular * spec * vec3(texture(material.specular, texCoords));
        ambient *= attenuation * intensity;
        diffuse *= attenuation * intensity;
        specular *= attenuation * intensity;
        return (ambient + diffuse + specular);
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

        light_color.setX(sin(current_m_epoch + 2.0));
        light_color.setY(sin(current_m_epoch + 0.7));
        light_color.setZ(sin(current_m_epoch + 1.3));

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

    glUniform1i(glGetUniformLocation(object_program, "material.diffuse"), 0);
    glUniform1i(glGetUniformLocation(object_program, "material.specular"), 1);

    /*
       Here we set all the uniforms for the 5/6 types of lights we have. We have to set them manually and index
       the proper PointLight struct in the array to set each uniform variable. This can be done more code-friendly
       by defining light types as classes and set their values in there, or by using a more efficient uniform approach
       by using 'Uniform buffer objects', but that is something we'll discuss in the 'Advanced GLSL' tutorial.
    */
    // directional light
    glUniform3f(glGetUniformLocation(object_program, "dirLight.direction"), -0.2f, -1.0f, -0.3f);
    glUniform3f(glGetUniformLocation(object_program, "dirLight.ambient"), 0.05f, 0.05f, 0.05f);
    glUniform3f(glGetUniformLocation(object_program, "dirLight.diffuse"), 0.4f, 0.4f, 0.4f);
    glUniform3f(glGetUniformLocation(object_program, "dirLight.specular"), 0.5f, 0.5f, 0.5f);
    // point light 1
    glUniform3f(glGetUniformLocation(object_program, "pointLights[0].position"), point_light_positions[0].x(), point_light_positions[0].y(), point_light_positions[0].z());
    glUniform3f(glGetUniformLocation(object_program, "pointLights[0].ambient"), 0.05f, 0.05f, 0.05f);
    glUniform3f(glGetUniformLocation(object_program, "pointLights[0].diffuse"), 0.8f, 0.8f, 0.8f);
    glUniform3f(glGetUniformLocation(object_program, "pointLights[0].specular"), 1.0f, 1.0f, 1.0f);
    glUniform1f(glGetUniformLocation(object_program, "pointLights[0].constant"), 1.0f);
    glUniform1f(glGetUniformLocation(object_program, "pointLights[0].linear"), 0.09f);
    glUniform1f(glGetUniformLocation(object_program, "pointLights[0].quadratic"), 0.032f);
    // point light 2
    glUniform3f(glGetUniformLocation(object_program, "pointLights[1].position"), point_light_positions[1].x(), point_light_positions[1].y(), point_light_positions[1].z());
    glUniform3f(glGetUniformLocation(object_program, "pointLights[1].ambient"), 0.05f, 0.05f, 0.05f);
    glUniform3f(glGetUniformLocation(object_program, "pointLights[1].diffuse"), 0.8f, 0.8f, 0.8f);
    glUniform3f(glGetUniformLocation(object_program, "pointLights[1].specular"), 1.0f, 1.0f, 1.0f);
    glUniform1f(glGetUniformLocation(object_program, "pointLights[1].constant"), 1.0f);
    glUniform1f(glGetUniformLocation(object_program, "pointLights[1].linear"), 0.09f);
    glUniform1f(glGetUniformLocation(object_program, "pointLights[1].quadratic"), 0.032f);
    // point light 3
    glUniform3f(glGetUniformLocation(object_program, "pointLights[2].position"), point_light_positions[2].x(), point_light_positions[2].y(), point_light_positions[2].z());
    glUniform3f(glGetUniformLocation(object_program, "pointLights[2].ambient"), 0.05f, 0.05f, 0.05f);
    glUniform3f(glGetUniformLocation(object_program, "pointLights[2].diffuse"), 0.8f, 0.8f, 0.8f);
    glUniform3f(glGetUniformLocation(object_program, "pointLights[2].specular"), 1.0f, 1.0f, 1.0f);
    glUniform1f(glGetUniformLocation(object_program, "pointLights[2].constant"), 1.0f);
    glUniform1f(glGetUniformLocation(object_program, "pointLights[2].linear"), 0.09f);
    glUniform1f(glGetUniformLocation(object_program, "pointLights[2].quadratic"), 0.032f);
    // point light 4
    glUniform3f(glGetUniformLocation(object_program, "pointLights[3].position"), point_light_positions[3].x(), point_light_positions[3].y(), point_light_positions[3].z());
    glUniform3f(glGetUniformLocation(object_program, "pointLights[3].ambient"), 0.05f, 0.05f, 0.05f);
    glUniform3f(glGetUniformLocation(object_program, "pointLights[3].diffuse"), 0.8f, 0.8f, 0.8f);
    glUniform3f(glGetUniformLocation(object_program, "pointLights[3].specular"), 1.0f, 1.0f, 1.0f);
    glUniform1f(glGetUniformLocation(object_program, "pointLights[3].constant"), 1.0f);
    glUniform1f(glGetUniformLocation(object_program, "pointLights[3].linear"), 0.09f);
    glUniform1f(glGetUniformLocation(object_program, "pointLights[3].quadratic"), 0.032f);


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

    // spotLight
    glUniform3f(glGetUniformLocation(object_program, "spotLight.position"), cam_pos.x(), cam_pos.y(), cam_pos.z());
    glUniform3f(glGetUniformLocation(object_program, "spotLight.direction"), cam_front.x(), cam_front.y(), cam_front.z());
    glUniform3f(glGetUniformLocation(object_program, "spotLight.ambient"), 0.0f, 0.0f, 0.0f);
    glUniform3f(glGetUniformLocation(object_program, "spotLight.diffuse"), 1.0f, 1.0f, 1.0f);
    glUniform3f(glGetUniformLocation(object_program, "spotLight.specular"), 1.0f, 1.0f, 1.0f);
    glUniform1f(glGetUniformLocation(object_program, "spotLight.constant"), 1.0f);
    glUniform1f(glGetUniformLocation(object_program, "spotLight.linear"), 0.09f);
    glUniform1f(glGetUniformLocation(object_program, "spotLight.quadratic"), 0.032f);
    glUniform1f(glGetUniformLocation(object_program, "spotLight.cutOff"), cos(12.5f / 360.0f * M_PI));
    glUniform1f(glGetUniformLocation(object_program, "spotLight.outerCutOff"), cos(15.0f / 360.0f * M_PI));
    glActiveTexture(GL_TEXTURE0);

    /*
     * cos(12.5f / 360.0f * M_PI)
     * 着色器中，我们会计算LightDir和SpotDir向量的点积，这个点积返回的将是一个余弦值而不是角度值，所以我们不能直接使用角度值和余弦值进行比较。
     * 为了获取角度值我们需要计算点积结果的反余弦，这是一个开销很大的计算。所以为了节约一点性能开销，我们将会计算切光角对应的余弦值，并将它的结果传入片段着色器中。
     * 由于这两个角度现在都由余弦角来表示了，我们可以直接对它们进行比较而不用进行任何开销高昂的计算。
    */

    glBindTexture(GL_TEXTURE_2D, texture_container2);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture_container2_specular);

    // point light 1
    glUniform3f(glGetUniformLocation(object_program, "pointLights[0].position"), point_light_positions[0].x(), point_light_positions[0].y(), point_light_positions[0].z());
    glUniform3f(glGetUniformLocation(object_program, "pointLights[0].ambient"), 0.05f, 0.05f, 0.05f);
    glUniform3f(glGetUniformLocation(object_program, "pointLights[0].diffuse"), light_color.x(), light_color.y(), light_color.z());
    glUniform3f(glGetUniformLocation(object_program, "pointLights[0].specular"), light_color.x(), light_color.y(), light_color.z());
    glUniform1f(glGetUniformLocation(object_program, "pointLights[0].constant"), 1.0f);
    glUniform1f(glGetUniformLocation(object_program, "pointLights[0].linear"), 0.09f);
    glUniform1f(glGetUniformLocation(object_program, "pointLights[0].quadratic"), 0.032f);

    // 之前遇到问题10个立方体怎么也出不来，只出来一个。原因：uniform 误写成 out
    for (unsigned int i = 0; i < 10; ++i)
    {
        glUniform3f(glGetUniformLocation(object_program, "cubePosition"), cube_positions[i].x(), cube_positions[i].y(), cube_positions[i].z());
        float angle = 20.0f * i;
        double radians = angle / 180 * M_PI;
        QVector4D quaternion = MakeQuaternion(QVector3D(1.0f, 0.3f, 0.5f), radians);
        glUniform4f(glGetUniformLocation(object_program, "quaternion"), quaternion.x(), quaternion.y(), quaternion.z(), quaternion.w());

        glBindVertexArray(object_VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    // we now draw as many light bulbs as we have point lights.
    glUseProgram(light_program);
    glBindVertexArray(light_VAO);
    for (int i = 0; i < 4; ++i)
    {
        glUniform3f(glGetUniformLocation(light_program, "lightPos"), point_light_positions[i].x(), point_light_positions[i].y(), point_light_positions[i].z());
        if (i == 0)
            glUniform4f(glGetUniformLocation(light_program, "lightColor"), light_color.x(), light_color.y(), light_color.z(), 1.0);
        else
            glUniform4f(glGetUniformLocation(light_program, "lightColor"), 1.0, 1.0, 1.0, 1.0);
        glUniformMatrix4fv(glGetUniformLocation(light_program, "view"), 1, GL_FALSE, view.constData());
        glDrawArrays(GL_TRIANGLES, 0, 36);
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
