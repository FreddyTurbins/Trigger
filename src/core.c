#include "TRIGGER/trigger.h"

#include <stdio.h>
#include <stdlib.h>

typedef enum {
  NONE_API = 0,
  TEGL
} RendererApi;
static RendererApi current_renderer_api = NONE_API;

typedef struct {unsigned short width; unsigned short height;} Resolution;

typedef struct TriggerWindow {
  const char*           title;
  bool                  v_sync;
  bool                  fullscreen;
  bool                  should_close;
  Resolution            screen;
  Resolution            render;
  
  Mat4                  projection_matrix;
} TriggerWindow;

typedef struct Keyboard {
  
} Keyboard;

TriggerWindow trigger_window  = {0};

#define COLOR_NUMBER(X)                    ((X).r<<(8*3))+((X).g<<(8*2))+((X).b<<(8*1))+(X).a

#if defined(PLATFORM_DESKTOP)
  #define TMATH_IMPLEMENTATION
  #include "trigger_math.h"
  #define TEGL_IMPLEMENTATION
  #include "trigger_gl.h"               //THIS IS THE RIGHT ORDER
  #include "platform/tcoregl.c"
  #define STB_IMAGE_IMPLEMENTATION
  #include "vendor/stb_image.h"
#endif

//Window options functions
//============================================================
void init_window(const char* title, const unsigned short width, const unsigned short height)
{
  if ((title != NULL) && (title[0] != 0)) trigger_window.title = title;
  trigger_window.render.width = width;
  trigger_window.render.height = height;
  trigger_window.v_sync = true;
  #if defined(PLATFORM_DESKTOP)
  init_opengl();
  current_renderer_api = TEGL;
  Mat4 projection_matrix = create_matrix_ortho(0.0f, (float)width, 0.0f, (float)height, -1.0f, 1.0f);
  trigger_window.projection_matrix = projection_matrix;
  #endif
}

bool window_should_close(void)
{
  #if defined(PLATFORM_DESKTOP)
  switch (current_renderer_api) {
    case NONE_API:
      trigger_log(LOG_WARN, "WindowShouldClose function is not detecting an API");
      return true;
    case TEGL: return opengl_should_close() || trigger_window.should_close;
  }
  #endif
  trigger_log(LOG_ERROR, "WindowShouldClose detecting an irregular rendererApi");
  return true;
}

void window_shutdown(void)
{
  trigger_window.should_close = true;
}

void close_window(void)
{
  #if defined(PLATFORM_DESKTOP)
  opengl_close_window();
  #endif
}

Vector2 get_window_size(void)
{
  return (Vector2){(float)trigger_window.render.width, (float)trigger_window.render.height};
}

void set_v_sync(bool enabled)
{
  switch (current_renderer_api) {
    case NONE_API: {trigger_log(LOG_DEBUG, "set_v_sync -> Trigger has not api selected"); break;}
    case TEGL: {opengl_set_v_sync(enabled); break;}
  }
  trigger_window.v_sync = enabled;
}

//Input options related functions
//============================================================
bool is_key_pressed(const int key_code)
{
  return opengl_is_key_pressed(key_code);
}

//Drawing functions
//============================================================
void draw_indexed(const VertexArrayObject vertex_array_object, const unsigned int index_count)
{
  tgl_bind_vao(vertex_array_object);
  tgl_draw_triangles(index_count);
}

void draw_lines(const VertexArrayObject vertex_array_object, const unsigned int vertex_count)
{
  tgl_bind_vao(vertex_array_object);
  tgl_draw_lines(vertex_count);
}

void set_line_thickness(const float thickness)
{
  tgl_set_line_thickness(thickness);
}

void set_background(const Color color)
{
  switch (current_renderer_api) {
    case NONE_API: {trigger_log(LOG_DEBUG, "set_background -> Trigger has not api selected"); return;}
    case TEGL: {tgl_set_background(color.r, color.g, color.b, color.a); return;}
  }
}

void set_render_matrix_projection(Mat4 mat)
{
  trigger_window.projection_matrix = mat;
}

Mat4 get_render_mat_projection(void)
{
  return trigger_window.projection_matrix;
}

void set_uniform1iv(int loc, int samples, int* samplers)
{
  tgl_set_uniform1iv(loc, samples, samplers);
}

void bind_texture_unit(unsigned int index, unsigned int slot)
{
  tgl_bind_texture_unit(index, slot);
}

void bind_texture(unsigned int texture)
{
  tgl_bind_texture(texture);
}

//Texture functions
//============================================================
Texture load_texture(const char* filepath)
{
  Texture texture = {0};

  Image img = read_image_file(filepath);
  
  if (img.data == NULL) {
    trigger_log(LOG_WARN, "TEXTURE -> [%s] Fail loading data from texture", filepath);
    return texture;
  }
  if (img.width == 0 || img.height == 0) {
    trigger_log(LOG_WARN, "TEXTURE -> [%s] Dimension data is not valid for a texture", filepath);
    return texture;
  }
  
  //USING OPENGL ABSTRACTION
  texture.id = tgl_create_texture(img.data, img.width, img.height, img.nr_channel);
  texture.width = img.width;
  texture.height = img.height;
  texture.nr_channel = img.nr_channel;
  free(img.data); //DO ABSTRACTION
  if (texture.id == 0) {
    trigger_log(LOG_WARN, "TEXTURE -> [%s] Fail creating texture", filepath);
  } else if (texture.id > 0) {
    trigger_log(LOG_INFO, "TEXTURE -> [ID: %d -> %d/%d] Texture created succesfuly", texture.id, img.width, img.height);
  }

  return texture;
}

