#include <agent_tracking/service.h>
#include <mutex>

using namespace std;
using namespace cell_world;
using namespace tcp_messages;

namespace agent_tracking {

    vector<easy_tcp::Connection *> consumers;
    mutex consumers_mutex;
    World_info world_info;

    bool Service::register_consumer() {
        cout << "register_consumer" << endl;
        consumers_mutex.lock();
        consumers.emplace_back(connection);
        consumers_mutex.unlock();
        return true;
    }

    bool Service::unregister_consumer() {
        cout << "unregister_consumer" << endl;
        remove_consumer();
        return true;
    }

    void Service::unrouted_message(const Message &) {
        send_data(Message("wrong_message_result", "fail").to_json());
    }

    void Service::on_connect() {
        cout << "on_connect" << endl;
    }

    void Service::on_disconnect() {
        cout << "on_disconnect" << endl;
        remove_consumer();
    }

    void remove_connection(easy_tcp::Connection *connection) {
        auto position = std::find(consumers.begin(), consumers.end(), connection);
        if (position != consumers.end()) {// == myVector.end() means the element was not found
            consumers.erase(position);
        }
    }

    void Service::remove_consumer() {
        consumers_mutex.lock();
        remove_connection(connection);
        consumers_mutex.unlock();
    }

    int Service::get_port() {
        string port_str(std::getenv("AGENT_TRACKING_PORT") ? std::getenv("AGENT_TRACKING_PORT") : "4510");
        return atoi(port_str.c_str());
    }

    void Service::send_update(const Message &message) {
        auto message_str = message.to_json();
        vector<easy_tcp::Connection *> to_remove;
        consumers_mutex.lock();
        for (auto &consumer: consumers) {
            try {
                consumer->send_data(message_str.c_str(), (int)message_str.size() + 1);
            } catch (...) {
                to_remove.push_back(consumer); // if fails I mark it for removal
            }
        }
        for (auto consumer:to_remove) remove_connection(consumer); // remove failed
        consumers_mutex.unlock();
    }

    World_info Service::get_world_info() {
        return world_info;
    }

    bool Service::set_world_configuration(const std::string &world_configuration_name) {
        world_info.world_configuration = world_configuration_name;
        return true;
    }

    bool Service::set_world_implementation(const std::string &world_implementation_name) {
        world_info.world_implementation = world_implementation_name;
        return true;
    }

    bool Service::set_occlusions(const std::string &occlusions_name) {
        world_info.occlusions = occlusions_name;
        return true;
    }

    void Service::send_step(const Step &step) {
        Step converted_step = step;
        Message update (converted_step.agent_name + "_step", converted_step);
    }
}