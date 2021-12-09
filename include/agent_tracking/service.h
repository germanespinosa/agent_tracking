#pragma once
#include <cell_world.h>
#include <agent_tracking/message.h>


namespace agent_tracking {
    struct Service : cell_world::Message_service {
        Routes(
                Add_route("new_episode", new_episode, New_episode_message);
                Add_route("end_episode", end_episode);
                Add_route("update_background", update_background);
                Add_route("register_consumer", register_consumer);
                Add_route("reset_cameras", reset_cameras);
                Add_route("unregister_consumer", unregister_consumer);
                Add_route("update_puff", update_puff);
                Add_route("show_occlusions", show_occlusions, std::string);
                Add_route("hide_occlusions", hide_occlusions);
                Add_route("new_experiment", new_experiment, std::string);
                Add_route("update", new_experiment, std::string);
        )

        // routes
        //consumer
        virtual void register_consumer();
        virtual void unregister_consumer();
        //experiment
        virtual void new_experiment(const std::string &);
        virtual void new_episode(New_episode_message);
        virtual void end_episode();
        //camera
        virtual void update_background();
        virtual void reset_cameras();
        virtual void update_puff();
        //visualization
        virtual void show_occlusions(const std::string &);
        virtual void hide_occlusions();

        //unrouted
        void unrouted_message(const cell_world::Message &) override;

        //connection events
        void on_connect() override;
        void on_disconnect() override;

        //static
        static int get_port();
        static void send_update(const cell_world::Message &);
    private:
        void remove_consumer();
    };
}