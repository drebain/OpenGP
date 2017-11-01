// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <memory>

#include <librealsense2/rs.hpp>

#include <OpenGP/RGBD/Stream.h>


//=============================================================================
namespace OpenGP {
//=============================================================================

inline SensorDevice get_realsense_device() {

    rs2::pipeline pipeline;

    pipeline.start();

    std::unordered_map<std::string, SensorStream> streams;
    std::shared_ptr<std::unordered_map<std::string, void*>> stream_data_ptrs(new std::unordered_map<std::string, void*>());

    for (auto profile : pipeline.get_active_profile().get_streams()) {

        StreamIntrinsics intrinsics; // TODO
        StreamExtrinsics extrinsics; // TODO

        auto &name = profile.stream_name();

        (*stream_data_ptrs)[name] = nullptr;
        streams[name] = SensorStream(&stream_data_ptrs->at(name), intrinsics, extrinsics);

    }

    return SensorDevice(streams, [=](bool block){

        rs2::frameset data;

        if (block) {
            data = pipeline.wait_for_frames();
        } else if (!pipeline.poll_for_frames(&data)) {
            return false;
        }

        for (auto frame : data) {
            auto &name = frame.get_profile().stream_name();
            (*stream_data_ptrs)[name] = frame.get_data();
        }

        return true;

    });

}

//=============================================================================
} // OpenGP::
//=============================================================================
