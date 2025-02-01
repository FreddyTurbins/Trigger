#include "vendor/glad/glad.h"
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include "TRIGGER/trigger.h"
#include "tgl.h"
#include "trenderer.h"

#define TEGL_MAX_QUADS                                20000
#define TEGL_MAX_VERTICES                             TEGL_MAX_QUADS*4
#define TEGL_MAX_INDICES                              TEGL_MAX_QUADS*6
#define TEGL_MAX_TEXTURES                             32

#define RED(X)                                        ((X)>>(8*3)&0xFF)
#define GREEN(X)                                      ((X)>>(8*2)&0xFF)
#define BLUE(X)                                       ((X)>>(8*1)&0xFF)
#define ALPHA(X)                                      ((X)>>(8*0)&0xFF)

typedef struct QuadVertex {
  Vector3 position;
  Vector4 color;
  Vector2 texCoord;
  float texIndex;
} QuadVertex;

typedef struct CircleVertex
	{
		//Vector3 WorldPosition;
		//Vector3 LocalPosition;
		Vector4 color;
		float thickness;
		float fade;
} CircleVetex;

typedef struct RenderBatchData {
  unsigned int*           textureSlots;
  unsigned int            textureSlotsIndex;
  unsigned int            defaultTexture;

  unsigned int            quadVertexArray;
  unsigned int            quadVertexBuffer;
  unsigned int            quadIndexBuffer;
  unsigned int            quadIndexCount;
  QuadVertex*             quadBufferptr;
  QuadVertex*             quadBuffer;
} RenderBatchData;

typedef struct DrawStats {
  uint32_t                quadCount;
  uint32_t                drawCalls;
} DrawStats;

static RenderBatchData batch = {0};
static DrawStats stats = {0};

//2D Renderer related functions
//============================================================
void tglInit2DRenderer(void)
{
  batch.textureSlots = (unsigned int*)calloc(TEGL_MAX_TEXTURES, sizeof(unsigned int));
  batch.quadBuffer = (QuadVertex*)calloc(TEGL_MAX_QUADS, sizeof(struct QuadVertex));
  batch.quadBufferptr = batch.quadBuffer;

  glCreateVertexArrays(1, &batch.quadVertexArray);
  glBindVertexArray(batch.quadVertexArray);
 
  batch.quadVertexBuffer = tglCreateVertexBuffer(TEGL_MAX_QUADS*sizeof(QuadVertex));

  glEnableVertexArrayAttrib(batch.quadVertexArray, 0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(QuadVertex), (const void*)offsetof(QuadVertex, position));
  glEnableVertexArrayAttrib(batch.quadVertexArray, 1);
  glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(QuadVertex), (const void*)offsetof(QuadVertex, color));
  glEnableVertexArrayAttrib(batch.quadVertexArray, 2);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(QuadVertex), (const void*)offsetof(QuadVertex, texCoord));
  glEnableVertexArrayAttrib(batch.quadVertexArray, 3);
  glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(QuadVertex), (const void*)offsetof(QuadVertex, texIndex));
  
  unsigned int indices[TEGL_MAX_INDICES] = {0};
  for (long k = 0, offset = 0; k < TEGL_MAX_INDICES; k+=6, offset+=4) {
    indices[k + 0] = 0 + offset;
    indices[k + 1] = 1 + offset;
    indices[k + 2] = 2 + offset;

    indices[k + 3] = 2 + offset;
    indices[k + 4] = 3 + offset;
    indices[k + 5] = 0 + offset;
  }
  
  batch.quadIndexBuffer = tglCreateIndexBuffer(indices, TEGL_MAX_INDICES);;

  unsigned char pixels[4] = { 255, 255, 255, 255 };
  batch.defaultTexture = tglCreateTexture(pixels, 1, 1, GL_RGBA);

  batch.textureSlots[0] = batch.defaultTexture;
  for (int i = 1; i < TEGL_MAX_TEXTURES; i++) {
    batch.textureSlots[i] = 0;
  }
  batch.textureSlotsIndex = 1;
}

