// Copyright (c) 2022 Graphcore Ltd. All rights reserved.

#pragma once

#include <vector>
#include <string>
#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>

struct ServerState {
    ServerState() : value(1.f), stop(false) {}
    std::string toString() const{
        return "ServerState(value=" + std::to_string(value) +
               ", stop=" + std::to_string(stop) + ")";
    }

    template <class Archive>
    void serialize(Archive& ar) {
        ar(value, stop);
    }

    float value;
    bool stop;
};

namespace packets {

const std::vector<std::string> packetTypes {
    "stop",                // Tell server to stop rendering and exit (client -> server)
    "progress",            // Send progress (server -> client)
    "state",               // Update client-side state to match the server (server->client)
    "value",               // Update server-side value (client -> server)
    "render_preview",      // used to send compressed video packets
                           // for render preview (server -> client)
    "ready",               // Used to sync with the server with the client once services are ready (bi-directional)
    "ui_ready",            // Used to sync with the client with the server once the UI is ready (bi-directional)
};

} // end namespace packets
