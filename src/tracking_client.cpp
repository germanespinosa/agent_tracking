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
        if (is_local) return true;
        return Message_client::connect(ip, Tracking_service::get_port());
    }

    bool Tracking_client::register_consumer() {
        if (is_local) return true;
        auto response = send_request(Message("register_consumer"));
        registered_consumer = response.body == "success";
        return registered_consumer;
    }

    bool Tracking_client::unregister_consumer() {
        if (is_local) return true;
        auto response = send_request(Message("unregister_consumer"));
        if (response.body == "success") {
            registered_consumer = false;
            return true;
        }
        return false;
    }

    Tracking_client::Tracking_client(): is_local(false) {

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

    World_info Tracking_client::get_world() {
        return send_request(Message("get_world")).get_body<World_info>();
    }

    void Tracking_client::on_world_update(const World_info &new_world_info) {
        world_info = new_world_info;
    }
}