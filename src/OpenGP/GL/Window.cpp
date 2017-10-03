// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <cassert>

#include <OpenGP/MLogger.h>
#include <OpenGP/GL/gl_debug.h>

#include "Window.h"


//=============================================================================
namespace OpenGP {
//=============================================================================

namespace {

    inline Window &wrapper(GLFWwindow *handle) {
        return *static_cast<Window*>(glfwGetWindowUserPointer(handle));
    }

    /// Callbacks

    inline void close_callback(GLFWwindow *handle) {
        wrapper(handle).close();
    }

}

Window::Window(std::function<void(Window&)> display_callback) : Window(display_callback, nullptr) {}

Window::Window(std::function<void(Window&)> display_callback, GLFWwindow *parent_context) {

    // note: can be called multiple times without problems
    // docs: "additional calls to glfwInit() after successful initialization but before
    //        termination will return GL_TRUE immediately"
    if( !glfwInit() )
        mFatal() << "Failed to initialize GLFW";

    //glfwSetErrorCallback(glfw_error_callback); // TODO

    // Hint GLFW that we would like a shader-based OpenGL context
    // docs: "sets hints for the next call to glfwCreateWindow"
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
    glfwWindowHint(GLFW_VISIBLE, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    /// Attempt to open the window: fails if required version unavailable
    /// @note Intel GPUs do not support OpenGL 3.0
    if( !(handle = glfwCreateWindow(800, 600, "New Window", nullptr, parent_context)) )
        mFatal() << "Failed to open OpenGL 3 GLFW window.";

    /// NOTE: this window's context must be current when this function returns
    glfwMakeContextCurrent(handle);
    if(glfwGetCurrentContext() != handle)
        mFatal() << "Failed to make GLFW context current.";
    glfwSetInputMode(handle, GLFW_CURSOR, GLFW_CURSOR_NORMAL); // can be GLFW_CURSOR_HIDDEN

    // GLEW Initialization (must have created a context)
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if( err != GLEW_OK )
        mFatal() << "Failed GLEW initialization";

    GLDebug::enable();

    glfwSetWindowUserPointer(handle, this);
    glfwSetWindowCloseCallback(handle, &close_callback);

    glfwSetKeyCallback(handle, key_callback);
    glfwSetMouseButtonCallback(handle, mouse_button_callback);
    glfwSetCursorPosCallback(handle, mouse_position_callback);
    glfwSetScrollCallback(handle, mouse_scroll_callback);
    glfwSetCursorEnterCallback(handle, mouse_enter_callback);

    /// Wipe Startup Errors (TODO: check who causes them? GLEW?)
    while (glGetError() != GL_NO_ERROR) {}

    this->display_callback = display_callback;

}

Window::~Window() {
    glfwDestroyWindow(handle);
}

float Window::get_pixel_scale() {
    int w_window, h_window;
    int w_framebuffer, h_framebuffer;
    glfwGetWindowSize(handle, &w_window, &h_window);
    glfwGetFramebufferSize(handle, &w_framebuffer, &h_framebuffer);
    int w_scale = w_framebuffer / w_window;
    int h_scale = h_framebuffer / h_window;
    assert(w_scale == h_scale);
    return (float) w_scale;
}

void Window::set_title(const std::string &title) {
    glfwSetWindowTitle(handle, title.c_str());
}

std::tuple<int, int> Window::get_size() {

    std::tuple<int, int> size;
    glfwGetWindowSize(handle, &std::get<0>(size), &std::get<1>(size));

    return size;
}

void Window::set_size(int width, int height) {
    glfwSetWindowSize(handle, width, height);
}

std::tuple<int, int, int> Window::get_GL_version() {
    int major = glfwGetWindowAttrib(handle, GLFW_CONTEXT_VERSION_MAJOR);
    int minor = glfwGetWindowAttrib(handle, GLFW_CONTEXT_VERSION_MINOR);
    int revision = glfwGetWindowAttrib(handle, GLFW_CONTEXT_REVISION);
    return std::tuple<int, int, int>(major, minor, revision);
}

void Window::draw() {

    glfwMakeContextCurrent(handle);

    display_callback(*this);

    glfwSwapBuffers(handle);

}

void Window::poll() {

    glfwPollEvents();

}

bool Window::should_close() const {

    return close_flag;

}

void Window::close() {

    close_flag = true;

}

void Window::mouse_button_callback(GLFWwindow *handle, int button, int action, int mods) {
    MouseButtonEvent event;
    event.button = button;
    event.released = (action == GLFW_RELEASE);
    wrapper(handle).send_event(event);
}

void Window::mouse_position_callback(GLFWwindow *handle, double x, double y) {
    MouseMoveEvent event;
    event.position = Vec2(x, y);
    wrapper(handle).send_event(event);
}

void Window::mouse_enter_callback(GLFWwindow *handle, int entered) {
    //wrapper(handle).send_event(event);
}

void Window::mouse_scroll_callback(GLFWwindow *handle, double dx, double dy) {
    MouseScrollEvent event;
    event.delta = Vec2(dx, dy);
    wrapper(handle).send_event(event);
}

void Window::key_callback(GLFWwindow *handle, int key, int scancode, int action, int mods) {
    KeyEvent event;
    event.key = key;
    event.released = (action == GLFW_RELEASE);
    wrapper(handle).send_event(event);
}


//=============================================================================
} // OpenGP::
//=============================================================================
