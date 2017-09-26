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


public:

    Vec3 center;
    float distance = 1;
    float sensitivity = 1;
    float scroll_sensitivity = 1;

    void init() {
        require<CameraComponent>();

        center = get<TransformComponent>().position + get<TransformComponent>().forward();
    }

    void update() {

    }

};

//=============================================================================
} // OpenGP::
//=============================================================================
