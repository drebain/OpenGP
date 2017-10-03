// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <OpenGP/GL/Entity.h>
#include <OpenGP/GL/ImguiRenderer.h>


//=============================================================================
namespace OpenGP {
//=============================================================================

class GUICanvasComponent : public Component {
private:

    ImguiRenderer renderer;

public:

    void set_window() {}

};

//=============================================================================
} // OpenGP::
//=============================================================================
