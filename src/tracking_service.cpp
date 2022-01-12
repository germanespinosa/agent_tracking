#include <agent_tracking/tracking_service.h>
#include <mutex>

using namespace std;
using namespace cell_world;
using namespace tcp_messages;

namespace agent_tracking {

    vector<easy_tcp::Connection *> consumers;
    mutex consumers_mutex;
    World_info world_info;

    bool Tracking_service::register_consumer() {
        cout << "register_consumer" << endl;
        consumers_mutex.lock();
        consumers.emplace_back(connection);
        consumers_mutex.unlock();
        return true;
    }

    bool Tracking_service::unregister_consumer() {
        cout << "unregister_consumer" << endl;
        remove_consumer();
        return true;
    }

    void Tracking_service::unrouted_message(const Message &) {
        send_data(Message("wrong_message_result", "fail").to_json());
    }

    void Tracking_service::on_connect() {
        cout << "on_connect" << endl;
    }

    void Tracking_service::on_disconnect() {
        cout << "on_disconnect" << endl;
        remove_consumer();
    }

    void remove_connection(easy_tcp::Connection *connection) {
        auto position = std::find(consumers.begin(), consumers.end(), connection);
        if (position != consumers.end()) {// == myVector.end() means the element was not found
            consumers.erase(position);
        }
    }

    void Tracking_service::remove_consumer() {
        consumers_mutex.lock();
        remove_connection(connection);
        consumers_mutex.unlock();
    }

    int Tracking_service::get_port() {
        string port_str(std::getenv("AGENT_TRACKING_PORT") ? std::getenv("AGENT_TRACKING_PORT") : "4510");
        return atoi(port_str.c_str());
    }

    void Tracking_service::send_update(const Message &message) {
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

    cell_world::World_info Tracking_service::get_world() {
        return world_info;
    }

    void Tracking_service::send_step(const Step &step) {
        Step converted_step = step;
        Message update (converted_step.agent_name + "_step", converted_step);
        send_update(update);
    }

    bool Tracking_service::set_world(const World_info &new_world_info) {
        world_info = new_world_info;
        send_update(Message("world_update", world_info));
        return true;
    }
}