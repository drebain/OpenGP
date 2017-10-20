// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <OpenGP/GL/MaterialRenderer.h>
#include <OpenGP/GL/GPUMesh.h>


//=============================================================================
namespace OpenGP {
//=============================================================================

class DepthSurfaceRenderer : public MaterialRenderer {
private:

    GPUMesh gpu_mesh;

    Shader shader;

    const GenericTexture *depth_texture = nullptr;

    GLuint width = 0, height = 0;

    HEADERONLY_INLINE static const char *vshader();
    HEADERONLY_INLINE static const char *fshader();

    HEADERONLY_INLINE void rebuild_mesh();

public:

    HEADERONLY_INLINE DepthSurfaceRenderer();

    virtual ~DepthSurfaceRenderer() {}

    HEADERONLY_INLINE void set_depth_texture(const GenericTexture &texture);

    HEADERONLY_INLINE void render(const RenderContext&);

    HEADERONLY_INLINE void rebuild();

    HEADERONLY_INLINE GPUMesh &get_gpu_mesh();

};

//=============================================================================
} // OpenGP::
//=============================================================================

#ifdef HEADERONLY
    #include "DepthSurfaceRenderer.cpp"
#endif
