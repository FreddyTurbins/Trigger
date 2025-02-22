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

typedef struct SubTexture {
  unsigned int    id;
  float           min[2];
  float           max[2];
  unsigned int    width;
  unsigned int    height;
} SubTexture;

//USE BMP FILES FOR NOW
typedef struct FontAtlas {
  SubTexture      cell;
  unsigned int    x_cells;
  unsigned int    y_cells;
} FontAtlas;

//SpriteSheet related functions
//============================================================
TRAPI SubTexture create_sub_texture(const Texture texture, const Vector2 coords, const Vector2 cell_size, const Vector2 sprite_size);
TRAPI FontAtlas create_font_atlas(const char* filepath, Vector2 cells);

//2D Renderer related functions
//============================================================
TRAPI void init_renderer2d(void);
TRAPI void start_batch(void);
TRAPI void flush_renderer2d(void);
TRAPI void flush_quad_renderer2d(void);
TRAPI void flush_disk_renderer2d(void);
TRAPI void flush_text_renderer2d(void);
TRAPI void flush_line_renderer2d(void);

//2D Renderer drawing related functions
//============================================================
TRAPI void draw_line(const Vector2 v1, const Vector2 v2, const Color color);
TRAPI void draw_line_thickness(const Vector2 v1, const Vector2 v2, const float thickness, const Color color);
TRAPI void draw_triangle(const Vector2 v1, const Vector2 v2, const Vector2 v3, const Color color);
TRAPI void draw_quad(const Rectangle data, const Color color);
TRAPI void draw_rectangle(const Rectangle data, const Color color);
TRAPI void draw_rectangle_thickness(const Rectangle data, const float thickness, const Color color);
TRAPI void draw_disk(const Vector2 center, const float radius, const Color color);
TRAPI void draw_disk_thickness(const Vector2 center, const float radius, float thickness, const Color color);
TRAPI void draw_texture(const Texture texture, const Vector2 pos, const Color color);
TRAPI void draw_texture_extended(const Texture texture, const Rectangle data, const float scale, const Color color);
TRAPI void draw_sub_texture(const SubTexture sub_texture, const Vector2 pos, const Color color);
TRAPI void draw_sub_texture_extended(const SubTexture sub_texture, const Rectangle data, const float scale, const Color color);
TRAPI void draw_text_atlas(const FontAtlas font_atlas, const char* text, const Vector2 pos, const float scale, const Color color);

//Shader mode functions
//============================================================
TRAPI void begin_shader(unsigned int shader);
TRAPI void end_shader(void);

//Time related fuctions
//============================================================
TRAPI double get_deltatime(void);
TRAPI int get_framerate(void);
TRAPI double get_time(void);
TRAPI void set_framerate_cap(int frame_rate);
#endif
