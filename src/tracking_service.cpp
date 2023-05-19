#include <agent_tracking/tracking_service.h>
#include <mutex>

using namespace std;
using namespace cell_world;
using namespace tcp_messages;

namespace agent_tracking {

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

    bool Tracking_service::set_throughput(float new_throughput) {
        throughput = new_throughput;
        return true;
    }

    void Tracking_service::send_step(const cell_world::Step &step) {
        ((Tracking_server *) _server)->send_step(step);
    }

    void Tracking_server::send_step(const Step &step) {
        for (auto local_client: subscribed_local_clients){
            local_client->on_step(step);
        }
        if (!clients.empty()) {
            Step converted_step = step;
            Message update;
            for (auto &client : clients){
                if (!client->_subscribed) continue;
                bool send_update = false;
                if (client->throughput > 0) {
                    if (client->last_step_send.contains(step.agent_name)){
                        if (client->last_step_send[step.agent_name].time_out()) {
                            send_update = true;
                            client->last_step_send[step.agent_name].reset();
                        }
                    } else {
                        client->last_step_send[step.agent_name] = Timer(1.0 / client->throughput);
                        send_update = true;
                    }
                }
                if (send_update){
                    if (update.header.empty()) {
                        update = Message(converted_step.agent_name + "_step", converted_step);
                    }
                    update.set_id();
                    client->send_message(update);
                }
            }
        }
    }
}
