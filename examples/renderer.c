#include <stddef.h>
#include <stdlib.h>
#include "TRIGGER/trigger.h"
#include "renderer.h"

#define RENDERER_MAX_QUADS                                20000
#define RENDERER_MAX_VERTICES                             RENDERER_MAX_QUADS*4
#define RENDERER_MAX_INDICES                              RENDERER_MAX_QUADS*6
#define RENDERER_MAX_TEXTURES                             32

#define RED(X)                                            ((X)>>(8*3)&0xFF)
#define GREEN(X)                                          ((X)>>(8*2)&0xFF)
#define BLUE(X)                                           ((X)>>(8*1)&0xFF)
#define ALPHA(X)                                          ((X)>>(8*0)&0xFF)

typedef struct QuadVertex {
  Vector3 position;
  Vector4 color;
  Vector2 tex_coord;
  float tex_index;
} QuadVertex;

typedef struct DiskVertex {
  Vector3 world_position;
  Vector2 local_position;
  Vector4 color;
  float thickness;
  float fade;
} DiskVertex;

typedef struct TextVertex {
  Vector3 position;
  Vector4 color;
  Vector2 tex_coord;
} TextVertex;

typedef struct LineVertex {
  Vector3 position;
  Vector4 color;
} LineVertex;

typedef struct RenderBatchData {
  uint32_t                quad_shader;
  uint32_t                current_shader;

  uint32_t*               texture_slots;
  uint32_t                texture_slots_index;
  uint32_t                default_texture;
  IndexBuffer             index_buffer;

  VertexArrayObject       quad_vertex_array;
  VertexBuffer            quad_vertex_buffer;
  uint32_t                quad_index_count;
  QuadVertex*             quad_buffer_ptr;
  QuadVertex*             quad_buffer;

  VertexArrayObject       disk_vertex_array;
  VertexBuffer            disk_vertex_buffer;
  uint32_t                disk_shader;
  uint32_t                disk_index_count;
  DiskVertex*             disk_buffer_ptr;
  DiskVertex*             disk_buffer;
  
  VertexArrayObject       text_vertex_array;
  VertexBuffer            text_vertex_buffer;
  uint32_t                text_shader;
  uint32_t                text_index_count;
  TextVertex*             text_buffer_ptr;
  TextVertex*             text_buffer;
  uint32_t                text_texture;

  
  VertexArrayObject       line_vertex_array;
  VertexBuffer            line_vertex_buffer;
  uint32_t                line_shader;
  uint32_t                line_vertex_count;
  LineVertex*             line_buffer_ptr;
  LineVertex*             line_buffer;
  float                   line_thickness;
} RenderBatchData;

typedef struct DrawStats {
  uint32_t                quad_count;
  uint32_t                draw_calls;
} DrawStats;

typedef struct Time {
  double                  last_time;
  double                  draw_time;
  double                  delta_time;
  double                  update_time;
  double                  initial_time;
  double                  target_framerate_cap;
  uint64_t                frame_counter;
} Time;

static RenderBatchData batch = {0};
static DrawStats stats = {0};
static Time time = {0};

static void set_samplers_textures(void);
static void draw_character(const FontAtlas font_atlas, const char character, const Rectangle data, const Color color);

#define COLOR_NUMBER(X)                    ((X).r<<(8*3))+((X).g<<(8*2))+((X).b<<(8*1))+(X).a

//SpriteSheet related functions
//============================================================
SubTexture create_sub_texture(const Texture texture, const Vector2 coords, const Vector2 cell_size, const Vector2 sprite_size)
{
  if (texture.id == 0) {
    trigger_log(LOG_WARN, "SUBTEXTURE -> Invalid texture");
  }
  SubTexture sub_texture = {
    .id = texture.id,
    .min = {
      (coords.x * cell_size.x) / texture.width,
      (coords.y * cell_size.y) / texture.height
    },
    .max = {
      ((coords.x + sprite_size.x) * cell_size.x) / texture.width,
      ((coords.y + sprite_size.y) * cell_size.y) / texture.height
    },
    .width = cell_size.x,
    .height = cell_size.y
  };

  return sub_texture;
}

FontAtlas create_font_atlas(const char* filepath, Vector2 cells)
{
  Texture font_texture = load_texture(filepath);
  SubTexture cell_font_sub_texture = create_sub_texture(font_texture, (Vector2){0, 0}, 
      (Vector2){font_texture.width/cells.x, font_texture.height/cells.y}, (Vector2){1.0f, 1.0f});
  FontAtlas font_atlas = {
    .cell = cell_font_sub_texture,
    .x_cells = cells.x,
    .y_cells = cells.y
  };

  return font_atlas;
}

