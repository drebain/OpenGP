// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <cassert>


//=============================================================================
namespace OpenGP {
//=============================================================================

class Entity;

class Component {

    friend class Entity;

private:

    Entity *entity = nullptr;

protected:

    Component() {}

public:

    Component(const Component&) = delete;
    Component &operator=(const Component&) = delete;

    virtual void init() {}

    virtual void update() {}

    Entity &get_entity() { assert(entity != nullptr); return *entity; }

};

//=============================================================================
} // OpenGP::
//=============================================================================
