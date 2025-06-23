#ifndef LZHOPENGLWIDGET_H
#define LZHOPENGLWIDGET_H

#include <QOpenGLWidget>
#include <QWidget>
#include <QOpenGLFunctions_4_5_Core>
#include <QTimer>
#include <QMatrix4x4>

class LzhOpenGLWidget : public QOpenGLWidget, QOpenGLFunctions_4_5_Core
{
public:
    LzhOpenGLWidget(QWidget *parent);
    virtual ~LzhOpenGLWidget();

public:
    void DrawRectangle();
    void Clear();
    void SetPolygonMode(bool is_wire);

    // QOpenGLWidget interface
protected:
    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();

private:
    void Perspective();

private:
    QTimer timer;
    float radian = 0.0f;

    bool is_draw = false;
    int  shape_type = GL_TRIANGLES;

    QMatrix4x4 perspective;

};

#endif // LZHOPENGLWIDGET_H

/*
//=============================================================

// 四元数
// https://blog.csdn.net/csxiaoshui/article/details/65445633

//=============================================================

// 两个四元数相加

    A(a+bi+cj+dk) + B(e+fi+gj+hk) = C( (a+e)+(b+f)i+(c+g)j+(d+h)k )

    实现代码：

    // Quat 的成员 _v[4] 代表四元数的 (x,y,z,w)
    inline Quat operator +(const Quat &rhs) const
    {
        return Quat(
            _v[0] + rhs._v[0],
            _v[1] + rhs._v[1],
            _v[2] + rhs._v[2],
            _v[3] + rhs._v[3]
        );
    }

//=============================================================

// 两个四元数相减

    (sa,va)-(sb,vb)=(sa-sb, va-vb)

    实现代码：

    inline Quat operator -(const Quat &rhs) const
    {
        return Quat(
            _v[0] - rhs._v[0],
            _v[1] - rhs._v[1],
            _v[2] - rhs._v[2],
            _v[3] - rhs._v[3]
        );
    }

//=============================================================

// 两个四元数相乘

    两个四元数相乘的规则和多项式乘法一样
    (a+ib+jc+kd)*(e+if+jg+kh)
    当有 i，j，k 参与时，规则如下：
    i*i = j*j = k*k = ijk = -1
    i*j = k
    j*i = -k
    j*k = i
    k*j = -i
    k*i = j
    i*k = -j
    使用多项式乘法展开，可以得到：
    (a*e - b*f - c*g - d*h) + i(b*e + a*f + c*h - d*g) + j(a*g - b*h + c*e + d*f) + k(a*h + b*g - c*f + d*e)

    实现代码：

    inline const Quat operator *(const Quat &rhs) const
    {
        return Quat(rhs._v[3] * _v[0] + rhs._v[0] * _v[3] + rhs._v[1] * _v[2] - rhs._v[2] * _v[1],
                    rhs._v[3] * _v[1] - rhs._v[0] * _v[2] + rhs._v[1] * _v[3] + rhs._v[2] * _v[0],
                    rhs._v[3] * _v[2] + rhs._v[0] * _v[1] - rhs._v[1] * _v[0] + rhs._v[2] * _v[3],
                    rhs._v[3] * _v[3] - rhs._v[0] * _v[0] - rhs._v[1] * _v[1] - rhs._v[2] * _v[2]
                   );
    }

//=============================================================

// 两四元数相除

    四元数一般来说不定义除法，因为四元数的乘法运算并不满足交换律。一般有四元数的类定义除法是，其实定义的是q1 * q2^(-1)，其中
    q^(-1) = conj(q) / |q^2|，为什么定义这么奇怪的表达式呢，其实是为了让 q * q^(-1) = 1，这个结论很容易推导出来。conj(q)称为q的共轭表达式，
    con(q) = w - xi - yj - zk，只需要四元数向量部分取负即可。

    实现如下：

    inline const Quat operator /(const Quat &denom) const
    {
        return ((*this) * denom.inverse());
    }

    // Multiplicative inverse method: q^(-1) = q^* / (q*q^*)
    inline const Quat inverse() const
    {
        return conj() / length2();
    }

    // Conjugate
    inline Quat conj() const
    {
        return Quat(-_v[0], -_v[1], -_v[2], _v[3]);
    }

    value_type length2() const
    {
        return _v[0]*_v[0] + _v[1]*_v[1] + _v[2]*_v[2] + _v[3]*_v[3];
    }

//=============================================================

// 获取四元数 和 反计算（通过四元数获取轴和角）

    //=====================================================
    // 2.2.1 绕向量u旋转角度 theta 构造四元数

        向量(u) = (x, y, z) = Ux i + Uy j + Uz k
        q = e^((theta/2)(Ux i + Uy j + Uz k)) = cos(theta/2) + (Ux i + Uy j + Uz k)sin(theta/2)

        实现代码如下：
        void makeRotate(value_type angle, value_type x, value_type y, value_type z)
        {
            const value_type epsilon = 1e-7;
            value_type length = sqrt(x*x + y*y + z*z);
            if (length < epsilon)
            {
                *this = Quat();
                return;
            }

            value_type inversenorm = 1.0 / length;
            value_type coshalfangle = cos(0.5*angle);
            value_type sinhalfangle = sin(0.5*angle);

            _v[0] = x * sinhalfangle * inversenorm;
            _v[1] = y * sinhalfangle * inversenorm;
            _v[2] = z * sinhalfangle * inversenorm;
            _v[3] = coshalfangle;
        }

    //=====================================================
    // 2.2.2 从一个向量旋转到另一个向量构造四元数

        按照最原始的想法，从一个向量旋转到另一个向量，那么旋转轴可以通过两个向量的叉乘得到，旋转角度可以通过两个向量间的夹角得到。（向量间的夹角的余弦可以通过两向量点乘去除以它们的模，再通过反余弦函数计算），得到旋转轴和旋转角度之后就转换成2.2.1中的情形了。
        也就是说最初的代码如下：

        void makeRatate(Vec3<value_type> &u, Vec3<value_type> &v)
        {
            u.normalize();
            v.normalize();

            double costheta = u*v;
            double angle = acos(costheta);

            Vec3<value_type> w = u^v;
            w.normalize();

            makeRotate(angle, w.x(), w.y(), w.z());
        }

        有一种特殊情况需要考虑：两向量共线（包括方向相同和方向相反，也就是夹角是0度和180度的情形）

        void makeRotate(Vec3<value_type> &from, Vec3<value_type> &to)
        {
            const value_type epsilon = 1e-7;
            value_type length1 = from.length();
            value_type length2 = to.length();

            value_type cosangle = from * to / (length1 * length2);

            if (fabs(cosangle - 1) < epsilon)
            {
                makeRotate(0.0, 0.0, 0.0, 1.0);
            }
            else if (fabs(cosangle + 1.0) < epsilon)
            {
                Vec3<velue_type> tmp;
                if ((fabs(from.x()) < (fabs(from.y()))
                {
                    if ((fabs(from.x()) < (fabs(from.z()))
                    {
                        tmp.set(1.0, 0.0, 0.0);
                    }
                    else
                    {
                        tmp.set(0.0, 0.0, 1.0);
                    }
                }
                else if ((fabs(from.y()) < (fabs(from.z()))
                {
                    tmp.set(0.0, 1.0, 0.0);
                }
                else
                {
                    tmp.set(0.0, 0.0, 1.0);
                }

                value_type fromd(from.x(), from.y(), from.z());
                value_type axis(fromd ^ tmp);
                axis.normalize();

                _v[0] = axis[0];
                _v[1] = axis[1];
                _v[2] = axis[2];
                _v[3] = 0.0;
            }
            else
            {
                value_type axis(fromd ^ to);
                value_type angle = acos(cosangle);
                makeRotate(angle, axis.x(), axis.y(), axis.z());
            }
        }

        上述的代码改进了之前代码，但是在计算过程中使用了反三角函数（相对比较耗时），可以通过三角函数公式，简化，不需要调用反三角函数
            sin(theta/2) = ( (1 - cos(theta)/2) ^ (1/2)
            cos(theta/2) = ( (1 + cos(theta)/2) ^ (1/2)
        代码可以修改为

        // 省略部分相同的代码
        ......

        else
        {
            Vec3<velue_type> axis(from ^ to);

            // 这两行换成下面几行
            // value_type angle = acos(cosangle);
            // makeRotate(angle, axis.x(), axis.y(), axis.z());

            axis.normalize();
            value_type half_cos = sqrt(0.5 * (1 + cosangle));
            value_type half_sin = sqrt(0.5 * (1 - cosangle));

            _v[0] = axis[0] * half_sin;
            _v[1] = axis[1] * half_sin;
            _v[2] = axis[2] * half_sin;
            _v[3] = half_cos;
        }

        这样修改之后，去掉了算法中复杂的三角函数运算，事实上还可以进一步改进计算过程，考虑到代码中多次的归一化（normalize）的操作，需要进行多次开方运算，为了简化，可以考虑：
            ||u × v|| = |u|.|v|.|sin(theta)|
            sin(theta) = 2 sin(theta/2)cos(theta/2)
            sqrt(a)sqrt(b) = sqrt(ab)
        于是代码可以修改为：

        else
        {
            value_type normFromAndTo = sqrt(from.length2() * to.length2());
            value_type cos_theta = from * to / normFromAndTo;
            value_type half_cos = sqrt(0.5 * (1 + cos_theta));
            value_type half_sin = sqrt(0.5 * (1 - cos_theta));

            Vec3<velue_type> axis = from ^ to / (normFromAndTo * 2 * half_sin * half_cos);

            _v[0] = axis[0] * half_sin;
            _v[1] = axis[1] * half_sin;
            _v[2] = axis[2] * half_sin;
            _v[3] = half_cos;
        }

        注意到_v[0]到_v[3]中乘以half_sin，之前axis计算的分母中就有half_sin，也就是说这一项可以被化简掉，于是代码简化成

        else
        {
            value_type normFromAndTo = sqrt(from.length2() * to.length2());
            value_type cos_theta = from * to / normFromAndTo;
            value_type half_cos = sqrt(0.5 * (1 + cos_theta));

            Vec3<velue_type> axis = from ^ to / (normFromAndTo * 2 * half_cos);

            _v[0] = axis[0];
            _v[1] = axis[1];
            _v[2] = axis[2];
            _v[3] = half_cos;
        }

    //=====================================================
    // 2.2.3 从四元数获取旋转矩阵和旋转角

        这个过程是上面的反过程，根据之前描述的公式反算就可以，得到的公式是

        // 设四元数是 xi+yj+zk+w，那么旋转角度和旋转轴(a,b,c)是
            angle = 2 * acos(w)
            a = x / sqrt(1-w*w)
            b = y / sqrt(1-w*w)
            c = z / sqrt(1-w*w)

        推导过程 （lzh暂时略）

        但是还需要考虑其他两个特殊情况&#xff1a;也就是共线的情形（角度theta是0度或者180度）
        ......（lzh暂时略）

        综合上面整体的描述，代码如下：

        void getRotate(value_type &angle, value_type &x, value_type &y, value_type &z) const
        {
            Quat q1 = *this;
            if (_v[3] > 1)
                q1.normalize();

            angle = 2 * acos(q1.w());
            value_type s = sqrt(1 - q1.w() * q1.w());

            if (s < 1e-6)
            {
                x = q1.x();
                y = q1.y();
                z = q1.z();
            }
            else
            {
                x = q1.x() / s;
                y = q1.y() / s;
                z = q1.z() / s;
            }
        }

//=============================================================

// 通过四元数进行旋转

    四元数变换向量的算法如下：
    1. 创建一个以v为虚部的纯虚的向量，（v.x + v.y + v.z + 0)
    2. 左乘四元数 q 接着右乘四元数q的共轭四元数 q*
    3. 计算得到的结果也是一个纯的四元数，它的虚部就是变换之后的向量v’
    尽管这样做可以得到变换后的向量，如果计算过程完全按照四元数乘法法则去展开计算，计算量略大 ，可以使用下面的方式优化一下：

    Vec3<value_type> operator *(const Vec3<value_type> &v)
    {
        //nVidia SDK implementation
        Vec3<value_type> uv, uuv;
        Vec3<value_type> qvec(_v[0], _v[1], _v[2]);
        uv = qvec ^ v;
        uuv = qvec ^ uv;
        uv *= (2.0f * _v[3]);
        uuv *= 2.0f;
        return v + uv + uuv;
    }

//=============================================================

// 四元数的插值

    使用四元数来表示旋转，在插值时非常的方便和平滑，如果使用欧拉角来进行插值运算，除了会出现万向节死锁外，插值的效果显得十分的生硬。四元数的球面插值使用下面的公式：
    其中：
    -qm：插值的四元数
    -qa：插值四元数的第一个值（起点）
    -qb：插值四元数的第二个值（终点）
    -t：(0.0,1.0) 之间的一个数
    -theta：qa和qb夹角的一半
    实现如下：

    void dslerp(value_type t, const Quat &from, const Quat &to)
    {
        const double epsilon = 0.00001;
        double omega, cosomega, sinomega, scale_from, scale_to;

        osg::Quat quatTo(to);
        // this is a dot product

        cosomega = from.asVec4() * to.asVec4();

        if (cosomega < 0.0)
        {
            cosomega = - cosomega;
            quatTo = -quatto;
        }

        if ((1.0 - cosomega) > epsilon)
        {
            omega = acos(cosomega);     // 0 <= omega <= Pi (see man acos)
            sinomega = sin(omega);      // this sinomega should always be +ve so
            // could try sinomega=sqrt(1-cosomega*cosomega) to avoid a sin()?
            scale_from = sin((1.0-t)*omega/sinomega;
            scale_to = sin(t*omega)/sinomega;
        }
        else
        {
            //----------------------------------------------------
                The end of the vectors ary very close
                we can use simple linear interpolation - no need
                to worry about the "spherical" interpolation
            //----------------------------------------------------
            scale_from = 1.0 - t;
            scale_to = t;
        }

        *this = (from * scale_from) + (to * scale_to);
    }

*/
