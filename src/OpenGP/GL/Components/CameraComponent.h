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

        for (auto &renderable : get_scene().all_of_type<RenderComponent>()) {

        }
    }

};

//=============================================================================
} // OpenGP::
//=============================================================================
