#pragma once
#include <cell_world.h>
#include <tcp_messages.h>
#include <agent_tracking/message.h>

namespace agent_tracking {
    struct Service : tcp_messages::Message_service {
        Routes(
                //consumer
                Add_route_with_response("register_consumer", register_consumer);
                Add_route_with_response("unregister_consumer", unregister_consumer);
                // experiment
                Add_route_with_response("new_experiment", new_experiment, std::string);
                Add_route_with_response("new_episode", new_episode, New_episode_message);
                Add_route_with_response("end_episode", end_episode);
                Add_route_with_response("update_puff", update_puff);
                //world
                Add_route_with_response("set_occlusions", set_occlusions, std::string);
                Add_route_with_response("set_world_configuration", set_world_configuration, std::string);
                Add_route_with_response("set_world_implementation", set_world_implementation, std::string);
                Add_route_with_response("get_world_info", get_world_info);
        )

        // routes
        //consumer
        virtual bool register_consumer();
        virtual bool unregister_consumer();
        //experiment
        virtual bool new_experiment(const std::string &) { return true; };
        virtual bool new_episode(New_episode_message) { return true; };
        virtual bool end_episode() { return true; };
        //camera
        virtual bool update_puff() { return true; };
        //world
        World_info get_world_info();
        static bool set_world_configuration(const std::string &);
        static bool set_world_implementation(const std::string &);
        static bool set_occlusions(const std::string &);

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
}