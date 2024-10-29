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
  unsigned char*          colors;
  unsigned int            iboId;
  unsigned int            vaoId;
  unsigned int            vboId[2];
} TriggerGLVertexBuffer;

typedef struct TriggerGLRenderBatch {
  TriggerGLVertexBuffer*  vertexBuffer;
} TriggerGLRenderBatch;

typedef struct TriggerGLData {
  int                     vertexCounter;
  int*                    attribBuffers;
  Color                   currentColor;           
  TriggerGLRenderBatch    defaultBatch;
  unsigned int            defaultVShaderId;
  unsigned int            defaultFShaderId;
  unsigned int            defaultShaderId;
} TriggerGLData;

static TriggerGLData TEGLData = {0};

static enum ContentTriggerGLVertexBuffer {
  TRIGGER_POSITION_VBO = 0,
  TRIGGER_COLOR_VBO = 1
};
//TGL initialize related functions
//============================================================
void tglInit();

//Vertex buffer related functions
//============================================================
unsigned int tglCreateVertexBuffer(const float* vertices, const size_t size);
void tglBindVertexBuffer(const unsigned int vboId);
void tglBindVertexBuffer(const unsigned int vboId);
void tglUpdateVertexBuffer(const unsigned int vboId);
unsigned int tglCreateIndexBuffer(const unsigned int* data, const unsigned int count);

//Vertex operations related functions
//============================================================
void tglSetUniform4(const unsigned char r, const unsigned char g, const unsigned char b, const unsigned char a);
void tglSetVertex3f(const float x, const float y, const float z);

//General render related functions
//============================================================
void tglClearScreenBuffer(void);
void tglSetBackground(const unsigned char r, const unsigned char g, const unsigned char b, const unsigned char a);

//Shader related functions
//============================================================
unsigned int tglCompileShader(const char* shaderText, int type);
unsigned int tglCreateShaderProgram(const unsigned int vShaderId, const unsigned int fShaderId);

//Batch rendering related functions
//============================================================
TriggerGLRenderBatch tglCreateBatchRender(int bufferElements);
void tglDrawBatchRender(TriggerGLRenderBatch* batch);
void tglDrawCurrentBatchRender(void);

//Static modules related functions
//============================================================
static void tglLoadDefaultShader(void);