unsigned int create_texture(const unsigned char* data, const int width, const int height, int nr_channel)
{
  if (nr_channel == 1) nr_channel = GL_LUMINANCE;
  else if (nr_channel == 2) nr_channel = GL_LUMINANCE_ALPHA;
  else if (nr_channel == 3) nr_channel = GL_RGB;
  else if (nr_channel == 4) nr_channel = GL_RGBA;
  return tgl_create_texture(data, width, height, nr_channel);
}

Image read_image_file(const char* filepath)
{
  Image img = {0};
  
  unsigned int data_count = 0;
  unsigned char* img_data = (unsigned char*)read_text_file(filepath, &data_count);

  if (img_data == NULL) {
    trigger_log(LOG_WARN, "IMAGE -> [%s] Fail loading data from file", filepath);
    return img;
  }
  
  img.data = stbi_load_from_memory(img_data, data_count, &img.width, &img.height, &img.nr_channel, 0);

  if (img.nr_channel == 1) img.nr_channel = GL_LUMINANCE;
  else if (img.nr_channel == 2) img.nr_channel = GL_LUMINANCE_ALPHA;
  else if (img.nr_channel == 3) img.nr_channel = GL_RGB;
  else if (img.nr_channel == 4) img.nr_channel = GL_RGBA;
  free_text_data(img_data);

  if (img.data == NULL) {
    trigger_log(LOG_WARN, "IMAGE -> [%s] Fail loading image from data", filepath);
  } else {
    trigger_log(LOG_INFO, "IMAGE -> [%s -> %d/%d] Image loaded succesfuly", filepath, img.width, img.height);
  }

  return img;
}

//Shader functions
//============================================================
unsigned int load_shader(const char* v_shader_path, const char* f_shader_path)
{
  if (v_shader_path == NULL || v_shader_path[0] == '\0') {
    trigger_log(LOG_WARN, "SHADER -> [%s] Invalid path", v_shader_path);
    return 0;
  }
  if(f_shader_path == NULL || f_shader_path[0] == '\0') {
    trigger_log(LOG_WARN, "SHADER -> [%s] Invalid path", f_shader_path);
    return 0;
  }

  int program = 0;
  unsigned int len = 0;

  char* v_shader_code = read_text_file(v_shader_path, &len);

  len = 0;
  char* f_shader_code = read_text_file(f_shader_path, &len);
  
  program = compile_shader(v_shader_code, f_shader_code);
  free(v_shader_code);
  free(f_shader_code);
  return program;
}

unsigned int compile_shader(const char* v_shader_code, const char* f_shader_code)
{
  uint32_t v_shader = tgl_compile_shader(v_shader_code, TRIGGER_VERTEX_SHADER);
  uint32_t f_shader = tgl_compile_shader(f_shader_code, TRIGGER_FRAGMENT_SHADER);
  return tgl_create_shader_program(v_shader, f_shader);
}

void bind_shader(const unsigned int shader)
{
  tgl_bind_shader(shader);
}

int get_shader_location(unsigned int shader, const char* uniform_name)
{
  return tgl_get_shader_location(shader, uniform_name);
}

void set_shader_uniform_mat4(const unsigned int shader, int loc, Mat4 mat)
{
  tgl_bind_shader(shader);
  tgl_set_uniform_mat4f(loc, mat);
}

VertexBuffer create_vertex_buffer(const unsigned long long size)
{
  VertexBuffer vertex_buffer = tgl_create_vertex_buffer(size);
  return vertex_buffer;
}

void bind_vertex_buffer(const VertexBuffer vertex_buffer)
{
  tgl_bind_vertex_buffer(vertex_buffer);
}

void set_vertex_buffer_data(const VertexBuffer vertex_buffer, const void* data, unsigned int size)
{
  tgl_vertex_buffer_data(vertex_buffer, data, size);
}

IndexBuffer create_index_buffer(const unsigned int* indices, const unsigned int count_indices)
{
  return tgl_create_index_buffer(indices, count_indices);
}

void bind_index_buffer(const IndexBuffer index_buffer)
{
  tgl_bind_index_buffer(index_buffer);
}

VertexArrayObject create_vao()
{
  VertexArrayObject vertex_array_object = tgl_create_vao();
  return vertex_array_object;
}

void bind_vao(VertexArrayObject vertex_array_object)
{
  tgl_bind_vao(vertex_array_object);
}

void set_vao_attribute(VertexArrayObject vao, int idAttr, int number_attr, unsigned long long size, const void* offset)
{
  tgl_set_vao_attribute(vao, idAttr, number_attr, size, offset);
}

void gfx_update(void) {
  #if defined(PLATFORM_DESKTOP)
    opengl_swap_screen_buffer();
    tgl_clear_screen_buffer();
  #endif
}

void input_polling(void) {

}

double get_run_time(void)
{
  double time = opengl_get_time();
  return time;
}
