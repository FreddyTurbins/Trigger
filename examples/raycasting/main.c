#include <math.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "TRIGGER/trigger.h"
#include "../renderer.h"
#include "../renderer.c"

#define PI 3.141592653589793

unsigned char map[64] = {
  1, 1, 1, 1, 1, 1, 1, 1,
  1, 0, 0, 0, 1, 0, 0, 1,
  1, 0, 1, 0, 1, 0, 0, 1,
  1, 0, 0, 0, 1, 0, 0, 1,
  1, 0, 0, 0, 0, 0, 0, 1,
  1, 0, 0, 0, 0, 0, 0, 1,
  1, 0, 0, 0, 0, 0, 0, 1,
  1, 1, 1, 1, 1, 1, 1, 1
};

int8_t get_sign(double n)
{
  return (n < 0) ? -1 : 1;
}

void render_map(void)
{
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      if (map[i * 8 + j])
        draw_quad((Rect){1 + 25 * j, 1059 - 25 * i, 24, 24}, (Color){255, 255, 255, 255});
    }
  }
}

signed main()
{
  init_window("Raycasting example", 1920, 1080);
  set_window_icon("./raycast_icon.png");
  init_renderer2d();
  FontAtlas font = create_font_atlas("./font.bmp", (Vector2){16.0f, 5.0f});
  char buffer[32];
  Vector2 player_position = {
    .x = 80.0f,
    .y = 970.0f
  };

  float fov = 90.0f;
  float angle_view = 0;
  float angle_steps = fov/330;

  while(!window_should_close()) {
    start_batch();
    set_background(DARK_GRAY);
    if (is_mouse_button_pressed(TRIGGER_MOUSE_BUTTON_LEFT)) {
      set_window_size((Vector2){1080, 720});
    }
    if (is_mouse_button_pressed(TRIGGER_MOUSE_BUTTON_RIGHT)) {
      set_window_size((Vector2){1920, 1080});
    }
    if (is_key_pressed(TRIGGER_KEY_F)) {
      toggle_full_screen();
    }
    if (is_key_down(TRIGGER_KEY_LEFT)) {
      angle_view += 55.5f*get_deltatime();
    }
    if (is_key_down(TRIGGER_KEY_RIGHT)) {
      angle_view -= 55.5f*get_deltatime();
    } 
    if (is_key_down(TRIGGER_KEY_A)) {
      player_position.x += 45.5f*get_deltatime()*cos((angle_view+90)*PI/180);
      player_position.y += 45.5f*get_deltatime()*sin((angle_view+90)*PI/180);
    }
    if (is_key_down(TRIGGER_KEY_D)) {
      player_position.x += 45.5f*get_deltatime()*cos((angle_view-90)*PI/180);
      player_position.y += 45.5f*get_deltatime()*sin((angle_view-90)*PI/180);
    }
    if (is_key_down(TRIGGER_KEY_W)) {
      player_position.x += 45.5f*get_deltatime()*cos(angle_view*PI/180);
      player_position.y += 45.5f*get_deltatime()*sin(angle_view*PI/180);
    }
    if (is_key_down(TRIGGER_KEY_S)) {
      //BURRITO'S RULE +180 COFUNCTION IS NOT CHANGED BUT WITH TODOS SIN TACOS U KNOW IS NEGATIVE
      player_position.x += 45.5f*get_deltatime()*-cos((angle_view)*PI/180);
      player_position.y += 45.5f*get_deltatime()*-sin((angle_view)*PI/180);
    }
    if (is_key_down(TRIGGER_KEY_P)) {
      fov += 50.0f*get_deltatime();
    }
    if (is_key_down(TRIGGER_KEY_O)) {
      fov -= 50.0f*get_deltatime();
    }

    if (angle_view < 0.0f) angle_view += 360.0f;
    if (angle_view > 360.0f) angle_view -= 360.0f;
    set_v_sync(1);
    angle_steps = fov/330;
    Vector2 rays_position[330];
    for (float ray_angle = angle_view - fov/2, count = 0.0f; count < 330.0f ; count++, ray_angle+=angle_steps) {
      if (ray_angle < 0.0f) ray_angle += 360.0f;
      if (ray_angle >= 360.0f) ray_angle -= 360.0f;
      float ray_angle_radians = ray_angle*PI/180;
      //===========================
      //Horizontal line
      float y_stride = -25.0f;
      if (ray_angle_radians < PI) {y_stride = 25.0f;}
      float x_stride = y_stride/tan(ray_angle_radians);
      if (ray_angle_radians == PI || ray_angle_radians == 0) {x_stride = y_stride;}
      float y_step = ((int)(player_position.y/25))*25 - 0.0001f;
      if (ray_angle_radians < PI) {y_step += 25.0001f;}
      float x_step = (player_position.y - y_step)/-tan(ray_angle_radians) + player_position.x;
      int steps = 0;
      float px_prime = 0;
      float py_prime = 0;
      while (1) {
        px_prime = x_stride * steps + x_step;
        py_prime = y_stride * steps + y_step;
        int index_x = (px_prime) / 25;
        int index_y = (py_prime - 900) / 25;
        if ((7-index_y)*8 + index_x < 0 || (7-index_y)*8 + index_x > 63) break;
        if (map[(7 - index_y) * 8 + index_x]) { break; }
        steps++;
      }
      float distance_horizontal = sqrt((px_prime-player_position.x)*(px_prime-player_position.x)+
          (py_prime-player_position.y)*(py_prime-player_position.y));
      py_prime -= 15;
      
      //===========================
      //Vertical line
      x_stride = -25.0f;
      if (ray_angle_radians < PI/2 || ray_angle_radians > 3*PI/2) {x_stride = 25.0f;}
      y_stride = x_stride*tan(ray_angle_radians);
      x_step = ((int)(player_position.x/25))*25 - 0.00001f;
      if (ray_angle_radians < PI/2 || ray_angle_radians > 3*PI/2) {x_step += 25.00001f;}
      y_step = (player_position.x-x_step)*-tan(ray_angle_radians) + player_position.y;
      steps = 0;
      float px_prime_v = 0;
      float py_prime_v = 0;
      while (1) {
        px_prime_v = x_stride * steps + x_step;
        py_prime_v = y_stride * steps + y_step;
        int index_x = (px_prime_v) / 25;
        int index_y = (py_prime_v - 900) / 25;
        if ((7-index_y)*8 + index_x < 0 || (7-index_y)*8 + index_x > 63) break;
        if (map[(7 - index_y) * 8 + index_x]) break;
        steps++;
      }
      float distance_vertical = sqrt((px_prime_v-player_position.x)*(px_prime_v-player_position.x)+
          (py_prime_v-player_position.y)*(py_prime_v-player_position.y));
      py_prime_v -= 15;

      Color color = {0};
      float fixed_x = 0.0f, fixed_y = 0.0f, distance = 0.0f;
      if (distance_horizontal < distance_vertical) {
        fixed_x = px_prime;
        fixed_y = py_prime;
        distance = distance_horizontal;
        color = (Color){180, 50, 90, 255};
      } else {
        fixed_x = px_prime_v;
        fixed_y = py_prime_v;
        distance = distance_vertical;
        color = (Color){150, 40, 80, 255};
      }
      float fixed_angle = angle_view-ray_angle;
      if (fixed_angle < 0) fixed_angle += 360;
      if (fixed_angle > 360) fixed_angle -= 360;
      fixed_angle = fixed_angle*PI/180;
      distance *= cos(fixed_angle);
      
      float h = (25*1080)/distance;
      float line_offset = 590-h/2;
      if (h > 1080.0f) h = 1080.0f;
      draw_line_thickness((Vector2){(330-count)*6, line_offset}, (Vector2){(330-count)*6, h+line_offset}, 6.0f, color);
      rays_position[(int)count] = (Vector2){fixed_x, fixed_y};
    
    }
    for (int i = 0; i < 330; i++) {
      draw_line_thickness(player_position, rays_position[i], 1.0f, HAKEN);
    }
    render_map();
    draw_quad((Rect){player_position.x - 7.5, player_position.y - 7.5, 15, 15}, HAKEN);
    snprintf(buffer, 32, "FPS: %4d", get_framerate());
    draw_rectangle_thickness((Rect){1740, 1042, 310, 60}, 6.0f, (Color){255, 255, 255, 255});
    draw_text_atlas(font, buffer, (Vector2){1600, 1000}, 4.0f, (Color){255, 255, 255, 255});
    end_batch();
  } 
  close_renderer2d();
  close_window();
  return 0;
}
