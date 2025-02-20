//==========================================================================
//  TRIGGER 0.01
//==========================================================================
#ifndef TRIGGER_H
#define TRIGGER_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _WIN32
  #define TRAPI __declspec(dllexport)
#endif

#ifndef TRAPI
  #define TRAPI
#endif

#ifndef bool
  typedef enum { false, true } bool;
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

typedef struct Color {
  unsigned char r;
  unsigned char g;
  unsigned char b;
  unsigned char a;
} Color;

typedef struct Image {
  unsigned char* data;
  int width;
  int height;
  int nr_channel;
} Image;

typedef struct Texture {
  unsigned int id;
  int width;
  int height;
  int nr_channel;
} Texture;

typedef struct Vector2 {
  float x;
  float y;
} Vector2;

typedef struct Vector3 {
  float x;
  float y;
  float z;
} Vector3;

typedef struct Vector4 {
  float x;
  float y;
  float z;
  float w;
} Vector4;

typedef struct Rectangle {
  float x;
  float y;
  float width;
  float height;
} Rectangle;

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

#define TRIGGER_VERTEX_SHADER                         0
#define TRIGGER_FRAGMENT_SHADER                       1
#define DARK_GRAY                                     (Color){30, 30, 30, 255}
#define PINK                                          (Color){200, 40, 180, 255}

//Window options related functions
//============================================================
TRAPI void init_window(const char* title, const unsigned short width, const unsigned short height);
TRAPI bool window_should_close(void);
TRAPI void window_shutdown(void);
TRAPI void close_window(void);
TRAPI Vector2 get_window_size(void);
TRAPI void set_render_matrix_projection(Mat4 mat);
TRAPI Mat4 get_render_mat_projection(void);

//Drawing related functions
//============================================================
TRAPI void draw_indexed(const VertexArrayObject vertex_array_object, const unsigned int index_count);
TRAPI void set_background(const Color color);
//Texture related functions
//============================================================
TRAPI Image read_image_file(const char* filepath);
TRAPI Texture load_texture(const char* filepath);
TRAPI unsigned int create_texture(const unsigned char* data, const int width, const int height, int nr_channel);

//Shader related functions
//============================================================
TRAPI unsigned int load_shader(const char* v_shader_path, const char* f_shader_path);
TRAPI unsigned int compile_shader(const char* v_shader_code, const char* f_shader_code);
TRAPI void bind_shader(const unsigned int shader);
TRAPI int get_shader_location(unsigned int shader, const char* uniform_name);
TRAPI void set_shader_uniform_mat4(const unsigned int shader, int loc, Mat4 mat);
TRAPI void set_uniform1iv(int loc, int samples, int* samplers);
TRAPI void bind_texture_unit(unsigned int index, unsigned int slot);

//NOT BELONG HERE
TRAPI void begin_shader(unsigned int shader);
TRAPI void end_shader(void);

//Frame update related fuctions
//============================================================
TRAPI void gfx_update(void);
TRAPI void input_polling(void);

//Vertex related fuctions
//============================================================
TRAPI VertexBuffer create_vertex_buffer(const unsigned long long size);
TRAPI void bind_vertex_buffer(const VertexBuffer vertex_buffer);
TRAPI void set_vertex_buffer_data(const VertexBuffer vertex_buffer, const void* data, unsigned int size);
TRAPI IndexBuffer create_index_buffer(const unsigned int* indices, const unsigned int count_indices);
TRAPI void bind_index_buffer(const IndexBuffer index_buffer);
TRAPI VertexArrayObject create_vao();
TRAPI void bind_vao(VertexArrayObject vertex_array_object);
//SHOULD ACCEPT TYPE AND NORMALIZED BOOL
TRAPI void set_vao_attribute(VertexArrayObject vao, int idAttr, int number_attr, unsigned long long size, const void* offset);

//Time related fuctions
//============================================================
TRAPI double get_deltatime(void);
TRAPI int get_framerate(void);
TRAPI double get_time(void);
TRAPI void set_framerate_cap(int frame_rate);

//File related functions
//============================================================
TRAPI char* read_text_file(const char* filepath, unsigned int* read_len);
TRAPI void free_text_data(void* data);

//Util related functions
//============================================================
TRAPI void trigger_log(const int log_level, const char* fmt, ...);
TRAPI void set_trigger_log_level(const int log_lLevel);
#endif
