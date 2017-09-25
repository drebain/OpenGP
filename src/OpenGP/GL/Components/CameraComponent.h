// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <OpenGP/GL/Component.h>
#include <OpenGP/GL/Components/TransformComponent.h>
#include <OpenGP/GL/Renderer.h>
#include <OpenGP/GL/Window.h>
#include <OpenGP/MLogger.h>


//=============================================================================
namespace OpenGP {
//=============================================================================

class CameraComponent : public Component {
private:


public:

    CameraComponent() {}

    void init() {
        get_entity().require<TransformComponent>();
    }

    void draw_frame(Window &window) {

        RenderContext context;

        int width, height;
        std::tie(width, height) = window.get_size();

        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        context.aspect = (float)width / (float)height;
        context.vfov = 80;
        context.near = 0.1;
        context.far = 100;
        context.eye = Vec3(0,0,-1);
        context.forward = Vec3(0,0,1);
        context.up = Vec3(0,1,0);

        context.update_view();
        context.update_projection();

        for (auto &renderable : get_scene().all_of_type<RenderComponent>()) {

            context.translation = Vec3(0,0,0);
            context.scale = Vec3(1,1,1);
            context.rotation = Quaternion::Identity();

            context.update_model();

            renderable.get_renderer().render(context);
        }
    }

};

//=============================================================================
} // OpenGP::
//=============================================================================
