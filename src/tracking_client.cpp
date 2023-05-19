#include <agent_tracking/tracking_client.h>
#include <agent_tracking/tracking_service.h>
#include <stdexcept>

using namespace cell_world;
using namespace std;
using namespace agent_tracking;
using namespace tcp_messages;

namespace agent_tracking {

    bool Tracking_client::connect() {
        return connect("127.0.0.1");
    }

    bool Tracking_client::connect(const std::string &ip) {
        return connect(ip, Tracking_service::get_port());
    }

    bool Tracking_client::connect(const std::string &ip, int port) {
        if (local_server) return true;
        is_connected = Message_client::connect(ip, port);
        return is_connected;
    }

    bool Tracking_client::register_consumer() {
        return subscribe();
    }

    void Tracking_client::send_step(const cell_world::Step &step) {
        if (is_connected) {
            if (local_server) {
                local_server->send_step(step);
            } else {
                send_message(Message("send_step", step));
            }
        } else {
            this->on_step(step);
        }
    }

    bool Tracking_client::unregister_consumer() {
        return unsubscribe();
    }

    Tracking_client::Tracking_client() {

    }

    cell_world::Step &Tracking_client::get_current_state(const string &agent_name) {
        return current_states[agent_name];
    }

    void Tracking_client::on_step(const cell_world::Step &step) {
        current_states[step.agent_name] = step;
    }

    bool Tracking_client::contains_agent_state(const string &agent_name) {
        return current_states.count(agent_name) != 0;
    }

    Tracking_client::~Tracking_client() {
        easy_tcp::Client::disconnect();
    }

    bool Tracking_client::subscribe() {
        if (local_server) {
            return local_server->subscribe_local(this);

        } else {
            return Message_client::subscribe();
        }
    }

    bool Tracking_client::unsubscribe() {
        if (local_server) {
            return local_server->unsubscribe_local(this);

        } else {
            return Message_client::unsubscribe();
        }
    }
}
