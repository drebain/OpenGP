// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <unordered_map>
#include <memory>
#include <typeindex>
#include <cassert>

#include <OpenGP/headeronly.h>


//=============================================================================
namespace OpenGP {
//=============================================================================

class Scene;
class Entity;

class Component {

    friend class Entity;

private:

    Entity *entity = nullptr;
    Scene *scene = nullptr;

protected:

    Component() {}

public:

    Component(const Component&) = delete;
    Component &operator=(const Component&) = delete;

    virtual void init() {}

    virtual void update() {}

    Entity &get_entity() { assert(entity != nullptr); return *entity; }
    const Entity &get_entity() const { assert(entity != nullptr); return *entity; }

    Scene &get_scene() { assert(scene != nullptr); return *scene; }
    const Scene &get_scene() const { assert(scene != nullptr); return *scene; }

    template <typename T>
    T &require();

    template <typename T>
    bool has();

    template <typename T>
    T &get();

    template <typename T>
    const T &get() const;

};

class Entity {

    friend class Scene;

private:

    Scene *scene = nullptr;

    std::unordered_map<std::type_index, std::unique_ptr<Component>> components;

public:

    Entity() {}

    HEADERONLY_INLINE void update();

    Entity(const Entity&) = delete;
    Entity &operator=(const Entity&) = delete;

    template <typename T>
    T &require() {
        assert(scene != nullptr);

        if (has<T>())
            return get<T>();

        T *component = new T();
        components[std::type_index(typeid(T))] = std::unique_ptr<Component>(component);

        component->entity = this;
        component->scene = scene;
        component->init();

        return *component;
    }

    template <typename T>
    bool has() const {
        return components.find(std::type_index(typeid(T))) != components.end();
    }

    template <typename T>
    const T &get() const {
        return dynamic_cast<const T&>(*(components.at(std::type_index(typeid(T)))));
    }

    template <typename T>
    T &get() {
        return dynamic_cast<T&>(*(components.at(std::type_index(typeid(T)))));
    }

    HEADERONLY_INLINE Scene &get_scene();

};

template <typename T>
T &Component::require() { return get_entity().require<T>(); }

template <typename T>
bool Component::has() { return get_entity().has<T>(); }

template <typename T>
T &Component::get() { return get_entity().get<T>(); }

template <typename T>
const T &Component::get() const { return get_entity().get<T>(); }

//=============================================================================
} // OpenGP::
//=============================================================================

#ifdef HEADERONLY
    #include "Entity.cpp"
#endif
