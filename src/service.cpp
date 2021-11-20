#include <agent_tracking/service.h>

using namespace std;
using namespace cell_world;

namespace agent_tracking {

    vector<std::reference_wrapper<Service>> consumers;
    vector<int> consumers_id;


    int Service::get_port() {
        string port_str(std::getenv("CELLWORLD_AGENT_TRACKING_PORT") ? std::getenv("CELLWORLD_AGENT_TRACKING_PORT") : "4000");
        return atoi(port_str.c_str());
    }

    void Service::new_experiment(const std::string &occlusions) {
        send_update(Message("new_experiment", New_experiment_message{occlusions}));
        send_message(Message("new_experiment_result", "ok"));
    }

    void Service::new_episode(New_episode_message nem) {
        cout << "new_episode" << endl;
        agent_tracking::new_episode(nem);
        send_message(Message("new_episode_result", "ok"));
    }

    void Service::end_episode() {
        cout << "new_episode" << endl;
        agent_tracking::end_episode();
        send_message(Message("end_episode_result", "ok"));
    }

    void Service::update_background() {
        cout << "update_background" << endl;
        agent_tracking::initialize_background();
        send_message(Message("update_background_result", "ok"));
    }

    void Service::register_consumer() {
        static unsigned int consumers_count = 0;
        cout << "register_consumer" << endl;
        consumers.emplace_back(*this);
        consumers_id.push_back(consumers_count);
        consumer_id =  consumers_count++;
        send_message(Message("register_consumer_result", "ok"));
    }

    void Service::reset_cameras() {
        cout << "reset_cameras" << endl;
        agent_tracking::reset_cameras();
        send_message(Message("reset_cameras_result", "ok"));
    }

    void Service::unregister_consumer() {
        cout << "unregister_consumer" << endl;
        remove_consumer();
        send_message(Message("unregister_consumer_result", "ok"));
    }

    void Service::update_puff() {
        cout << "update_puff" << endl;
        agent_tracking::update_puff();
        send_message(Message("update_puff_result", "ok"));
    }

    void Service::show_occlusions(const string &occlusions) {
        cout << "set_occlusions" << endl;
        agent_tracking::set_occlusions(occlusions);
        send_message(Message("show_occlusions_result", "ok"));
    }

    void Service::unrouted_message(const cell_world::Message &) {
        send_data(Message("wrong_message_result", "fail").to_json());
    }

    void Service::on_connect() {
        cout << "new connection" << endl;
    }

    void Service::on_disconnect() {
        if (consumer_id >= 0) {
            remove_consumer();
        }
    }

    void Service::send_update(const cell_world::Message &message) {
        auto message_str = message.to_json();
        for (auto &consumer: consumers) {
            try {
                consumer.get().send_data(message_str.c_str(), (int)message_str.size() + 1);
            } catch (...) {
                consumer.get().remove_consumer();
            }
        }
    }

    void Service::remove_consumer() {
        vector<std::reference_wrapper<Service>> new_consumers;
        vector<int> new_consumers_id;
        for (unsigned int i = 0; i < consumers_id.size(); i++) {
            if (consumers_id[i] != consumer_id) {
                new_consumers.emplace_back(consumers[i]);
                new_consumers_id.emplace_back(consumers_id[i]);
            }
        }
        consumers = new_consumers;
        consumers_id = new_consumers_id;
    }

    void Service::hide_occlusions() {
        cout << "set_occlusions" << endl;
        agent_tracking::set_occlusions("00_00");
        send_message(Message("hide_occlusions_result", "ok"));
    }

}