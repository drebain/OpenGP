#pragma once

#include <OpenGP/GL/Component.h>
#include <OpenGP/GL/Renderer.h>


//=============================================================================
namespace OpenGP {
//=============================================================================

template <typename R>
class RenderComponent : public Component {
private:

    R renderer;

};

//=============================================================================
} // OpenGP::
//=============================================================================
