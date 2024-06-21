////////////////////////////////////////////////////////

#if defined(_WIN32) || defined(__CYGWIN__)
    #define _GLFW_WIN32
    #define _GLFW_BUILD_DLL
#endif
#if defined(__linux__)
    #define _GLFW_X11
#endif

#if defined(__TINYC__)
    #define _WIN32_WINNT_WINXP      0x0501
#endif

#include "external/glfw/src/init.c"
#include "external/glfw/src/platform.c"
#include "external/glfw/src/context.c"
#include "external/glfw/src/monitor.c"
#include "external/glfw/src/window.c"
#include "external/glfw/src/input.c"
#include "external/glfw/src/vulkan.c"

#if defined(_WIN32) || defined(__CYGWIN__)
    #include "external/glfw/src/win32_init.c"
    #include "external/glfw/src/win32_module.c"
    #include "external/glfw/src/win32_monitor.c"
    #include "external/glfw/src/win32_window.c"
    #include "external/glfw/src/win32_joystick.c"
    #include "external/glfw/src/win32_time.c"
    #include "external/glfw/src/win32_thread.c"
    #include "external/glfw/src/wgl_context.c"

    #include "external/glfw/src/egl_context.c"
    #include "external/glfw/src/osmesa_context.c"
#endif

#if defined(__linux__)
    #include "external/glfw/src/posix_module.c"
    #include "external/glfw/src/posix_thread.c"
    #include "external/glfw/src/posix_time.c"
    #include "external/glfw/src/posix_poll.c"
    #include "external/glfw/src/linux_joystick.c"
    #include "external/glfw/src/xkb_unicode.c"

    #include "external/glfw/src/egl_context.c"
    #include "external/glfw/src/osmesa_context.c"

    #if defined(_GLFW_WAYLAND)
        #include "external/glfw/src/wl_init.c"
        #include "external/glfw/src/wl_monitor.c"
        #include "external/glfw/src/wl_window.c"
    #endif
    #if defined(_GLFW_X11)
        #include "external/glfw/src/x11_init.c"
        #include "external/glfw/src/x11_monitor.c"
        #include "external/glfw/src/x11_window.c"
        #include "external/glfw/src/glx_context.c"
    #endif
#endif
