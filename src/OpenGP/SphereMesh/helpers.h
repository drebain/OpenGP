// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <OpenGP/util/cuda_support.h>
#include <OpenGP/SphereMesh/SphereMesh.h>


//=============================================================================
namespace OpenGP {
//=============================================================================

inline Vec2 pill_tangent(const Vec4 &s0_3D, const Vec4 &s1_3D) {

    /// Adapted from pill_dist.m

    Scalar dist_3D = (s1_3D.block<3, 1>(0, 0) - s0_3D.block<3, 1>(0, 0)).norm();

    Vec3 s0(0, 0, s0_3D(3));
    Vec3 s1(dist_3D, 0, s1_3D(3));

    Vec3 a = s1 - s0;
    Scalar a2 = (a(0)*a(0)) + (a(1)*a(1));
    Scalar ad = std::sqrt(a2);
    Vec2 ab = a.block<2, 1>(0, 0);

    Scalar beta = ab.dot(Vec2(std::sqrt((ad*ad)-(a(2)*a(2))), a(2))) / (ad * ad);
    Scalar alpha = std::sqrt(1 - (beta * beta));

    return Vec2(beta, alpha);

}
/*
OPENGP_DEVICE_FUNC inline Vec3 get_barycentric(Vec3 v0, Vec3 v1, Vec3 v2, Vec3 p) {
    Vec3 l0 = v1 - v0;
    Vec3 l1 = v2 - v0;
    Vec3 d = p - v0;
    float d00 = l0.dot(l0);
    float d01 = l0.dot(l1);
    float d11 = l1.dot(l1);
    float d20 = d.dot(l0);
    float d21 = d.dot(l1);
    float denom = d00 * d11 - d01 * d01;
    float v = (d11 * d20 - d01 * d21) / denom;
    float w = (d00 * d21 - d01 * d20) / denom;
    float u = 1.0f - v - w;
    return Vec3(u, v, w);
}*/

OPENGP_DEVICE_FUNC inline Vec3 get_barycentric(Vec3 v0, Vec3 v1, Vec3 v2, Vec3 p) {
    Vec3 A0 = (v1 - v0).cross(v2 - v0);
    Vec3 A1 = (v2 - v1).cross(v0 - v1);
    Vec3 A2 = (v0 - v2).cross(v1 - v2);
    Vec3 l0 = v0 - p;
    Vec3 l1 = v1 - p;
    Vec3 l2 = v2 - p;
    return Vec3(
        l1.cross(l2).dot(A0),
        l2.cross(l0).dot(A1),
        l0.cross(l1).dot(A2)
    ) / A0.squaredNorm();
}

OPENGP_DEVICE_FUNC inline float stop_nan(float t) {
    return (std::isinf(t) || std::isnan(t)) ? 0 : t;
}

OPENGP_DEVICE_FUNC inline float clamp01(float t) {
    return std::fmax(std::fmin(t, 1), 0);
}

OPENGP_DEVICE_FUNC inline Vec3 wedge_normal(const Vec4 &s0, const Vec4 &s1, const Vec4 &s2) {

    Vec3 c0c1 = s1.head<3>() - s0.head<3>();
    Vec3 c0c2 = s2.head<3>() - s0.head<3>();

    float beta = std::asin((s2(3) - s0(3)) / c0c2.norm());

    Vec3 n_tri = c0c1.cross(c0c2).normalized();
    Vec3 a = -c0c2.normalized();
    Vec3 b = a.cross(n_tri);

    float sb = std::sin(beta);
    float cb = std::cos(beta);

    float alpha = std::asin((s0(3) - s1(3) - sb * c0c1.dot(a)) / (cb * c0c1.dot(b)));

    float sa = std::sin(alpha);
    float ca = std::cos(alpha);

    return a * sb + cb * (n_tri * ca + b * sa);
}

OPENGP_DEVICE_FUNC inline Vec3 wedge_normal_toward(const Vec4 &s0, const Vec4 &s1, const Vec4 &s2, const Vec3& p) {

    Vec3 c0c1 = s1.head<3>() - s0.head<3>();
    Vec3 c0c2 = s2.head<3>() - s0.head<3>();

    float beta = std::asin((s2(3) - s0(3)) / c0c2.norm());

    Vec3 n_tri = c0c1.cross(c0c2).normalized();
    n_tri *= std::copysign(1.0f, n_tri.dot(p - s0.head<3>()));
    Vec3 a = -c0c2.normalized();
    Vec3 b = a.cross(n_tri);

    float sb = std::sin(beta);
    float cb = std::cos(beta);

    float alpha = std::asin((s0(3) - s1(3) - sb * c0c1.dot(a)) / (cb * c0c1.dot(b)));

    float sa = std::sin(alpha);
    float ca = std::cos(alpha);

    return a * sb + cb * (n_tri * ca + b * sa);

}

OPENGP_DEVICE_FUNC inline Vec3 wedge_normal(const Vec4 &s0, const Vec4 &s1, const Vec4 &s2, bool flipped) {

    Vec3 c0c1 = s1.head<3>() - s0.head<3>();
    Vec3 c0c2 = s2.head<3>() - s0.head<3>();

    Vec3 n_tri = c0c1.cross(c0c2);

    Vec3 result = wedge_normal(s0, s1, s2);
    if (flipped) result -= 2 * n_tri * n_tri.dot(result) / n_tri.squaredNorm();
    return result;
}

OPENGP_DEVICE_FUNC inline Vec3 sphere_project(const Vec3& p, const Vec4& s, float* sdf_out=nullptr) {
    Vec3 d = p - s.head<3>();
    float spdist = d.norm();
    if (sdf_out != nullptr) *sdf_out = spdist - s(3);
    return s.head<3>() + s(3) * d / spdist;
}

OPENGP_DEVICE_FUNC inline Vec3 pill_project(const Vec3 &p, const Vec4 &s0, const Vec4 &s1, float *sdf_out=nullptr) {

    // Abbreviations

    Vec3 c0 = s0.head<3>();

    Vec4 a = s1 - s0;

    Vec3 d = a.head<3>();
    float l = d.norm();

    // Direction of axis
    Vec3 an = d / l;

    // Offset of p from c0
    Vec3 d2 = p - c0;

    // Projection onto skeleton
    Vec3 p_proj = d2 - an * d2.dot(an);

    // Angle of bitangent
    float beta = std::asin(a(3) / l);

    // Offset along axis
    Vec3 offset = an * p_proj.norm() * std::tan(beta);

    // Position along axis
    float t = an.dot(p + p_proj + offset - c0) / l;
    // Clamped between 0 and 1
    t = std::max(std::min(t, 1.0f), 0.0f);

    // Lerped position + radius
    Vec4 s2 = (1 - t) * s0 + t * s1;

    return sphere_project(p, s2, sdf_out);

}

inline Vec4 pill_s2(const Vec3 &p, const Vec4 &s0, const Vec4 &s1) {
        Vec3 c0 = s0.head<3>();
        Vec4 a = s1 - s0;
        Vec3 d = a.head<3>();
        float l = d.norm();
        Vec3 an = d / l;
        Vec3 d2 = p - c0;
        Vec3 p_proj = d2 - an * d2.dot(an);
        float beta = std::asin(a(3) / l);
        Vec3 offset = an * p_proj.norm() * std::tan(beta);
        float t = an.dot(p + p_proj + offset - c0) / l;
        t = std::max(std::min(t, 1.0f), 0.0f);
        return (1 - t) * s0 + t * s1;
}

OPENGP_DEVICE_FUNC inline Vec3 wedge_project(const Vec3 &p, const Vec4 &s0, const Vec4 &s1, const Vec4 &s2, float *sdf_out=nullptr) {

    Vec3 n_wedge = wedge_normal_toward(s0, s1, s2, p);

    Vec3 tt0 = s0.head<3>() + n_wedge * s0(3);
    Vec3 tt1 = s1.head<3>() + n_wedge * s1(3);
    Vec3 tt2 = s2.head<3>() + n_wedge * s2(3);
    Vec3 ttp = p - n_wedge * (p - tt0).dot(n_wedge);

    Vec3 baryc = get_barycentric(tt0, tt1, tt2, ttp);
    std::cout << baryc(0) << ", " << baryc(1) << ", " << baryc(2) << std::endl;

    std::pair<const Vec4*, float> s_pairs[] = {
        std::pair<const Vec4*, float>(&s0, baryc(0)),
        std::pair<const Vec4*, float>(&s1, baryc(1)),
        std::pair<const Vec4*, float>(&s2, baryc(2))
    };

    std::sort(std::begin(s_pairs), std::end(s_pairs), [](std::pair<const Vec4*, float>& p0, std::pair<const Vec4*, float>& p1){
        return p0.second > p1.second;
    });

    {

        auto& s0 = *(s_pairs[0].first);
        auto& s1 = *(s_pairs[1].first);
        auto& s2 = *(s_pairs[2].first);

        Vec3 c1 = s1.head<3>();

        Vec3 c0c1 = s1.head<3>() - s0.head<3>();
        Vec3 c1c2 = s2.head<3>() - s1.head<3>();

        Vec3 n_plane = n_wedge.cross(p - s0.head<3>());

        Vec3 c3 = c1 - c1c2 * c0c1.dot(n_plane) / c1c2.dot(n_plane);

        float t = (c3 - c1).dot(c1c2) / c1c2.squaredNorm();
        t = clamp01(stop_nan(t));

        Vec4 s3 = (1 - t) * s1 + t * s2;

        return pill_project(p, s0, s3, sdf_out);


    }

}

inline std::pair<Vec4, Vec4> wedge_subpill(const Vec3 &p, const Vec4 &s0, const Vec4 &s1, const Vec4 &s2) {

    Vec3 n_wedge = wedge_normal_toward(s0, s1, s2, p);

    Vec3 tt0 = s0.head<3>() + n_wedge * s0(3);
    Vec3 tt1 = s1.head<3>() + n_wedge * s1(3);
    Vec3 tt2 = s2.head<3>() + n_wedge * s2(3);
    Vec3 ttp = p - n_wedge * (p - tt0).dot(n_wedge);

    Vec3 baryc = get_barycentric(tt0, tt1, tt2, ttp);

    std::pair<const Vec4*, float> s_pairs[] = {
        std::pair<const Vec4*, float>(&s0, baryc(0)),
        std::pair<const Vec4*, float>(&s1, baryc(1)),
        std::pair<const Vec4*, float>(&s2, baryc(2))
    };

    std::sort(std::begin(s_pairs), std::end(s_pairs), [](std::pair<const Vec4*, float>& p0, std::pair<const Vec4*, float>& p1){
        return p0.second > p1.second;
    });

    {

        auto& s0 = *(s_pairs[0].first);
        auto& s1 = *(s_pairs[1].first);
        auto& s2 = *(s_pairs[2].first);

        Vec3 c1 = s1.head<3>();

        Vec3 c0c1 = s1.head<3>() - s0.head<3>();
        Vec3 c1c2 = s2.head<3>() - s1.head<3>();

        Vec3 n_plane = n_wedge.cross(p - s0.head<3>());

        Vec3 c3 = c1 - c1c2 * c0c1.dot(n_plane) / c1c2.dot(n_plane);

        float t = (c3 - c1).dot(c1c2) / c1c2.squaredNorm();
        t = clamp01(stop_nan(t));

        Vec4 s3 = (1 - t) * s1 + t * s2;

        return std::pair<Vec4, Vec4> (s0, s3);


    }

}

/*
inline Vec3 wedge_project(const Vec3 &p, const Vec4 &s0, const Vec4 &s1, const Vec4 &s2, float *sdf_out=nullptr) {

    // Abbreviations

    Vec3 c0 = s0.head<3>();
    Vec3 c1 = s1.head<3>();
    Vec3 c2 = s2.head<3>();

    Vec4 a0 = s1 - s0;
    Vec4 a1 = s2 - s0;

    Vec3 d0 = a0.head<3>();
    Vec3 d1 = a1.head<3>();

    float l0 = d0.norm();
    float l1 = d1.norm();

    // Angles of bitangent for pills
    float beta = std::asin(a1(3) / l1);

    // Normal vector of the skeleton triangle
    Vec3 tri_normal = d0.cross(d1).normalized();

    // Length of projection onto skeleton
    float p_proj_len = (p - c0).dot(tri_normal);

    // Bring normal onto same side as p
    tri_normal *= std::copysign(1.0f, p_proj_len);

    Vec3 A = -d1 / l1;
    Vec3 S = A.cross(tri_normal);

    float alpha = std::asin((s0(3) - s1(3) - std::sin(beta) * (c1 - c0).dot(A)) / (std::cos(beta) * (c1 - c0).dot(S)));

    // Normal vector of the planar section
    Vec3 n = A * std::sin(beta) + std::cos(beta) * (tri_normal * std::cos(alpha) + S * std::sin(alpha));

    // Bring normal onto same side as p
    n *= std::copysign(1.0f, p_proj_len);
    p_proj_len = std::abs(p_proj_len);

    // Skeleton projection with angle correction
    Vec3 cproj = p - n * p_proj_len / n.dot(tri_normal);

    Vec3 u = get_barycentric(c0, c1, c2, cproj);
    Vec3 best_proj;
    float best_dist = FLT_MAX;
    if (u.minCoeff() < 0) {

        // Project onto a pill segment

        float dist;
        Vec3 proj = pill_project(p, s0, s1, &dist);
        if (dist < best_dist) {
            best_dist = dist;
            best_proj = proj;
        }
        proj = pill_project(p, s2, s1, &dist);
        if (dist < best_dist) {
            best_dist = dist;
            best_proj = proj;
        }
        proj = pill_project(p, s2, s0, &dist);
        if (dist < best_dist) {
            best_dist = dist;
            best_proj = proj;
        }

    } else {

        // Project onto triangle section

        Vec4 s3 = u(0) * s0 + u(1) * s1 + u(2) * s2;
        Vec3 c3 = s3.head<3>();

        float spdist = (p - c3).norm();

        best_dist = spdist - s3(3);

        best_proj = c3 + s3(3) * (p - c3) / spdist;

    }

    if (sdf_out != nullptr) *sdf_out = best_dist;

    return best_proj;

}
*/

//=============================================================================
} // namespace OpenGP
//=============================================================================
