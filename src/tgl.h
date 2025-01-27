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

#define TEGL_DEFAULT_BATCHING_BUFFER_ELEMENTS         8192
#define TRIGGER_POSITION_VBO                          0
#define TRIGGER_COLOR_VBO                             1
#define TEGL_MAX_TEXTURES                             32

#include "vendor/glad/glad.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include "tmath.h"

typedef struct Vertex {
  Vector3 position;
  Vector4 color;
  Vector2 texCoord;
  float texIndex;
} Vertex;

typedef struct RenderBatch {
  unsigned int            whiteTexture;
  unsigned int            vertexArray;
  unsigned int            vertexBuffer;
  unsigned int            indexBuffer;
  unsigned int            indexCount;
  unsigned int            textureSlotsIndex;
  unsigned int*           textureSlots;
  unsigned int*           indices;
  Vertex*                 quadBuffer;
  Vertex*                 quadBufferptr;
} RenderBatch;

static RenderBatch defaultBatch = {0};

typedef struct TriggerGLData {
  int                     quadCount;
  Vector4                 currentColor;
  unsigned int            defaultVShaderId;
  unsigned int            defaultFShaderId;
  unsigned int            defaultShaderId;
} TriggerGLData;

static TriggerGLData TEGLData = {0};

#ifdef __cplusplus
extern "C" {
#endif
//TGL initialize related functions
//============================================================
TRAPI void tglInit();

//Vertex buffer related functions
//============================================================
TRAPI unsigned int tglCreateVertexBuffer(const float* vertices, const size_t size);
TRAPI void tglBindVertexBuffer(const unsigned int vboId);
TRAPI void tglBindVertexBuffer(const unsigned int vboId);
TRAPI void tglUpdateVertexBuffer(const unsigned int vboId);
TRAPI unsigned int tglCreateIndexBuffer(const unsigned int* data, const unsigned int count);

//Vertex operations related functions
//============================================================
TRAPI void tglSetUniformMat4f(char* uniformName, Mat4 mat);
TRAPI void tglSetUniform4(const unsigned char r, const unsigned char g, const unsigned char b, const unsigned char a);
TRAPI void tglSetVertex3f(const float x, const float y, const float z);

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
TRAPI unsigned int tglCreateShaderProgram(const unsigned int vShaderId, const unsigned int fShaderId);

//Batch rendering related functions
//============================================================
TRAPI RenderBatch tglCreateBatchRender(int bufferElements);
TRAPI void tglDrawBatchRender(RenderBatch* batch);
TRAPI void tglDrawCurrentBatchRender(void);

#if defined(TEGL_IMPLEMENTATION)

//Static modules related functions
//============================================================
static void tglLoadDefaultShader(void);
//TGL initialize related functions
//============================================================
void tglInit()
{
  //TEXTURE THINGS
  tglLoadDefaultShader();
  defaultBatch = tglCreateBatchRender(TEGL_DEFAULT_BATCHING_BUFFER_ELEMENTS);
}

//Vertex buffer functions
//============================================================
unsigned int tglCreateVertexBuffer(const float* vertices, const size_t size)
{
  unsigned int vboId = 0;
  glGenBuffers(1, &vboId);
  glBindBuffer(GL_ARRAY_BUFFER, vboId);
  glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_DYNAMIC_DRAW);
  return vboId;
}

void tglBindVertexBuffer(const unsigned int vboId)
{
  glBindBuffer(GL_ARRAY_BUFFER, vboId);
}

void tglUnbindVertexBuffer(void)
{
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void tglUpdateVertexBuffer(const unsigned int vboId)
{

}

unsigned int tglCreateIndexBuffer(const unsigned int* data, const unsigned int count)
{
  unsigned int iboId = 0;
  glGenBuffers(1, &iboId);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboId);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, count*sizeof(unsigned int), data, GL_STATIC_DRAW);
  return iboId;
}

//Vertex operations
//============================================================
void tglSetUniformMat4f(char* uniformName, Mat4 mat)
{
  int loc = glGetUniformLocation(TEGLData.defaultShaderId, uniformName);
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

void tglSetUniform4(const unsigned char r, const unsigned char g, const unsigned char b, const unsigned char a)
{
  TEGLData.currentColor.x = (float)r/255;
  TEGLData.currentColor.y = (float)g/255;
  TEGLData.currentColor.z = (float)b/255;
  TEGLData.currentColor.w = (float)a/255;
}

void tglSetTexCoord2f(float x, float y)
{
  defaultBatch.quadBufferptr->texCoord = (Vector2){x, y};
}

void tglSetTexIndex(float index)
{
  if (index == 0) {
    defaultBatch.quadBufferptr->texIndex = index;
    return;
  }
  float textIndex = 0.0f;
  for (uint32_t i = 1; i < defaultBatch.textureSlotsIndex; i++)
  {
    if (defaultBatch.textureSlots[i] == index) {
      textIndex = (float)i;
      break;
    }
  }
  if (textIndex == 0.0f) {
    textIndex = (float)defaultBatch.textureSlotsIndex;
    defaultBatch.textureSlots[defaultBatch.textureSlotsIndex] = index;
    defaultBatch.textureSlotsIndex++;
  }
  defaultBatch.quadBufferptr->texIndex = textIndex;
}

void tglSetVertex3f(float x, float y, float z)
{
  defaultBatch.quadBufferptr->position = (Vector3){x, y, z};
  defaultBatch.quadBufferptr->color = TEGLData.currentColor;
  defaultBatch.quadBufferptr++;
}

void tglAddIndexCount()
{
  defaultBatch.indexCount += 6;
  TEGLData.quadCount++;
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

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
  glGenerateMipmap(GL_TEXTURE_2D);
  if (id > 0) {
    TriggerLogCall(LOG_INFO, "TEXTURE -> [ID: %d -> %d/%d] Texture created succesfuly", id, width, height);
  }
  return id;
}

//Shader
//============================================================
unsigned int tglCompileShader(const char* shaderText, int type)
{
  if (type != GL_VERTEX_SHADER && type != GL_FRAGMENT_SHADER) {
    TriggerLogCall(LOG_DEBUG, "\n[%s]", shaderText);
    TriggerLogCall(LOG_WARN, "SHADER -> Invalid type");
    return 0;
  }

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
    free(message);
  } else {
    TriggerLogCall(LOG_DEBUG, "SHADER -> [ID: %d] Succesfully compile %s shader", id,
        (type == GL_VERTEX_SHADER) ? "vertex" : "fragment");
  }
  return id;
}

