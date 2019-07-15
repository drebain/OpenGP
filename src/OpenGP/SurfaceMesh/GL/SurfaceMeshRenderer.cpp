// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "SurfaceMeshRenderer.h"


//=============================================================================
namespace OpenGP {
//=============================================================================

// vshader code
const char *SurfaceMeshRenderer::vshader() {
    return R"GLSL(

        uniform sampler2D bones;
        uniform int use_skinning;
        uniform int n_bones;

        in vec3 vposition;
        in vec3 vnormal;
        in vec2 vtexcoord;

        in vec4 vweights;
        in ivec4 vbones;

        out vec3 gposition;
        out vec3 gnormal;
        out vec2 gtexcoord;

        mat4 get_bone_transform(int i_in, int j_in) {
            int i = i_in * 4;
            int j = j_in * 4;
            mat4 result;

            result[0][0] = texelFetch(bones, ivec2(i + 0, j + 0), 0).r;
            result[1][0] = texelFetch(bones, ivec2(i + 0, j + 1), 0).r;
            result[2][0] = texelFetch(bones, ivec2(i + 0, j + 2), 0).r;
            result[3][0] = texelFetch(bones, ivec2(i + 0, j + 3), 0).r;

            result[0][1] = texelFetch(bones, ivec2(i + 1, j + 0), 0).r;
            result[1][1] = texelFetch(bones, ivec2(i + 1, j + 1), 0).r;
            result[2][1] = texelFetch(bones, ivec2(i + 1, j + 2), 0).r;
            result[3][1] = texelFetch(bones, ivec2(i + 1, j + 3), 0).r;
                
            result[0][2] = texelFetch(bones, ivec2(i + 2, j + 0), 0).r;
            result[1][2] = texelFetch(bones, ivec2(i + 2, j + 1), 0).r;
            result[2][2] = texelFetch(bones, ivec2(i + 2, j + 2), 0).r;
            result[3][2] = texelFetch(bones, ivec2(i + 2, j + 3), 0).r;
                
            result[0][3] = texelFetch(bones, ivec2(i + 3, j + 0), 0).r;
            result[1][3] = texelFetch(bones, ivec2(i + 3, j + 1), 0).r;
            result[2][3] = texelFetch(bones, ivec2(i + 3, j + 2), 0).r;
            result[3][3] = texelFetch(bones, ivec2(i + 3, j + 3), 0).r;

            return result;
        }

        void vertex() {
            mat4 bone_transform = mat4(0);
            if (bool(use_skinning)) {
                int offset = n_bones * gl_InstanceID;
                bone_transform += vweights.x * get_bone_transform(offset + vbones.x, 0) * get_bone_transform(offset + vbones.x, 1);
                bone_transform += vweights.y * get_bone_transform(offset + vbones.y, 0) * get_bone_transform(offset + vbones.y, 1);
                bone_transform += vweights.z * get_bone_transform(offset + vbones.z, 0) * get_bone_transform(offset + vbones.z, 1);
                bone_transform += vweights.w * get_bone_transform(offset + vbones.w, 0) * get_bone_transform(offset + vbones.w, 1);

            } else {
                bone_transform = mat4(1, 0, 0, 0,
                                      0, 1, 0, 0,
                                      0, 0, 1, 0,
                                      0, 0, 0, 1);
            }
            gl_Position = get_MVP() * bone_transform * vec4(vposition, 1.0);
            mat4 final_m = get_M() * bone_transform;
            gposition = (final_m * vec4(vposition, 1.0)).xyz;
            gnormal = normalize( inverse(transpose(mat3(final_m))) * vnormal);
            gtexcoord = vtexcoord;
        }

    )GLSL";
}

// gshader code
const char *SurfaceMeshRenderer::gshader() {
    return R"GLSL(

        // reddit.com/r/opengl/comments/34dhi7/wireframe_shader/cquax7r

        layout (triangles) in;
        layout (triangle_strip, max_vertices = 3) out;

        in vec3 gnormal[];
        in vec3 gposition[];
        in vec2 gtexcoord[];

        out vec3 fnormal;
        out vec3 fposition;
        out vec2 ftexcoord;

        noperspective out vec3 fwireframe;

        void geometry() {
            for (int i = 0;i < 3;i++) {
                gl_Position = gl_in[i].gl_Position;
                fnormal = gnormal[i];
                ftexcoord = gtexcoord[i];
                fposition = gposition[i];
                fwireframe = vec3(0, 0, 0);
                fwireframe[i] = 1;
                geometry_vertex_shade(i);
                EmitVertex();
            }
        }

    )GLSL";
}

