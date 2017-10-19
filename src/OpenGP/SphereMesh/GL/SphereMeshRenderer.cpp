// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <algorithm>

#include "SphereMeshRenderer.h"


//=============================================================================
namespace OpenGP {
//=============================================================================

const char *SphereMeshRenderer::sphere_vshader() {
    return R"GLSL(

        in vec4 vsphere;

        in vec3 vposition;
        in vec3 vnormal;

        out vec3 fposition;
        out vec3 fnormal;

        void vertex() {
            vec3 real_pos = vposition * vsphere.w + vsphere.xyz;
            gl_Position = get_MVP() * vec4(real_pos, 1.0);
            fposition = (get_M() * vec4(real_pos, 1.0)).xyz;
            fnormal = normalize( inverse(transpose(mat3(get_M()))) * vnormal );
        }

    )GLSL";
}

const char *SphereMeshRenderer::sphere_fshader() {
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

const char *SphereMeshRenderer::cone_vshader() {
    return R"GLSL(

        uniform mat4 cone_matrix;
        uniform float cone_ratio;
        uniform float cone_angle;

        in vec3 vposition;
        in vec3 vnormal;

        out vec3 fposition;
        out vec3 fnormal;

        /// http://www.neilmendoza.com/glsl-rotation-about-an-arbitrary-axis/
        mat3 rotationMatrix(vec3 axis, float angle) {
            axis = normalize(axis);
            float s = sin(angle);
            float c = cos(angle);
            float oc = 1.0 - c;

            return mat3(oc * axis.x * axis.x + c,           oc * axis.x * axis.y - axis.z * s,  oc * axis.z * axis.x + axis.y * s,
                        oc * axis.x * axis.y + axis.z * s,  oc * axis.y * axis.y + c,           oc * axis.y * axis.z - axis.x * s,
                        oc * axis.z * axis.x - axis.y * s,  oc * axis.y * axis.z + axis.x * s,  oc * axis.z * axis.z + c);
        }

        void vertex() {
            vec3 real_pos = vposition;
            real_pos.x *= 1 + real_pos.z * (cone_ratio - 1);
            real_pos.y *= 1 + real_pos.z * (cone_ratio - 1);
            gl_Position = get_MVP() * cone_matrix * vec4(real_pos, 1.0);
            fposition = (get_M() * vec4(vposition, 1.0)).xyz;

            vec3 axis = cross(vnormal, vec3(0, 0, 1));
            mat3 rot = rotationMatrix(axis, cone_angle);
            fnormal = normalize( inverse(transpose(mat3(get_M() * cone_matrix))) * rot * vnormal );
        }

    )GLSL";
}

const char *SphereMeshRenderer::cone_fshader() {
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

const char *SphereMeshRenderer::triangle_vshader() {
    return R"GLSL(

        uniform vec3 v0;
        uniform vec3 v1;
        uniform vec3 v2;
        uniform vec3 vn;

        in vec3 vposition; // Actually barycentric coordinates: not position

        out vec3 fposition;
        out vec3 fnormal;

        void vertex() {
            vec3 real_pos = vposition.x * v0 + vposition.y * v1 + vposition.z * v2;
            gl_Position = get_MVP() * vec4(real_pos, 1.0);
            fposition = (get_M() * vec4(real_pos, 1.0)).xyz;
            fnormal = normalize( inverse(transpose(mat3(get_M()))) * vn );
        }

    )GLSL";
}

