// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "MaterialRenderer.h"


//=============================================================================
namespace OpenGP {
//=============================================================================

namespace {

    inline const char *global_uniforms() {
        return R"GLSL(

            uniform float _uniform_aspect;
            float get_aspect() {
                return _uniform_aspect;
            }

            uniform float _uniform_vfov;
            float get_vfov() {
                return _uniform_vfov;
            }

            uniform float _uniform_near;
            float get_near() {
                return _uniform_near;
            }

            uniform float _uniform_far;
            float get_far() {
                return _uniform_far;
            }

            uniform mat4 _uniform_M;
            mat4 get_M() {
                return _uniform_M;
            }

            uniform mat4 _uniform_V;
            mat4 get_V() {
                return _uniform_V;
            }

            uniform mat4 _uniform_P;
            mat4 get_P() {
                return _uniform_P;
            }

            uniform mat4 _uniform_MV;
            mat4 get_MV() {
                return _uniform_MV;
            }

            uniform mat4 _uniform_VP;
            mat4 get_VP() {
                return _uniform_VP;
            }

            uniform mat4 _uniform_MVP;
            mat4 get_MVP() {
                return _uniform_MVP;
            }

            uniform int _uniform_wireframe;
            int get_wireframe() {
                return _uniform_wireframe;
            }


        )GLSL";
    }

    inline const char *vshader_preamble() {
        return R"GLSL(

        )GLSL";
    }

    inline const char *base_vshader() {
        return R"GLSL(

            void main() {
                vertex();
            }

        )GLSL";
    }

    inline const char *fshader_preamble() {
        return R"GLSL(

            out vec4 _out_color;

            vec3 _fragment_position = vec3(0, 0, 0);
            void set_position(vec3 position) { _fragment_position = position; }
            vec3 get_position() { return _fragment_position; }

            vec3 _fragment_normal = vec3(0, 0, 0);
            void set_normal(vec3 normal) { _fragment_normal = normal; }
            vec3 get_normal() { return _fragment_normal; }

            float _fragment_wire_weight = 0;
            void set_wire_weight(float wire_weight) { _fragment_wire_weight = wire_weight; }
            float get_wire_weight() { return _fragment_wire_weight; }

        )GLSL";
    }

    inline const char *base_fshader() {
        return R"GLSL(

            void main() {
                fragment();
                _out_color = shade();
            }

        )GLSL";
    }

}

void MaterialRenderer::set_material(const Material &material) {
    this->material = material;
}

void MaterialRenderer::build_shader(Shader &shader, const Material &material, const std::string &vshader, const std::string &fshader) {

    shader.clear();

    std::string vshader_source = "#version 330 core\n";
    vshader_source += global_uniforms();
    vshader_source += vshader_preamble();
    vshader_source += vshader;
    vshader_source += base_vshader();

    std::string fshader_source = "#version 330 core\n";
    fshader_source += global_uniforms();
    fshader_source += fshader_preamble();
    fshader_source += material.get_shading_code();
    fshader_source += fshader;
    fshader_source += base_fshader();

    shader.add_vshader_from_source(vshader_source.c_str());
    shader.add_fshader_from_source(fshader_source.c_str());
    shader.link();

}

void MaterialRenderer::update_shader(Shader &shader, const RenderContext &context) {

    shader.set_uniform("_uniform_aspect", context.aspect);
    shader.set_uniform("_uniform_vfov", context.vfov);
    shader.set_uniform("_uniform_near", context.near);
    shader.set_uniform("_uniform_far", context.far);
    shader.set_uniform("_uniform_eye", context.eye);
    shader.set_uniform("_uniform_forward", context.forward);
    shader.set_uniform("_uniform_up", context.up);
    shader.set_uniform("_uniform_M", context.M);
    shader.set_uniform("_uniform_V", context.V);
    shader.set_uniform("_uniform_P", context.P);
    shader.set_uniform("_uniform_MV", context.MV);
    shader.set_uniform("_uniform_VP", context.VP);
    shader.set_uniform("_uniform_MVP", context.MVP);
    shader.set_uniform("_uniform_wireframe", context.wireframe);

}

//=============================================================================
} // OpenGP::
//=============================================================================