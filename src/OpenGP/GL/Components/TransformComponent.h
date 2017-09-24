// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <OpenGP/types.h>
#include <OpenGP/GL/Component.h>


//=============================================================================
namespace OpenGP {
//=============================================================================

class TransformComponent : public Component {
private:

    Vec3 translation;
    Vec3 scale;
    Quaternion rotation;

public:

    TransformComponent() {}

};

//=============================================================================
} // OpenGP::
//=============================================================================
