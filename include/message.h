#pragma once

#include <json_cpp.h>

struct Message : json_cpp::Json_object {
    Message() = default;
    Message(const std::string &command, const std::string &content) : command(command), content(content) {}
    Json_object_members(
            Add_member(command);
            Add_member(content);
    );
    std::string command;
    std::string content;
};