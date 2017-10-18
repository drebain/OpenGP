// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <OpenGP/util/Transform.h>
#include <OpenGP/GL/Entity.h>


//=============================================================================
namespace OpenGP {
//=============================================================================

/// A component representing the position, orientation and scale of an entity
class TransformComponent : public Transform, public Component {
public:

    // TODO: add parent + children here to make actual scene graph

};

//=============================================================================
} // OpenGP::
//=============================================================================