//2D Renderer related functions
//============================================================
void init_renderer2d(void)
{
  //Redfine max vertices to all buffers
  batch.texture_slots = (uint32_t*)calloc(RENDERER_MAX_TEXTURES, sizeof(uint32_t));
  batch.quad_buffer = (QuadVertex*)calloc(RENDERER_MAX_VERTICES, sizeof(struct QuadVertex));
  batch.disk_buffer = (DiskVertex*)calloc(RENDERER_MAX_VERTICES, sizeof(struct DiskVertex));
  batch.text_buffer = (TextVertex*)calloc(RENDERER_MAX_VERTICES, sizeof(struct TextVertex));
  batch.line_buffer = (LineVertex*)calloc(RENDERER_MAX_VERTICES, sizeof(struct LineVertex));
  batch.quad_buffer_ptr = batch.quad_buffer;
  batch.disk_buffer_ptr = batch.disk_buffer;
  batch.text_buffer_ptr = batch.text_buffer;
  batch.line_buffer_ptr = batch.line_buffer;

  //Quad
  batch.quad_vertex_array = create_vao();
  batch.quad_vertex_buffer = create_vertex_buffer(RENDERER_MAX_VERTICES*sizeof(QuadVertex));
  set_vao_attribute(batch.quad_vertex_array, 0, 3, sizeof(QuadVertex), (const void*)offsetof(QuadVertex, position));
  set_vao_attribute(batch.quad_vertex_array, 1, 4, sizeof(QuadVertex), (const void*)offsetof(QuadVertex, color));
  set_vao_attribute(batch.quad_vertex_array, 2, 2, sizeof(QuadVertex), (const void*)offsetof(QuadVertex, tex_coord));
  set_vao_attribute(batch.quad_vertex_array, 3, 1, sizeof(QuadVertex), (const void*)offsetof(QuadVertex, tex_index));

  const char* quad_vertex_shader_code = 
    "#version 450 core                                        \n"
    "                                                         \n"
    "layout (location = 0) in vec3 vertexPosition;            \n"
    "layout (location = 1) in vec4 vertex_color;              \n"
    "layout (location = 2) in vec2 texture_coord;             \n"
    "layout (location = 3) in float texture_index;            \n"
    "                                                         \n"
    "uniform mat4 projMatrix;                                 \n"
    "                                                         \n"
    "out vec4 f_color;                                        \n"
    "out vec2 f_tex_coord;                                    \n"
    "out float f_tex_index;                                   \n"
    "void main()                                              \n"
    "{                                                        \n"
    " f_color = vertex_color;                                 \n"
    " f_tex_coord = texture_coord;                            \n"
    " f_tex_index = texture_index;                            \n"
    " gl_Position = projMatrix*vec4(vertexPosition, 1.0);     \n"
    "}                                                        \n";

  const char* quad_fragment_shader_code =
    "#version 450 core                                        \n"
    "                                                         \n"
    "layout (location = 0) out vec4 color;                    \n"
    "                                                         \n"
    "in vec4 f_color;                                         \n"
    "in vec2 f_tex_coord;                                     \n"
    "in float f_tex_index;                                    \n"
    "uniform sampler2D textures[32];                          \n"
    "void main()                                              \n"
    "{                                                        \n"
    " int index = int(f_tex_index);                           \n"
    " color = f_color*texture(textures[index], f_tex_coord);  \n"
    "}                                                        \n";
  batch.quad_shader     = compile_shader(quad_vertex_shader_code, quad_fragment_shader_code);
  batch.current_shader = batch.quad_shader;
  bind_shader(batch.current_shader);
  if (batch.quad_shader > 0) {
    trigger_log(LOG_INFO, "SHADER -> [ID %d] Default quad shader loaded", batch.quad_shader);
  } else {
    trigger_log(LOG_WARN, "SHADER -> [ID %d] Default quad shader failed loading", batch.quad_shader);
  }
  
  uint32_t indices[RENDERER_MAX_INDICES] = {0};
  for (long k = 0, offset = 0; k < RENDERER_MAX_INDICES; k+=6, offset+=4) {
    indices[k + 0] = 0 + offset;
    indices[k + 1] = 1 + offset;
    indices[k + 2] = 2 + offset;

    indices[k + 3] = 2 + offset;
    indices[k + 4] = 3 + offset;
    indices[k + 5] = 0 + offset;
  }
  
  batch.index_buffer = create_index_buffer(indices, RENDERER_MAX_INDICES);

  //Disk
  batch.disk_vertex_array = create_vao();
  batch.disk_vertex_buffer = create_vertex_buffer(RENDERER_MAX_VERTICES*sizeof(DiskVertex));
  set_vao_attribute(batch.disk_vertex_array, 0, 3, sizeof(DiskVertex), (const void*)offsetof(DiskVertex, world_position));
  set_vao_attribute(batch.disk_vertex_array, 1, 2, sizeof(DiskVertex), (const void*)offsetof(DiskVertex, local_position));
  set_vao_attribute(batch.disk_vertex_array, 2, 4, sizeof(DiskVertex), (const void*)offsetof(DiskVertex, color));
  set_vao_attribute(batch.disk_vertex_array, 3, 1, sizeof(DiskVertex), (const void*)offsetof(DiskVertex, thickness));
  set_vao_attribute(batch.disk_vertex_array, 4, 1, sizeof(DiskVertex), (const void*)offsetof(DiskVertex, fade));
  bind_index_buffer(batch.index_buffer);

  const char* disk_vertex_shader_code = 
    "#version 450 core                                                            \n"
    "                                                                             \n"
    "layout (location = 0) in vec3 world_position;                                \n"
    "layout (location = 1) in vec2 local_position;                                \n"
    "layout (location = 2) in vec4 vertex_color;                                  \n"
    "layout (location = 3) in float thickness;                                    \n"
    "layout (location = 4) in float fade;                                         \n"
    "                                                                             \n"
    "uniform mat4 projMatrix;                                                     \n"
    "                                                                             \n"
    "out vec2 f_local_position;                                                   \n"
    "out vec4 f_color;                                                            \n"
    "out float f_thickness;                                                       \n"
    "out float f_fade;                                                            \n"
    "void main()                                                                  \n"
    "{                                                                            \n"
    " f_local_position = local_position;                                          \n"
    " f_color = vertex_color;                                                     \n"
    " f_thickness = thickness;                                                    \n"
    " f_fade = fade;                                                              \n"
    " gl_Position = projMatrix*vec4(world_position, 1.0);                         \n"
    "}                                                                            \n";

  const char* disk_fragment_shader_code =
    "#version 450 core                                                            \n"
    "                                                                             \n"
    "layout (location = 0) out vec4 color;                                        \n"
    "                                                                             \n"
    "in vec2 f_local_position;                                                    \n"
    "in vec4 f_color;                                                             \n"
    "in float f_thickness;                                                        \n"
    "in float f_fade;                                                             \n"
    "void main()                                                                  \n"
    "{                                                                            \n"
    " float distance = 1.0 - length(f_local_position);                            \n"
    " float color_alpha = smoothstep(0.0, f_fade, distance);                      \n"
    " color_alpha *= smoothstep(f_thickness + f_fade, f_thickness, distance);     \n"
    " color = f_color;                                                            \n"
    " color.a *= color_alpha;                                                     \n"
    "}                                                                            \n";

  batch.disk_shader = compile_shader(disk_vertex_shader_code, disk_fragment_shader_code);
  if (batch.disk_shader > 0) {
    trigger_log(LOG_INFO, "SHADER -> [ID %d] Default disk shader loaded", batch.disk_shader);
  } else {
    trigger_log(LOG_WARN, "SHADER -> [ID %d] Default disk shader failed loading", batch.disk_shader);
  }
  
  //Text
  batch.text_vertex_array = create_vao();
  batch.text_vertex_buffer = create_vertex_buffer(RENDERER_MAX_VERTICES*sizeof(TextVertex));
  set_vao_attribute(batch.text_vertex_array, 0, 3, sizeof(TextVertex), (const void*)offsetof(TextVertex, position));
  set_vao_attribute(batch.text_vertex_array, 1, 4, sizeof(TextVertex), (const void*)offsetof(TextVertex, color));
  set_vao_attribute(batch.text_vertex_array, 2, 2, sizeof(TextVertex), (const void*)offsetof(TextVertex, tex_coord));
  bind_index_buffer(batch.index_buffer);
  
  const char* text_vertex_shader_code = 
    "#version 450 core                                        \n"
    "                                                         \n"
    "layout (location = 0) in vec3 vertexPosition;            \n"
    "layout (location = 1) in vec4 vertex_color;              \n"
    "layout (location = 2) in vec2 texture_coord;             \n"
    "                                                         \n"
    "uniform mat4 projMatrix;                                 \n"
    "                                                         \n"
    "out vec4 f_color;                                        \n"
    "out vec2 f_tex_coord;                                    \n"
    "void main()                                              \n"
    "{                                                        \n"
    " f_color = vertex_color;                                 \n"
    " f_tex_coord = texture_coord;                            \n"
    " gl_Position = projMatrix*vec4(vertexPosition, 1.0);     \n"
    "}                                                        \n";

  const char* text_fragment_shader_code =
    "#version 450 core                                        \n"
    "                                                         \n"
    "layout (location = 0) out vec4 color;                    \n"
    "                                                         \n"
    "in vec4 f_color;                                         \n"
    "in vec2 f_tex_coord;                                     \n"
    "uniform sampler2D font_atlas;                            \n"
    "void main()                                              \n"
    "{                                                        \n"
    " color = f_color*texture(font_atlas, f_tex_coord);       \n"
    " if(color.rgb == vec3(0.0, 0.0, 0.0)) {                  \n"
    "   color.a *= 0.0;                                       \n"
    " }                                                       \n"
    "}                                                        \n";
  
  batch.text_shader = compile_shader(text_vertex_shader_code, text_fragment_shader_code);
  if (batch.text_shader > 0) {
    trigger_log(LOG_INFO, "SHADER -> [ID %d] Default text shader loaded", batch.text_shader);
  } else {
    trigger_log(LOG_WARN, "SHADER -> [ID %d] Default text shader failed loading", batch.text_shader);
  }
  //Line
  batch.line_vertex_array = create_vao();
  batch.line_vertex_buffer = create_vertex_buffer(RENDERER_MAX_VERTICES*sizeof(LineVertex));
  set_vao_attribute(batch.line_vertex_array, 0, 3, sizeof(LineVertex), (const void*)offsetof(LineVertex, position));
  set_vao_attribute(batch.line_vertex_array, 1, 4, sizeof(LineVertex), (const void*)offsetof(LineVertex, color));

  const char* line_vertex_shader_code = 
    "#version 450 core                                        \n"
    "                                                         \n"
    "layout (location = 0) in vec3 vertexPosition;            \n"
    "layout (location = 1) in vec4 vertex_color;              \n"
    "                                                         \n"
    "uniform mat4 projMatrix;                                 \n"
    "                                                         \n"
    "out vec4 f_color;                                        \n"
    "void main()                                              \n"
    "{                                                        \n"
    " f_color = vertex_color;                                 \n"
    " gl_Position = projMatrix*vec4(vertexPosition, 1.0);     \n"
    "}                                                        \n";

  const char* line_fragment_shader_code =
    "#version 450 core                                        \n"
    "                                                         \n"
    "layout (location = 0) out vec4 color;                    \n"
    "                                                         \n"
    "in vec4 f_color;                                         \n"
    "void main()                                              \n"
    "{                                                        \n"
    " color = f_color;                                        \n"
    "}                                                        \n";
  
  batch.line_shader = compile_shader(line_vertex_shader_code, line_fragment_shader_code);
  if (batch.line_shader > 0) {
    trigger_log(LOG_INFO, "SHADER -> [ID %d] Default line shader loaded", batch.line_shader);
  } else {
    trigger_log(LOG_WARN, "SHADER -> [ID %d] Default line shader failed loading", batch.line_shader);
  }

  uint8_t pixels[4] = {255, 255, 255, 255};
  //4 should be a define, nr_channel 4 is not obvius that is RGBA
  batch.default_texture = create_texture(pixels, 1, 1, 4);

  batch.texture_slots[0] = batch.default_texture;
  for (int32_t i = 1; i < RENDERER_MAX_TEXTURES; i++) {
    batch.texture_slots[i] = 0;
  }
  batch.texture_slots_index = 1;
  time.initial_time = get_run_time();
  time.last_time = time.initial_time;
  time.frame_counter = 0;
  set_v_sync(false);
}

