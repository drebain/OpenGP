#pragma once

#include <OpenGP/GL/Components/Renderer.h>
#include <OpenGP/SurfaceMesh/SurfaceMesh.h>


//=============================================================================
namespace OpenGP {
//=============================================================================

class SurfaceMeshRenderer : public Renderer {
private:



public:

    HEADERONLY_INLINE SurfaceMeshRenderer() {}

    virtual ~SurfaceMeshRenderer() {}

    void render(const RenderContext&) {}

};

//=============================================================================
} // OpenGP::
//=============================================================================
