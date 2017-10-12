// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <string>
#include <vector>
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

    std::unordered_map<std::string, GenericBuffer> vbos;

    ArrayBuffer<Vec3> vpoint;
    ArrayBuffer<Vec3> vnormal;
    ArrayBuffer<Vec2> vtexcoord;

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

        vao.bind();

        auto vpoints = mesh.get_vertex_property<Vec3>("v:point");
        //vpoint.upload_raw(vpoints.data(), mesh.n_vertices());
        set_vbo_raw("vpoint", vpoints.data(), mesh.n_vertices());

        auto vnormals = mesh.get_vertex_property<Vec3>("v:normal");
        if (vnormals) {
            //vnormal.upload_raw(vnormals.data(), mesh.n_vertices());
            set_vbo_raw("vnormal", vnormals.data(), mesh.n_vertices());
        } else {
            void *uninitialized_data = malloc(mesh.n_vertices() * sizeof(Vec3));
            //vnormal.upload_raw(uninitialized_data, mesh.n_vertices());
            set_vbo_raw("vnormal", uninitialized_data, mesh.n_vertices());
            free(uninitialized_data);
        }

        // TODO: read texture coordinates
        {
            void *uninitialized_data = malloc(mesh.n_vertices() * sizeof(Vec2));
            //vtexcoord.upload_raw(uninitialized_data, mesh.n_vertices());
            set_vbo_raw("vtexcoord", uninitialized_data, mesh.n_vertices());
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
        this->triangles.upload(triangles);

        vao.unbind();

        mode = GL_TRIANGLES;

    }

    void set_mode(GLenum mode) {
        this->mode = mode;
    }

    template <typename T>
    void set_vbo(const std::string &name, const std::vector<T> &data) {
        set_vbo_raw<T>(name, data.size(), &(data[0]));
    }

    template <typename T>
    void set_vbo_raw(const std::string &name, const T *data, GLsizeiptr num_elems) {

        auto *buffer = dynamic_cast<ArrayBuffer<T>*>(&(vbos[name]));

        assert(buffer != nullptr);
        vao.bind();
        buffer->upload_raw(vpoint);
        vao.unbind();

    }

    void set_vpoint(const std::vector<Vec3> &vpoint) {
        //vao.bind();
        //this->vpoint.upload(vpoint);
        //vao.unbind();
        set_vbo<Vec3>("vposition", vpoint);
    }

    void set_vnormal(const std::vector<Vec3> &vnormal) {
        //vao.bind();
        //this->vnormal.upload(vnormal);
        //vao.unbind();
        set_vbo<Vec3>("vnormal", vnormal);
    }

    void set_vnormal(const std::vector<Vec2> &vtexcoord) {
        //vao.bind();
        //this->vtexcoord.upload(vtexcoord);
        //vao.unbind();
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

        /*if (shader.has_attribute("vposition")) {
            shader.set_attribute("vposition", vpoint);
        }

        if (shader.has_attribute("vnormal")) {
            shader.set_attribute("vnormal", vnormal);
        }

        if (shader.has_attribute("vtexcoord")) {
            shader.set_attribute("vtexcoord", vtexcoord);
        }*/
        for (auto &pair : vbos) {
            std::string &name = pair.first;
            GenericBuffer &buffer = pair.second;
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