void close_renderer2d(void)
{
  free(batch.texture_slots);
  free(batch.quad_buffer);
  free(batch.disk_buffer);
  free(batch.text_buffer);
  free(batch.line_buffer);
}

void start_batch(void)
{
  stats.quad_count = 0;
  double current_time = get_run_time();
  time.update_time = current_time - time.last_time;
  time.last_time = current_time;
}

void end_batch(void)
{
  input_polling();
  flush_renderer2d();
  gfx_update();
  double current_time = get_run_time();
  time.draw_time = current_time - time.last_time;
  time.last_time = current_time;
  time.delta_time = time.update_time + time.draw_time;
  /*if (time.draw_time < time.target_framerate_cap)
  {
    //Do wait_time
    //wait_time(time.target_framerate_cap - draw_time);

    current_time = get_system_time();
    double wait_time = current_time - time.last_time;
    time.last_time = current_time;

    time.delta_time += wait_time;
  }*/
  time.frame_counter++;
}

void flush_quad_renderer2d(void)
{
  if (batch.quad_index_count) {
    size_t size = (uint8_t*)batch.quad_buffer_ptr - (uint8_t*)batch.quad_buffer;
    set_vertex_buffer_data(batch.quad_vertex_buffer, batch.quad_buffer, size);

    for (uint32_t i = 0; i < batch.texture_slots_index; i++) {
      bind_texture_unit(i, batch.texture_slots[i]);
    }

    bind_shader(batch.current_shader);
    set_samplers_textures();
    int32_t loc = get_shader_location(batch.current_shader, "projMatrix");
    set_shader_uniform_mat4(batch.current_shader, loc, get_render_mat_projection());

    draw_indexed(batch.quad_vertex_array, batch.quad_index_count);

    batch.quad_buffer_ptr = batch.quad_buffer;
    batch.texture_slots_index = 1;
    batch.quad_index_count = 0;
    stats.draw_calls++;
  }
}

