// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <OpenGP/GL/Entity.h>
#include <OpenGP/GL/Components/CameraComponent.h>


//=============================================================================
namespace OpenGP {
//=============================================================================

class TrackballComponent : public Component {
private:

    bool mouse_control_active = false;

    float hangle, vangle;
    Quaternion base_rotation;

public:

    Vec3 center;

    float sensitivity = 0.01;
    float scroll_sensitivity = -0.1;

    void init() {
        require<CameraComponent>();

        center = get<TransformComponent>().position + get<TransformComponent>().forward();
    }

    void update() {

        auto &transform = get<TransformComponent>();

        float radius = (center - transform.position).norm();

        auto &camera = get<CameraComponent>();
        if (camera.has_window()) {

            auto &input = camera.get_window().get_input();

            if (mouse_control_active) {

                if (input.get_mouse(2)) {

                    hangle += input.mouse_delta[0] * sensitivity;
                    vangle += input.mouse_delta[1] * sensitivity;

                    Quaternion hrot(Eigen::AngleAxisf(hangle, Vec3(0, 1, 0)));
                    Quaternion vrot(Eigen::AngleAxisf(vangle, Vec3(1, 0, 0)));

                    transform.rotation = base_rotation * hrot * vrot;

                } else {
                    mouse_control_active = false;
                }

            } else if (!input.mouse_captured) {

                if (input.get_mouse(2)) {
                    mouse_control_active = true;

                    base_rotation = transform.rotation;
                    hangle = 0;
                    vangle = 0;

                }

            }

            if (!input.mouse_captured) {
                radius *= 1 + input.mouse_scroll_delta(1) * scroll_sensitivity;
            }

        }

        transform.position = center - radius * transform.forward();

    }

};

//=============================================================================
} // OpenGP::
//=============================================================================
