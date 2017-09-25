// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <vector>

#include "Application.h"


//=============================================================================
namespace OpenGP {
//=============================================================================

Application::Application() {

}

int Application::run() {

    running = true;

    while (running) {

        std::vector<decltype(windows.begin())> to_close;

        glfwPollEvents();
        for (auto it = windows.begin();it != windows.end();++it)
            if ((*it).should_close())
                to_close.push_back(it);

        for (auto it : to_close) {
            windows.erase(it);
        }

        update_callback();

        running = false;

        for (auto &window : windows) {

            running = true;

            window.draw();

        }

    }

    return 0;

}

Window &Application::create_window() {

    windows.emplace_back();
    auto &window = windows.back();

    return window;

}

void Application::set_update_callback(std::function<void()> fn) {
    update_callback = fn;
}

//=============================================================================
} // OpenGP::
//=============================================================================