void flush_disk_renderer2d(void)
{
  if (batch.disk_index_count) {
    size_t size = (uint8_t*)batch.disk_buffer_ptr - (uint8_t*)batch.disk_buffer;
    set_vertex_buffer_data(batch.disk_vertex_buffer, batch.disk_buffer, size);

    bind_shader(batch.disk_shader);
    int32_t loc = get_shader_location(batch.disk_shader, "projMatrix");
    set_shader_uniform_mat4(batch.disk_shader, loc, get_render_mat_projection());

    draw_indexed(batch.disk_vertex_array, batch.disk_index_count);

    batch.disk_buffer_ptr = batch.disk_buffer;
    batch.disk_index_count = 0;
    stats.draw_calls++;
  }
}

void flush_text_renderer2d(void)
{
  if (batch.text_index_count) {
    size_t size = (uint8_t*)batch.text_buffer_ptr - (uint8_t*)batch.text_buffer;
    set_vertex_buffer_data(batch.text_vertex_buffer, batch.text_buffer, size);
      
    bind_texture(batch.text_texture);
    bind_shader(batch.text_shader);
    int32_t loc = get_shader_location(batch.text_shader, "projMatrix");
    set_shader_uniform_mat4(batch.text_shader, loc, get_render_mat_projection());

    draw_indexed(batch.text_vertex_array, batch.text_index_count);

    batch.text_buffer_ptr = batch.text_buffer;
    batch.text_index_count = 0;
    batch.text_texture = 0;
    stats.draw_calls++;
  }
}

