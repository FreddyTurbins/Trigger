#include "TRIGGER/trigger.h"

#include <stdio.h>
#include <stdlib.h>

#define MAX_KEYBOARD_KEYS_CODE            512
#define MAX_KEY_DOWN_QUEUE                16

typedef enum {
  NONE_API = 0,
  TEGL
} RendererApi;

static RendererApi current_renderer_api = NONE_API;

typedef struct {uint16_t width; uint16_t height;} Resolution;
typedef struct {int32_t x; int32_t y;} WindowPosition;

typedef struct TriggerWindow {
  const char*           title;
  bool                  v_sync;
  bool                  full_screen;
  bool                  should_close;
  Resolution            screen;
  Resolution            render;
  WindowPosition        position;
  WindowPosition        previous_position;

  Mat4                  projection_matrix;
} TriggerWindow;

typedef struct Keyboard {
  int32_t               key_down_queue[MAX_KEY_DOWN_QUEUE];
  uint8_t               current_key_state[MAX_KEYBOARD_KEYS_CODE];
  uint8_t               previous_key_state[MAX_KEYBOARD_KEYS_CODE];
  uint8_t               key_down_queue_count;
} Keyboard;

TriggerWindow trigger_window = {0};
Keyboard keyboard = {0};

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
void init_window(const char* title, const uint16_t width, const uint16_t height)
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

void set_window_icon(const char* filepath)
{
  Image image = {0};
  image.data = stbi_load(filepath, &image.width, &image.height, 0, 4);
  opengl_set_window_icon(image);
}

Vector2 get_window_size(void)
{
  return (Vector2){(float)trigger_window.render.width, (float)trigger_window.render.height};
}

void set_window_size(Vector2 window_size)
{
  opengl_set_viewport(window_size.x, window_size.y);
  trigger_window.render.width = window_size.x;
  trigger_window.render.height = window_size.y;
}

void toggle_full_screen(void)
{
  opengl_toggle_full_screen();
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
int32_t get_key_down(void)
{
  if (keyboard.key_down_queue_count == 0) return 0;

  int32_t key_code = keyboard.key_down_queue[0];
  for (uint8_t key_down_stride = 0; key_down_stride < keyboard.key_down_queue_count; key_down_stride++) {
    keyboard.key_down_queue[key_down_stride] = keyboard.key_down_queue[key_down_stride+1];
  }
  keyboard.key_down_queue[--keyboard.key_down_queue_count] = 0;
  return key_code;
}

bool is_key_pressed(const int32_t key_code)
{
  return keyboard.current_key_state[key_code] == 1 && keyboard.previous_key_state[key_code] == 0;
}

bool is_key_down(const int32_t key_code)
{
  return keyboard.current_key_state[key_code];
}

bool is_mouse_button_pressed(const int32_t button)
{
  return opengl_is_mouse_button_pressed(button);
}

Vector2 get_mouse_position(void)
{
  return (Vector2) {
    .x = opengl_get_mouse_x_position(),
    .y = opengl_get_mouse_y_position()
  };
}

//Drawing functions
//============================================================
void draw_indexed(const VertexArrayObject vertex_array_object, const uint32_t index_count)
{
  tgl_bind_vao(vertex_array_object);
  tgl_draw_triangles(index_count);
}

void draw_lines(const VertexArrayObject vertex_array_object, const uint32_t vertex_count)
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

void set_uniform1iv(int32_t loc, int32_t samples, int32_t* samplers)
{
  tgl_set_uniform1iv(loc, samples, samplers);
}

void bind_texture_unit(uint32_t index, uint32_t slot)
{
  tgl_bind_texture_unit(index, slot);
}

void bind_texture(uint32_t texture)
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

uint32_t create_texture(const uint8_t* data, const int32_t width, const int32_t height, int32_t nr_channel)
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
  
  uint32_t data_count = 0;
  uint8_t* img_data = (uint8_t*)read_text_file(filepath, &data_count);

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
uint32_t load_shader(const char* v_shader_path, const char* f_shader_path)
{
  if (v_shader_path == NULL || v_shader_path[0] == '\0') {
    trigger_log(LOG_WARN, "SHADER -> [%s] Invalid path", v_shader_path);
    return 0;
  }
  if(f_shader_path == NULL || f_shader_path[0] == '\0') {
    trigger_log(LOG_WARN, "SHADER -> [%s] Invalid path", f_shader_path);
    return 0;
  }

  int32_t program = 0;
  uint32_t len = 0;

  char* v_shader_code = read_text_file(v_shader_path, &len);

  len = 0;
  char* f_shader_code = read_text_file(f_shader_path, &len);
  
  program = compile_shader(v_shader_code, f_shader_code);
  free(v_shader_code);
  free(f_shader_code);
  return program;
}

uint32_t compile_shader(const char* v_shader_code, const char* f_shader_code)
{
  uint32_t v_shader = tgl_compile_shader(v_shader_code, TRIGGER_VERTEX_SHADER);
  uint32_t f_shader = tgl_compile_shader(f_shader_code, TRIGGER_FRAGMENT_SHADER);
  return tgl_create_shader_program(v_shader, f_shader);
}

void bind_shader(const uint32_t shader)
{
  tgl_bind_shader(shader);
}

int get_shader_location(uint32_t shader, const char* uniform_name)
{
  return tgl_get_shader_location(shader, uniform_name);
}

void set_shader_uniform_mat4(const uint32_t shader, int loc, Mat4 mat)
{
  tgl_bind_shader(shader);
  tgl_set_uniform_mat4f(loc, mat);
}

VertexBuffer create_vertex_buffer(const uint64_t size)
{
  VertexBuffer vertex_buffer = tgl_create_vertex_buffer(size);
  return vertex_buffer;
}

void bind_vertex_buffer(const VertexBuffer vertex_buffer)
{
  tgl_bind_vertex_buffer(vertex_buffer);
}

void set_vertex_buffer_data(const VertexBuffer vertex_buffer, const void* data, uint32_t size)
{
  tgl_vertex_buffer_data(vertex_buffer, data, size);
}

IndexBuffer create_index_buffer(const uint32_t* indices, const uint32_t count_indices)
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

void set_vao_attribute(VertexArrayObject vao, int32_t idAttr, int32_t number_attr, uint64_t size, const void* offset)
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
  keyboard.key_down_queue_count = 0;
  for (int i = 0; i < MAX_KEYBOARD_KEYS_CODE; i++)
  {
    keyboard.previous_key_state[i] = keyboard.current_key_state[i];
    //keyboard.key_repeat_in_frame[i] = 0;
  }
}

double get_run_time(void)
{
  double time = opengl_get_time();
  return time;
}

//Static functions
//============================================================
/*
static void record_input_event(void)
{
}
*/
