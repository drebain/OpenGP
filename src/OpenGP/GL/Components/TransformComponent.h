// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <OpenGP/types.h>
#include <OpenGP/GL/Entity.h>


//=============================================================================
namespace OpenGP {
//=============================================================================

class TransformComponent : public Component {
public:

    Vec3 position;
    Vec3 scale;
    Quaternion rotation;

    Vec3 right() const {
        return rotation * Vec3(1, 0, 0);
    }

    Vec3 up() const {
        return rotation * Vec3(0, 1, 0);
    }

    Vec3 forward() const {
        return rotation * Vec3(0, 0, 1);
    }

};

//=============================================================================
} // OpenGP::
//=============================================================================
