#include <cell_world.h>
#include <mutex>
#include <agent_tracking.h>
#include <easy_tcp.h>

using namespace habitat_cv;
using namespace cell_world;
using namespace std;
using namespace json_cpp;
using namespace agent_tracking;

int main(int argc, char **argv){
    if (argc==1){
        cerr << "missing camera configuration parameter." << endl;
        exit(1);
    }
    easy_tcp::Server<Service> server;
    string cam_config = argv[1];
    string bg_path = "/maze/agent_tracking/backgrounds/" + cam_config + "/";
    string cam_file = "/maze/agent_tracking/config/EPIX_" + cam_config + ".fmt";
    Service::set_world_configuration("hexagonal");
    Service::set_world_implementation("CV");
    agent_tracking::set_camera_file(cam_file);
    agent_tracking::set_background_path(bg_path);
    server.start(Service::get_port());
    agent_tracking::tracking_process();
    exit(0);
}