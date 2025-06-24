//==========================================================================
//  TRIGGER 0.1
//==========================================================================
#ifndef TRIGGER_H
#define TRIGGER_H

#define TRIGGER_VERSION                               0.1 

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _WIN32
  #define TRAPI __declspec(dllexport)
#endif

#ifndef TRAPI
  #define TRAPI
#endif

typedef enum {
  LOG_ALL,      //To activate logs
  LOG_TRACE,    //To trace internal code (function specifically)
  LOG_DEBUG,    //To display helpful information to developers
  LOG_INFO,     //To display a log information like configuration assumptions
  LOG_WARN,     //To display an odd situation, but with an automatically recovering
  LOG_ERROR,    //To display an error which is fatal to an operation or function
  LOG_FATAL,    //To display a fatal error that force a shutdown
  LOG_OFF       //To disable logs
} LogLevel;

#define TRIGGER_KEY_SPACE                             32
#define TRIGGER_KEY_APOSTROPHE                        39  /* ' */
#define TRIGGER_KEY_COMMA                             44  /* , */
#define TRIGGER_KEY_MINUS                             45  /* - */
#define TRIGGER_KEY_PERIOD                            46  /* . */
#define TRIGGER_KEY_SLASH                             47  /* / */
#define TRIGGER_KEY_0                                 48
#define TRIGGER_KEY_1                                 49
#define TRIGGER_KEY_2                                 50
#define TRIGGER_KEY_3                                 51
#define TRIGGER_KEY_4                                 52
#define TRIGGER_KEY_5                                 53
#define TRIGGER_KEY_6                                 54
#define TRIGGER_KEY_7                                 55
#define TRIGGER_KEY_8                                 56
#define TRIGGER_KEY_9                                 57
#define TRIGGER_KEY_SEMICOLON                         59  /* ; */
#define TRIGGER_KEY_EQUAL                             61  /* = */
#define TRIGGER_KEY_A                                 65
#define TRIGGER_KEY_B                                 66
#define TRIGGER_KEY_C                                 67
#define TRIGGER_KEY_D                                 68
#define TRIGGER_KEY_E                                 69
#define TRIGGER_KEY_F                                 70
#define TRIGGER_KEY_G                                 71
#define TRIGGER_KEY_H                                 72
#define TRIGGER_KEY_I                                 73
#define TRIGGER_KEY_J                                 74
#define TRIGGER_KEY_K                                 75
#define TRIGGER_KEY_L                                 76
#define TRIGGER_KEY_M                                 77
#define TRIGGER_KEY_N                                 78
#define TRIGGER_KEY_O                                 79
#define TRIGGER_KEY_P                                 80
#define TRIGGER_KEY_Q                                 81
#define TRIGGER_KEY_R                                 82
#define TRIGGER_KEY_S                                 83
#define TRIGGER_KEY_T                                 84
#define TRIGGER_KEY_U                                 85
#define TRIGGER_KEY_V                                 86
#define TRIGGER_KEY_W                                 87
#define TRIGGER_KEY_X                                 88
#define TRIGGER_KEY_Y                                 89
#define TRIGGER_KEY_Z                                 90
#define TRIGGER_KEY_LEFT_BRACKET                      91  /* [ */
#define TRIGGER_KEY_BACKSLASH                         92  /* \ */
#define TRIGGER_KEY_RIGHT_BRACKET                     93  /* ] */
#define TRIGGER_KEY_GRAVE_ACCENT                      96  /* ` */
#define TRIGGER_KEY_WORLD_1                           161 /* non-US #1 */
#define TRIGGER_KEY_WORLD_2                           162 /* non-US #2 */

