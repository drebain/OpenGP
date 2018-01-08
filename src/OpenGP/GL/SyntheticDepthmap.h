// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <vector>
#include <list>
#include <OpenGP/types.h>

#include <OpenGP/GL/Shader.h>
#include <OpenGP/GL/Buffer.h>
#include <OpenGP/GL/GPUMesh.h>
#include <OpenGP/GL/Framebuffer.h>
#include <OpenGP/GL/VertexArrayObject.h>
#include <OpenGP/GL/check_error_gl.h>


//=============================================================================
namespace OpenGP {
//=============================================================================

class DepthmapRenderer {
public:

    virtual void draw() = 0;

    virtual void draw_normals() { throw std::runtime_error("not implemented"); }

    virtual Shader &get_shader() = 0;

    virtual Shader &get_normals_shader() { throw std::runtime_error("not implemented"); }

};

class SyntheticDepthmap {
private:

    RGB32FTexture points;
    RGB32FTexture normals;
    D16Texture depth, normals_depth;

    Framebuffer framebuffer;
    Framebuffer normals_framebuffer;

    float *data;

    GLsizei width, height;

public:

    SyntheticDepthmap(GLsizei width, GLsizei height) : width(width), height(height) {

        points.allocate(width, height);
        normals.allocate(width, height);
        depth.allocate(width, height);
        normals_depth.allocate(width, height);

        framebuffer.attach_color_texture(points);
        framebuffer.attach_depth_texture(depth);

        normals_framebuffer.attach_color_texture(normals);
        normals_framebuffer.attach_depth_texture(normals_depth);

    }

    void clear() {

        framebuffer.bind();

        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        framebuffer.unbind();

    }

    void clear_normals() {

        normals_framebuffer.bind();

        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        normals_framebuffer.unbind();

    }

    void render(DepthmapRenderer &renderer, const Eigen::Matrix4f &M, const Eigen::Matrix4f &V, const Eigen::Matrix4f &P) {

        glViewport(0, 0, width, height);

        framebuffer.bind();
        renderer.get_shader().bind();

        renderer.get_shader().set_uniform("M", M);
        renderer.get_shader().set_uniform("V", V);
        renderer.get_shader().set_uniform("P", P);

        renderer.draw();

        renderer.get_shader().unbind();
        framebuffer.unbind();

    }

    void render_normals(DepthmapRenderer &renderer, const Eigen::Matrix4f &M, const Eigen::Matrix4f &V, const Eigen::Matrix4f &P) {

        glViewport(0, 0, width, height);

        normals_framebuffer.bind();
        renderer.get_normals_shader().bind();

        renderer.get_normals_shader().set_uniform("M", M);
        renderer.get_normals_shader().set_uniform("V", V);
        renderer.get_normals_shader().set_uniform("P", P);

        renderer.draw_normals();

        renderer.get_normals_shader().unbind();
        normals_framebuffer.unbind();

    }

    void download() {

        glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB32F, GL_FLOAT, data);

    }

    RGB32FTexture &get_texture() {

        return points;

    }

    RGB32FTexture &get_normals_texture() {

        return normals;

    }

    D16Texture &get_depth_texture() {

        return depth;

    }

    const float *cpu_data() const { return data; }

};

//=============================================================================
} // namespace OpenGP
//=============================================================================
