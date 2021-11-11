#pragma once
#include <cell_world.h>
#include <easy_tcp.h>

namespace agent_tracking{
    struct Client{
        bool connect (std::string, int);
        void new_info(cell_world::Step &);
        easy_tcp::Connection connection;
    };
}