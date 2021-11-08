#include <easy_tcp.h>
#include <message.h>
#include <time_stamp.h>

using namespace easy_tcp;
using namespace std;

struct Client {
    Client(Connection &connection) : connection(connection) {
        running = true;
        client_thread = thread([&] {
            while(this->running) {
                if (this->new_message) continue;
                if (this->connection.receive_data()) {
                    string(this->connection.buffer) >> this->message;
                    this->new_message = true;
                }
            }
        });
    }
    ~Client(){
        running = false;
        connection.disconnect();
        client_thread.join();
    }
    bool message_received(){
        bool nm = new_message;
        new_message = false;
        return nm;
    }
    Message &get_message(){
        return message;
    }
    Connection &connection;
    thread client_thread;
    Message message;
    atomic<bool> new_message;
    atomic<bool> running;
};

bool wait_for_result(const string &command, Client &connection){
    while (!connection.message_received());
    Message &message = connection.get_message();
    return message.command == command + "_result" &&  message.content == "ok";
}

int main(int argc, char **argv) {
    Time_stamp ts;
    Connection connection = Connection::connect_remote("127.0.0.1", 4000);
    Client client(connection);
    string message;
    message = Message("register_consumer","").to_json();
    connection.send_data(message.c_str(), message.size() + 1);
    wait_for_result("register_consumer", client);
    New_episode_message nem;
    nem.subject = "FPP1";
    nem.experiment = "ROBOT";
    nem.episode = 18;
    nem.destination_folder= "/maze/agent_tracking/cmake-build-release";
    message = nem.to_message().to_json();
    connection.send_data(message.c_str(), message.size() + 1);
    wait_for_result("new_episode", client);
    ts.reset();
    while (ts.to_seconds()<10){
        if (client.message_received()){
            cout << client.get_message() << endl;
        }
    }
    message = Message("end_episode","").to_json();
    connection.send_data(message.c_str(), message.size() + 1);
    wait_for_result("end_episode",client);
    message = Message("deregister_consumer","").to_json();
    connection.send_data(message.c_str(), message.size() + 1);
    wait_for_result("deregister_consumer",client);
    connection.disconnect();
    return 0;
}