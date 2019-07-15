// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <OpenGP/GL/Entity.h>
#include <OpenGP/GL/Components/CameraComponent.h>


//=============================================================================
namespace OpenGP {
//=============================================================================

/// A component representing a camera that is controlled by user input
class TrackballComponent : public Component {
private:

    bool mouse_control_active = false;
    bool updated_once = false;

    float hangle = 0, vangle = 0;
    Quaternion base_rotation;

public:

    // Up axis to orient camera
    Vec3 up_direction = Vec3(0, 1, 0);

    // Should the camera be oriented w.r.t the up direction?
    bool oriented = false;

    /// The point about which the camera will rotate
    Vec3 center;

    /// Multiplier for panning mouse input
    float pan_sensitivity = 0.005;

    /// Multiplier for rotating mouse input
    float rotate_sensitivity = 0.01;

    /// Multiplier for zooming scroll input
    float scroll_sensitivity = -0.1;

    void init() {
        require<CameraComponent>();

        center = get<TransformComponent>().position + get<TransformComponent>().forward();

        base_rotation = Quaternion::Identity();
    }

    void update() {

        auto &transform = get<TransformComponent>();

        float radius = (center - transform.position).norm();

        if (oriented) {
            // Transform t_base;
            // t_base.set_up(up_direction);
            // base_rotation = t_base.rotation;
        }

        bool something_happened = false;

        auto &camera = get<CameraComponent>();
        if (camera.has_window()) {

            auto &input = camera.get_window().get_input();

            if (mouse_control_active) {

                something_happened = true;

                if (input.get_mouse(2)) {

                    if (input.get_key(GLFW_KEY_LEFT_SHIFT)) {

                        center += radius * pan_sensitivity * (transform.right() * input.mouse_delta[0] + transform.up() * input.mouse_delta[1]);

                    } else {

                        hangle -= input.mouse_delta[0] * rotate_sensitivity;
                        vangle += input.mouse_delta[1] * rotate_sensitivity;

                        Quaternion hrot(Eigen::AngleAxisf(hangle, Vec3(0, 1, 0)));
                        Quaternion vrot(Eigen::AngleAxisf(vangle, Vec3(1, 0, 0)));

                        transform.rotation = base_rotation * hrot * vrot;

                    }

                } else {
                    mouse_control_active = false;
                }

            } else if (!input.mouse_captured) {

                if (input.get_mouse(2)) {
                    mouse_control_active = true;

                    if (!oriented) {
                        base_rotation = transform.rotation;
                        hangle = 0;
                        vangle = 0;
                    }

                    something_happened = true;

                }

            }

            if (!input.mouse_captured) {
                radius *= 1 + input.mouse_scroll_delta(1) * scroll_sensitivity;

                if (std::fabs(input.mouse_scroll_delta(1)) > 1e-2)
                    something_happened = true;
            }

        }

        if (something_happened || !updated_once) {
            transform.position = center - radius * transform.forward();
            updated_once = true;
        }

    }

};

//=============================================================================
} // OpenGP::
//=============================================================================
