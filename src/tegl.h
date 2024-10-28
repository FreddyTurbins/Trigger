/*************************************************************************************************
*
*     Trigger open-gl abstraction API Version 0.1
*
*************************************************************************************************/

#ifndef TGL_H
#define TGL_H

#define TRIGGER_GL_VERSION                            0.1

#define TEGL_DEFAULT_BATCHING_BUFFER_ELEMENTS         8192

#include "external/glad/glad.h"
#include <stdio.h>

typedef struct TriggerGLVertexBuffer {
  unsigned int            elementCount;
  float*                  vertices;
  unsigned int*           indices;
  unsigned int            iboId;
  unsigned int            vaoId;
  unsigned int            vboId;
} TriggerGLVertexBuffer;

typedef struct TriggerGLRenderBatch {
  TriggerGLVertexBuffer*  vertexBuffer;
} TriggerGLRenderBatch;

typedef struct TriggerGLData {
  int                     vertexCounter;
  int*                    attribBuffers;
  TriggerGLRenderBatch    defaultBatch;
  unsigned int            defaultVShaderId;
  unsigned int            defaultFShaderId;
  unsigned int            defaultShaderId;
} TriggerGLData;

static TriggerGLData TEGLData = {0};

//TGL initialize related methods
//============================================================
void tglInit();

//Vertex buffer related methods
//============================================================
unsigned int tglCreateVertexBuffer(const float* vertices, const size_t size);
void tglBindVertexBuffer(const unsigned int vboId);
void tglBindVertexBuffer(const unsigned int vboId);
void tglUpdateVertexBuffer(const unsigned int vboId);
unsigned int tglCreateIndexBuffer(const unsigned int* data, const unsigned int count);
//General render related methods
//============================================================
void tglClearScreenBuffer(void);
void tglSetBackground(const unsigned char r, const unsigned char g, const unsigned char b, const unsigned char a);

//Shader related methods
//============================================================
unsigned int tglCompileShader(const char* shaderText, int type);
unsigned int tglCreateShaderProgram(const unsigned int vShaderId, const unsigned int fShaderId);

//Batch rendering related methods
//============================================================
TriggerGLRenderBatch tglCreateBatchRender(int bufferElements);

//Static modules related methods
//============================================================
static void tglLoadDefaultShader(void);
//TGL initialize related methods
//============================================================
void tglInit()
{
  //TEXTURE THINGS
  currentRendererApi = TEGL;
  tglLoadDefaultShader();
  TEGLData.defaultBatch = tglCreateBatchRender(TEGL_DEFAULT_BATCHING_BUFFER_ELEMENTS);
}

//Vertex buffer methods
//============================================================
unsigned int tglCreateVertexBuffer(const float* vertices, const size_t size)
{
  unsigned int vboId = 0;
  //glCreateBuffers(1, &vboId); //IDK IF ITS CREATE
  glGenBuffers(1, &vboId);
  glBindBuffer(GL_ARRAY_BUFFER, vboId);
  glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_DYNAMIC_DRAW);
  return vboId;
}

void tglBindVertexBuffer(const unsigned int vboId)
{
  glBindBuffer(GL_ARRAY_BUFFER, vboId);
}

