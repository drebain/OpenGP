// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <OpenGP/GL/Component.h>
#include <OpenGP/GL/Renderer.h>


//=============================================================================
namespace OpenGP {
//=============================================================================

class RenderComponent : public Component {
private:

    std::unique_ptr<Renderer> renderer;

public:

    template <typename T, typename ...Args>
    T &set_renderer(Args ...args) {
        T *t = new T(args...);
        renderer = std::unique_ptr<Renderer>(t);
        return *t;
    }

    Renderer &get_renderer() {
        assert(renderer);
        return *renderer;
    }

};

//=============================================================================
} // OpenGP::
//=============================================================================
