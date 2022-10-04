#pragma once
#include <cell_world.h>
#include <tcp_messages.h>
#include <map>

namespace agent_tracking {
    struct Tracking_server;

    struct Tracking_client : tcp_messages::Message_client {
        Routes(
                Add_route("(.*)(step)", on_step, cell_world::Step);
                )

        Tracking_client();
        virtual ~Tracking_client();

        virtual void on_step(const cell_world::Step &);

        bool connect();

        bool connect(const std::string &ip);

        bool register_consumer();

        bool unregister_consumer();

        bool subscribe();

        bool unsubscribe();

        cell_world::Step &get_current_state(const std::string &agent_name);
        bool contains_agent_state(const std::string &agent_name);

        std::atomic<bool> registered_consumer;
        std::map<std::string, cell_world::Step> current_states;
        Tracking_server *local_server = nullptr;
    };
}