#include <cell_world.h>
#include <mutex>
#include <agent_tracking.h>
#include <agent_tracking_service.h>

using namespace habitat_cv;
using namespace cell_world;
using namespace std;
using namespace json_cpp;
using namespace easy_tcp;

int main(int argc, char **argv){
    if (argc==1){
        cerr << "missing camera configuration parameter." << endl;
        exit(1);
    }
    Server<Agent_tracking_service> server;
    string cam_config = argv[1];
    string bg_path = "/maze/agent_tracking/backgrounds/" + cam_config + "/";
    string cam_file = "/maze/agent_tracking/config/EPIX_" + cam_config + ".fmt";
    Agent_tracking::set_camera_file(cam_file);
    Agent_tracking::set_background_path(bg_path);
    server.start(Agent_tracking_service::get_port());
    Agent_tracking::tracking_process();
    exit(0);
}