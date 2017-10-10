// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <OpenGP/GL/Entity.h>
#include <OpenGP/GL/Components/CameraComponent.h>
#include <OpenGP/GL/ImguiRenderer.h>


//=============================================================================
namespace OpenGP {
//=============================================================================

class GUICanvasComponent : public Component {
private:

    EventSentinel sentinel;

    ImguiRenderer renderer;

    CameraComponent *camera = nullptr;

    bool mouse_was_down_this_frame[3];
    bool mouse_down[3];

    std::function<void()> action;

public:

    void set_camera(CameraComponent &camera) {

        // Clear any old callbacks
        sentinel.reset();

        this->camera = &camera;

        camera.add_listener<GUIRenderEvent>([this](const GUIRenderEvent &event){

            auto &cam = *(this->camera);
            auto &window = cam.get_window();

            int width, height;
            std::tie(width, height) = window.get_size();

            auto &io = ImGui::GetIO();
            auto &input = window.get_input();

            io.MousePos = ImVec2(input.mouse_position(0), input.mouse_position(1));

            for (int i = 0;i < 3;i++)
                io.MouseDown[i] = input.get_mouse(i);

            renderer.begin_frame(width, height);

            if (action)
                action();

            renderer.end_frame();

            if (io.WantCaptureMouse)
                window.capture_mouse();
            else
                window.release_mouse();


            if (io.WantCaptureKeyboard)
                window.capture_keyboard();
            else
                window.release_keyboard();

        }, sentinel);

    }

    void set_action(std::function<void()> action) {
        this->action = action;
    }

};

//=============================================================================
} // OpenGP::
//=============================================================================