#define TRIGGER_KEY_ESCAPE                            256
#define TRIGGER_KEY_ENTER                             257
#define TRIGGER_KEY_TAB                               258
#define TRIGGER_KEY_BACKSPACE                         259
#define TRIGGER_KEY_INSERT                            260
#define TRIGGER_KEY_DELETE                            261
#define TRIGGER_KEY_RIGHT                             262
#define TRIGGER_KEY_LEFT                              263
#define TRIGGER_KEY_DOWN                              264
#define TRIGGER_KEY_UP                                265
#define TRIGGER_KEY_PAGE_UP                           266
#define TRIGGER_KEY_PAGE_DOWN                         267
#define TRIGGER_KEY_HOME                              268
#define TRIGGER_KEY_END                               269
#define TRIGGER_KEY_CAPS_LOCK                         280
#define TRIGGER_KEY_SCROLL_LOCK                       281
#define TRIGGER_KEY_NUM_LOCK                          282
#define TRIGGER_KEY_PRINT_SCREEN                      283
#define TRIGGER_KEY_PAUSE                             284
#define TRIGGER_KEY_F1                                290
#define TRIGGER_KEY_F2                                291
#define TRIGGER_KEY_F3                                292
#define TRIGGER_KEY_F4                                293
#define TRIGGER_KEY_F5                                294
#define TRIGGER_KEY_F6                                295
#define TRIGGER_KEY_F7                                296
#define TRIGGER_KEY_F8                                297
#define TRIGGER_KEY_F9                                298
#define TRIGGER_KEY_F10                               299
#define TRIGGER_KEY_F11                               300
#define TRIGGER_KEY_F12                               301
#define TRIGGER_KEY_F13                               302
#define TRIGGER_KEY_F14                               303
#define TRIGGER_KEY_F15                               304
#define TRIGGER_KEY_F16                               305
#define TRIGGER_KEY_F17                               306
#define TRIGGER_KEY_F18                               307
#define TRIGGER_KEY_F19                               308
#define TRIGGER_KEY_F20                               309
#define TRIGGER_KEY_F21                               310
#define TRIGGER_KEY_F22                               311
#define TRIGGER_KEY_F23                               312
#define TRIGGER_KEY_F24                               313
#define TRIGGER_KEY_F25                               314
#define TRIGGER_KEY_KP_0                              320
#define TRIGGER_KEY_KP_1                              321
#define TRIGGER_KEY_KP_2                              322
#define TRIGGER_KEY_KP_3                              323
#define TRIGGER_KEY_KP_4                              324
#define TRIGGER_KEY_KP_5                              325
#define TRIGGER_KEY_KP_6                              326
#define TRIGGER_KEY_KP_7                              327
#define TRIGGER_KEY_KP_8                              328
#define TRIGGER_KEY_KP_9                              329
#define TRIGGER_KEY_KP_DECIMAL                        330
#define TRIGGER_KEY_KP_DIVIDE                         331
#define TRIGGER_KEY_KP_MULTIPLY                       332
#define TRIGGER_KEY_KP_SUBTRACT                       333
#define TRIGGER_KEY_KP_ADD                            334
#define TRIGGER_KEY_KP_ENTER                          335
#define TRIGGER_KEY_KP_EQUAL                          336
#define TRIGGER_KEY_LEFT_SHIFT                        340
#define TRIGGER_KEY_LEFT_CONTROL                      341
#define TRIGGER_KEY_LEFT_ALT                          342
#define TRIGGER_KEY_LEFT_SUPER                        343
#define TRIGGER_KEY_RIGHT_SHIFT                       344
#define TRIGGER_KEY_RIGHT_CONTROL                     345
#define TRIGGER_KEY_RIGHT_ALT                         346
#define TRIGGER_KEY_RIGHT_SUPER                       347
#define TRIGGER_KEY_MENU                              348

#define TRIGGER_MOUSE_BUTTON_1                        0
#define TRIGGER_MOUSE_BUTTON_2                        1
#define TRIGGER_MOUSE_BUTTON_3                        2
#define TRIGGER_MOUSE_BUTTON_4                        3
#define TRIGGER_MOUSE_BUTTON_5                        4
#define TRIGGER_MOUSE_BUTTON_6                        5
#define TRIGGER_MOUSE_BUTTON_7                        6
#define TRIGGER_MOUSE_BUTTON_8                        7
#define TRIGGER_MOUSE_BUTTON_LAST                     TRIGGER_MOUSE_BUTTON_8
#define TRIGGER_MOUSE_BUTTON_LEFT                     TRIGGER_MOUSE_BUTTON_1
#define TRIGGER_MOUSE_BUTTON_RIGHT                    TRIGGER_MOUSE_BUTTON_2
#define TRIGGER_MOUSE_BUTTON_MIDDLE                   TRIGGER_MOUSE_BUTTON_3

#define TRIGGER_VERTEX_SHADER                         0
#define TRIGGER_FRAGMENT_SHADER                       1
#define WHITE                                         (Color){255, 255, 255, 255}
#define DARK_GRAY                                     (Color){30, 30, 30, 255}
#define PINK                                          (Color){200, 40, 180, 255}
#define HAKEN                                         (Color){0, 204, 153, 255}

typedef struct Color {
  uint8_t       r;
  uint8_t       g;
  uint8_t       b;
  uint8_t       a;
} Color;

typedef struct Image {
  uint8_t*      data;
  int32_t       width;
  int32_t       height;
  int32_t       nr_channel;
} Image;

