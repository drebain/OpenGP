// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <OpenGP/GL/Entity.h>
#include <OpenGP/GL/MaterialRenderer.h>
#include <OpenGP/GL/Components/TransformComponent.h>


//=============================================================================
namespace OpenGP {
//=============================================================================

class WorldRenderComponent : public Component {
private:

    std::unique_ptr<MaterialRenderer> renderer;

public:
    
    bool visible = true;

    void init() {
        require<TransformComponent>();
    }

    template <typename T, typename ...Args>
    T &set_renderer(Args ...args) {
        T *t = new T(args...);
        renderer = std::unique_ptr<MaterialRenderer>(t);
        return *t;
    }

    template <typename T = MaterialRenderer>
    T &get_renderer() {
        assert(renderer);
        return *dynamic_cast<T*>(renderer.get());
    }

};

//=============================================================================
} // OpenGP::
//=============================================================================
