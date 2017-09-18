#include "Material.h"


//=============================================================================
namespace OpenGP {
//=============================================================================

namespace {

    const static char *default_code = R"GLSL(

        vec4 shade() {
            return vec4(1, 0, 0, 1);
        }

    )GLSL";

}

Material::Material() : Material(default_code) {}

Material::Material(const std::string &code) {
    shading_code = code;
}

const std::string &Material::get_shading_code() const {
    return shading_code;
}

//=============================================================================
} // OpenGP::
//=============================================================================
