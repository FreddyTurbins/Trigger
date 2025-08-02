#include "TRIGGER/trigger.h"

#include <stdio.h>
#include <stdlib.h>

#define MAX_KEYBOARD_KEYS_CODE            512
#define MAX_KEY_DOWN_QUEUE                16

#define MAX_GAMEPADS                      8

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
  #include "vendor/glad/glad.h"
  #include "vendor/glfw/include/GLFW/glfw3.h"
  #define STB_IMAGE_IMPLEMENTATION
  #include "vendor/stb_image.h"
#endif

typedef struct {
  GLFWwindow *m_window;
} PlatformData;

PlatformData platform = {0};

static void key_call_back(GLFWwindow *window, int key, int scancode, int action, int mods);

//Window options functions
//============================================================
void init_window(const char* title, const uint16_t width, const uint16_t height)
{
  if ((title != NULL) && (title[0] != 0)) trigger_window.title = title;
  trigger_window.render.width = width;
  trigger_window.render.height = height;
  trigger_window.v_sync = true;
  #if defined(PLATFORM_DESKTOP)
  int result = glfwInit();
  if (result == GLFW_FALSE) { 
    trigger_log(LOG_FATAL, "Failed to initialize GLFW");
    return;
  }
  platform.m_window = glfwCreateWindow(trigger_window.render.width, trigger_window.render.height, trigger_window.title, NULL, NULL);
  if(!platform.m_window) 
  {
    trigger_log(LOG_FATAL, "Fail creating window");
    glfwTerminate();
    return;
  }
  //MAKE A ABSTRACTION TO OPENGLCONTEXT
  glfwMakeContextCurrent(platform.m_window);
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
  {
    trigger_log(LOG_FATAL, "Failed initializing GLAD");
    return;
  }
  glfwSetKeyCallback(platform.m_window, key_call_back);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_LINE_SMOOTH);
  glEnable(GL_BLEND);

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
    case TEGL: return glfwWindowShouldClose(platform.m_window) || trigger_window.should_close;
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
  glfwTerminate();
  #endif
}

void set_window_icon(const char* filepath)
{
  Image image = {0};
  image.data = stbi_load(filepath, &image.width, &image.height, 0, 4);
  if (image.data == NULL) {
    trigger_log(LOG_WARN, "set_window_icon -> Null image data");
    return;
  }
  //OPENGL
  GLFWimage icon = {
    .width = image.width,
    .height = image.height,
    .pixels = (uint8_t*)image.data
  };
  glfwSetWindowIcon(platform.m_window, 1, &icon);
  trigger_log(LOG_INFO, "set_window_icon -> Icon succesfully setted");
}

Vector2 get_window_size(void)
{
  return (Vector2){(float)trigger_window.render.width, (float)trigger_window.render.height};
}

void set_window_size(Vector2 window_size)
{
  //glViewport(0, 0, width, height);
  glfwSetWindowSize(platform.m_window, window_size.x, window_size.y);
  trigger_window.render.width = window_size.x;
  trigger_window.render.height = window_size.y;
}

void toggle_full_screen(void)
{
  if (!trigger_window.full_screen) {
    GLFWmonitor *monitor = glfwGetPrimaryMonitor();
    

    if (monitor == NULL) {
      trigger_log(LOG_WARN, "toggle_full_scren -> Failed getting monitor");
      return;
    } 
    trigger_window.full_screen = true;
    glfwGetWindowPos(platform.m_window, &trigger_window.position.x, &trigger_window.position.y);
    trigger_window.previous_position.x = trigger_window.position.x;
    trigger_window.previous_position.y = trigger_window.position.y;
    const GLFWvidmode * mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

    int32_t screen_width = mode->width;
    int32_t screen_height = mode->height;
    glfwSetWindowMonitor(platform.m_window, monitor, 0, 0, screen_width, screen_height, GLFW_DONT_CARE);
  } else {
    trigger_window.full_screen = false;
    glfwSetWindowMonitor(platform.m_window, NULL, trigger_window.previous_position.x, trigger_window.previous_position.y, 
        trigger_window.render.width, trigger_window.render.height, GLFW_DONT_CARE);

    trigger_window.position.x = trigger_window.previous_position.x;
    trigger_window.position.y = trigger_window.previous_position.y;
  }
}

