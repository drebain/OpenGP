// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <vector>

#include <OpenGP/util/Transform.h>
#include <OpenGP/util/GenericIterable.h>
#include <OpenGP/GL/Entity.h>


//=============================================================================
namespace OpenGP {
//=============================================================================

/// A component representing the position, orientation and scale of an entity
class TransformComponent : public Transform, public Component {
private:

    TransformComponent *parent = nullptr;

    std::vector<TransformComponent*> children;

public:

    Transform world() const {
        if (parent == nullptr) {
            return Transform(*this);
        }
        auto base = parent->world();
        base.apply_transformation(*this);
        return base;
    }

    TransformComponent *get_parent() {
        return parent;
    }

    const TransformComponent *get_parent() const {
        return parent;
    }

    GenericIterable<TransformComponent> get_children() {

        // TODO: add const version

        auto map_pred = [](TransformComponent *tc) {
            return tc;
        };

        return GenericIterable<TransformComponent*>::adaptor(children).map(map_pred);
    }

};

//=============================================================================
} // OpenGP::
//=============================================================================
