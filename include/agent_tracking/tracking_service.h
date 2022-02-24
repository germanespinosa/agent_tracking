#pragma once
#include <cell_world.h>
#include <tcp_messages.h>
#include <agent_tracking/tracking_client.h>

namespace agent_tracking {
    struct Tracking_service : tcp_messages::Message_service {
        Routes(
                //consumer
                Add_route_with_response("register_consumer", _subscribe);
                Add_route_with_response("unregister_consumer", _unsubscribe);
                //world
                Add_route_with_response("set_world", set_world, cell_world::World_info);
                Add_route_with_response("get_world", get_world);
                Allow_subscription();
        )

        // routes
        //world
        static cell_world::World_info get_world();
        static bool set_world(const cell_world::World_info &);
        //unrouted
        void unrouted_message(const tcp_messages::Message &) override;

        //connection events
        void on_connect() override;
        void on_disconnect() override;

        //static
        static int get_port();
    };

    struct Tracking_server : tcp_messages::Message_server<Tracking_service>{
        void send_step(const cell_world::Step &);

        template<class T>
        T &create_local_client(){
            static_assert(std::is_base_of<Tracking_client, T>::value, "T must inherit from Key");
            auto new_local_client = new T();
            local_clients.push_back((Tracking_client *) new_local_client);
            new_local_client->is_local = true;
            return *new_local_client;
        }
        std::vector<Tracking_client * > local_clients;
    };
}