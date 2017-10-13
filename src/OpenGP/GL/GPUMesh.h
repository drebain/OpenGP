// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

#include <OpenGP/GL/gl.h>
#include <OpenGP/GL/Shader.h>
#include <OpenGP/GL/Buffer.h>
#include <OpenGP/GL/VertexArrayObject.h>

#include <OpenGP/SurfaceMesh/SurfaceMesh.h>

//=============================================================================
namespace OpenGP {
//=============================================================================

class GPUMesh {
private:

    VertexArrayObject vao;

    std::unordered_map<std::string, std::unique_ptr<VectorArrayBuffer>> vbos;

    ElementArrayBuffer<unsigned int> triangles;

    int element_count = 0;

    GLenum mode = GL_TRIANGLES;

public:

    GPUMesh() {};

    GPUMesh(const SurfaceMesh &mesh) {

        init_from_mesh(mesh);

    }

    virtual ~GPUMesh() {}

    void init_from_mesh(const SurfaceMesh &mesh) {

        auto vpoints = mesh.get_vertex_property<Vec3>("v:point");
        set_vbo_raw<Vec3>("vposition", vpoints.data(), mesh.n_vertices());

        auto vnormals = mesh.get_vertex_property<Vec3>("v:normal");
        if (vnormals) {
            set_vbo_raw<Vec3>("vnormal", vnormals.data(), mesh.n_vertices());
        } else {
            void *uninitialized_data = malloc(mesh.n_vertices() * sizeof(Vec3));
            set_vbo_raw<Vec3>("vnormal", uninitialized_data, mesh.n_vertices());
            free(uninitialized_data);
        }

        // TODO: read texture coordinates
        {
            void *uninitialized_data = malloc(mesh.n_vertices() * sizeof(Vec2));
            set_vbo_raw<Vec2>("vtexcoord", uninitialized_data, mesh.n_vertices());
            free(uninitialized_data);
        }

        element_count = 0;
        std::vector<unsigned int> triangles;
        for(auto f: mesh.faces()) {
            for(auto v: mesh.vertices(f)) {
                element_count++;
                triangles.push_back(v.idx());
            }
        }

        vao.bind();
        this->triangles.upload(triangles);
        vao.unbind();

        mode = GL_TRIANGLES;

    }

    void set_mode(GLenum mode) {
        this->mode = mode;
    }

    template <typename T>
    void set_vbo(const std::string &name, const std::vector<T> &data) {
        set_vbo_raw<T>(name, &(data[0]), data.size());
    }

    template <typename T>
    void set_vbo_raw(const std::string &name, const void *data, GLsizeiptr num_elems) {

        auto *buffer = dynamic_cast<ArrayBuffer<T>*>(vbos[name].get());
        if (buffer == nullptr) {
            buffer = new ArrayBuffer<T>();
            vbos[name] = std::unique_ptr<VectorArrayBuffer>(buffer);
        }

        vao.bind();
        buffer->upload_raw(data, num_elems);
        vao.unbind();

    }

    void set_vpoint(const std::vector<Vec3> &vpoint) {
        set_vbo<Vec3>("vposition", vpoint);
    }

    void set_vnormal(const std::vector<Vec3> &vnormal) {
        set_vbo<Vec3>("vnormal", vnormal);
    }

    void set_vtexcoord(const std::vector<Vec2> &vtexcoord) {
        set_vbo<Vec2>("vtexcoord", vtexcoord);
    }

    void set_triangles(const std::vector<unsigned int> &triangles) {
        vao.bind();
        this->triangles.upload(triangles);
        vao.unbind();
        element_count = triangles.size();
    }

    void set_attributes(Shader &shader) {

        vao.bind();

        for (auto &pair : vbos) {
            auto name = pair.first.c_str();
            auto &buffer = *pair.second;
            shader.set_attribute(name, buffer);
        }

        vao.unbind();

    }

    void draw() {

        vao.bind();
        triangles.bind();

        glDrawElements(mode, element_count, GL_UNSIGNED_INT, 0);

        triangles.unbind();
        vao.unbind();

    }

};

//=============================================================================
} // namespace OpenGP
//=============================================================================
