/*************************************************************************************************
*
*     Trigger open-gl abstraction API Version 0.1
*
*************************************************************************************************/
#ifndef TGL_H
#define TGL_H

#ifdef _WIN32
  #define TRAPI __declspec(dllexport)
#endif

#ifndef TRAPI
  #define TRAPI
#endif

#define TRIGGER_GL_VERSION                            0.1
#define DEFAULT_VERTEX_SHADER                         -1
#define DEFAULT_FRAGMENT_SHADER                       -1

#include "vendor/glad/glad.h"
#include <stdio.h>
#include <stdlib.h>
#include "tmath.h"

typedef struct TriggerGLData {
  unsigned int            quadVShader;
  unsigned int            quadFShader;
  unsigned int            quadShader;
  unsigned int            currentShader;

  Mat4                    projectionMatrix;
} TriggerGLData;

#ifdef __cplusplus
extern "C" {
#endif

//TGL initialize related functions
//============================================================
TRAPI void tglInit();

//Vertex buffer related functions
//============================================================
TRAPI unsigned int tglCreateVertexBuffer(const size_t size);
TRAPI void tglBindVertexBuffer(const unsigned int vertexBuffer);
TRAPI unsigned int tglCreateIndexBuffer(const unsigned int* data, const unsigned int count);

//Vertex operations related functions
//============================================================
TRAPI void tglSetUniformMat4f(char* uniformName, Mat4 mat);
TRAPI void tglSetVertex3f(const float x, const float y, const float z);
TRAPI void tglSetUniformSamplersTextures(void);

//General data related functions
//============================================================
TRAPI void tglSetRenderMatProjection(Mat4 mat);
TRAPI Mat4 tglGetRenderMatProjection(void);

//General render related functions
//============================================================
TRAPI void tglClearScreenBuffer(void);
TRAPI void tglSetBackground(const unsigned char r, const unsigned char g, const unsigned char b, const unsigned char a);

//Texture related functions
//============================================================
TRAPI unsigned int tglCreateTexture(const unsigned char* data, int width, int height, int nrChannel);

//Shader related functions
//============================================================
TRAPI unsigned int tglCompileShader(const char* shaderText, int type);
TRAPI unsigned int tglCreateShaderProgram(const unsigned int vShader, const unsigned int fShader);
TRAPI unsigned int tglGetDefaultVertexShader(void);
TRAPI unsigned int tglGetDefaultFragmentShader(void);
TRAPI unsigned int tglGetDefaultShader(void);
TRAPI void tglSetShader(const unsigned int shader);
TRAPI void tglBindCurrentShader();
TRAPI unsigned int tglGetCurrentShader(void);

#if defined(TEGL_IMPLEMENTATION)

static TriggerGLData TEGLData = {0};
#include "trenderer.h"

//Static modules related functions
//============================================================
static void tglLoadDefaultShader(void);

//TGL initialize methods
//============================================================
void tglInit()
{
  tglLoadDefaultShader();
  tglInit2DRenderer();
}

//Vertex buffer methods
//============================================================
unsigned int tglCreateVertexBuffer(const size_t size)
{
  unsigned int vertexBuffer = 0;
  glGenBuffers(1, &vertexBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
  glBufferData(GL_ARRAY_BUFFER, size, NULL, GL_DYNAMIC_DRAW);
  return vertexBuffer;
}

void tglBindVertexBuffer(const unsigned int vertexBuffer)
{
  glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
}

void tglUnbindVertexBuffer(void)
{
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

unsigned int tglCreateIndexBuffer(const unsigned int* data, const unsigned int count)
{
  unsigned int indexBuffer = 0;
  glGenBuffers(1, &indexBuffer);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, count*sizeof(unsigned int), data, GL_STATIC_DRAW);
  return indexBuffer;
}

//Vertex operations
//============================================================
void tglSetUniformMat4f(char* uniformName, Mat4 mat)
{
  int loc = glGetUniformLocation(TEGLData.currentShader, uniformName);
  if (loc < 0) {
    TriggerLogCall(LOG_WARN, "SHADER-> [%s] uniform is not valid", uniformName);
  }
  float fMat[16] = {
    mat.m0, mat.m1, mat.m2, mat.m3,
    mat.m4, mat.m5, mat.m6, mat.m7,
    mat.m8, mat.m9, mat.m10, mat.m11,
    mat.m12, mat.m13, mat.m14, mat.m15
  };
  glUniformMatrix4fv(loc, 1, GL_FALSE, fMat);
}

void tglSetUniformSamplersTextures(void)
{
  int loc = glGetUniformLocation(TEGLData.currentShader, "textures");
  int samplers[32];
  for(int i = 0; i < 32; i++)
    samplers[i] = i;
  glUniform1iv(loc, 32, samplers);
}

//General data methods
//============================================================
void tglSetRenderMatProjection(Mat4 mat)
{
  TEGLData.projectionMatrix = mat;
}

Mat4 tglGetRenderMatProjection(void)
{
  return TEGLData.projectionMatrix;
}

//General render
//============================================================
void tglClearScreenBuffer(void)
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void tglSetBackground(const unsigned char r, const unsigned char g, const unsigned char b, const unsigned char a)
{
  float red = (float)r/255;
  float green = (float)g/255;
  float blue = (float)b/255;
  float alpha = (float)a/255;
  glClearColor(red, green, blue, alpha);
}

//Texture
//============================================================
unsigned int tglCreateTexture(const unsigned char* data, int width, int height, int nrChannel)
{
  unsigned int id = 0;
  glBindTexture(GL_TEXTURE_2D, 0);

  glGenTextures(1, &id);
  glBindTexture(GL_TEXTURE_2D, id);

  //WITHOUT MIMMAP
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, nrChannel, GL_UNSIGNED_BYTE, data);
  glGenerateMipmap(GL_TEXTURE_2D);
 
  return id;
}

//Shader
//============================================================
unsigned int tglCompileShader(const char* shaderText, int type)
{
  if (type != TRIGGER_VERTEX_SHADER && type != TRIGGER_FRAGMENT_SHADER) {
    TriggerLogCall(LOG_DEBUG, "\n[%s]", shaderText);
    TriggerLogCall(LOG_WARN, "SHADER -> Invalid type");
    return 0;
  }
  
  type = (type) ? GL_FRAGMENT_SHADER : GL_VERTEX_SHADER;

  unsigned int id = glCreateShader(type);
  glShaderSource(id, 1, &shaderText, NULL);
  glCompileShader(id);

  int result;
  glGetShaderiv(id, GL_COMPILE_STATUS, &result);
  if (result == GL_FALSE) {
    int countResult = 0;
    glGetShaderiv(id, GL_INFO_LOG_LENGTH, &countResult);
    char* message = (char*)calloc(countResult, sizeof(char));
    glGetShaderInfoLog(id, countResult, &countResult, message);
    message[countResult-1] = '\0';
    TriggerLogCall(LOG_WARN, "SHADER -> [ID: %d] Failed compiling %s shader\n%s", id,
        (type == GL_VERTEX_SHADER) ? "vertex" : "fragment", message);
    FreeTextData(message);
  } else {
    TriggerLogCall(LOG_DEBUG, "SHADER -> [ID: %d] Succesfully compile %s shader", id,
        (type == GL_VERTEX_SHADER) ? "vertex" : "fragment");
  }
  return id;
}

unsigned int tglCreateShaderProgram(const unsigned int vShader, const unsigned int fShader)
{
  unsigned int program = glCreateProgram();

  glAttachShader(program, vShader);
  glAttachShader(program, fShader);
  glLinkProgram(program);
  glValidateProgram(program);

  int success = 0;
  glGetProgramiv(program, GL_LINK_STATUS, &success);
  if (success == GL_FALSE) {
    int countResult = 0;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &countResult);
    char* message = (char*)calloc(countResult, sizeof(char));
    glGetProgramInfoLog(program, countResult, &countResult, message);
    message[countResult-1] = '\0';
    TriggerLogCall(LOG_WARN, "SHADER -> [ID: %d] Program failed linking\n%s", program, message);
    FreeTextData(message);
  } else {
    TriggerLogCall(LOG_DEBUG, "SHADER -> [ID: %d] Program link succesfuly", program);
  }
  return program;
}

