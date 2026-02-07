#ifndef SHADER_H
#define SHADER_H

#include <QOpenGLFunctions_4_5_Core>
#include <QFile>
#include <qevent.h>
#include <qmatrix4x4.h>
#include <string>
#include <iostream>
using namespace std;

class Shader : public QOpenGLFunctions_4_5_Core
{
public:
    unsigned int ID;

    Shader() {}

    void Init(const char *vertex_path, const char *fragment_path, const char *geometry_path = nullptr)
    {
        initializeOpenGLFunctions();

        // 1. retrieve the vertex/fragment source code from filePath
        // 用ifstream无法直接打开Qt工程的资源文件，要用QFile

        QFile v_shader_file(vertex_path);
        if (!v_shader_file.open(QIODevice::ReadOnly))
        {
            return;
        }
        QByteArray vertex_code = v_shader_file.readAll();
        v_shader_file.close();
        const char* v_shader_code = vertex_code.constData();

        QFile f_shader_file(fragment_path);
        if (!f_shader_file.open(QIODevice::ReadOnly))
        {
            return;
        }
        QByteArray fragment_code = f_shader_file.readAll();
        f_shader_file.close();
        const char* f_shader_code = fragment_code.constData();

        // 2. compile shaders
        unsigned int vertex, fragment, geometry;
        // vertex shader
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &v_shader_code, NULL);
        glCompileShader(vertex);
        CheckCompileErrors(vertex, "VERTEX");
        // fragment shader
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &f_shader_code, NULL);
        glCompileShader(fragment);
        CheckCompileErrors(fragment, "FRAGMENT");
        // geometry shader
        if (geometry_path != nullptr)
        {
            QFile g_shader_file(geometry_path);
            if (!g_shader_file.open(QIODevice::ReadOnly))
            {
                glDeleteShader(vertex);
                glDeleteShader(fragment);
                return;
            }
            QByteArray geometry_code = g_shader_file.readAll();
            g_shader_file.close();
            const char *g_shader_code = geometry_code.constData();
            geometry = glCreateShader(GL_GEOMETRY_SHADER);
            glShaderSource(geometry, 1, &g_shader_code, NULL);
            glCompileShader(geometry);
            CheckCompileErrors(fragment, "GEOMETRY");
        }

        // shader program
        ID = glCreateProgram();
        glAttachShader(ID, vertex);
        glAttachShader(ID, fragment);
        if (geometry_path != nullptr)
        {
            glAttachShader(ID, geometry);
        }
        glLinkProgram(ID);
        CheckCompileErrors(ID, "PROGRAM");
        // delete the shaders as they're linked into our program now and no longer necessary
        glDeleteShader(vertex);
        glDeleteShader(fragment);
        if (geometry_path != nullptr)
        {
            glDeleteShader(geometry);
        }
    }

    void Use()
    {
        glUseProgram(ID);
    }

    void SetFloat(const std::string &name, float value)
    {
        glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
    }

    void SetInt(const std::string &name, int value)
    {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
    }

    void SetVec3(const std::string &name, const QVector3D &vec)
    {
        glUniform3f(glGetUniformLocation(ID, name.c_str()), vec[0], vec[1], vec[2]);
    }

    void SetMat3(const std::string &name, const QMatrix3x3 &mat)
    {
        glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, mat.constData());
    }

    void SetMat4(const std::string &name, const QMatrix4x4 &mat)
    {
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, mat.constData());
    }

private:
    void CheckCompileErrors(GLuint shader, string type)
    {
        GLint success;
        GLchar infoLog[1024];
        if(type != "PROGRAM")
        {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if(!success)
            {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << endl;
            }
        }
        else
        {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if(!success)
            {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << endl;
            }
        }
    }
};

#endif // SHADER_H
