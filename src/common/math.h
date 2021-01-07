//
// Created by 王文优 on 12/31/20.
//

#ifndef SIMPLEPHYSICSENGINE_MATH_H
#define SIMPLEPHYSICSENGINE_MATH_H


#include "macro.h"

class Angle {
public:
    float c, s;
    Angle(float ang = 0) {
        c = cos(ang);
        s = sin(ang);
    }
};

struct Vector2f
{
    /// Default constructor does nothing (for performance).
    Vector2f() {x = 0.0f; y = 0.0f; }

    /// Construct using coordinates.
    Vector2f(float xIn, float yIn) : x(xIn), y(yIn) {}



    /// Set this vector to all zeros.
    void SetZero() { x = 0.0f; y = 0.0f; }

    /// Set this vector to some specified coordinates.
    void Set(float x_, float y_) { x = x_; y = y_;}

    void Set(const Vector2f &vec){x = vec.x; y = vec.y;}

    /// Negate this vector.
    Vector2f operator -() const { Vector2f v; v.Set(-x, -y); return v; }

    /// Read from and indexed element.
    float operator () (uint i) const
    {
        return (&x)[i];
    }

    /// Write to an indexed element.
    float& operator () (uint i)
    {
        return (&x)[i];
    }

    /// Add a vector to this vector.
    void operator += (const Vector2f& v)
    {
        x += v.x; y += v.y;
    }

    /// Subtract a vector from this vector.
    void operator -= (const Vector2f& v)
    {
        x -= v.x; y -= v.y;
    }

    /// Multiply this vector by a scalar.
    void operator *= (float a)
    {
        x *= a; y *= a;
    }

    /// Get the length of this vector (the norm).
    float Length() const
    {
        return sPE_Sqrt(x * x + y * y);
    }

    /// Get the length squared. For performance, use this instead of
    /// Vector2f::Length (if possible).
    float LengthSquared() const
    {
        return x * x + y * y;
    }

    /// Convert this vector into a unit vector. Returns the length.
    float Normalize()
    {
        float length = Length();
        if (length < sPE_epsilon)
        {
            return 0.0f;
        }
        float invLength = 1.0f / length;
        x *= invLength;
        y *= invLength;

        return length;
    }

    /// Does this vector contain finite coordinates?
    bool IsValid() const
    {
        return sPE_isValid(x) && sPE_isValid(y);
    }

    Vector2f Rotate(float angle) const {
        return Vector2f(x * cos(angle) - y * sin(angle), y * cos(angle) + x * sin(angle));
    }

    float x, y;

};

struct Matrix3x3 {
    float a[3][3];
    float *operator[] (int i) {return a[i];}
    const float *operator[] (int i) const {return a[i];}
    Matrix3x3() {memset(a, 0, sizeof(a));}
    Matrix3x3 Transpose() {
        Matrix3x3 res = *this;
        std::swap(res[0][1], res[1][0]);
        std::swap(res[0][2], res[2][0]);
        std::swap(res[1][2], res[2][1]);
        return res;
    }
    Vector2f operator() (Vector2f vec) const {
        return Vector2f(vec.x * a[0][0] + vec.y * a[0][1] + 1.0f * a[0][2],
                        vec.x * a[1][0] + vec.y * a[1][1] + 1.0f * a[1][2]);
    }
    void SetAsAxisTransform(Vector2f movement, float rotationAngle) {
        a[0][0] = cos(rotationAngle); a[0][1] = -sin(rotationAngle);
        a[1][0] = -a[0][1], a[1][1] = a[0][0];
        a[0][2] = movement.x; a[1][2] = movement.y;
        a[2][0] = a[2][1] = 0; a[2][2] = 1;
    }
    void SetAsAxisTransformInverse(Vector2f movement, float rotationAngle) {
        float cosV = cos(rotationAngle), sinV = sin(rotationAngle);
        a[0][0] = a[1][1] = cosV;
        a[0][1] = sinV; a[1][0] = -sinV;
        a[2][0] = a[2][1] = 0; a[2][2] = 1;
        a[0][2] = -movement.x * cosV - movement.y * sinV;
        a[1][2] = movement.x * sinV - movement.y * cosV;
    }
};






////////////////////////////////////////////////////////////
///functions
///Vector2f

/// Add two vectors component-wise.
inline Vector2f operator + (const Vector2f& a, const Vector2f& b)
{
    return Vector2f(a.x + b.x, a.y + b.y);
}

/// Subtract two vectors component-wise.
inline Vector2f operator - (const Vector2f& a, const Vector2f& b)
{
    return Vector2f(a.x - b.x, a.y - b.y);
}

inline Vector2f operator * (float s, const Vector2f& a)
{
    return Vector2f(s * a.x, s * a.y);
}

inline bool operator == (const Vector2f& a, const Vector2f& b)
{
    return a.x == b.x && a.y == b.y;
}

inline bool operator != (const Vector2f& a, const Vector2f& b)
{
    return a.x != b.x || a.y != b.y;
}

/// Perform the cross product on two vectors. In 2D this produces a scalar.
inline float Cross(const Vector2f& a, const Vector2f& b)
{
    return a.x * b.y - a.y * b.x;
}
/// Perform the dot product on two vectors.
inline float Dot(const Vector2f& a, const Vector2f& b)
{
    return a.x * b.x + a.y * b.y;
}




/////////////////////////////////
///Matrix3x3

inline Matrix3x3 operator *(const Matrix3x3 &a, const Matrix3x3 &b) {
    Matrix3x3 c;
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            for (int k = 0; k < 3; k++)
                c[i][j] += a[i][k] * b[k][j];
    return c;
}











#endif //SIMPLEPHYSICSENGINE_MATH_H