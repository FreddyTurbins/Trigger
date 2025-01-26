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

#endif
