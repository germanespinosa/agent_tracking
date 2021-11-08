#include <agent_tracking_service.h>
#include <agent_tracking.h>
#include <message.h>

using namespace std;

int Agent_tracking_service::get_port() {
    string port_str (std::getenv("CELLWORLD_ROBOT_CONTROLLER_PORT")?std::getenv("CELLWORLD_ROBOT_CONTROLLER_PORT"):"4000");
    return atoi(port_str.c_str());
}

void Agent_tracking_service::on_incoming_data(const string &message_str) {
    Message message;
    message_str >> message;
    if (message.command == "new_episode") {
        cout << "new_episode" << endl;
        New_episode_message nem;
        message.content >> nem;
        Agent_tracking::new_episode(nem);
        send_data(Message("new_episode_result","ok").to_json());
        return;
    }
    if (message.command == "end_episode") {
        cout << "end_episode" << endl;
        Agent_tracking::end_episode();
        send_data(Message("end_episode_result","ok").to_json());
        return;
    }
    if (message.command == "update_background") {
        cout << "update_background" << endl;
        Agent_tracking::initialize_background();
        send_data(Message("update_background_result","ok").to_json());
        return;
    }
    if (message.command == "register_consumer") {
        cout << "register_consumer" << endl;
        consumer_id = Agent_tracking::register_consumer(*this);
        send_data(Message("register_consumer_result","ok").to_json());
        return;
    }
    if (message.command == "reset_cameras") {
        cout << "reset_cameras" << endl;
        Agent_tracking::reset_cameras();
        send_data(Message("reset_cameras_result","ok").to_json());
        return;
    }
    if (message.command == "deregister_consumer") {
        cout << "deregister_consumer" << endl;
        Agent_tracking::deregister_consumer(consumer_id);
        send_data(Message("deregister_consumer_result","ok").to_json());
        return;
    }
    if (message.command == "update_puff") {
        cout << "update_puff" << endl;
        Agent_tracking::update_puff();
        send_data(Message("update_puff_result","ok").to_json());
        return;
    }
    send_data(Message("wrong_message_result","fail").to_json());
}

void Agent_tracking_service::on_connect() {
    cout << "new connection" << endl;
}

void Agent_tracking_service::on_disconnect() {
    if (consumer_id >= 0){
        Agent_tracking::deregister_consumer(consumer_id);
    }
}
