// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "Stream.h"


//=============================================================================
namespace OpenGP {
//=============================================================================

SensorStream::SensorStream(const char *name, const void **data_ptr, const StreamIntrinsics &intrinsics, const StreamExtrinsics &extrinsics) :
    data_ptr(data_ptr),
    intrinsics(intrinsics),
    extrinsics(extrinsics),
    name(name) {}

const void *SensorStream::get_data() const {
    return *data_ptr;
}

void SensorDevice::advance_frame() {
    advance_frame_callback(true);
}

bool SensorDevice::try_advance_frame() {
    return advance_frame_callback(false);
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
