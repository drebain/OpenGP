// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <string>

#include <OpenGP/headeronly.h>
#include <OpenGP/types.h>
#include <OpenGP/GL/Shader.h>
#include <OpenGP/GL/Material.h>

//=============================================================================
namespace OpenGP {
//=============================================================================

class RenderContext {
public:

    virtual ~RenderContext() {}

    virtual float aspect() const = 0;
    virtual float vfov() const = 0;

    virtual float near() const = 0;
    virtual float far() const = 0;

    virtual Vec3 eye() const = 0;
    virtual Vec3 forward() const = 0;
    virtual Vec3 up() const = 0;

    virtual Mat4x4 M() const = 0;
    virtual Mat4x4 V() const = 0;
    virtual Mat4x4 P() const = 0;
    virtual Mat4x4 MV() const { return V() * M(); }
    virtual Mat4x4 VP() const { return P() * V(); }
    virtual Mat4x4 MVP() const { return P() * V() * M(); }

    virtual bool wireframe() const { return false; }

};

class Renderer {
protected:

    Material material;

public:

    Renderer() {}

    virtual ~Renderer() {}

    virtual void render(const RenderContext&) = 0;

    virtual void rebuild() {}

    HEADERONLY_INLINE void set_material(const Material &material);

    HEADERONLY_INLINE static void build_shader(Shader &shader, const Material &material, const std::string &vshader, const std::string &fshader);

    HEADERONLY_INLINE static void update_shader(Shader &shader, const RenderContext &context);

};

//=============================================================================
} // OpenGP::
//=============================================================================

#ifdef HEADERONLY
    #include "Renderer.cpp"
#endif
