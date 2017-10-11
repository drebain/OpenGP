// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "Material.h"


//=============================================================================
namespace OpenGP {
//=============================================================================

const char *Material::default_code() {
    return R"GLSL(

        uniform vec3 base_color;

        vec4 shade() {
            vec3 lightdir = -get_forward();
            float diffuse = clamp(abs(dot(get_normal(), normalize(lightdir))), 0, 1);
            vec3 ambient = vec3(0.1,0.11,0.13);

            return vec4(diffuse * base_color + ambient, 1);
        }

    )GLSL";
}

Material::Material() : Material(default_code()) {
    set_property("base_color", Vec3(0.6, 0.6, 0.6));
}

Material::Material(const std::string &code) {
    shading_code = code;
}

const std::string &Material::get_shading_code() const {
    return shading_code;
}

void Material::apply_properties(Shader &shader) const {
    for (auto &pair : properties) {
        pair.second(shader);
    }
}

//=============================================================================
} // OpenGP::
//=============================================================================
