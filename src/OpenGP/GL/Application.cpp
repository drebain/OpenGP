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

        for (auto it : to_close)
            windows.erase(it);

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

//=============================================================================
} // OpenGP::
//=============================================================================
