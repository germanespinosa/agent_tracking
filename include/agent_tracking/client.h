#pragma once
#include <cell_world.h>
#include <map>

namespace agent_tracking {
    struct Client : cell_world::Message_client {
        Routes(
                Add_route("(.*)(step)", process_step, cell_world::Step);
                )

        Client();
        ~Client();
        virtual void process_step(const cell_world::Step &);
        bool connect();
        bool connect(const std::string &ip);
        bool new_episode( const std::string &experiment, int episode, const std::string &subject, const std::string &occlusions, const std::string &destination_folder);
        bool end_episode();
        bool register_consumer();
        bool unregister_consumer();
        bool reset_cameras();
        bool update_background();
        bool update_puff();
        cell_world::Step &get_current_state(const std::string &agent_name);
        bool contains_agent_state(const std::string &agent_name);
        cell_world::Message wait_for_result(const std::string &operation, double time_out = 5.0);
        std::atomic<bool> registered_consumer;
        bool episode_in_progress;
        std::thread consumer;
        std::map<std::string, cell_world::Step> current_states;
    };

    struct Filter : json_cpp::Json_object {
        Json_object_members(
                Add_member(outlier_distance);
                Add_member(outlier_threshold);
                Add_member(complementary_filter);
        );
        double outlier_distance;
        unsigned int outlier_threshold;
        double complementary_filter;
        int outlier_count = -1;
        cell_world::Location filtered_location(const cell_world::Location &previous_location, const cell_world::Location &current_reading);
    };

    struct Filtered_client : Client{
        Filtered_client( const Filter &);
        void process_step(const cell_world::Step &);
        Filter filter_template;
        std::map<std::string, Filter> agent_filters;
    };
}
;