#pragma once
#include <json_cpp.h>

namespace agent_tracking {
    struct New_episode_message : json_cpp::Json_object {
        std::string subject;
        std::string experiment;
        std::string destination_folder = ".";
        std::string occlusions;
        int episode;

        Json_object_members(
                Add_member(subject);
                Add_member(experiment);
                Add_member(destination_folder);
                Add_member(occlusions);
                Add_member(episode);
        )
    };
}