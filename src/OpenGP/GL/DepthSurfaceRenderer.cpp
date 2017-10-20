// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "DepthSurfaceRenderer.h"


//=============================================================================
namespace OpenGP {
//=============================================================================

// vshader code
const char *DepthSurfaceRenderer::vshader() {
    return R"GLSL(

        uniform sampler2D depth_texture;

        uniform mat4 sensor_matrix_inv;

        in vec3 vposition;

        out vec3 fposition;
        out vec3 fnormal;

        void vertex() {
            // TODO: deproject points and calculate normals

            vec2 uv = (vposition.xy + vec2(1, 1)) / 2;
            float depth = texture(depth_texture, uv).r;
            float depth_px = textureOffset(depth_texture, uv, ivec2(1, 0)).r;
            float depth_py = textureOffset(depth_texture, uv, ivec2(0, 1)).r;

            vec2 tex_size = vec2(textureSize(depth_texture, 0));

            vec3 v_center = vposition + vec3(0, 0, depth);
            vec3 v_px = vposition + vec3(1 / tex_size.x, 0, depth_px);
            vec3 v_py = vposition + vec3(0, 1 / tex_size.y, depth_py);

            gl_Position = get_MVP() * vec4(v_center, 1.0);
            fposition = (get_M() * vec4(vposition, 1.0)).xyz;
            vec3 vnormal = normalize(cross(v_px - v_center, v_py - v_center));
            fnormal = normalize( inverse(transpose(mat3(get_M()))) * vnormal );
        }

    )GLSL";
}

// fshader code
const char *DepthSurfaceRenderer::fshader() {
    return R"GLSL(

        in vec3 fnormal;
        in vec3 fposition;

        void fragment() {
            set_normal(normalize(fnormal));
            set_position(fposition);
            set_wire_weight(0);
        }

    )GLSL";
}

DepthSurfaceRenderer::DepthSurfaceRenderer() {
    rebuild();
}

void DepthSurfaceRenderer::rebuild_mesh() {

    std::vector<Vec3> vposition;
    std::vector<unsigned int> triangles;

    GLuint n = 0;
    for (GLuint i = 0;i < width;i++) {
        for (GLuint j = 0;j < height;j++) {

            float x = 2 * (float)i / (float)(width - 1) - 1;
            float y = 2 * (float)j / (float)(height - 1) - 1;
            Vec3 vert(x, y, 0);
            vposition.push_back(vert);
            if (i > 0 && j > 0) {
                GLuint v0 = n;
                GLuint v1 = v0 - 1;
                GLuint v2 = v0 - height;
                GLuint v3 = v1 - height;
                triangles.push_back(v0);
                triangles.push_back(v1);
                triangles.push_back(v2);
                triangles.push_back(v1);
                triangles.push_back(v2);
                triangles.push_back(v3);
            }
            n++;

        }
    }

    gpu_mesh.set_vbo<Vec3>("vposition", vposition);
    gpu_mesh.set_triangles(triangles);

}

void DepthSurfaceRenderer::set_depth_texture(const GenericTexture &texture) {

    if (texture.get_width() != width || texture.get_height() != height) {
        width = texture.get_width();
        height = texture.get_height();
        rebuild_mesh();
    }

    depth_texture = &texture;

}

void DepthSurfaceRenderer::set_sensor_matrix(const Mat4x4 &sensor_matrix) {
    this->sensor_matrix_inv = sensor_matrix.inverse();
    shader.bind();
    shader.set_uniform("sensor_matrix_inv", sensor_matrix_inv);
    shader.unbind();
}

void DepthSurfaceRenderer::render(const RenderContext &context) {

    shader.bind();

    glActiveTexture(GL_TEXTURE0);
    if (depth_texture) depth_texture->bind();

    gpu_mesh.set_attributes(shader);
    update_shader(shader, context);

    if (wireframe_mode == WireframeMode::WiresOnly)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    gpu_mesh.draw();

    if (wireframe_mode == WireframeMode::WiresOnly)
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    shader.unbind();
}

void DepthSurfaceRenderer::rebuild() {

    build_shader(shader, vshader(), fshader());

    shader.bind();
    gpu_mesh.set_attributes(shader);
    shader.set_uniform("depth_texture", 0);
    shader.set_uniform("sensor_matrix_inv", sensor_matrix_inv);
    shader.unbind();

}

GPUMesh &DepthSurfaceRenderer::get_gpu_mesh() {
    return gpu_mesh;
}

//=============================================================================
} // OpenGP::
//=============================================================================
