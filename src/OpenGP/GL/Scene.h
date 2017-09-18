#pragma once

#include <list>

#include <OpenGP/GL/Entity.h>
#include <OpenGP/GL/System.h>


//=============================================================================
namespace OpenGP {
//=============================================================================

class Scene {
private:

    std::list<Entity> entities;

    std::unordered_map<std::type_index, std::unique_ptr<System>> systems;

public:

    Scene() {}

    Entity &create_entity();

    template <typename T>
    Entity &create_entity_with() {
        auto &entity = create_entity();
        entity.require<T>();
        return entity;
    }

    template <typename T>
    T &require() {
        if (has<T>())
            return get<T>();

        T *system = new T();
        systems[std::type_index(typeid(T))] = std::unique_ptr<System>(system);
        return *system;
    }

    template <typename T>
    bool has() {
        return systems.find(std::type_index(typeid(T))) != systems.end();
    }

    template <typename T>
    T &get() {
        return dynamic_cast<T&>(*(systems[std::type_index(typeid(T))]));
    }

};

//=============================================================================
} // OpenGP::
//=============================================================================

#ifdef HEADERONLY
    #include "Scene.cpp"
#endif
