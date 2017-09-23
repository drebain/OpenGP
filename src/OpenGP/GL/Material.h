// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <string>

#include <OpenGP/types.h>
#include <OpenGP/headeronly.h>

//=============================================================================
namespace OpenGP{
//=============================================================================

class Material {
private:

    std::string shading_code;

public:

    HEADERONLY_INLINE Material();
    HEADERONLY_INLINE Material(const std::string &code);

    HEADERONLY_INLINE const std::string &get_shading_code() const;

};

//=============================================================================
} // OpenGP::
//=============================================================================

#ifdef HEADERONLY
    #include "Material.cpp"
#endif
