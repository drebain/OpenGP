#pragma once

#include <OpenGP/GL/Component.h>
#include <OpenGP/GL/Renderer.h>


//=============================================================================
namespace OpenGP {
//=============================================================================

class CameraComponent : public Component {
private:


public:

    CameraComponent() {
        require<TransformComponent>();
    }

};

//=============================================================================
} // OpenGP::
//=============================================================================
