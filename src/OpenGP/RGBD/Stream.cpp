// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "Stream.h"


//=============================================================================
namespace OpenGP {
//=============================================================================

SensorStream::SensorStream(std::function<const void*(bool)> data_callback, const StreamIntrinsics &intrinsics, const StreamExtrinsics &extrinsics) :
    data_callback(data_callback),
    intrinsics(intrinsics),
    extrinsics(extrinsics) {}

const void *SensorStream::get_data(bool block) const {
    return data_callback(block);
}

const SensorStream &SensorDevice::get_stream(const char *name) const {
    return streams.at(name);
}

GenericIterable<const SensorStream> SensorDevice::get_streams() const {

    using ptype = typename decltype(streams)::value_type;

    auto pred = [](const ptype &pair) -> const SensorStream* {
        return &(pair.second);
    };

    return GenericIterable<const ptype>::adaptor(streams).map(pred);
}


//=============================================================================
} // OpenGP::
//=============================================================================
