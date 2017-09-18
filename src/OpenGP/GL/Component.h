#pragma once


//=============================================================================
namespace OpenGP {
//=============================================================================

class Entity;

class Component {

    friend class Entity;

private:

    Entity *entity = nullptr;

protected:

    Component() {}

public:

    Component(const Component&) = delete;
    Component &operator=(const Component&) = delete;

    virtual void init() {}

    virtual void update() {}

};

//=============================================================================
} // OpenGP::
//=============================================================================
