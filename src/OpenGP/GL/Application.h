// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <list>
#include <memory>

#include <OpenGP/headeronly.h>
#include <OpenGP/GL/Window.h>
#include <OpenGP/GL/Framebuffer.h>
#include <OpenGP/GL/FullscreenQuad.h>


//=============================================================================
namespace OpenGP {
//=============================================================================

class Application {
private:

    struct WindowContainer {

        int width, height;

        RGB8Texture color_texture;
        D16Texture depth_texture;
        Framebuffer framebuffer;

        std::unique_ptr<Window> window;
        std::unique_ptr<FullscreenQuad> fsquad;

        HEADERONLY_INLINE WindowContainer(Application &app, std::function<void(Window&)> user_display_callback);

    };

    GLFWwindow *hidden_window;

    std::list<WindowContainer> windows;

    bool running = false;
    bool close_requested = false;

    std::function<void()> update_callback = [](){};

public:

    HEADERONLY_INLINE Application(const char *name="OpenGP Application");

    HEADERONLY_INLINE int run();

    HEADERONLY_INLINE Window &create_window(std::function<void(Window&)> display_callback);

    HEADERONLY_INLINE void set_update_callback(std::function<void()> fn);

    HEADERONLY_INLINE void close();

};

//=============================================================================
} // OpenGP::
//=============================================================================

#ifdef HEADERONLY
    #include "Application.cpp"
#endif