typedef struct Texture {
  uint32_t      id;
  int32_t       width;
  int32_t       height;
  int32_t       nr_channel;
} Texture;

typedef struct Vector2 {
  float         x;
  float         y;
} Vector2;

typedef struct Vector3 {
  float         x;
  float         y;
  float         z;
} Vector3;

typedef struct Vector4 {
  float         x;
  float         y;
  float         z;
  float         w;
} Vector4;

typedef struct Rect {
  float         x;
  float         y;
  float         width;
  float         height;
} Rect;

typedef struct Mat4 {
  float m0, m1, m2, m3;
  float m4, m5, m6, m7;
  float m8, m9, m10, m11;
  float m12, m13, m14, m15;
} Mat4;

typedef unsigned int VertexArrayObject;
typedef unsigned int VertexBuffer;
typedef unsigned int IndexBuffer;

#define T_VECTOR2
#define T_VECTOR3
#define T_VECTOR4
#define T_MAT4
#define T_RECTANGLE

//Window options related functions
//============================================================
TRAPI void init_window(const char* title, const uint16_t width, const uint16_t height);
TRAPI bool window_should_close(void);
TRAPI void window_shutdown(void);
TRAPI void close_window(void);
TRAPI void set_window_icon(const char* filepath);
TRAPI Vector2 get_window_size(void);
TRAPI void set_window_size(Vector2 window_size);
TRAPI void toggle_full_screen(void);
TRAPI void set_v_sync(bool enabled);
TRAPI void set_render_matrix_projection(Mat4 mat);
TRAPI Mat4 get_render_mat_projection(void);

//Input related functions
//============================================================
TRAPI int32_t get_key_down(void);
TRAPI bool is_key_pressed(const int32_t key_code);
TRAPI bool is_key_down(const int32_t key_code);
TRAPI bool is_mouse_button_pressed(const int32_t button);
TRAPI Vector2 get_mouse_position(void);
TRAPI char* get_clipboard_string(void);

//Drawing related functions
//============================================================
TRAPI void draw_indexed(const VertexArrayObject vertex_array_object, const uint32_t index_count);
TRAPI void draw_lines(const VertexArrayObject vertex_array_object, const uint32_t vertex_count);
TRAPI void set_line_thickness(const float thickness);
TRAPI void set_background(const Color color);

//Texture related functions
//============================================================
TRAPI Image read_image_file(const char* filepath);
TRAPI Texture load_texture(const char* filepath);
TRAPI unsigned int create_texture(const uint8_t* data, const int width, const int32_t height, int32_t nr_channel);

//Shader related functions
//============================================================
TRAPI unsigned int load_shader(const char* v_shader_path, const char* f_shader_path);
TRAPI unsigned int compile_shader(const char* v_shader_code, const char* f_shader_code);
TRAPI void bind_shader(const uint32_t shader);
TRAPI int get_shader_location(uint32_t shader, const char* uniform_name);
TRAPI void set_shader_uniform_mat4(const uint32_t shader, int32_t loc, Mat4 mat);
TRAPI void set_uniform1iv(int32_t loc, int32_t samples, int32_t* samplers);
TRAPI void bind_texture_unit(uint32_t index, uint32_t slot);
TRAPI void bind_texture(uint32_t texture);

//Frame update related fuctions
//============================================================
TRAPI void gfx_update(void);
TRAPI void input_polling(void);

//Vertex related fuctions
//============================================================
TRAPI VertexBuffer create_vertex_buffer(const uint64_t size);
TRAPI void bind_vertex_buffer(const VertexBuffer vertex_buffer);
TRAPI void set_vertex_buffer_data(const VertexBuffer vertex_buffer, const void* data, uint32_t size);
TRAPI IndexBuffer create_index_buffer(const uint32_t* indices, const uint32_t count_indices);
TRAPI void bind_index_buffer(const IndexBuffer index_buffer);
TRAPI VertexArrayObject create_vao();
TRAPI void bind_vao(VertexArrayObject vertex_array_object);
//SHOULD ACCEPT TYPE AND NORMALIZED BOOL
TRAPI void set_vao_attribute(VertexArrayObject vao, int32_t idAttr, int32_t number_attr, uint64_t size, const void* offset);

//Time
TRAPI double get_run_time(void);

//File related functions
//============================================================
TRAPI char* read_text_file(const char* filepath, uint32_t* read_len);
TRAPI void free_text_data(void* data);

//Util related functions
//============================================================
TRAPI void trigger_log(const int log_level, const char* fmt, ...);
TRAPI void set_trigger_log_level(const int log_lLevel);
#endif