void flush_line_renderer2d(void)
{
  if (batch.line_vertex_count) {
    size_t size = (uint8_t*)batch.line_buffer_ptr - (uint8_t*)batch.line_buffer;
    set_vertex_buffer_data(batch.line_vertex_buffer, batch.line_buffer, size);
      
    bind_shader(batch.line_shader);
    int32_t loc = get_shader_location(batch.line_shader, "projMatrix");
    set_shader_uniform_mat4(batch.line_shader, loc, get_render_mat_projection());

    draw_lines(batch.line_vertex_array, batch.line_vertex_count);

    batch.line_buffer_ptr = batch.line_buffer;
    batch.line_vertex_count = 0;
    stats.draw_calls++;
  }
}

void flush_renderer2d(void)
{
  flush_quad_renderer2d();
  flush_disk_renderer2d();
  flush_text_renderer2d();
  flush_line_renderer2d();
}

//2D Renderer drawing related functions
//============================================================
void draw_line(const Vector2 v1, const Vector2 v2, const Color color)
{
  //if (batch.line_vertex_count >= RENDERER_MAX_INDICES) flush_quad_renderer2d();
  uint32_t parsed_color = COLOR_NUMBER(color);
  const Vector4 color4f = {
    (float)RED(parsed_color)/255, (float)GREEN(parsed_color)/255, (float)BLUE(parsed_color)/255, (float)ALPHA(parsed_color)/255
  };
  const Vector3 vertex_position[2] = {
    {v1.x, v1.y, 0.0f},
    {v2.x, v2.y, 0.0f},
  };
  
  for (int k = 0; k < 2; k++) {
    batch.line_buffer_ptr->position  = vertex_position[k];
    batch.line_buffer_ptr->color     = color4f;
    batch.line_buffer_ptr++;
  }
  
  batch.line_vertex_count += 2;
}

void draw_line_thickness(const Vector2 v1, const Vector2 v2, const float thickness, const Color color)
{
  if (thickness <= 0.0f) return;
  if (batch.line_thickness != thickness) {flush_line_renderer2d();}
  set_line_thickness(thickness);
  batch.line_thickness = thickness;
  draw_line(v1, v2, color);
}

