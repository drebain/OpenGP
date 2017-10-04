// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <string>
#include <tuple>
#include <functional>

#include <OpenGP/headeronly.h>
#include <OpenGP/types.h>
#include <OpenGP/GL/EventProvider.h>
#include <OpenGP/GL/glfw.h>


//=============================================================================
namespace OpenGP {
//=============================================================================

typedef int KeyCode;

struct KeyEvent {
    KeyCode key;
    bool released;
};

struct MouseMoveEvent {
    Vec2 delta, position;
};

struct MouseButtonEvent {
    int button;
    bool released;
};

struct MouseScrollEvent {
    Vec2 delta;
};

class Application;

class Window : public EventProvider {

    friend class Application;

private:

    GLFWwindow* handle = nullptr;

    bool close_flag = false;

    std::function<void(Window&)> display_callback = [](Window&){};

    HEADERONLY_INLINE static void mouse_button_callback(GLFWwindow *handle, int button, int action, int mods);
    HEADERONLY_INLINE static void mouse_position_callback(GLFWwindow *handle, double x, double y);
    HEADERONLY_INLINE static void mouse_enter_callback(GLFWwindow *handle, int entered);
    HEADERONLY_INLINE static void mouse_scroll_callback(GLFWwindow *handle, double dx, double dy);
    HEADERONLY_INLINE static void key_callback(GLFWwindow *handle, int key, int scancode, int action, int mods);

public:

    HEADERONLY_INLINE Window(std::function<void(Window&)> display_callback);
    HEADERONLY_INLINE Window(std::function<void(Window&)> display_callback, GLFWwindow *parent_context);

    Window(const Window&) = delete;
    Window &operator=(const Window&) = delete;

    HEADERONLY_INLINE virtual ~Window();

    HEADERONLY_INLINE float get_pixel_scale();

    HEADERONLY_INLINE void set_title(const std::string &title);

    HEADERONLY_INLINE std::tuple<int, int> get_size();
    HEADERONLY_INLINE void set_size(int width, int height);

    HEADERONLY_INLINE std::tuple<int, int, int> get_GL_version();

    HEADERONLY_INLINE void draw();

    HEADERONLY_INLINE void poll();

    HEADERONLY_INLINE bool should_close() const;

    HEADERONLY_INLINE void close();

    HEADERONLY_INLINE void capture_mouse();
    HEADERONLY_INLINE void capture_keyboard();

    struct Input {

        bool mouse_captured, keyboard_captured;

        Vec2 mouse_position;
    };

private:

    Input input;

public:

    const Input &get_input() const { return input; }

};

//=============================================================================
} // namespace OpenGP
//=============================================================================

#ifdef HEADERONLY
    #include "Window.cpp"
#endif
