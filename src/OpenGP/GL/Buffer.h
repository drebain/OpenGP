// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once
#include <OpenGP/GL/gl.h>
#include <vector>

//=============================================================================
namespace OpenGP {
//=============================================================================

class GenericBuffer {
protected:

    GLenum target;
    GLuint buffer = 0;     ///< 0: invalid
    GLsizeiptr num_elems = 0; ///< # of uploaded elements
    GLsizeiptr elem_size; ///< size of a single elements (bytes)

public:

    virtual ~GenericBuffer() {}

    void bind() { glBindBuffer(target, buffer); }
    void unbind() { glBindBuffer(target, 0); }
    GLsizeiptr size() const { return num_elems; }

    GLenum get_target() const { return target; }
    GLenum get_data_type() const {}
    GLuint get_components() const {}

};

template <GLenum TARGET, class T>
class Buffer : public GenericBuffer {
public:

    Buffer() {
        elem_size = sizeof(T);
        target = TARGET;
        glGenBuffers(1, &buffer);
    }

    Buffer(const Buffer&) = delete;
    Buffer &operator=(const Buffer&) = delete;

    ~Buffer(){ glDeleteBuffers(1, &buffer); }

    void upload(const std::vector<T>& data, GLenum usage=GL_STATIC_DRAW){
        this->num_elems = data.size();
        upload_raw(data.data(), num_elems, usage);
    }

    /// @note use the other upload functions whenever possible
    void upload_raw(const GLvoid* raw_data_ptr, GLsizeiptr num_elems, GLenum usage=GL_STATIC_DRAW){
        this->num_elems = num_elems;
        glBindBuffer(TARGET, buffer);
        glBufferData(TARGET, num_elems * elem_size, raw_data_ptr, usage);
    }
};

///--- Specializations

template <class T>
using ArrayBuffer = Buffer<GL_ARRAY_BUFFER, T>;

template <class T>
using ElementArrayBuffer = Buffer<GL_ELEMENT_ARRAY_BUFFER, T>;

//=============================================================================
} // namespace OpenGP
//=============================================================================
