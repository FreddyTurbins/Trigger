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

#include "vendor/glfw/src/init.c"
#include "vendor/glfw/src/platform.c"
#include "vendor/glfw/src/context.c"
#include "vendor/glfw/src/monitor.c"
#include "vendor/glfw/src/window.c"
#include "vendor/glfw/src/input.c"
#include "vendor/glfw/src/vulkan.c"

#if defined(_WIN32) || defined(__CYGWIN__)
    #include "vendor/glfw/src/win32_init.c"
    #include "vendor/glfw/src/win32_module.c"
    #include "vendor/glfw/src/win32_monitor.c"
    #include "vendor/glfw/src/win32_window.c"
    #include "vendor/glfw/src/win32_joystick.c"
    #include "vendor/glfw/src/win32_time.c"
    #include "vendor/glfw/src/win32_thread.c"
    #include "vendor/glfw/src/wgl_context.c"

    #include "vendor/glfw/src/egl_context.c"
    #include "vendor/glfw/src/osmesa_context.c"
#endif

#if defined(__linux__)
    #include "vendor/glfw/src/posix_module.c"
    #include "vendor/glfw/src/posix_thread.c"
    #include "vendor/glfw/src/posix_time.c"
    #include "vendor/glfw/src/posix_poll.c"
    #include "vendor/glfw/src/linux_joystick.c"
    #include "vendor/glfw/src/xkb_unicode.c"

    #include "vendor/glfw/src/egl_context.c"
    #include "vendor/glfw/src/osmesa_context.c"

    #if defined(_GLFW_WAYLAND)
        #include "vendor/glfw/src/wl_init.c"
        #include "vendor/glfw/src/wl_monitor.c"
        #include "vendor/glfw/src/wl_window.c"
    #endif
    #if defined(_GLFW_X11)
        #include "vendor/glfw/src/x11_init.c"
        #include "vendor/glfw/src/x11_monitor.c"
        #include "vendor/glfw/src/x11_window.c"
        #include "vendor/glfw/src/glx_context.c"
    #endif
#endif