void tglunbindvertexbuffer(void)
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

  float vertices[3 * 3] = {
    -0.5f, -0.5f, 0.0f,
    0.5f, -0.5f, 0.0f,
    0.0f, 0.5f, 0.0f
  };
  TEGLData.defaultBatch.vertexBuffer[0].vertices[0] = vertices[0];
  TEGLData.defaultBatch.vertexBuffer[0].vertices[1] = vertices[1];
  TEGLData.defaultBatch.vertexBuffer[0].vertices[2] = vertices[2];
  TEGLData.defaultBatch.vertexBuffer[0].vertices[3] = vertices[3];
  TEGLData.defaultBatch.vertexBuffer[0].vertices[4] = vertices[4];
  TEGLData.defaultBatch.vertexBuffer[0].vertices[5] = vertices[5];
  TEGLData.defaultBatch.vertexBuffer[0].vertices[6] = vertices[6];
  TEGLData.defaultBatch.vertexBuffer[0].vertices[7] = vertices[7];
  TEGLData.defaultBatch.vertexBuffer[0].vertices[8] = vertices[8];

  glBufferData(GL_ARRAY_BUFFER, 3*3*sizeof(float), TEGLData.defaultBatch.vertexBuffer[0].vertices, GL_DYNAMIC_DRAW);
  glUseProgram(TEGLData.defaultShaderId);
  glUniform4f(TEGLData.attribBuffers[1], 0.0, 0.7, 0.3, 1.0);
  //glBindBuffer(GL_ARRAY_BUFFER, TEGLData.defaultBatch.vertexBuffer[0].vboId);
  glBindVertexArray(TEGLData.defaultBatch.vertexBuffer[0].vaoId);
  //glBindBuffer(GL_ARRAY_BUFFER, TEGLData.attribBuffers[0]);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, TEGLData.defaultBatch.vertexBuffer[0].iboId);
  glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, NULL);
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
    char* message = (char*)alloca(countResult * sizeof(char));
    glGetShaderInfoLog(id, countResult, &countResult, message);
    message[countResult-1] = '\0';
    TriggerLogCall(LOG_WARN, "SHADER -> [ID: %d] Failed compiling %s shader\n%s", id,
        (type == GL_VERTEX_SHADER) ? "vertex" : "fragment", message);
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
    char* message = (char*)alloca(countResult * sizeof(char));
    glGetProgramInfoLog(program, countResult, &countResult, message);
    message[countResult-1] = '\0';
    TriggerLogCall(LOG_WARN, "SHADER -> [ID: %d] Program failed linking\n%s", program, message);
  } else {
    TriggerLogCall(LOG_DEBUG, "SHADER -> [ID: %d] Program link succesfuly", program);
  }

  return program;
}
//Batch rendering related methods
//============================================================
TriggerGLRenderBatch tglCreateBatchRender(int bufferElements)
{
  TriggerGLRenderBatch batch = {0};
  
  batch.vertexBuffer = (TriggerGLVertexBuffer*)malloc(sizeof(struct TriggerGLVertexBuffer));
  batch.vertexBuffer[0].vertices = (float*)calloc(bufferElements*3*4, sizeof(float));
  batch.vertexBuffer[0].indices = (unsigned int*)calloc(bufferElements*6, sizeof(unsigned int));
  
  for (int k = 0, j = 0; j < 6*bufferElements; j+=6, k++) {
    batch.vertexBuffer[0].indices[j] = 4*k;
    batch.vertexBuffer[0].indices[j+1] = 4*k+1;
    batch.vertexBuffer[0].indices[j+2] = 4*k+2;
    batch.vertexBuffer[0].indices[j+3] = 4*k;
    batch.vertexBuffer[0].indices[j+4] = 4*k+2;
    batch.vertexBuffer[0].indices[j+5] = 4*k+3;
  }
  
  glGenBuffers(1, &batch.vertexBuffer[0].vaoId);
  glBindVertexArray(batch.vertexBuffer[0].vaoId);
  batch.vertexBuffer[0].vboId = tglCreateVertexBuffer(batch.vertexBuffer[0].vertices, bufferElements*3*4*sizeof(float));
  glEnableVertexAttribArray(TEGLData.attribBuffers[0]);
  glVertexAttribPointer(TEGLData.attribBuffers[0], 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), NULL);
  batch.vertexBuffer[0].iboId = tglCreateIndexBuffer(batch.vertexBuffer[0].indices, bufferElements*6);

  return batch;
}

//Static modules methods
//============================================================
static void tglLoadDefaultShader(void)
{
  TEGLData.attribBuffers = (int*)calloc(2, sizeof(int));
  TEGLData.attribBuffers[0] = -1;
  TEGLData.attribBuffers[1] = -1;

  const char* defaultVertexShaderCode = 
    "#version 330 core                      \n"
    "in vec4 vertexPosition;                \n"
    "void main()                            \n"
    "{                                      \n"
    " gl_Position = vertexPosition;         \n"
    "}                                      \n";

  const char* defaultFragmentShaderCode =
    "#version 330 core                      \n"
    "out vec4 color;                        \n"
    "uniform vec4 vertexColor;              \n"
    "void main()                            \n"
    "{                                      \n"
    " color = vertexColor;                  \n"
    "}                                      \n";
  TEGLData.defaultVShaderId = tglCompileShader(defaultVertexShaderCode, GL_VERTEX_SHADER);
  TEGLData.defaultFShaderId = tglCompileShader(defaultFragmentShaderCode, GL_FRAGMENT_SHADER);
  TEGLData.defaultShaderId = tglCreateShaderProgram(TEGLData.defaultVShaderId, TEGLData.defaultFShaderId);
  glUseProgram(TEGLData.defaultShaderId);

  if (TEGLData.defaultShaderId > 0) {
    TriggerLogCall(LOG_INFO, "SHADER -> [ID %d] Default shader loaded", TEGLData.defaultShaderId);
    TEGLData.attribBuffers[0] = glGetAttribLocation(TEGLData.defaultShaderId, "vertexPosition");
    TEGLData.attribBuffers[1] = glGetUniformLocation(TEGLData.defaultShaderId, "vertexColor");
  } else {
    TriggerLogCall(LOG_WARN, "SHADER -> [ID %d] Default shader failed loading", TEGLData.defaultShaderId);
  }
}
#endif