// fshader code
const char *SurfaceMeshRenderer::fshader() {
    return R"GLSL(

        in vec3 fnormal;
        in vec3 fposition;
        in vec2 ftexcoord;
        noperspective in vec3 fwireframe;

        // http://codeflow.org/entries/2012/aug/02/easy-wireframe-display-with-barycentric-coordinates
        float edgeFactor(){
            vec3 d = 1.5 * fwidth(fwireframe);
            vec3 a3 = smoothstep(vec3(0.0), d, fwireframe);
            return 1.0f - min(min(a3.x, a3.y), a3.z);
        }

        void fragment() {
            set_normal(normalize(fnormal));
            set_position(fposition);
            set_texcoord(ftexcoord);
            set_wire_weight(edgeFactor());
        }

    )GLSL";
}

SurfaceMeshRenderer::SurfaceMeshRenderer() {
    rebuild();
}

void SurfaceMeshRenderer::render(const RenderContext &context) {

    shader.bind();
    shader.set_uniform("use_skinning", skinning);

    gpu_mesh.set_attributes(shader);
    update_shader(shader, context);

    if (wireframe_mode == WireframeMode::WiresOnly)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    if (!depth_test)
        glDisable(GL_DEPTH_TEST);

    if (skinning) {
        glActiveTexture(GL_TEXTURE0);
        bone_texture.bind();
    }

    if (instancing.enabled)
        gpu_mesh.draw_instanced(instancing.count);
    else
        gpu_mesh.draw();

    if (skinning)
        bone_texture.unbind();

    if (!depth_test)
        glEnable(GL_DEPTH_TEST);

    if (wireframe_mode == WireframeMode::WiresOnly)
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    shader.unbind();
}

void SurfaceMeshRenderer::rebuild() {

    build_shader(shader, vshader(), fshader(), gshader());

    shader.bind();
    gpu_mesh.set_attributes(shader);
    shader.unbind();

}

void SurfaceMeshRenderer::upload_mesh(const SurfaceMesh &mesh) {

    skinning = false;

    gpu_mesh.init_from_mesh(mesh);

    shader.bind();
    gpu_mesh.set_attributes(shader);
    shader.unbind();

}

void SurfaceMeshRenderer::upload_mesh(const SurfaceMesh &mesh, const std::vector<Mat4x4>& init_transforms) {
    upload_mesh(mesh, { init_transforms });
}

void SurfaceMeshRenderer::upload_mesh(const SurfaceMesh &mesh, const std::vector<std::vector<Mat4x4>>& init_transforms) {

    instancing.count = init_transforms.size();
    assert(instancing.count > 0);

    if (instancing.count == 1) {
        instancing.enabled = false;
    } else {
        instancing.enabled = true;
    }

    skinning = true;

    inverse_bind_poses.resize(instancing.count);
    for (int i = 0; i < instancing.count; ++i) {
        for (auto& t : init_transforms[i]) {
            inverse_bind_poses[i].push_back(t.inverse());
        }
    }

    gpu_mesh.init_from_mesh(mesh);

    gpu_mesh.set_vbo<Vec4>("vweights", mesh.get_vertex_property<Vec4>("v:skinweight").vector());
    gpu_mesh.set_vbo<Vec4i>("vbones", mesh.get_vertex_property<Vec4i>("v:boneid").vector());

    shader.bind();
    gpu_mesh.set_attributes(shader);
    shader.set_uniform("bones", 0);
    shader.set_uniform("n_bones", int(init_transforms[0].size()));
    shader.unbind();

    upload_transforms(init_transforms);

}

void SurfaceMeshRenderer::upload_transforms(const std::vector<Mat4x4>& transforms) {
    upload_transforms({ transforms });
}

void SurfaceMeshRenderer::upload_transforms(const std::vector<std::vector<Mat4x4>>& transforms) {

    assert(transforms.size() == instancing.count);

    int n_bones = inverse_bind_poses[0].size();
    assert(n_bones == transforms[0].size());
    Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic> bone_img(4 * n_bones * instancing.count, 8);

    bone_img.setZero();

    for (int j = 0; j < instancing.count; ++j) {
        for (int i = 0; i < n_bones; ++i) {
            bone_img.block<4, 4>((j * n_bones + i) * 4, 0) = transforms[j][i];
            bone_img.block<4, 4>((j * n_bones + i) * 4, 4) = inverse_bind_poses[j][i];
        }
    }

    bone_texture.upload(bone_img.transpose());

}

GPUMesh &SurfaceMeshRenderer::get_gpu_mesh() {
    return gpu_mesh;
}

//=============================================================================
} // OpenGP::
//=============================================================================
