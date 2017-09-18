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
