/*************************************************************************************************
*
*     GameRenderer abstraction API Version 0.1
*
*************************************************************************************************/
#ifndef RENDERER_H
#define RENDERER_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _WIN32
  #define TRAPI __declspec(dllexport)
#endif

#ifndef TRAPI
  #define TRAPI
#endif

//2D Renderer related functions
//============================================================
TRAPI void init_renderer2d(void);
TRAPI void start_batch(void);
TRAPI void flush_renderer2d(void);
TRAPI void flush_quad_renderer2d(void);

//2D Renderer drawing related functions
//============================================================
TRAPI void draw_triangle(const Vector2 v1, const Vector2 v2, const Vector2 v3, const Color color);
TRAPI void draw_quad(const Rectangle data, const Color color);
TRAPI void draw_disk(const Vector2 center, const float radius, const Color color);
TRAPI void draw_disk_thickness(const Vector2 center, const float radius, float thickness, const Color color);
TRAPI void draw_texture(const Texture texture, const Vector2 pos, const Color color);
TRAPI void draw_texture_extended(const Texture texture, const Rectangle data, const float scale, const Color color);

//Shader mode functions
//============================================================
TRAPI void begin_shader(unsigned int shader);
TRAPI void end_shader(void);
#endif
