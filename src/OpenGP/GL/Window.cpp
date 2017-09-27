// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <cassert>

#include <OpenGP/MLogger.h>

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

Window::Window() {

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

    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    /// Attempt to open the window: fails if required version unavailable
    /// @note Intel GPUs do not support OpenGL 3.0
    if( !(handle = glfwCreateWindow(800, 600, "New Window", nullptr, nullptr)) )
        mFatal() << "Failed to open OpenGL 3 GLFW window.";

    glfwMakeContextCurrent(handle);
    if(glfwGetCurrentContext() != handle)
        mFatal() << "Failed to make GLFW context current.";
    glfwSetInputMode(handle, GLFW_CURSOR, GLFW_CURSOR_NORMAL); // can be GLFW_CURSOR_HIDDEN

    // GLEW Initialization (must have created a context)
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if( err != GLEW_OK )
        mFatal() << "Failed GLEW initialization";

    glfwSetWindowUserPointer(handle, this);
    glfwSetWindowCloseCallback(handle, &close_callback);

    /// Wipe Startup Errors (TODO: check who causes them? GLEW?)
    while (glGetError() != GL_NO_ERROR) {}

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

void Window::set_display_callback(std::function<void(Window&)> fn) {
    display_callback = fn;
}

void Window::draw() {

    glfwMakeContextCurrent(handle);

    display_callback(*this);

    glfwSwapBuffers(handle);

}

bool Window::should_close() const {

    return close_flag;

}

void Window::close() {

    close_flag = true;

}

//=============================================================================
} // OpenGP::
//=============================================================================
