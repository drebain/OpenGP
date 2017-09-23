// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <list>

#include <OpenGP/GL/Entity.h>


//=============================================================================
namespace OpenGP {
//=============================================================================

class Scene {
private:

    std::list<Entity> entities;

public:

    Scene() {}

    Entity &create_entity();

    template <typename T>
    Entity &create_entity_with() {
        auto &entity = create_entity();
        entity.require<T>();
        return entity;
    }

};

//=============================================================================
} // OpenGP::
//=============================================================================

#ifdef HEADERONLY
    #include "Scene.cpp"
#endif
