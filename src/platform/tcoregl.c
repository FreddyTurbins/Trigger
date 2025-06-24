#include "../vendor/glfw/include/GLFW/glfw3.h"
#include "../vendor/glad/glad.h"

#if defined(_WIN32)

#endif
#if defined(__linux__)

#endif

typedef struct {
  GLFWwindow *m_window;
} PlatformData;

extern TriggerWindow trigger_window;
extern Keyboard keyboard;
PlatformData platform = {0};

static void key_call_back(GLFWwindow *window, int key, int scancode, int action, int mods);

int init_opengl(void)
{
  int result = glfwInit();
  if (result == GLFW_FALSE) { 
    trigger_log(LOG_FATAL, "Failed to initialize GLFW");
    return -1;
  }
  platform.m_window = glfwCreateWindow(trigger_window.render.width, trigger_window.render.height, trigger_window.title, NULL, NULL);
  if(!platform.m_window) 
  {
    trigger_log(LOG_FATAL, "Fail creating window");
    glfwTerminate();
    return -1;
  }
  //MAKE A ABSTRACTION TO OPENGLCONTEXT
  glfwMakeContextCurrent(platform.m_window);
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
  {
    trigger_log(LOG_FATAL, "Failed initializing GLAD");
    return -1;
  }
  glfwSetKeyCallback(platform.m_window, key_call_back);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_LINE_SMOOTH);
  glEnable(GL_BLEND);
  return 0;
}

void opengl_set_viewport(uint32_t width, uint32_t height)
{
  //glViewport(0, 0, width, height);
  glfwSetWindowSize(platform.m_window, width, height);
}

void opengl_toggle_full_screen(void)
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

bool opengl_should_close(void)
{
  return glfwWindowShouldClose(platform.m_window);
}

void opengl_close_window(void)
{
  glfwTerminate();
}

void opengl_set_window_icon(Image image)
{
  if (image.data == NULL) {
    trigger_log(LOG_WARN, "set_window_icon -> Null image data");
    return;
  }
  GLFWimage icon = {
    .width = image.width,
    .height = image.height,
    .pixels = (uint8_t*)image.data
  };
  glfwSetWindowIcon(platform.m_window, 1, &icon);
  trigger_log(LOG_INFO, "set_window_icon -> Icon succesfully setted");
}

void opengl_swap_screen_buffer(void)
{
  glfwSwapBuffers(platform.m_window);
  glfwPollEvents();
}

void opengl_set_v_sync(const bool enabled)
{
  if (enabled)
    glfwSwapInterval(1);
  else
    glfwSwapInterval(0);
}

double opengl_get_time(void)
{
  double time = glfwGetTime();
  return time;
}

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

bool opengl_is_mouse_button_pressed(const int32_t button)
{
  int32_t status = glfwGetMouseButton(platform.m_window, button);
  return status == GLFW_PRESS;
}

float opengl_get_mouse_x_position(void)
{
  double x_pos, y_pos;
  glfwGetCursorPos(platform.m_window, &x_pos, &y_pos);
  return (float)x_pos;
}

float opengl_get_mouse_y_position(void)
{
  double x_pos, y_pos;
  glfwGetCursorPos(platform.m_window, &x_pos, &y_pos);
  return (float)y_pos;
}

char* opengl_get_clipboard_string(void)
{
  const char* clipboard = glfwGetClipboardString(platform.m_window);
  return (char*)clipboard;
}