//TGL initialize related functions
//============================================================
void tglInit()
{
  //TEXTURE THINGS
  currentRendererApi = TEGL;
  tglLoadDefaultShader();
  TEGLData.defaultBatch = tglCreateBatchRender(TEGL_DEFAULT_BATCHING_BUFFER_ELEMENTS);
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

//Vertex operations
//============================================================
void tglSetUniform4(const unsigned char r, const unsigned char g, const unsigned char b, const unsigned char a)
{
  TEGLData.currentColor.r = r;
  TEGLData.currentColor.g = g;
  TEGLData.currentColor.b = b;
  TEGLData.currentColor.a = a;
}

void tglSetVertex3f(float x, float y, float z)
{
  TEGLData.defaultBatch.vertexBuffer[0].vertices[3*TEGLData.vertexCounter] = x;
  TEGLData.defaultBatch.vertexBuffer[0].vertices[3*TEGLData.vertexCounter+1] = y;
  TEGLData.defaultBatch.vertexBuffer[0].vertices[3*TEGLData.vertexCounter+2] = z;

  TEGLData.defaultBatch.vertexBuffer[0].colors[4*TEGLData.vertexCounter] = TEGLData.currentColor.r;
  TEGLData.defaultBatch.vertexBuffer[0].colors[4*TEGLData.vertexCounter+1] = TEGLData.currentColor.g;
  TEGLData.defaultBatch.vertexBuffer[0].colors[4*TEGLData.vertexCounter+2] = TEGLData.currentColor.b;
  TEGLData.defaultBatch.vertexBuffer[0].colors[4*TEGLData.vertexCounter+3] = TEGLData.currentColor.a;
  
  TEGLData.vertexCounter++;
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
//Batch rendering
//============================================================
TriggerGLRenderBatch tglCreateBatchRender(int bufferElements)
{
  TriggerGLRenderBatch batch = {0};
  
  batch.vertexBuffer = (TriggerGLVertexBuffer*)malloc(sizeof(struct TriggerGLVertexBuffer));
  batch.vertexBuffer[0].vertices = (float*)calloc(bufferElements*3*4, sizeof(float));
  batch.vertexBuffer[0].indices = (unsigned int*)calloc(bufferElements*6, sizeof(unsigned int));
  batch.vertexBuffer[0].colors = (unsigned char*)calloc(bufferElements*4*4, sizeof(unsigned char));
  
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

  batch.vertexBuffer[0].vboId[0] = tglCreateVertexBuffer(batch.vertexBuffer[0].vertices, bufferElements*3*4*sizeof(float));
  glEnableVertexAttribArray(TEGLData.attribBuffers[0]);
  glVertexAttribPointer(TEGLData.attribBuffers[0], 3, GL_FLOAT, GL_FALSE, 0, NULL);
  
  glGenBuffers(1, &batch.vertexBuffer[0].vboId[TRIGGER_COLOR_VBO]);
  glBindBuffer(GL_ARRAY_BUFFER, batch.vertexBuffer[0].vboId[TRIGGER_COLOR_VBO]);
  glBufferData(GL_ARRAY_BUFFER, bufferElements*4*4*sizeof(unsigned char), 
      &batch.vertexBuffer[0].vboId[TRIGGER_COLOR_VBO], GL_DYNAMIC_DRAW);
  glEnableVertexAttribArray(TEGLData.attribBuffers[1]);
  glVertexAttribPointer(TEGLData.attribBuffers[1], 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, 0);
  batch.vertexBuffer[0].iboId = tglCreateIndexBuffer(batch.vertexBuffer[0].indices, bufferElements*6);
  
  glEnableVertexAttribArray(TEGLData.attribBuffers[2]);
  return batch;
}

void tglDrawBatchRender(TriggerGLRenderBatch* batch)
{
  glBindVertexArray(batch->vertexBuffer[0].vaoId);
  
  glBindBuffer(GL_ARRAY_BUFFER, batch->vertexBuffer[0].vboId[TRIGGER_POSITION_VBO]);
  glBufferSubData(GL_ARRAY_BUFFER, 0, 3*TEGLData.vertexCounter*sizeof(float), batch->vertexBuffer[0].vertices);
  
  glBindBuffer(GL_ARRAY_BUFFER, batch->vertexBuffer[0].vboId[TRIGGER_COLOR_VBO]);
  glBufferSubData(GL_ARRAY_BUFFER, 0, 4*TEGLData.vertexCounter*sizeof(unsigned char), batch->vertexBuffer[0].colors);
  

  glUseProgram(TEGLData.defaultShaderId);
  glDrawElements(GL_TRIANGLES, TEGLData.vertexCounter/4*6, GL_UNSIGNED_INT, NULL);

  TEGLData.vertexCounter = 0;
  glUseProgram(0);
  glBindVertexArray(0);
}

void tglDrawCurrentBatchRender(void)
{
  tglDrawBatchRender(&TEGLData.defaultBatch);
}

//Static modules
//============================================================
static void tglLoadDefaultShader(void)
{
  TEGLData.attribBuffers = (int*)calloc(2, sizeof(int));
  TEGLData.attribBuffers[0] = -1;
  TEGLData.attribBuffers[1] = -1;
  //TEGLData.attribBuffers[2] = -1;

  const char* defaultVertexShaderCode = 
    "#version 330 core                      \n"
    "in vec4 vertexPosition;                \n"
    "in vec4 vertexColor;                   \n"
    "out vec4 fragColor;                    \n"
    "void main()                            \n"
    "{                                      \n"
    " fragColor = vertexColor;              \n"
    " gl_Position = vertexPosition;         \n"
    "}                                      \n";

  const char* defaultFragmentShaderCode =
    "#version 330 core                      \n"
    "out vec4 color;                        \n"
    "in vec4 fragColor;                     \n"
    "uniform vec4 colorDiffuse;             \n"
    "void main()                            \n"
    "{                                      \n"
    " color = fragColor;                    \n"
    "}                                      \n";
  TEGLData.defaultVShaderId = tglCompileShader(defaultVertexShaderCode, GL_VERTEX_SHADER);
  TEGLData.defaultFShaderId = tglCompileShader(defaultFragmentShaderCode, GL_FRAGMENT_SHADER);
  TEGLData.defaultShaderId = tglCreateShaderProgram(TEGLData.defaultVShaderId, TEGLData.defaultFShaderId);
  glUseProgram(TEGLData.defaultShaderId);

  if (TEGLData.defaultShaderId > 0) {
    TriggerLogCall(LOG_INFO, "SHADER -> [ID %d] Default shader loaded", TEGLData.defaultShaderId);
    TEGLData.attribBuffers[0] = glGetAttribLocation(TEGLData.defaultShaderId, "vertexPosition");
    TEGLData.attribBuffers[1] = glGetAttribLocation(TEGLData.defaultShaderId, "vertexColor");
    //TEGLData.attribBuffers[2] = glGetUniformLocation(TEGLData.defaultShaderId, "colorDiffuse");
  } else {
    TriggerLogCall(LOG_WARN, "SHADER -> [ID %d] Default shader failed loading", TEGLData.defaultShaderId);
  }
}
#endif