unsigned int tglCreateShaderProgram(const unsigned int vShaderId, const unsigned int fShaderId)
{
  unsigned int program = glCreateProgram();

  glAttachShader(program, vShaderId);
  glAttachShader(program, fShaderId);
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
    free(message);
  } else {
    TriggerLogCall(LOG_DEBUG, "SHADER -> [ID: %d] Program link succesfuly", program);
  }
  return program;
}
//Batch rendering
//============================================================
 RenderBatch tglCreateBatchRender(int bufferElements)
{
  RenderBatch batch = {0};
  batch.textureSlots = (unsigned int*)calloc(TEGL_MAX_TEXTURES, sizeof(unsigned int));
  batch.indices = (unsigned int*)calloc(bufferElements*6, sizeof(unsigned int));
  batch.quadBuffer = (Vertex*)calloc(bufferElements, sizeof(struct Vertex));
  batch.quadBufferptr = batch.quadBuffer;

  glCreateVertexArrays(1, &batch.vertexArray);
  glBindVertexArray(batch.vertexArray);
 
  glCreateBuffers(1, &batch.vertexBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, batch.vertexBuffer);
  glBufferData(GL_ARRAY_BUFFER, bufferElements*sizeof(struct Vertex), NULL, GL_DYNAMIC_DRAW);

  glEnableVertexArrayAttrib(batch.vertexArray, 0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, position));
  
  glEnableVertexArrayAttrib(batch.vertexArray, 1);
  glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, color));
  
  glEnableVertexArrayAttrib(batch.vertexArray, 2);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, texCoord));
  
  glEnableVertexArrayAttrib(batch.vertexArray, 3);
  glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, texIndex));
  
  for (long k = 0, offset = 0; k < 6*bufferElements; k+=6, offset+=4) {
    batch.indices[k + 0] = 0 + offset;
    batch.indices[k + 1] = 1 + offset;
    batch.indices[k + 2] = 2 + offset;

    batch.indices[k + 3] = 2 + offset;
    batch.indices[k + 4] = 3 + offset;
    batch.indices[k + 5] = 0 + offset;
  }
  
  glCreateBuffers(1, &batch.indexBuffer);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, batch.indexBuffer);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6*bufferElements*sizeof(unsigned int), batch.indices, GL_STATIC_DRAW);

  glCreateTextures(GL_TEXTURE_2D, 1, &batch.whiteTexture);
  glBindTexture(GL_TEXTURE_2D, batch.whiteTexture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  uint32_t color = 0xffffffff;
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, &color);
  batch.textureSlots[0] = batch.whiteTexture;
  for (int i = 1; i < TEGL_MAX_TEXTURES; i++) {
    batch.textureSlots[i] = 0;
  }
  batch.textureSlotsIndex = 1;
  return batch;
}

void tglDrawBatchRender(RenderBatch* batch)
{
  size_t size = (uint8_t*)batch->quadBufferptr - (uint8_t*)batch->quadBuffer;
  glBindBuffer(GL_ARRAY_BUFFER, batch->vertexBuffer);
  glBufferSubData(GL_ARRAY_BUFFER, 0, size, batch->quadBuffer);

  for (uint32_t i = 0; i < batch->textureSlotsIndex; i++) {
    glBindTextureUnit(i, batch->textureSlots[i]);
  }

  glBindVertexArray(batch->vertexArray);
  glDrawElements(GL_TRIANGLES, batch->indexCount, GL_UNSIGNED_INT, NULL);

  batch->quadBufferptr = batch->quadBuffer;
  batch->textureSlotsIndex = 1;
  TEGLData.quadCount = 0;
  batch->indexCount = 0;
}

void tglDrawCurrentBatchRender(void)
{
  tglDrawBatchRender(&defaultBatch);
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
  TEGLData.defaultVShaderId = tglCompileShader(defaultVertexShaderCode, GL_VERTEX_SHADER);
  TEGLData.defaultFShaderId = tglCompileShader(defaultFragmentShaderCode, GL_FRAGMENT_SHADER);
  TEGLData.defaultShaderId = tglCreateShaderProgram(TEGLData.defaultVShaderId, TEGLData.defaultFShaderId);
  glUseProgram(TEGLData.defaultShaderId);

  if (TEGLData.defaultShaderId > 0) {
    TriggerLogCall(LOG_INFO, "SHADER -> [ID %d] Default shader loaded", TEGLData.defaultShaderId);
    //ON ATTACH
    int loc = glGetUniformLocation(TEGLData.defaultShaderId, "textures");
    int samplers[32];
    for(int i = 0; i < 32; i++)
      samplers[i] = i;
    glUniform1iv(loc, 32, samplers);
  } else {
    TriggerLogCall(LOG_WARN, "SHADER -> [ID %d] Default shader failed loading", TEGLData.defaultShaderId);
  }
}
#endif
#endif
