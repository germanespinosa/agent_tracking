#pragma once
#include <json_cpp.h>

struct Message : json_cpp::Json_object{
    Message() = default;
    Message(std::string command,std::string content):
            command(command),
            content(content){}
    std::string command;
    std::string content;
    Json_object_members(
            Add_member(command);
            Add_member(content);
    );
};

struct New_episode_message : json_cpp::Json_object{
    std::string subject;
    std::string experiment;
    std::string destination_folder = ".";
    std::string occlusions;
    int episode;
    Message to_message(){
        Message message;
        message.command = "new_episode";
        message.content = this->to_json();
        return message;
    }
    Json_object_members(
            Add_member(subject);
            Add_member(experiment);
            Add_member(destination_folder);
            Add_member(occlusions);
            Add_member(episode);
    )
};
