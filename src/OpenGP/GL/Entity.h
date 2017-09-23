// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <unordered_map>
#include <memory>
#include <typeindex>
#include <cassert>

#include <OpenGP/GL/Component.h>


//=============================================================================
namespace OpenGP {
//=============================================================================

class Scene;

class Entity {

    friend class Scene;

private:

    Scene *scene = nullptr;

    std::unordered_map<std::type_index, std::unique_ptr<Component>> components;

public:

    Entity() {}

    void update();

    Entity(const Entity&) = delete;
    Entity &operator=(const Entity&) = delete;

    template <typename T>
    T &require() {
        if (has<T>())
            return get<T>();

        T *component = new T();
        components[std::type_index(typeid(T))] = std::unique_ptr<Component>(component);
        return *component;
    }

    template <typename T>
    bool has() {
        return components.find(std::type_index(typeid(T))) != components.end();
    }

    template <typename T>
    T &get() {
        return dynamic_cast<T&>(*(components[std::type_index(typeid(T))]));
    }

};

//=============================================================================
} // OpenGP::
//=============================================================================

#ifdef HEADERONLY
    #include "Entity.cpp"
#endif