void set_v_sync(bool enabled)
{
  switch (current_renderer_api) {
    case NONE_API: {trigger_log(LOG_DEBUG, "set_v_sync -> Trigger has not api selected"); break;}
    case TEGL: {glfwSwapInterval(enabled); break;}
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
  int32_t status = glfwGetMouseButton(platform.m_window, button);
  return status == GLFW_PRESS;
}

Vector2 get_mouse_position(void)
{
  double x_pos, y_pos;
  glfwGetCursorPos(platform.m_window, &x_pos, &y_pos);
  return (Vector2) {
    .x = x_pos,
    .y = y_pos
  };
}

char* get_clipboard_string(void)
{
  const char* clipboard = glfwGetClipboardString(platform.m_window);
  return (char*)clipboard;
}

//Drawing functions
//============================================================
void draw_indexed(const VertexArrayObject vertex_array_object, const uint32_t index_count)
{
  bind_vao(vertex_array_object);
  glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_INT, NULL);
}

void draw_lines(const VertexArrayObject vertex_array_object, const uint32_t vertex_count)
{
  bind_vao(vertex_array_object);
  glDrawArrays(GL_LINES, 0, vertex_count);
}

void set_line_thickness(const float thickness)
{
  glLineWidth(thickness);
}

void set_background(const Color color)
{
  float red = (float)color.r/255;
  float green = (float)color.g/255;
  float blue = (float)color.b/255;
  float alpha = (float)color.a/255;
  switch (current_renderer_api) {
    case NONE_API: {trigger_log(LOG_DEBUG, "set_background -> Trigger has not api selected"); return;}
    case TEGL: {glClearColor(red, green, blue, alpha); return;}
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
  glUniform1iv(loc, samples, samplers);
}

void bind_texture_unit(uint32_t index, uint32_t slot)
{
  glBindTextureUnit(index, slot);
}

void bind_texture(uint32_t texture)
{
  glBindTexture(GL_TEXTURE_2D, texture);
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
  texture.id = create_texture(img.data, img.width, img.height, img.nr_channel);
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
  unsigned int id = 0;
  glBindTexture(GL_TEXTURE_2D, 0);

  glGenTextures(1, &id);
  glBindTexture(GL_TEXTURE_2D, id);

  //WITHOUT MIMMAP
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, nr_channel, GL_UNSIGNED_BYTE, data);
  glGenerateMipmap(GL_TEXTURE_2D);
 
  return id;
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

unsigned int compile_shader_text(const char* shader_text, int type)
{
  if (type != TRIGGER_VERTEX_SHADER && type != TRIGGER_FRAGMENT_SHADER) {
    trigger_log(LOG_DEBUG, "\n[%s]", shader_text);
    trigger_log(LOG_WARN, "SHADER -> Invalid type");
    return 0;
  }
  
  type = (type) ? GL_FRAGMENT_SHADER : GL_VERTEX_SHADER;

  unsigned int id = glCreateShader(type);
  glShaderSource(id, 1, &shader_text, NULL);
  glCompileShader(id);

  int result;
  glGetShaderiv(id, GL_COMPILE_STATUS, &result);
  if (result == GL_FALSE) {
    int count_result = 0;
    glGetShaderiv(id, GL_INFO_LOG_LENGTH, &count_result);
    char* message = (char*)calloc(count_result, sizeof(char));
    glGetShaderInfoLog(id, count_result, &count_result, message);
    message[count_result-1] = '\0';
    trigger_log(LOG_WARN, "SHADER -> [ID: %d] Failed compiling %s shader\n%s", id,
        (type == GL_VERTEX_SHADER) ? "vertex" : "fragment", message);
    free_text_data(message);
  } else {
    trigger_log(LOG_DEBUG, "SHADER -> [ID: %d] Succesfully compile %s shader", id,
        (type == GL_VERTEX_SHADER) ? "vertex" : "fragment");
  }
  return id;
}

uint32_t compile_shader(const char* v_shader_code, const char* f_shader_code)
{
  uint32_t v_shader = compile_shader_text(v_shader_code, TRIGGER_VERTEX_SHADER);
  uint32_t f_shader = compile_shader_text(f_shader_code, TRIGGER_FRAGMENT_SHADER);
  
  unsigned int program = glCreateProgram();

  glAttachShader(program, v_shader);
  glAttachShader(program, f_shader);
  glLinkProgram(program);
  glValidateProgram(program);

  int success = 0;
  glGetProgramiv(program, GL_LINK_STATUS, &success);
  if (success == GL_FALSE) {
    int count_result = 0;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &count_result);
    char* message = (char*)calloc(count_result, sizeof(char));
    glGetProgramInfoLog(program, count_result, &count_result, message);
    message[count_result-1] = '\0';
    trigger_log(LOG_WARN, "SHADER -> [ID: %d] Program failed linking\n%s", program, message);
    free_text_data(message);
  } else {
    trigger_log(LOG_DEBUG, "SHADER -> [ID: %d] Program link succesfuly", program);
  }
  return program;
}

