#include <agent_tracking/client.h>
#include <agent_tracking/message.h>
#include <agent_tracking/time_stamp.h>
#include <stdexcept>

using namespace cell_world;
using namespace std;

namespace agent_tracking {
    bool Client::connect() {
        return connect("127.0.0.1");
    }

    bool Client::connect(const std::string &ip) {
//        return connect(ip, Service::get_port());
        return connect(ip, 4000);
    }

    bool Client::connect(const std::string &ip, int port) {
        return Message_client::connect(ip, port);
    }

    bool Client::new_episode(const std::string &experiment, int episode, const std::string &subject,
                             const std::string &occlusions, const std::string &destination_folder) {
        New_episode_message nem;
        nem.experiment = experiment;
        nem.episode = episode;
        nem.subject = subject;
        nem.occlusions = occlusions;
        nem.destination_folder = destination_folder;
        if (!send_message(Message("new_episode",nem))) return false;
        return wait_for_result("new_episode").body == "ok";
    }

    bool Client::end_episode() {
        if (!send_message(Message("end_episode"))) return false;
        return wait_for_result("end_episode").body == "ok";
    }

    bool Client::register_consumer(auto &callback) {
        {
            if (!send_message(cell_world::Message("register_consumer"))) return false;
            registered_consumer = wait_for_result("register_consumer").body == "ok";
            consumer = std::thread ([this, &callback]() {
                while (registered_consumer)
                    if (contains("step"))
                        callback(get_message("step").get_body<cell_world::Step>());
            });
            return registered_consumer;
        }
    }

    bool Client::register_consumer(bool automatic) {
        if (automatic) {
            auto lambda = [this](cell_world::Step step) { cout << step << endl;};
            return register_consumer(lambda);
        } else {
            if (!send_message(cell_world::Message("register_consumer"))) return false;
            registered_consumer = wait_for_result("register_consumer").body == "ok";
            return registered_consumer;
        }
    }

    bool Client::reset_cameras() {
        if (!send_message(Message("reset_cameras"))) return false;
        return wait_for_result("reset_cameras").body == "ok";
    }

    bool Client::unregister_consumer() {
        if (!send_message(Message("unregister_consumer"))) return false;
        return wait_for_result("unregister_consumer").body == "ok";
    }

    bool Client::update_background() {
        if (!send_message(Message("update_background"))) return false;
        return wait_for_result("update_background").body == "ok";
    }

    bool Client::update_puff() {
        if (!send_message(Message("update_puff"))) return false;
        return wait_for_result("update_puff").body == "ok";
    }

    cell_world::Message Client::wait_for_result(const std::string &operation, double time_out) {
        Time_stamp ts;
        std::string header = operation + "_result";
        ts.reset();
        while (!contains(header) && (time_out == 0 || ts.to_seconds() < time_out));
        return get_message(header);
    }

    Client::Client() {

    }

    void Client::new_step(cell_world::Step) {

    }

}