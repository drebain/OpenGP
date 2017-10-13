// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <OpenGP/types.h>


//=============================================================================
namespace OpenGP {
//=============================================================================

class Transform {
public:

    Vec3 position = Vec3::Zero();
    Vec3 scale = Vec3::Ones();
    Quaternion rotation = Quaternion::Identity();

    Vec3 right() const {
        return rotation * Vec3(1, 0, 0);
    }

    void set_right(Vec3 right) {
        right.normalize();
        Vec3 axis = this->right().cross(right);
        float angle = axis.norm();
        rotation = rotation * Quaternion(Eigen::AngleAxisf(std::asin(angle), axis / angle));
    }

    Vec3 up() const {
        return rotation * Vec3(0, 1, 0);
    }

    void set_up(Vec3 up) {
        up.normalize();
        Vec3 axis = this->up().cross(up);
        float angle = axis.norm();
        rotation = rotation * Quaternion(Eigen::AngleAxisf(std::asin(angle), axis / angle));
    }

    Vec3 forward() const {
        return rotation * Vec3(0, 0, 1);
    }

    void set_forward(Vec3 forward) {
        forward.normalize();
        Vec3 axis = this->forward().cross(forward);
        float angle = axis.norm();
        rotation = rotation * Quaternion(Eigen::AngleAxisf(std::asin(angle), axis / angle));
    }

    Mat4x4 get_matrix() const {

        Mat4x4 t = Mat4x4::Identity();
        t(0, 3) = position(0);
        t(1, 3) = position(1);
        t(2, 3) = position(2);

        Mat4x4 r = Mat4x4::Identity();
        r.block<3, 3>(0, 0) = rotation.matrix();

        Mat4x4 s = Mat4x4::Identity();
        t(0, 0) = scale(0);
        t(1, 1) = scale(1);
        t(2, 2) = scale(2);

        return t * r * s;

    }

    void set_transformation(const Mat4x4 &mat) {

        Mat4x4 lmat = mat;
        position = lmat.block<3, 1>(0, 3);

        scale = Vec3(
            lmat.block<3, 1>(0, 0).norm(),
            lmat.block<3, 1>(0, 1).norm(),
            lmat.block<3, 1>(0, 2).norm()
        );

        lmat.block<3, 1>(0, 0) /= scale(0);
        lmat.block<3, 1>(0, 1) /= scale(1);
        lmat.block<3, 1>(0, 2) /= scale(2);

        rotation = Quaternion(lmat.block<3, 3>(0, 0));

    }

    void set_translation(const Mat4x4 &mat) {
        position = mat.block<3, 1>(0, 3);
    }

    void set_rotation(const Mat4x4 &mat) {

        Mat4x4 lmat = mat;

        lmat.block<3, 1>(0, 0).normalize();
        lmat.block<3, 1>(0, 1).normalize();
        lmat.block<3, 1>(0, 2).normalize();

        rotation = Quaternion(lmat.block<3, 3>(0, 0));

    }

    void set_scale(const Mat4x4 &mat) {

        scale = Vec3(
            mat.block<3, 1>(0, 0).norm(),
            mat.block<3, 1>(0, 1).norm(),
            mat.block<3, 1>(0, 2).norm()
        );

    }

};

//=============================================================================
} // OpenGP::
//=============================================================================