const char *SphereMeshRenderer::triangle_fshader() {
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

SphereMeshRenderer::SphereMeshRenderer() {

    int segments = 64;

    { // Sphere generation
        std::vector<Vec3> vpoint;
        std::vector<Vec3> vnormal;
        std::vector<unsigned int> triangles;

        int n = 0;
        for (int i = 0;i <= segments;i++) {
            for (int j = 0;j <= segments;j++) {
                Scalar lon = 2 * PI * ((Scalar)i) / segments;
                Scalar lat = PI * (((Scalar)j) / segments - 0.5);
                Vec3 vert(std::cos(lon) * std::cos(lat), std::sin(lon) * std::cos(lat), std::sin(lat));
                vpoint.push_back(vert);
                vnormal.push_back(vert);
                if (i > 0 && j > 0) {
                    int v0 = n;
                    int v1 = v0 - 1;
                    int v2 = v0 - segments - 1;
                    int v3 = v1 - segments - 1;
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

        sphere.set_vpoint(vpoint);
        sphere.set_vnormal(vnormal);
        sphere.set_triangles(triangles);
    }

    { // Cone generation
        std::vector<Vec3> vpoint;
        std::vector<Vec3> vnormal;
        std::vector<unsigned int> triangles;

        for (int i = 0;i <= segments;i++) {
            Scalar theta = 2 * PI * ((Scalar)i) / segments;
            Vec3 vert(std::cos(theta), std::sin(theta), 0);
            vpoint.push_back(vert);
            vnormal.push_back(vert);
            vpoint.push_back(vert + Vec3(0, 0, 1));
            vnormal.push_back(vert);
            if (i > 0) {
                int v0 = 1 + 2 * i;
                int v1 = v0 - 1;
                int v2 = v0 - 2;
                int v3 = v0 - 3;
                triangles.push_back(v0);
                triangles.push_back(v1);
                triangles.push_back(v2);
                triangles.push_back(v1);
                triangles.push_back(v2);
                triangles.push_back(v3);
            }
        }

        cone.set_vpoint(vpoint);
        cone.set_vnormal(vnormal);
        cone.set_triangles(triangles);
    }

    { // Triangle generation
        std::vector<Vec3> vpoint;
        std::vector<Vec3> vnormal;
        std::vector<unsigned int> triangles;

        vpoint.push_back(Vec3(1, 0, 0));
        vnormal.push_back(Vec3(0, 0, 1));

        vpoint.push_back(Vec3(0, 1, 0));
        vnormal.push_back(Vec3(0, 0, 1));

        vpoint.push_back(Vec3(0, 0, 1));
        vnormal.push_back(Vec3(0, 0, 1));

        triangles.push_back(0);
        triangles.push_back(1);
        triangles.push_back(2);

        triangle.set_vpoint(vpoint);
        triangle.set_vnormal(vnormal);
        triangle.set_triangles(triangles);
    }

    rebuild();
}

void SphereMeshRenderer::render(const RenderContext &context) {

    auto vpoint = mesh.get_vertex_property<Vec4>("v:point");

    sphere_shader.bind();

    sphere.set_attributes(sphere_shader);
    update_shader(sphere_shader, context);

    sphere.draw_instanced(sphere_count);

    sphere_shader.unbind();

    cone_shader.bind();

    cone.set_attributes(cone_shader);
    update_shader(cone_shader, context);

    auto draw_cone = [&](SphereMesh::Vertex v0, SphereMesh::Vertex v1) {

        Vec4 s0 = vpoint[v0];
        Vec4 s1 = vpoint[v1];
        Vec3 p0 = s0.block<3, 1>(0, 0);
        Vec3 p1 = s1.block<3, 1>(0, 0);
        Scalar r0 = s0(3);
        Scalar r1 = s1(3);

        Scalar b = std::asin((r1 - r0) / (p1 - p0).norm());

        Vec2 tangent = pill_tangent(s0, s1);
        Scalar alpha = tangent(1);
        Scalar beta = tangent(0);

        Scalar angle = std::acos(Vec3(0, 0, 1).dot((p1 - p0).normalized()));
        Vec3 axis = Vec3(0, 0, 1).cross(p1 - p0).normalized();
        Mat4x4 r = Eigen::Affine3f(Eigen::AngleAxis<Scalar>(angle, axis)).matrix();

        Mat4x4 s = Mat4x4::Identity();
        Vec3 o;
        s(0, 0) = s(1, 1) = r0 * beta;
        if (r1 > r0) {
            s(2, 2) = (p1 - p0).norm() + alpha * (r0 - r1);
            o = -alpha * r0 * (p1 - p0).normalized();
        } else {
            s(2, 2) = (p1 - p0).norm() + alpha * (r1 - r0);
            o = alpha * r0 * (p1 - p0).normalized();
        }

        Mat4x4 t = Eigen::Affine3f(Eigen::Translation3f(p0 + o)).matrix();

        Scalar ratio = r1 / r0;

        Mat4x4 M = t * r * s;

        cone_shader.set_uniform("cone_matrix", M);
        cone_shader.set_uniform("cone_ratio", ratio);
        cone_shader.set_uniform("cone_angle", b);

        cone.draw();

    };

    for (auto e : mesh.edges()) {
        draw_cone(mesh.vertex(e, 0), mesh.vertex(e, 1));
    }

    for (auto f : mesh.faces()) {
        draw_cone(mesh.vertex(f, 0), mesh.vertex(f, 1));
        draw_cone(mesh.vertex(f, 1), mesh.vertex(f, 2));
        draw_cone(mesh.vertex(f, 2), mesh.vertex(f, 0));
    }

    cone_shader.unbind();

    triangle_shader.bind();

    triangle.set_attributes(triangle_shader);
    update_shader(triangle_shader, context);

    auto draw_triangles = [&](SphereMesh::Vertex v0, SphereMesh::Vertex v1, SphereMesh::Vertex v2) {

        Vec4 s0 = vpoint[v0];
        Vec4 s1 = vpoint[v1];
        Vec4 s2 = vpoint[v2];
        Vec3 p0 = s0.block<3, 1>(0, 0);
        Vec3 p1 = s1.block<3, 1>(0, 0);
        Vec3 p2 = s2.block<3, 1>(0, 0);
        Scalar r0 = s0(3);
        Scalar r1 = s1(3);
        Scalar r2 = s2(3);

        Vec3 sn = (p2 - p0).cross(p1 - p0).normalized();

        Vec2 ta, tb;
        Vec3 ms_tangent_a, ms_tangent_b, tn;
        ta = pill_tangent(s0, s1);
        tb = pill_tangent(s0, s2);
        ms_tangent_a = ta(0) * (p1 - p0).normalized() - ta(1) * sn;
        ms_tangent_b = tb(0) * (p2 - p0).normalized() - tb(1) * sn;
        tn = ms_tangent_a.cross(ms_tangent_b).normalized();

        // tangent point 0
        Vec3 t0 = p0 + r0 * tn;

        // tangent point 1
        Vec3 t1 = p1 + r1 * tn;

        // tangent point 2
        Vec3 t2 = p2 + r2 * tn;

        triangle_shader.set_uniform("v0", t0);
        triangle_shader.set_uniform("v1", t1);
        triangle_shader.set_uniform("v2", t2);
        triangle_shader.set_uniform("vn", tn);

        triangle.draw();

        ms_tangent_a = ta(0) * (p1 - p0).normalized() + ta(1) * sn;
        ms_tangent_b = tb(0) * (p2 - p0).normalized() + tb(1) * sn;
        tn = ms_tangent_b.cross(ms_tangent_a).normalized();

        // tangent point 0
        t0 = p0 + r0 * tn;

        // tangent point 1
        t1 = p1 + r1 * tn;

        // tangent point 2
        t2 = p2 + r2 * tn;

        triangle_shader.set_uniform("v0", t0);
        triangle_shader.set_uniform("v1", t1);
        triangle_shader.set_uniform("v2", t2);
        triangle_shader.set_uniform("vn", tn);

        triangle.draw();

    };

    for (auto f : mesh.faces()) {
        draw_triangles(mesh.vertex(f, 0), mesh.vertex(f, 1), mesh.vertex(f, 2));
    }

    triangle_shader.unbind();


}

void SphereMeshRenderer::rebuild() {

    build_shader(sphere_shader, sphere_vshader(), sphere_fshader());
    sphere_shader.bind();
    sphere.set_attributes(sphere_shader);
    sphere_shader.unbind();

    build_shader(cone_shader, cone_vshader(), cone_fshader());
    cone_shader.bind();
    cone.set_attributes(cone_shader);
    cone_shader.unbind();

    build_shader(triangle_shader, triangle_vshader(), triangle_fshader());
    triangle_shader.bind();
    triangle.set_attributes(triangle_shader);
    triangle_shader.unbind();

}

void SphereMeshRenderer::upload_mesh(const SphereMesh &mesh) {

    this->mesh = mesh;

    auto vpoint = mesh.get_vertex_property<Vec4>("v:point");

    sphere_count = 0;
    std::vector<Vec4> vsphere;
    for (auto sphere : mesh.spheres()) {
        vsphere.push_back(vpoint[mesh.vertex(sphere)]);
        sphere_count++;
    }
    for (auto edge : mesh.edges()) {
        vsphere.push_back(vpoint[mesh.vertex(edge, 0)]);
        vsphere.push_back(vpoint[mesh.vertex(edge, 1)]);
        sphere_count += 2;
    }
    for (auto face : mesh.faces()) {
        vsphere.push_back(vpoint[mesh.vertex(face, 0)]);
        vsphere.push_back(vpoint[mesh.vertex(face, 1)]);
        vsphere.push_back(vpoint[mesh.vertex(face, 2)]);
        sphere_count += 3;
    }

    sphere.set_vbo<Vec4>("vsphere", vsphere, 1);

}

//=============================================================================
} // OpenGP::
//=============================================================================
