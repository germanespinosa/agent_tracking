#include <catch.h>
#include <agent_tracking/tracking_client.h>
#include <agent_tracking/tracking_service.h>
#include <cell_world.h>

using namespace agent_tracking;
using namespace cell_world;
using namespace std;
using namespace tcp_messages;
using namespace this_thread;

struct Client : Tracking_client{
    void on_step(const Step &step) override{
        cout << step << endl;
    }
};


TEST_CASE("tracking simulator") {
    Timer ts;
    Tracking_server server;
    server.start(Tracking_service::get_port());
    Step step;
    step.agent_name = "predator";
    step.frame = 0;
    step.data = "";
    step.rotation = 0;
    step.location = {0,0};
    step.coordinates = {0,0};
    step.time_stamp = 0;
    Client client;
    client.connect("127.0.0.1");
    client.register_consumer();
    sleep_for(1s);
    for (step.frame=0; step.frame<10000; step.frame++) {
        step.location.x = (float) step.frame / 10;
        step.location.y = (float) step.frame / 20;
        step.coordinates.x = step.frame % 10;
        step.coordinates.y = -(step.frame % 20);
        step.time_stamp = ts.to_seconds();
        Tracking_service::send_step(step);
        sleep_for(1s);
    }
    server.stop();
}