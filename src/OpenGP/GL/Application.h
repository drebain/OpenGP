// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <list>

#include <OpenGP/headeronly.h>
#include <OpenGP/GL/Window.h>


//=============================================================================
namespace OpenGP {
//=============================================================================

class Application {
private:

    std::list<Window> windows;

    bool running = false;

public:

    HEADERONLY_INLINE Application();

    HEADERONLY_INLINE int run();

    HEADERONLY_INLINE Window &create_window();

};

//=============================================================================
} // OpenGP::
//=============================================================================

#ifdef HEADERONLY
    #include "Application.cpp"
#endif
