#include <agent_tracking/tracking_service.h>
#include <mutex>

using namespace std;
using namespace cell_world;
using namespace tcp_messages;

namespace agent_tracking {


    World_info world_info;

    void Tracking_service::unrouted_message(const Message &) {
        send_data(Message("wrong_message_result", "fail").to_json());
    }

    void Tracking_service::on_connect() {
        cout << "on_connect" << endl;
    }

    void Tracking_service::on_disconnect() {
        cout << "on_disconnect" << endl;
    }

    int Tracking_service::get_port() {
        string port_str(std::getenv("AGENT_TRACKING_PORT") ? std::getenv("AGENT_TRACKING_PORT") : "4510");
        return atoi(port_str.c_str());
    }

    cell_world::World_info Tracking_service::get_world() {
        return world_info;
    }

    void Tracking_server::send_step(const Step &step) {
        for (auto local_client: local_clients){
            local_client->on_step(step);
        }
        if (!clients.empty()) {
            Step converted_step = step;
            Message update(converted_step.agent_name + "_step", converted_step);
            broadcast_subscribed(update);
        }
    }

    bool Tracking_service::set_world(const World_info &new_world_info) {
        world_info = new_world_info;
        return true;
    }
}