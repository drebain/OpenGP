#include "SurfaceMeshRenderer.h"


//=============================================================================
namespace OpenGP {
//=============================================================================

namespace {

    // vshader code
    const static char *vshader = R"GLSL(

        in vec3 vbaryc;      ///< per-vertex barycentric
        in vec3 vposition;   ///< per-vertex position
        in vec3 vnormal;     ///< per-vertex normal

        out vec3 fbaryc;     ///< per-fragment barycentric
        out vec3 fposition;  ///< per-fragment position
        out vec3 fnormal;    ///< per-fragment normal

        void vertex() {
            gl_Position = get_MVP() * vec4(vposition, 1.0);
            fposition = (get_M() * vec4(vposition, 1.0)).xyz;
            fnormal = normalize( inverse(transpose(mat3(get_MV()))) * vnormal );
            fbaryc = vbaryc;
        }

    )GLSL";

    // fshader code
    const static char *fshader = R"GLSL(

        in vec3 fnormal;
        in vec3 fbaryc;
        in vec3 fposition;

        // http://codeflow.org/entries/2012/aug/02/easy-wireframe-display-with-barycentric-coordinates
        float edgeFactor(){
            vec3 d = 1.5 * fwidth(fbaryc);
            vec3 a3 = smoothstep(vec3(0.0), d, fbaryc);
            return min(min(a3.x, a3.y), a3.z);
        }

        void fragment() {
            set_normal(normalize(fnormal));
            set_position(fposition);
            set_wire_weight(edgeFactor());
        }

    )GLSL";

}

SurfaceMeshRenderer::SurfaceMeshRenderer() {
    rebuild();
}

void SurfaceMeshRenderer::render(const RenderContext &context) {

    shader.bind();

    gpu_mesh.set_attributes(shader);
    update_shader(shader, context);

    gpu_mesh.draw();

    shader.unbind();
}

void SurfaceMeshRenderer::rebuild() {

    build_shader(shader, material, vshader, fshader);

    shader.bind();
    gpu_mesh.set_attributes(shader);
    shader.unbind();

}

void SurfaceMeshRenderer::upload_mesh(const SurfaceMesh &mesh) {

    gpu_mesh.init_from_mesh(mesh);

    shader.bind();
    gpu_mesh.set_attributes(shader);
    shader.unbind();

}

GPUMesh &SurfaceMeshRenderer::get_gpu_mesh() {
    return gpu_mesh;
}

//=============================================================================
} // OpenGP::
//=============================================================================