void draw_triangle(const Vector2 v1, const Vector2 v2, const Vector2 v3, const Color color)
{
  if (batch.quad_index_count >= RENDERER_MAX_INDICES) flush_quad_renderer2d();
  uint32_t parsed_color = COLOR_NUMBER(color);
  const Vector4 color4f = {
    (float)RED(parsed_color)/255, (float)GREEN(parsed_color)/255, (float)BLUE(parsed_color)/255, (float)ALPHA(parsed_color)/255
  };
  const Vector3 vertex_position[4] = {
    {v1.x, v1.y, 0.0f},
    {v2.x, v2.y, 0.0f},
    {v2.x, v2.y, 0.0f},
    {v3.x, v3.y, 0.0f}
  };
  const Vector2 texture_coord[4] = {
    {0.0f, 0.0f},
    {1.0f, 0.0f},
    {1.0f, 1.0f},
    {0.0f, 1.0f}
  };
  const float texture_index = 0.0f;                       //DEFAULT
  
  for (int k = 0; k < 4; k++) {
    batch.quad_buffer_ptr->position  = vertex_position[k];
    batch.quad_buffer_ptr->color     = color4f;
    batch.quad_buffer_ptr->tex_coord = texture_coord[k];
    batch.quad_buffer_ptr->tex_index = texture_index;
    batch.quad_buffer_ptr++;
  }
  
  batch.quad_index_count += 6;
  stats.quad_count++;
}

void draw_quad(const Rectangle data, const Color color)
{
  if (batch.quad_index_count >= RENDERER_MAX_INDICES) flush_quad_renderer2d();
  
  uint32_t parsed_color = COLOR_NUMBER(color);
  const Vector4 color4f = {
    (float)RED(parsed_color)/255, (float)GREEN(parsed_color)/255, (float)BLUE(parsed_color)/255, (float)ALPHA(parsed_color)/255
  };
  const Vector3 vertex_position[4] = {
    {data.x             , data.y              , 0.0f},  //TOP-LEFT
    {data.x             , data.y + data.height, 0.0f},  //BOT-LEFT
    {data.x + data.width, data.y + data.height, 0.0f},  //BOT-RIGHT
    {data.x + data.width, data.y              , 0.0f}   //TOP-RIGHT
  };
  const Vector2 texture_coord[4] = {
    {0.0f, 0.0f},
    {1.0f, 0.0f},
    {1.0f, 1.0f},
    {0.0f, 1.0f}
  };
  const float texture_index = 0.0f;                      //DEFAULT
  
  for (int k = 0; k < 4; k++) {
    batch.quad_buffer_ptr->position  = vertex_position[k];
    batch.quad_buffer_ptr->color     = color4f;
    batch.quad_buffer_ptr->tex_coord = texture_coord[k];
    batch.quad_buffer_ptr->tex_index = texture_index;
    batch.quad_buffer_ptr++;
  }
  
  batch.quad_index_count += 6;
  stats.quad_count++;
}

//The center is the point
void draw_rectangle(const Rectangle data, const Color color)
{
  Vector2 p0 = {data.x - data.width * 0.5f, data.y - data.height * 0.5f};
  Vector2 p1 = {data.x + data.width * 0.5f, data.y - data.height * 0.5f};
  Vector2 p2 = {data.x + data.width * 0.5f, data.y + data.height * 0.5f};
  Vector2 p3 = {data.x - data.width * 0.5f, data.y + data.height * 0.5f};

  draw_line(p0, p1, color);
  draw_line(p1, p2, color);
  draw_line(p2, p3, color);
  draw_line(p3, p0, color);
}

void draw_rectangle_thickness(const Rectangle data, const float thickness, const Color color)
{
  if (thickness <= 0.0f) return;
  if (batch.line_thickness != thickness) flush_line_renderer2d();
  set_line_thickness(thickness);
  batch.line_thickness = thickness;
  draw_rectangle(data, color);
}

void draw_disk(const Vector2 center, const float radius, const Color color)
{
  draw_disk_thickness(center, radius, 1.0f, color);
}

void draw_disk_thickness(const Vector2 center, const float radius, float thickness, const Color color)
{
  if (batch.disk_index_count >= RENDERER_MAX_INDICES) flush_disk_renderer2d();
  if (thickness > 1.0f || thickness < 0.0f) thickness = 1.0f;
  const Vector3 world_position[4] = {
    {center.x - radius, center.y - radius, 0.0f},  //TOP-LEFT
    {center.x - radius, center.y + radius, 0.0f},  //BOT-LEFT
    {center.x + radius, center.y + radius, 0.0f},  //BOT-RIGHT
    {center.x + radius, center.y - radius, 0.0f}   //TOP-RIGHT
  };
  const Vector2 local_position[4] = {
    {-1.0f, -1.0f},
    { 1.0f, -1.0f},
    { 1.0f, 1.0f},
    {-1.0f, 1.0f}
  };
  uint32_t parsed_color = COLOR_NUMBER(color);
  const Vector4 color4f = {
    (float)RED(parsed_color)/255, (float)GREEN(parsed_color)/255, (float)BLUE(parsed_color)/255, (float)ALPHA(parsed_color)/255
  };
  const float fade = 0.005;
  
  for (int k = 0; k < 4; k++) {
    batch.disk_buffer_ptr->world_position = world_position[k];
    batch.disk_buffer_ptr->local_position = local_position[k];
    batch.disk_buffer_ptr->color         = color4f;
    batch.disk_buffer_ptr->thickness     = thickness;
    batch.disk_buffer_ptr->fade          = fade;
    batch.disk_buffer_ptr++;
  }
  
  batch.disk_index_count += 6;
  stats.quad_count++;
}

