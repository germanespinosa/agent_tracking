#pragma once
#include <cell_world.h>
#include <tcp_messages.h>

namespace agent_tracking {
    struct Tracking_service : tcp_messages::Message_service {
        Routes(
                //consumer
                Add_route_with_response("register_consumer", register_consumer);
                Add_route_with_response("unregister_consumer", unregister_consumer);
                //world
                Add_route_with_response("set_world", set_world, cell_world::World_info);
                Add_route_with_response("get_world", get_world);
        )

        // routes
        //consumer
        virtual bool register_consumer();
        virtual bool unregister_consumer();
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
        static void send_step(const cell_world::Step &);
        static void send_update(const tcp_messages::Message &);
    private:
        void remove_consumer();
    };

    using Tracking_server = tcp_messages::Message_server<Tracking_service>;
}