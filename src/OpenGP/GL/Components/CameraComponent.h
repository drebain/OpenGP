// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <OpenGP/GL/Entity.h>
#include <OpenGP/GL/Components/RenderComponent.h>
#include <OpenGP/GL/Components/TransformComponent.h>
#include <OpenGP/GL/Renderer.h>
#include <OpenGP/GL/Window.h>
#include <OpenGP/MLogger.h>


//=============================================================================
namespace OpenGP {
//=============================================================================

class CameraComponent : public Component {
private:

    Window *window = nullptr;

public:

    float near_plane = 0.1f;
    float far_plane = 1000;
    float vfov = 60;

    CameraComponent() {}

    void init() {
        require<TransformComponent>();
    }

    bool has_window() {
        return window != nullptr;
    }

    void set_window(Window &window) {

        this->window = &window;

        window.set_display_callback([this] (Window &window) {

            RenderContext context;

            int width, height;
            std::tie(width, height) = window.get_size();

            glViewport(0, 0, width, height);
            glClearColor(1, 1, 1, 1);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glEnable(GL_DEPTH_TEST);

            context.aspect = (float)width / (float)height;
            context.vfov = vfov;
            context.near = near_plane;
            context.far = far_plane;
            context.eye = get<TransformComponent>().position;
            context.forward = get<TransformComponent>().forward();
            context.up = get<TransformComponent>().up();

            context.update_view();
            context.update_projection();

            for (auto &renderable : get_scene().all_of_type<RenderComponent>()) {

                context.translation = Vec3(0,0,0);
                context.scale = Vec3(1,1,1);
                context.rotation = Quaternion::Identity();

                context.update_model();

                renderable.get_renderer().render(context);
            }

        });
    }

    Window &get_window() {
        return *window;
    }

};

//=============================================================================
} // OpenGP::
//=============================================================================