void bind_shader(const uint32_t shader)
{
  glUseProgram(shader);
}

int get_shader_location(uint32_t shader, const char* uniform_name)
{
  int loc = glGetUniformLocation(shader, uniform_name);
  if (loc < 0) {
    trigger_log(LOG_WARN, "SHADER-> [%s] uniform is not valid", uniform_name);
  }
  return loc;
}

void set_shader_uniform_mat4(const uint32_t shader, int loc, Mat4 mat)
{
  bind_shader(shader);
  float fMat[16] = {
    mat.m0, mat.m1, mat.m2, mat.m3,
    mat.m4, mat.m5, mat.m6, mat.m7,
    mat.m8, mat.m9, mat.m10, mat.m11,
    mat.m12, mat.m13, mat.m14, mat.m15
  };
  glUniformMatrix4fv(loc, 1, GL_FALSE, fMat);
}

VertexBuffer create_vertex_buffer(const uint64_t size)
{
  VertexBuffer vertex_buffer = 0;
  glGenBuffers(1, &vertex_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
  glBufferData(GL_ARRAY_BUFFER, size, NULL, GL_DYNAMIC_DRAW);
  return vertex_buffer;
}

void bind_vertex_buffer(const VertexBuffer vertex_buffer)
{
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
}

void set_vertex_buffer_data(const VertexBuffer vertex_buffer, const void* data, uint32_t size)
{
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
  glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
}

IndexBuffer create_index_buffer(const uint32_t* indices, const uint32_t count_indices)
{
  unsigned int index_buffer = 0;
  glGenBuffers(1, &index_buffer);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, count_indices*sizeof(unsigned int), indices, GL_STATIC_DRAW);
  return index_buffer;
}

void bind_index_buffer(const IndexBuffer index_buffer)
{
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
}

VertexArrayObject create_vao()
{
  VertexArrayObject vao = 0;
  glCreateVertexArrays(1, &vao);
  glBindVertexArray(vao);
  return vao;
}

void bind_vao(VertexArrayObject vertex_array_object)
{
  glBindVertexArray(vertex_array_object);
}

void set_vao_attribute(VertexArrayObject vao, int32_t id_attr, int32_t number_attr, uint64_t size, const void* offset)
{
  glEnableVertexArrayAttrib(vao, id_attr);
  glVertexAttribPointer(id_attr, number_attr, GL_FLOAT, GL_FALSE, size, offset);
}

void gfx_update(void) {
  #if defined(PLATFORM_DESKTOP)
    glfwSwapBuffers(platform.m_window);
    glfwPollEvents();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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
  double time = glfwGetTime();
  return time;
}

//Static functions
//============================================================
static void key_call_back(GLFWwindow *window, int key, int scancode, int action, int mods)
{
  if (action == GLFW_RELEASE) keyboard.current_key_state[key] = 0;
  else if(action == GLFW_PRESS) keyboard.current_key_state[key] = 1;
  
  if ((keyboard.key_down_queue_count < MAX_KEY_DOWN_QUEUE) && (action == GLFW_PRESS || action == GLFW_REPEAT))
  {
    keyboard.key_down_queue[keyboard.key_down_queue_count++] = key;
  }

  trigger_log(LOG_TRACE, "[%c] -> %d action: %d; mods: %d", key, scancode, action, mods);
  glfwWindowShouldClose(window);
}

bool opengl_is_key_pressed(const int32_t key_code)
{
  int32_t status = glfwGetKey(platform.m_window, key_code);
  return status == GLFW_PRESS;
}

bool opengl_is_key_down(const int32_t key_code)
{
  int32_t status = glfwGetKey(platform.m_window, key_code);
  return status == GLFW_PRESS || status == GLFW_REPEAT;
}
/*
static void record_input_event(void)
{
}
*/
