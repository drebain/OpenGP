#pragma once

#include <OpenGP/GL/Component.h>
#include <OpenGP/GL/Renderer.h>


//=============================================================================
namespace OpenGP {
//=============================================================================

class RenderComponent : public Component {
private:

    std::unique_ptr<Renderer> renderer;

};

//=============================================================================
} // OpenGP::
//=============================================================================
