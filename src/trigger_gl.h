/*************************************************************************************************
*
*     Trigger open-gl abstraction API Version 0.1
*
*************************************************************************************************/
#ifndef TRIGGER_GL_H
#define TRIGGER_GL_H

#ifdef _WIN32
  #define TRAPI __declspec(dllexport)
#endif

#ifndef TRAPI
  #define TRAPI
#endif

#define TRIGGER_GL_VERSION                            0.2

#include "vendor/glad/glad.h"
#include <stdio.h>
#include <stdlib.h>

#ifdef __cplusplus
  extern "C" {
#endif

//TGL initialize related functions
//============================================================

//Vertex buffer related functions
//============================================================
TRAPI unsigned int tgl_create_vertex_buffer(const size_t size);
TRAPI void tgl_bind_vertex_buffer(const unsigned int vertex_buffer);
TRAPI void tgl_vertex_buffer_data(VertexBuffer vertex_buffer, const void* data, unsigned size);
TRAPI unsigned int tgl_create_index_buffer(const unsigned int* data, const unsigned int count);
TRAPI void tgl_bind_index_buffer(const unsigned int index_buffer);
TRAPI uint32_t tgl_create_vao();
TRAPI void tgl_bind_vao(const unsigned int vertex_array_object);
TRAPI void tgl_set_vao_attribute(VertexArrayObject vao, int id_attr, int number_attr, size_t size, const void* offset);

//Vertex operations related functions
//============================================================
TRAPI void tgl_set_uniform_mat4f(int loc, Mat4 mat);
TRAPI void tgl_set_uniform1iv(int loc, int samples, int* samplers);
TRAPI void tgl_bind_texture_unit(unsigned int index, unsigned int slot);

//General data related functions
//============================================================

//General render related functions
//============================================================
TRAPI void tgl_clear_screen_buffer(void);
TRAPI void tgl_set_background(const unsigned char r, const unsigned char g, const unsigned char b, const unsigned char a);
TRAPI void tgl_draw_triangles(const uint32_t index_count);

//Texture related functions
//============================================================
TRAPI unsigned int tgl_create_texture(const unsigned char* data, int width, int height, int nr_channel);

//Shader related functions
//============================================================
TRAPI unsigned int tgl_compile_shader(const char* shader_text, int type);
TRAPI unsigned int tgl_create_shader_program(const unsigned int v_shader, const unsigned int f_shader);
TRAPI int tgl_get_shader_location(unsigned int shader, const char* uniform_name);
TRAPI void tgl_bind_shader(const unsigned int shader);

#if defined(TEGL_IMPLEMENTATION)

//Static modules related functions
//============================================================

//TGL initialize methods
//============================================================

//Vertex buffer methods
//============================================================
unsigned int tgl_create_vertex_buffer(const size_t size)
{
  unsigned int vertex_buffer = 0;
  glGenBuffers(1, &vertex_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
  glBufferData(GL_ARRAY_BUFFER, size, NULL, GL_DYNAMIC_DRAW);
  return vertex_buffer;
}

void tgl_bind_vertex_buffer(const unsigned int vertex_buffer)
{
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
}

void tgl_vertex_buffer_data(VertexBuffer vertex_buffer, const void* data, unsigned size)
{
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
  glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
}

unsigned int tgl_create_index_buffer(const unsigned int* data, const unsigned int count)
{
  unsigned int index_buffer = 0;
  glGenBuffers(1, &index_buffer);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, count*sizeof(unsigned int), data, GL_STATIC_DRAW);
  return index_buffer;
}

void tgl_bind_index_buffer(const unsigned int index_buffer)
{
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
}

unsigned int tgl_create_vao()
{
  unsigned int vao;
  glCreateVertexArrays(1, &vao);
  glBindVertexArray(vao);
  
  return vao;
}

void tgl_bind_vao(const unsigned int vertex_array_object)
{
  glBindVertexArray(vertex_array_object);
}

void tgl_set_vao_attribute(VertexArrayObject vao, int id_attr, int number_attr, size_t size, const void* offset)
{
  glEnableVertexArrayAttrib(vao, id_attr);
  glVertexAttribPointer(id_attr, number_attr, GL_FLOAT, GL_FALSE, size, offset);
}

//Vertex operations
//============================================================

void tgl_set_uniform_mat4f(int loc, Mat4 mat)
{
  float fMat[16] = {
    mat.m0, mat.m1, mat.m2, mat.m3,
    mat.m4, mat.m5, mat.m6, mat.m7,
    mat.m8, mat.m9, mat.m10, mat.m11,
    mat.m12, mat.m13, mat.m14, mat.m15
  };
  glUniformMatrix4fv(loc, 1, GL_FALSE, fMat);
}

void tgl_set_uniform1iv(int loc, int samples, int* samplers) {
  glUniform1iv(loc, samples, samplers);
}

void tgl_bind_texture_unit(unsigned int index, unsigned int slot)
{
  glBindTextureUnit(index, slot);
}

//General data methods
//============================================================

//General render
//============================================================
void tgl_clear_screen_buffer(void)
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void tgl_set_background(const unsigned char r, const unsigned char g, const unsigned char b, const unsigned char a)
{
  float red = (float)r/255;
  float green = (float)g/255;
  float blue = (float)b/255;
  float alpha = (float)a/255;
  glClearColor(red, green, blue, alpha);
}

void tgl_draw_triangles(const uint32_t index_count)
{
  glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_INT, NULL);
}

//Texture
//============================================================
unsigned int tgl_create_texture(const unsigned char* data, int width, int height, int nr_channel)
{
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

//Shader
//============================================================
unsigned int tgl_compile_shader(const char* shader_text, int type)
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

unsigned int tgl_create_shader_program(const unsigned int v_shader, const unsigned int f_shader)
{
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

void tgl_bind_shader(unsigned int shader)
{
  glUseProgram(shader);
}

int tgl_get_shader_location(unsigned int shader, const char* uniform_name)
{
  int loc = glGetUniformLocation(shader, uniform_name);
  if (loc < 0) {
    trigger_log(LOG_WARN, "SHADER-> [%s] uniform is not valid", uniform_name);
  }
  return loc;
}

//Static modules
//============================================================
#endif
#endif
