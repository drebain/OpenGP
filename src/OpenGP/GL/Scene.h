// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <list>

#include <OpenGP/headeronly.h>
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
    T &create_entity_with() {
        auto &entity = create_entity();
        T &t = entity.require<T>();
        return t;
    }

    //template <typename T>
    //SimpleIterable<T> all_of_type() {
    //
    //}

    HEADERONLY_INLINE void update();

};

//=============================================================================
} // OpenGP::
//=============================================================================

#ifdef HEADERONLY
    #include "Scene.cpp"
#endif
