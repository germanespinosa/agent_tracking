#include <agent_tracking/service.h>
#include <mutex>
#if XLIBALL_PRESENT
#include <agent_tracking.h>
#endif

using namespace std;
using namespace cell_world;
using namespace tcp_messages;

namespace agent_tracking {

    vector<easy_tcp::Connection *> consumers;
    mutex consumers_mutex;


#if XLIBALL_PRESENT
    void Service::new_experiment(const std::string &occlusions) {
        cout << "new_experiment" << endl;
        send_update(Message("new_experiment", New_experiment_message{occlusions}));
        send_message(Message("new_experiment_result", "ok"));
    }

    void Service::new_episode(New_episode_message nem) {
        cout << "new_episode" << endl;
        agent_tracking::new_episode(nem);
        send_message(Message("new_episode_result", "ok"));
    }

    void Service::end_episode() {
        cout << "end_episode" << endl;
        agent_tracking::end_episode();
        send_message(Message("end_episode_result", "ok"));
    }

    void Service::update_background() {
        cout << "update_background" << endl;
        agent_tracking::initialize_background();
        send_message(Message("update_background_result", "ok"));
    }

    void Service::reset_cameras() {
        cout << "reset_cameras" << endl;
        agent_tracking::reset_cameras();
        send_message(Message("reset_cameras_result", "ok"));
    }

    void Service::update_puff() {
        cout << "update_puff" << endl;
        agent_tracking::update_puff();
        send_message(Message("update_puff_result", "ok"));
    }

    void Service::show_occlusions(const string &occlusions) {
        cout << "show_occlusions" << endl;
        agent_tracking::set_occlusions(occlusions);
        send_message(Message("show_occlusions_result", "ok"));
    }

    void Service::hide_occlusions() {
        cout << "hide_occlusions" << endl;
        agent_tracking::set_occlusions("00_00");
        send_message(Message("hide_occlusions_result", "ok"));
    }
#else
    void Service::new_experiment(const std::string &) {
        cout << "new_experiment" << endl;
        send_message(Message("new_experiment_result", "ok"));
    }

    void Service::new_episode(New_episode_message ) {
        cout << "new_episode" << endl;
        send_message(Message("new_episode_result", "ok"));
    }

    void Service::end_episode() {
        cout << "end_episode" << endl;
        send_message(Message("end_episode_result", "ok"));
    }

    void Service::update_background() {
        cout << "update_background" << endl;
        send_message(Message("update_background_result", "ok"));
    }

    void Service::reset_cameras() {
        cout << "reset_cameras" << endl;
        send_message(Message("reset_cameras_result", "ok"));
    }

    void Service::update_puff() {
        cout << "update_puff" << endl;
        send_message(Message("update_puff_result", "ok"));
    }

    void Service::show_occlusions(const string &) {
        cout << "show_occlusions" << endl;
        send_message(Message("show_occlusions_result", "ok"));
    }

    void Service::hide_occlusions() {
        cout << "hide_occlusions" << endl;
        send_message(Message("hide_occlusions_result", "ok"));
    }

#endif

    void Service::register_consumer() {
        cout << "register_consumer" << endl;
        consumers_mutex.lock();
        consumers.emplace_back(connection);
        consumers_mutex.unlock();
        send_message(Message("register_consumer_result", "ok"));
    }

    void Service::unregister_consumer() {
        cout << "unregister_consumer" << endl;
        remove_consumer();
        send_message(Message("unregister_consumer_result", "ok"));
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
}