#pragma once
#include <cell_world.h>
#include <tcp_messages.h>
#include <agent_tracking/message.h>
#include <map>

namespace agent_tracking {
    struct Tracking_client : tcp_messages::Message_client {
        Routes(
                Add_route("(.*)(step)", process_step, cell_world::Step);
                Add_route("world_update", process_world_update, cell_world::World_info);
                )

        Tracking_client();
        ~Tracking_client();

        virtual void process_step(const cell_world::Step &);
        virtual void process_world_update(const cell_world::World_info &);

        bool connect();

        bool connect(const std::string &ip);

        bool register_consumer();

        bool unregister_consumer();

        cell_world::World_info get_world();
        cell_world::Step &get_current_state(const std::string &agent_name);
        bool contains_agent_state(const std::string &agent_name);

        std::atomic<bool> registered_consumer;
        std::map<std::string, cell_world::Step> current_states;
        cell_world::World_info world_info;
    };
}