void draw_texture(const Texture texture, const Vector2 pos, const Color color)
{
  draw_texture_extended(texture, (Rectangle){pos.x, pos.y, texture.width, texture.height}, 1.0, color);  
}

void draw_texture_extended(const Texture texture, const Rectangle data, const float scale, const Color color)
{
  if (batch.quad_index_count >= RENDERER_MAX_INDICES) flush_quad_renderer2d();
  uint32_t parsed_color = COLOR_NUMBER(color);
  const Vector4 color4f = {
    (float)RED(parsed_color)/255, (float)GREEN(parsed_color)/255, (float)BLUE(parsed_color)/255, (float)ALPHA(parsed_color)/255
  };
  const Vector3 vertex_position[4] = {
    {data.x                     , data.y                      , 0.0f},  //TOP-LEFT
    {data.x                     , data.y + data.height * scale, 0.0f},  //BOT-LEFT
    {data.x + data.width * scale, data.y + data.height * scale, 0.0f},  //BOT-RIGHT
    {data.x + data.width * scale, data.y                      , 0.0f}   //TOP-RIGHT
  };
  const Vector2 texture_coord[4] = {
    {0.0f, 1.0f},
    {0.0f, 0.0f},
    {1.0f, 0.0f},
    {1.0f, 1.0f}
  };
  
  float texture_index = 0.0f;                            //DEFAULT
  for (uint32_t k = 1; k < batch.texture_slots_index; k++)
  {
    if (batch.texture_slots[k] == texture.id) {
      texture_index = (float)k;
      break;
    }
  }

  if (texture_index == 0.0f) {
    if (batch.texture_slots_index >= RENDERER_MAX_TEXTURES) flush_quad_renderer2d();

    texture_index = (float)batch.texture_slots_index;
    batch.texture_slots[batch.texture_slots_index] = texture.id;
    batch.texture_slots_index++;
  }

  for (int k = 0; k < 4; k++) {
    batch.quad_buffer_ptr->position  = vertex_position[k];
    batch.quad_buffer_ptr->color     = color4f;
    batch.quad_buffer_ptr->tex_coord = texture_coord[k];
    batch.quad_buffer_ptr->tex_index = texture_index;
    batch.quad_buffer_ptr++;
  }
  
  batch.quad_index_count += 6;
  stats.quad_count++;
}

void draw_sub_texture(const SubTexture sub_texture, const Vector2 pos, const Color color)
{
  draw_sub_texture_extended(sub_texture, (Rectangle){pos.x, pos.y, sub_texture.width, sub_texture.height}, 1.0, color); 
}

void draw_sub_texture_extended(const SubTexture sub_texture, const Rectangle data, const float scale, const Color color)
{
  if (batch.quad_index_count >= RENDERER_MAX_INDICES) flush_quad_renderer2d();
  uint32_t parsed_color = COLOR_NUMBER(color);
  const Vector4 color4f = {
    (float)RED(parsed_color)/255, (float)GREEN(parsed_color)/255, (float)BLUE(parsed_color)/255, (float)ALPHA(parsed_color)/255
  };
  const Vector3 vertex_position[4] = {
    {data.x                     , data.y                      , 0.0f},  //TOP-LEFT
    {data.x                     , data.y + data.height * scale, 0.0f},  //BOT-LEFT
    {data.x + data.width * scale, data.y + data.height * scale, 0.0f},  //BOT-RIGHT
    {data.x + data.width * scale, data.y                      , 0.0f}   //TOP-RIGHT
  };
  const Vector2 texture_coord[4] = {
    {sub_texture.min[0], sub_texture.max[1]},
    {sub_texture.min[0], sub_texture.min[1]},
    {sub_texture.max[0], sub_texture.min[1]},
    {sub_texture.max[0], sub_texture.max[1]}
  };
  
  float texture_index = 0.0f;                            //DEFAULT
  //So, how texture_slots is unsigned, uint32_t get rid of a warning
  for (uint32_t k = 1; k < batch.texture_slots_index; k++)
  {
    if (batch.texture_slots[k] == sub_texture.id) {
      texture_index = (float)k;
      break;
    }
  }

  if (texture_index == 0.0f) {
    if (batch.texture_slots_index >= RENDERER_MAX_TEXTURES) flush_quad_renderer2d();

    texture_index = (float)batch.texture_slots_index;
    batch.texture_slots[batch.texture_slots_index] = sub_texture.id;
    batch.texture_slots_index++;
  }

  for (int32_t k = 0; k < 4; k++) {
    batch.quad_buffer_ptr->position  = vertex_position[k];
    batch.quad_buffer_ptr->color     = color4f;
    batch.quad_buffer_ptr->tex_coord = texture_coord[k];
    batch.quad_buffer_ptr->tex_index = texture_index;
    batch.quad_buffer_ptr++;
  }
  
  batch.quad_index_count += 6;
  stats.quad_count++;
}

