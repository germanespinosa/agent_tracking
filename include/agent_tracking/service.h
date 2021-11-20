#pragma once
#include <cell_world.h>
#include <agent_tracking.h>

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
        )

        void unrouted_message(const cell_world::Message &) override;

        // routes
        void new_episode(New_episode_message);
        void end_episode();
        void update_background();
        void register_consumer();
        void reset_cameras();
        void unregister_consumer();
        void update_puff();
        void new_experiment(const std::string &);
        void show_occlusions(const std::string &);
        void hide_occlusions();
        void on_connect() override;
        void on_disconnect() override;
        static int get_port();
        int consumer_id = -1;
        static void send_update(const cell_world::Message &);
    private:
        void remove_consumer();
    };
}