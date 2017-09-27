// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "Material.h"


//=============================================================================
namespace OpenGP {
//=============================================================================

namespace {

    inline const char *default_code() {
        return R"GLSL(

            vec4 shade() {
                vec3 base_color = 0.6 * vec3(1,1,1);
                float diffuse = clamp(abs(dot(get_normal(), normalize(vec3(1,1,1)))), 0, 1);
                vec3 ambient = vec3(0.1,0.11,0.13);

                return vec4(diffuse * base_color + ambient, 1);
            }

        )GLSL";
    }

}

Material::Material() : Material(default_code()) {}

Material::Material(const std::string &code) {
    shading_code = code;
}

const std::string &Material::get_shading_code() const {
    return shading_code;
}

//=============================================================================
} // OpenGP::
//=============================================================================
