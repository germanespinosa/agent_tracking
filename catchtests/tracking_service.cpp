#include <catch.h>
#include <agent_tracking/client.h>
#include <agent_tracking/service.h>
#include <cell_world.h>

using namespace agent_tracking;
using namespace cell_world;
using namespace std;
using namespace tcp_messages;

TEST_CASE("tracking simulator") {
    Timer ts;
    easy_tcp::Server<Service> tracker;
    tracker.start(Service::get_port());
    Step step;
    step.agent_name = "predator";
    step.frame = 0;
    step.data = "";
    step.rotation = 0;
    step.location = {0,0};
    step.coordinates = {0,0};
    step.time_stamp = 0;
    agent_tracking::Client c;
    c.connect();
    c.register_consumer();
    while (!c.contains_agent_state("predator")){
        step.frame++;
        step.time_stamp = ts.to_seconds();
        auto msg = Message(step.agent_name+"_step", step);
        Service::send_update(msg);
        Timer::wait(.5);
        cout << step << endl;
    }
    c.disconnect();
}