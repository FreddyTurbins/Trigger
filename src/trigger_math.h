#ifndef TMATH_H
#define TMATH_H

#ifdef _WIN32
  #define TMAPI __declspec(dllexport)
#endif

#ifndef TMAPI
  #define TMAPI
#endif

#ifndef T_VECTOR2
#define T_VECTOR2
typedef struct Vector2 {
  float x;
  float y;
} Vector2;
#endif

#ifndef T_VECTOR3
#define T_VECTOR3
typedef struct Vector3 {
  float x;
  float y;
  float z;
} Vector3;
#endif

#ifndef T_VECTOR4
#define T_VECTOR4
typedef struct Vector4 {
  float x;
  float y;
  float z;
  float w;
} Vector4;
#endif

#ifndef T_MAT4
#define T_MAT4
typedef struct Mat4 {
  float m0, m1, m2, m3;
  float m4, m5, m6, m7;
  float m8, m9, m10, m11;
  float m12, m13, m14, m15;
} Mat4;
#endif

#ifndef T_RECTANGLE
#define T_RECTANGLE
typedef struct Rectangle {
  float x;
  float y;
  float width;
  float height;
} Rectangle;
#endif

TMAPI Mat4 create_matrix_ortho(double left, double right, double bottom, double top, double zNear, double zFar);

#if defined(TMATH_IMPLEMENTATION)

TMAPI Mat4 create_matrix_ortho(double left, double right, double bottom, double top, double zNear, double zFar)
{
    Mat4 matrix = {0};
    matrix.m0 = 2.0f / (right - left);
    matrix.m5 = 2.0f / (top - bottom);
    matrix.m10 = - 2.0f / (zFar - zNear);
    matrix.m12 = - (right + left) / (right - left);
    matrix.m13 = - (top + bottom) / (top - bottom);
    matrix.m14 = - (zFar + zNear) / (zFar - zNear);
    matrix.m15 = 1.0f;
    return matrix;
}

#endif
#endif