void draw_text_atlas(const FontAtlas font_atlas, const char* text, const Vector2 pos, const float scale, const Color color)
{
  if (batch.text_index_count >= RENDERER_MAX_INDICES) flush_text_renderer2d();
  if (text == NULL) return;
  uint32_t width = font_atlas.cell.width, height = font_atlas.cell.height;
  

  if (batch.text_texture != 0.0f && batch.text_texture != font_atlas.cell.id) flush_text_renderer2d();
  batch.text_texture = font_atlas.cell.id;
  
  float character_count = 0;
  while (*text != '\0') {
    if (*text == ' ') {
      text++;
      character_count++;
      continue;;
    }

    draw_character(font_atlas, *text, 
      (Rectangle){pos.x + character_count * width * scale/2, pos.y,
      width * scale, height * scale}, color);
    
    if (*text == 'l' || *text == 'i') character_count -= 0.5;
    if (*text == 'f') character_count -= 0.2;
    text++;
    character_count++;
  }
}

void begin_shader(uint32_t shader)
{
  flush_quad_renderer2d();
  batch.current_shader = shader;
}

void end_shader(void)
{
  flush_quad_renderer2d();
  batch.current_shader = batch.quad_shader;
}

//Static methods
static void set_samplers_textures(void)
{
  int32_t loc = get_shader_location(batch.current_shader, "textures");
  int32_t samplers[32];
  for(int32_t i = 0; i < 32; i++)
    samplers[i] = i;
  set_uniform1iv(loc, 32, samplers);
}

static void draw_character(const FontAtlas font_atlas, const char character, const Rectangle data, const Color color)
{
  int32_t index = 0;
  if (character >= 'a' && character <= 'z') {
    index = character-'a';
  } else if (character >= 'A' && character <= 'Z') {
    index = character-'A'+26;
  } else if (character >= '0' && character <= '9') {
    index = character-'0'+52;
  } else if (character == ':') {
    index = 62;
  } else if (character == '.') {
    index = 63;
  } else {
    trigger_log(LOG_WARN, "[%c] Not in atlas", character);
    return;
  }
  int stride_x = index%16;
  int stride_y = index/16;
  
  float cell_width = font_atlas.cell.max[0], cell_height = font_atlas.cell.max[1];
  uint32_t parsed_color = COLOR_NUMBER(color);
  const Vector4 color4f = {
    (float)RED(parsed_color)/255, (float)GREEN(parsed_color)/255, (float)BLUE(parsed_color)/255, (float)ALPHA(parsed_color)/255
  };
  const Vector3 vertex_position[4] = {
    {data.x             , data.y              , 0.0f},  //TOP-LEFT
    {data.x             , data.y + data.height, 0.0f},  //BOT-LEFT
    {data.x + data.width, data.y + data.height, 0.0f},  //BOT-RIGHT
    {data.x + data.width, data.y              , 0.0f}   //TOP-RIGHT
  };
  const Vector2 texture_coord[4] = {
    {font_atlas.cell.min[0] + cell_width * stride_x, font_atlas.cell.max[1] + cell_height * stride_y},
    {font_atlas.cell.min[0] + cell_width * stride_x, font_atlas.cell.min[1] + cell_height * stride_y},
    {font_atlas.cell.max[0] + cell_width * stride_x, font_atlas.cell.min[1] + cell_height * stride_y},
    {font_atlas.cell.max[0] + cell_width * stride_x, font_atlas.cell.max[1] + cell_height * stride_y}
  };

  for (int k = 0; k < 4; k++) {
    batch.text_buffer_ptr->position  = vertex_position[k];
    batch.text_buffer_ptr->color     = color4f;
    batch.text_buffer_ptr->tex_coord = texture_coord[k];
    batch.text_buffer_ptr++;
  }
  
  batch.text_index_count += 6;
  stats.quad_count++;
}

//Time related fuctions
//============================================================
double get_deltatime(void)
{
  return time.delta_time;
}

int get_framerate(void)
{
  return (1/time.delta_time);
}

double get_time(void)
{
  return time.initial_time - time.last_time;
}

void set_framerate_cap(int frame_rate)
{
  time.target_framerate_cap = 1.0/(double)frame_rate;
}