unsigned int tglGetDefaultVertexShader(void)
{
  return TEGLData.quadVShader;
}

unsigned int tglGetDefaultFragmentShader(void)
{
  return TEGLData.quadFShader;
}

unsigned int tglGetDefaultShader(void)
{
  return TEGLData.quadShader;
}

void tglSetShader(const unsigned int shader)
{
  TEGLData.currentShader = shader;
}

void tglBindCurrentShader()
{
  glUseProgram(TEGLData.currentShader);
}

unsigned int tglGetCurrentShader(void)
{
  return TEGLData.currentShader;
}

//Static modules
//============================================================
static void tglLoadDefaultShader(void)
{  
  const char* defaultVertexShaderCode = 
    "#version 450 core                                        \n"
    "                                                         \n"
    "layout (location = 0) in vec3 vertexPosition;            \n"
    "layout (location = 1) in vec4 vertexColor;               \n"
    "layout (location = 2) in vec2 textureCoord;              \n"
    "layout (location = 3) in float textureIndex;             \n"
    "                                                         \n"
    "uniform mat4 projMatrix;                                 \n"
    "                                                         \n"
    "out vec4 f_Color;                                        \n"
    "out vec2 f_TexCoord;                                     \n"
    "out float f_TexIndex;                                    \n"
    "void main()                                              \n"
    "{                                                        \n"
    " f_Color = vertexColor;                                  \n"
    " f_TexCoord = textureCoord;                              \n"
    " f_TexIndex = textureIndex;                              \n"
    " gl_Position = projMatrix*vec4(vertexPosition, 1.0);     \n"
    "}                                                        \n";

  const char* defaultFragmentShaderCode =
    "#version 450 core                                        \n"
    "                                                         \n"
    "layout (location = 0) out vec4 color;                    \n"
    "                                                         \n"
    "in vec4 f_Color;                                         \n"
    "in vec2 f_TexCoord;                                      \n"
    "in float f_TexIndex;                                     \n"
    "uniform sampler2D textures[32];                          \n"
    "void main()                                              \n"
    "{                                                        \n"
    " int index = int(f_TexIndex);                            \n"
    " color = f_Color*texture(textures[index], f_TexCoord);   \n"
    "}                                                        \n";
  TEGLData.quadVShader    = tglCompileShader(defaultVertexShaderCode, TRIGGER_VERTEX_SHADER);
  TEGLData.quadFShader    = tglCompileShader(defaultFragmentShaderCode, TRIGGER_FRAGMENT_SHADER);
  TEGLData.quadShader     = tglCreateShaderProgram(TEGLData.quadVShader, TEGLData.quadFShader);
  glUseProgram(TEGLData.quadShader);
  tglSetShader(TEGLData.quadShader);

  if (TEGLData.quadShader > 0) {
    TriggerLogCall(LOG_INFO, "SHADER -> [ID %d] Default shader loaded", TEGLData.quadShader);
  } else {
    TriggerLogCall(LOG_WARN, "SHADER -> [ID %d] Default shader failed loading", TEGLData.quadShader);
  }
  
}
#endif
#endif
