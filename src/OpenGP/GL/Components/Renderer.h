#pragma once

#include <string>

#include <OpenGP/headeronly.h>
#include <OpenGP/types.h>
#include <OpenGP/GL/Component.h>

//=============================================================================
namespace OpenGP {
//=============================================================================

class RenderContext {
public:

    virtual ~RenderContext() {}

    virtual Mat4x4 M() const = 0;
    virtual Mat4x4 V() const = 0;
    virtual Mat4x4 P() const = 0;
    virtual Mat4x4 MV() const { return V() * M(); }
    virtual Mat4x4 VP() const { return P() * V(); }
    virtual Mat4x4 MVP() const { return P() * V() * M(); }

    virtual bool wireframe() { return false; }

};

class Renderer : public Component {
protected:

    std::string shading_code;

public:

    HEADERONLY_INLINE Renderer();

    virtual ~Renderer() {}

    virtual void render(const RenderContext&) = 0;

};

//=============================================================================
} // OpenGP::
//=============================================================================
