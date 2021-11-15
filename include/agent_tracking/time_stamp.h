#pragma once
#include <chrono>

namespace agent_tracking {
    struct Time_stamp {
        Time_stamp();
        void reset();
        std::chrono::time_point<std::chrono::high_resolution_clock> check_point;
        double to_seconds();
    };
}