#pragma once

#include <string>
#include <tuple>
#include <functional>

#include <OpenGP/headeronly.h>
#include <OpenGP/GL/glfw.h>


//=============================================================================
namespace OpenGP {
//=============================================================================

class Window {
private:

    GLFWwindow* handle = nullptr;

    bool close_flag = false;

    std::function<void(Window&)> display_callback = [](Window&){};

public:

    HEADERONLY_INLINE Window();

    Window(const Window&) = delete;
    Window &operator=(const Window&) = delete;

    HEADERONLY_INLINE virtual ~Window();

    HEADERONLY_INLINE float get_pixel_scale();

    HEADERONLY_INLINE void set_title(const std::string &title);

    HEADERONLY_INLINE std::tuple<int, int> get_size();
    HEADERONLY_INLINE void set_size(int width, int height);

    HEADERONLY_INLINE std::tuple<int, int, int> get_GL_version();

    HEADERONLY_INLINE void set_display_callback(std::function<void(Window&)> fn);

    HEADERONLY_INLINE void draw();

    HEADERONLY_INLINE bool should_close() const;

    HEADERONLY_INLINE void close();

};

//=============================================================================
} // namespace OpenGP
//=============================================================================

#ifdef HEADERONLY
    #include "Window.cpp"
#endif
