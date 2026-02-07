#ifndef LZHOPENGLWIDGET_H
#define LZHOPENGLWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions_4_5_Core>
#include <QMatrix4x4>
#include "shader.h"

class TmpMatrix;

class LzhOpenGLWidget : public QOpenGLWidget, QOpenGLFunctions_4_5_Core
{
public:
    LzhOpenGLWidget(QWidget *parent);
    virtual ~LzhOpenGLWidget();

    // QOpenGLWidget interface
protected:
    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();

    // QWidget interface
protected:
    void mouseMoveEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

private:
    void RenderCube();
    void RenderQuad();
    void RenderSphere();

    // 加速插值
    float OurLerp(float a, float b, float f);

    QMatrix4x4 LookAt( QVector3D &eye, const QVector3D &center, const QVector3D &up);
    QMatrix4x4 Perspective(float fov, float aspect, float near, float far);
    QVector4D MakeQuaternion(QVector3D rotation_axis, double radian);

    unsigned int LoadTexture(const char *path);
    TmpMatrix GaussJordanInverse(TmpMatrix matrix, double epsilon = 1e-9);

public:

    Shader shader;
    const QVector3D light_positions[4];
    QVector3D light_colors[4];
    int nr_rows    = 7;
    int nr_columns = 7;
    float spacing  = 2.5;

    unsigned int quad_vao = 0, quad_vbo = 0;
    unsigned int cube_vao = 0, cube_vbo = 0;
    unsigned int sphere_vao = 0;
    unsigned int index_count;

    // bool         b_key_released = true;
    // bool         bloom = true;
    // float        exposure = 1.0f;

    // for LookAt
    QVector3D cam_pos;
    QVector3D cam_front;
    QVector3D cam_up;

    QMatrix4x4 perspective;
    float far = 50.0f;
    float near = 0.1f;
    float fov = 45.0f;

    bool first_mouse = true;
    float yaw = -90.0f;
    float pitch = 0.0f;
};

class TmpMatrix {
public:
    TmpMatrix(int rows, int cols) : data(rows, std::vector<double>(cols, 0)) {}

    double& operator()(int row, int col) {
        return data[row][col];
    }

    const double& operator()(int row, int col) const {
        return data[row][col];
    }

    int get_rows() const {
        return data.size();
    }

    int get_cols() const {
        return data[0].size();
    }

private:
    std::vector<std::vector<double>> data;
};

#endif // LZHOPENGLWIDGET_H



/*
 *初始化矩阵和辅助数据结构

#include <iostream>
#include <vector>

std::vector<std::vector<double>> initializeMatrix(int rows, int cols) {
    return std::vector<std::vector<double>>(rows, std::vector<double>(cols));
}


// 简化版（该简化版算法的效率并不高，时间复杂度为O(n^3)，对于大矩阵来说效率很低。可以通过各种优化方法来提高效率)
// std::vector<std::vector<double>> inverseMatrix(std::vector<std::vector<double>> matrix) {
//     // 获取矩阵的大小
//     int n = matrix.size();
//     std::vector<std::vector<double>> inv(n, std::vector<double>(n, 0));
//     // 初始化逆矩阵为单位矩阵
//     for (int i = 0; i < n; i++) {
//         inv[i][i] = 1;
//     }
//     // 高斯-约当消元法求逆
//     for (int i = 0; i < n; i++) {
//         // 寻找主元
//         double pivot = matrix[i][i];
//         for (int j = 0; j < n; j++) {
//             matrix[i][j] /= pivot;
//             inv[i][j] /= pivot;
//         }
//         for (int j = 0; j < n; j++) {
//             if (j != i) {
//                 double factor = matrix[j][i];
//                 for (int k = 0; k < n; k++) {
//                     matrix[j][k] -= factor * matrix[i][k];
//                     inv[j][k] -= factor * inv[i][k];
//                 }
//             }
//         }
//     }
//     return inv;
// }
 */

/*
 * 高斯消元法求矩阵逆
 *

#include <iostream>
#include <vector>
#include <cmath>

// 声明一个用于存储矩阵的类
class Matrix {
public:
    Matrix(int rows, int cols) : data(rows, std::vector<double>(cols, 0)) {}

    double& operator()(int row, int col) {
        return data[row][col];
    }

    const double& operator()(int row, int col) const {
        return data[row][col];
    }

    int get_rows() const {
        return data.size();
    }

    int get_cols() const {
        return data[0].size();
    }

private:
    std::vector<std::vector<double>> data;
};

// 高斯消元法求矩阵逆
Matrix gaussJordanInverse(Matrix matrix, double epsilon = 1e-9) {
    int size = matrix.get_rows();
    Matrix inverse(size, size);

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

// 示例用法
int main() {
    Matrix m(3, 3);
    // 假设这里有一个3x3的矩阵m，需要被计算逆矩阵
    // ... (此处应当有m的具体初始化代码)

    Matrix m_inv = gaussJordanInverse(m);
    // 打印逆矩阵
    for (int i = 0; i < m_inv.get_rows(); ++i) {
        for (int j = 0; j < m_inv.get_cols(); ++j) {
            std::cout << m_inv(i, j) << " ";
        }
        std::cout << std::endl;
    }

    return 0;
}
*/
