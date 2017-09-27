#pragma once

#include <OpenGP/GL/MaterialRenderer.h>
#include <OpenGP/GL/GPUMesh.h>
#include <OpenGP/SurfaceMesh/SurfaceMesh.h>


//=============================================================================
namespace OpenGP {
//=============================================================================

class SurfaceMeshRenderer : public MaterialRenderer {
private:

    GPUMesh gpu_mesh;

    Shader shader;

public:

    HEADERONLY_INLINE SurfaceMeshRenderer();

    virtual ~SurfaceMeshRenderer() {}

    HEADERONLY_INLINE void render(const RenderContext&);

    HEADERONLY_INLINE void rebuild();

    HEADERONLY_INLINE void upload_mesh(const SurfaceMesh &mesh);

};

//=============================================================================
} // OpenGP::
//=============================================================================

#ifdef HEADERONLY
    #include "SurfaceMeshRenderer.cpp"
#endif
