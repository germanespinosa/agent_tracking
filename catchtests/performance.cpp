#include <catch.h>
#include <agent_tracking/tracking_client.h>
#include <agent_tracking/tracking_service.h>
#include <cell_world.h>

using namespace agent_tracking;
using namespace cell_world;
using namespace std;
using namespace tcp_messages;
using namespace this_thread;


atomic<bool> ready;

struct Client : Tracking_client{

    void on_step(const Step &step) override{
        counter ++;
        ready = true;
    }
    int counter = 0;
};


TEST_CASE("Over TCP") {
    Timer ts;
    Tracking_server server;
    server.start(Tracking_service::get_port());
    Step step;
    step.agent_name = "predator";
    step.frame = 0;
    step.data = "";
    step.rotation = 0;
    step.location = {0,0};
    step.time_stamp = 0;
    Client client;
    client.connect("127.0.0.1");
    client.register_consumer();
    ready = true;
    auto t = Timer(3);
    while(!t.time_out()){
        step.location.x = (float) step.frame / 10;
        step.location.y = (float) step.frame / 20;
        step.time_stamp = ts.to_seconds();
        while (!ready);
        ready = false;
        server.send_step(step);
    }
    cout << client.counter << " messages processed over tcp" << endl;
    server.stop();

}

TEST_CASE("Local") {
    Timer ts;
    Tracking_server server;
    server.start(Tracking_service::get_port());
    Step step;
    step.agent_name = "predator";
    step.frame = 0;
    step.data = "";
    step.rotation = 0;
    step.location = {0,0};
    step.time_stamp = 0;
    auto  &client = server.create_local_client<Client>();
    client.connect("127.0.0.1");
    client.register_consumer();
    ready = true;
    auto t = Timer(3);
    while(!t.time_out()){
        step.location.x = (float) step.frame / 10;
        step.location.y = (float) step.frame / 20;
        step.time_stamp = ts.to_seconds();
        while (!ready);
        ready = false;
        server.send_step(step);
    }
    cout << client.counter << " messages processed locally" << endl;
    server.stop();

}