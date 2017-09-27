// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <OpenGP/external/imgui/imgui.h>

#include <OpenGP/GL/Shader.h>
#include <OpenGP/GL/VertexArrayObject.h>
#include <OpenGP/GL/Buffer.h>
#include <OpenGP/GL/Texture.h>


//=============================================================================
namespace OpenGP {
//=============================================================================

class ImguiRenderer {
private:

    const char* vshader = R"GLSL(
        #version 330 core

        uniform mat4 P;

        in vec2 vposition;
        in vec2 vtexcoord;
        in vec4 vcolor;

        out vec2 ftexcoord;
        out vec4 fcolor;

        void main() {
            gl_Position = P * vec4(vposition, 0, 1);
            ftexcoord = vtexcoord;
            fcolor = vcolor;
        }
    )GLSL";

    const char* fshader = R"GLSL(
        #version 330 core

        uniform sampler2D tex;

        in vec2 ftexcoord;
        in vec4 fcolor;

        out vec4 color_out;

        void main() {
            color_out = fcolor * texture2D(tex, ftexcoord);
        }
    )GLSL";

    Shader shader;

    RGBA8Texture texture;

    VertexArrayObject vao;

    ArrayBuffer<ImDrawVert> vbuffer;

    ElementArrayBuffer<ImDrawIdx> elements;

    int width, height;

public:

    ImguiRenderer() {
        shader.add_vshader_from_source(vshader);
        shader.add_fshader_from_source(fshader);
        shader.link();

        shader.bind();
        vao.bind();
        vbuffer.bind();

        GLuint position_attrib_location = glGetAttribLocation(shader.programId(), "vposition");
        GLuint uv_attrib_location = glGetAttribLocation(shader.programId(), "vtexcoord");
        GLuint color_attrib_location = glGetAttribLocation(shader.programId(), "vcolor");

        glEnableVertexAttribArray(position_attrib_location);
        glEnableVertexAttribArray(uv_attrib_location);
        glEnableVertexAttribArray(color_attrib_location);

#define OFFSETOF(TYPE, ELEMENT) (GLvoid*)((size_t)&(((TYPE *)0)->ELEMENT))
        glVertexAttribPointer(position_attrib_location, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), OFFSETOF(ImDrawVert, pos));
        glVertexAttribPointer(uv_attrib_location, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), OFFSETOF(ImDrawVert, uv));
        glVertexAttribPointer(color_attrib_location, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(ImDrawVert), OFFSETOF(ImDrawVert, col));
#undef OFFSETOF

        vbuffer.unbind();
        vao.unbind();
        shader.unbind();

        uint8_t *tex_data;
        int width, height;
        ImGui::GetIO().Fonts->GetTexDataAsRGBA32(&tex_data, &width, &height);

        texture.upload_raw(width, height, tex_data);
    }

    void begin_frame(int width, int height) {

        this->width = width;
        this->height = height;

        ImGuiIO &io = ImGui::GetIO();

        io.DisplaySize = ImVec2(width, height);
        io.DisplayFramebufferScale = ImVec2(1, 1);

        io.DeltaTime = 0.1;

        ImGui::NewFrame();

    }

    void end_frame() {

        ImGui::Render();

        auto draw_data = ImGui::GetDrawData();

        draw_data->ScaleClipRects(ImGui::GetIO().DisplayFramebufferScale);

        glEnable(GL_BLEND);
        glBlendEquation(GL_FUNC_ADD);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDisable(GL_CULL_FACE);
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_SCISSOR_TEST);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glViewport(0, 0, width, height);

        Mat4x4 P;
        P << 2.0f/width, 0,            0,  -1,
             0,          -2.0f/height, 0,  1,
             0,          0,            -1, 0,
             0,          0,            0,  1;

        shader.bind();
        shader.set_uniform("tex", 0);
        shader.set_uniform("P", P);
        glBindSampler(0, 0);

        vao.bind();

        for (int i = 0;i < draw_data->CmdListsCount;i++) {

            auto cmd_list = draw_data->CmdLists[i];
            const ImDrawIdx *idx_buffer_offset = 0;

            vbuffer.bind();
            vbuffer.upload_raw(cmd_list->VtxBuffer.Data, cmd_list->VtxBuffer.Size, GL_STREAM_DRAW);

            elements.bind();
            elements.upload_raw(cmd_list->IdxBuffer.Data, cmd_list->IdxBuffer.Size, GL_STREAM_DRAW);

            for (int j = 0;j < cmd_list->CmdBuffer.Size;j++) {

                auto &cmd = cmd_list->CmdBuffer[j];

                if (cmd.UserCallback) {

                    cmd.UserCallback(cmd_list, &cmd);

                } else {

                    texture.bind();

                    glScissor((int)cmd.ClipRect.x, (int)(height - cmd.ClipRect.w), (int)(cmd.ClipRect.z - cmd.ClipRect.x), (int)(cmd.ClipRect.w - cmd.ClipRect.y));
                    glDrawElements(GL_TRIANGLES, (GLsizei)cmd.ElemCount, sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, idx_buffer_offset);

                    texture.unbind();

                }
                idx_buffer_offset += cmd.ElemCount;

            }

            elements.unbind();
            vbuffer.unbind();


        }

        vao.unbind();
        shader.unbind();

        glDisable(GL_SCISSOR_TEST);

    }

};

//=============================================================================
} // OpenGP::
//=============================================================================