void tglStartBatch(void)
{
  stats.quadCount = 0;
}


void tglFlushQuad2DRenderer(void)
{
  if (batch.quadIndexCount) {
    size_t size = (uint8_t*)batch.quadBufferptr - (uint8_t*)batch.quadBuffer;
    glBindBuffer(GL_ARRAY_BUFFER, batch.quadVertexBuffer);
    glBufferSubData(GL_ARRAY_BUFFER, 0, size, batch.quadBuffer);

    for (uint32_t i = 0; i < batch.textureSlotsIndex; i++) {
      glBindTextureUnit(i, batch.textureSlots[i]);
    }

    tglBindCurrentShader();
    tglSetUniformSamplersTextures();
    tglSetUniformMat4f("projMatrix", tglGetRenderMatProjection());

    glBindVertexArray(batch.quadVertexArray);
    glDrawElements(GL_TRIANGLES, batch.quadIndexCount, GL_UNSIGNED_INT, NULL);

    batch.quadBufferptr = batch.quadBuffer;
    batch.textureSlotsIndex = 1;
    batch.quadIndexCount = 0;
    stats.drawCalls++;
  }
}

void tglFlush2DRenderer(void)
{
  tglFlushQuad2DRenderer();
}

//2D Renderer drawing related functions
//============================================================
void tglDrawTriangle(const Vector2 v1, const Vector2 v2, const Vector2 v3, const unsigned long color)
{
  if (batch.quadIndexCount >= TEGL_MAX_INDICES) tglFlush2DRenderer();

  const Vector4 color4f = {
    (float)RED(color)/255, (float)GREEN(color)/255, (float)BLUE(color)/255, (float)ALPHA(color)/255
  };
  const Vector3 vertexPosition[4] = {
    {v1.x, v1.y, 0.0f},
    {v2.x, v2.y, 0.0f},
    {v2.x, v2.y, 0.0f},
    {v3.x, v3.y, 0.0f}
  };
  const Vector2 textureCoord[4] = {
    {0.0f, 0.0f},
    {1.0f, 0.0f},
    {1.0f, 1.0f},
    {0.0f, 1.0f}
  };
  const float textureIndex = 0.0f;                       //DEFAULT
  
  for (int k = 0; k < 4; k++) {
    batch.quadBufferptr->position = vertexPosition[k];
    batch.quadBufferptr->color    = color4f;
    batch.quadBufferptr->texCoord = textureCoord[k];
    batch.quadBufferptr->texIndex = textureIndex;
    batch.quadBufferptr++;
  }
  
  batch.quadIndexCount += 6;
  stats.quadCount++;
}

void tglDrawQuad(const Rectangle data, const unsigned long color)
{
  if (batch.quadIndexCount >= TEGL_MAX_INDICES) tglFlush2DRenderer();

  const Vector4 color4f = {
    (float)RED(color)/255, (float)GREEN(color)/255, (float)BLUE(color)/255, (float)ALPHA(color)/255
  };
  const Vector3 vertexPosition[4] = {
    {data.x             , data.y              , 0.0f},  //TOP-LEFT
    {data.x             , data.y + data.height, 0.0f},  //BOT-LEFT
    {data.x + data.width, data.y + data.height, 0.0f},  //BOT-RIGHT
    {data.x + data.width, data.y              , 0.0f}   //TOP-RIGHT
  };
  const Vector2 textureCoord[4] = {
    {0.0f, 0.0f},
    {1.0f, 0.0f},
    {1.0f, 1.0f},
    {0.0f, 1.0f}
  };
  const float textureIndex = 0.0f;                       //DEFAULT
  
  for (int k = 0; k < 4; k++) {
    batch.quadBufferptr->position = vertexPosition[k];
    batch.quadBufferptr->color    = color4f;
    batch.quadBufferptr->texCoord = textureCoord[k];
    batch.quadBufferptr->texIndex = textureIndex;
    batch.quadBufferptr++;
  }
  
  batch.quadIndexCount += 6;
  stats.quadCount++;
}

