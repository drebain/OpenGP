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

public:

    void set_camera(CameraComponent &camera) {

        // Clear any old callbacks
        sentinel.reset();

        this->camera = &camera;

        Window &window = camera.get_window();

        window.add_listener<MouseButtonEvent>([this](const MouseButtonEvent &event) {

            if (event.button < 0 || event.button > 2)
                return;

            if (event.released) {

                mouse_down[event.button] = mouse_was_down_this_frame[event.button];

            } else {

                mouse_down[event.button] = true;
                mouse_was_down_this_frame[event.button] = true;

            }

        }, sentinel);
    }

};

//=============================================================================
} // OpenGP::
//=============================================================================
