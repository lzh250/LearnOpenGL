#ifndef MESH_H
#define MESH_H

#include "shader.h"

#include <QOpenGLFunctions_4_5_Core>
#include <QVector3D>
#include <string>
#include <vector>

using namespace std;

#define MAX_BONE_INFLUENCE 4

// 顶点
struct Vertex
{
    QVector3D position;     // 位置
    QVector3D normal;       // 法线
    QVector2D tex_coords;   // 纹理像素坐标
    // QVector3D tangent;      // 切线
    // QVector3D bitangent;    // 副切线
    // int       bone_IDs[MAX_BONE_INFLUENCE];  // 骨骼索引
    // float     weights[MAX_BONE_INFLUENCE];  // 骨骼重量
};

// 纹理
struct Texture
{
    unsigned int id;
    string type;        // 类型。比如是漫反射贴图或者是镜面光贴图
    string path;        // 我们储存纹理的路径用于与其它纹理进行比较
};

// 网格
class Mesh : public QOpenGLFunctions_4_5_Core
{
public:
    vector<Vertex>       vertices;
    vector<unsigned int> indices;
    vector<Texture>      textures;
    unsigned int          VAO;

    Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures)
    {
        initializeOpenGLFunctions();


        this->vertices = vertices;
        this->indices = indices;
        this->textures = textures;

        // now that we have all the required data, set the vertex buffers and its attribute pointers.
        setupMesh();
    }

    // 复制构造函数。如果不显性实现，因为QOpenGLFunctions_4_5_Core中携带不可复制和移动的成员 meshes[i]无法编译通过
    Mesh(const Mesh &mesh)
    {
        initializeOpenGLFunctions();

        this->vertices = mesh.vertices;
        this->indices = mesh.indices;
        this->textures = mesh.textures;
        this->VAO = mesh.VAO;
        this->VBO = mesh.VBO;
        this->EBO = mesh.EBO;
    }

    // 赋值运算符。如果不显性实现，因为QOpenGLFunctions_4_5_Core中携带不可复制和移动的成员 meshes.push_back(processMesh(mesh, scene))无法编译通过。 // vector<Mesh>::push_back(Mesh&&)
    Mesh &operator =(const Mesh &mesh)
    {
        if (this != &mesh)
        {
            initializeOpenGLFunctions();

            this->vertices = mesh.vertices;
            this->indices = mesh.indices;
            this->textures = mesh.textures;
            this->VAO = mesh.VAO;
            this->VBO = mesh.VBO;
            this->EBO = mesh.EBO;
        }

        return *this;
    }

    void Draw(Shader &shader)
    {
        unsigned int diffuse_nr = 1;
        unsigned int specular_nr = 1;
        for(unsigned int i = 0; i < textures.size(); i++)
        {
            glActiveTexture(GL_TEXTURE0 + i); // 在绑定之前激活相应的纹理单元
            // 获取纹理序号（diffuse_textureN 中的 N）
            string number;
            string name = textures[i].type;
            if(name == "texture_diffuse")
                number = to_string(diffuse_nr++);
            // else if(name == "texture_specular")
            //     number = to_string(specular_nr++);
            // ...

            glUniform1i(glGetUniformLocation(shader.ID, (name + number).c_str()), i);
            glBindTexture(GL_TEXTURE_2D, textures[i].id);
        }

        // 绘制网格
        glBindVertexArray(VAO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // always good practice to set everything back to defaults once configured.
        glActiveTexture(GL_TEXTURE0);
    }

private:
    // render data
    unsigned int VBO, EBO;

    // initializes all the buffer objects/arrays
    void setupMesh()
    {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tex_coords));

        glBindVertexArray(0);
    }
};

#endif // MESH_H
