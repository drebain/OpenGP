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