void tglDrawCircle(const Vector2 center, const float radius, const float thickness, const unsigned long color)
{
  if (batch.quadIndexCount+6 >= TEGL_MAX_INDICES) tglFlush2DRenderer();
  //const float fade = 0.005;
  const Vector4 color4f = {
    (float)RED(color)/255, (float)GREEN(color)/255, (float)BLUE(color)/255, (float)ALPHA(color)/255
  };
  const Vector3 vertexPosition[8] = {
    {center.x + 0 * radius, center.y + 0 * radius, 0},
    {center.x - 1 * radius, center.y + 0 * radius, 0},
    {center.x + 0 * radius, center.y + 1 * radius, 0},
    {center.x + 1 * radius, center.y + 0 * radius, 0},

    {center.x + 0 * radius, center.y + 0 * radius, 0},
    {center.x + 1 * radius, center.y + 0 * radius, 0},
    {center.x + 0 * radius, center.y - 1 * radius, 0},
    {center.x - 1 * radius, center.y + 0 * radius, 0}
  };
  const Vector2 textureCoord[4] = {
    {0.0f, 1.0f},
    {0.0f, 0.0f},
    {1.0f, 0.0f},
    {1.0f, 1.0f}
  };
  const float textureIndex = 0.0f;  
  
  for (int k = 0; k < 8; k++) {
    batch.quadBufferptr->position = vertexPosition[k];
    batch.quadBufferptr->color    = color4f;
    batch.quadBufferptr->texCoord = textureCoord[k%4];
    batch.quadBufferptr->texIndex = textureIndex;
    batch.quadBufferptr++;
  }
  
  batch.quadIndexCount += 12;
  stats.quadCount++;
  stats.quadCount++;
}

void tglDrawTexture(const float texture, const Rectangle data, const unsigned long color)
{
  if (batch.quadIndexCount >= TEGL_MAX_INDICES) tglFlush2DRenderer();
  const Vector4 color4f = {
    (float)RED(color)/255, (float)GREEN(color)/255, (float)BLUE(color)/255, (float)ALPHA(color)/255
  };
  const Vector3 vertexPosition[4] = {
    {data.x             , data.y              , 0.0f},  //TOP-LEFT
    {data.x             , data.y + data.height, 0.0f},  //BOT-LEFT
    {data.x + data.width, data.y + data.height, 0.0f},  //BOT-RIGHT
    {data.x + data.width, data.y              , 0.0f}   //TOP-RIGHT
  };
  const Vector2 textureCoord[4] = {
    {0.0f, 1.0f},
    {0.0f, 0.0f},
    {1.0f, 0.0f},
    {1.0f, 1.0f}
  };
  
  float textureIndex = 0.0f;                            //DEFAULT
  for (uint32_t k = 1; k < batch.textureSlotsIndex; k++)
  {
    if (batch.textureSlots[k] == texture) {
      textureIndex = (float)k;
      break;
    }
  }

  if (textureIndex == 0.0f) {
    if (batch.textureSlotsIndex >= TEGL_MAX_TEXTURES) tglFlush2DRenderer();

    textureIndex = (float)batch.textureSlotsIndex;
    batch.textureSlots[batch.textureSlotsIndex] = texture;
    batch.textureSlotsIndex++;
  }

  for (int k = 0; k < 4; k++) {
    batch.quadBufferptr->position = vertexPosition[k];
    batch.quadBufferptr->color    = color4f;
    batch.quadBufferptr->texCoord = textureCoord[k];
    batch.quadBufferptr->texIndex = textureIndex;
    batch.quadBufferptr++;
  }
  
  batch.quadIndexCount += 6;
  stats.quadCount++;
}
