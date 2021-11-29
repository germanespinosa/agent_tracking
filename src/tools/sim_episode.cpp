#include <agent_tracking/client.h>
#include <cell_world.h>

using namespace cell_world;
using namespace std;
using namespace agent_tracking;


int main() {
    Timer ts;
    agent_tracking::Client client;
    client.connect();
    cout << "client connected" << endl;
    if (!client.register_consumer()) exit(1);
    cout << "consumer registered" << endl;
    if (!client.new_episode("TEST",18, "FPP1", "10_05", "/home/german")) exit(1);
    cout << "episode started" << endl;
    ts.reset();
    while (ts.to_seconds()<10){
        cout << "\rseconds: " << ts.to_seconds() << "     ";
        if(client.contains("step")) cout << client.get_message("step") << endl;
    }
    client.end_episode();
    cout << endl << "episode finished" << endl;
    client.unregister_consumer();
    cout << "consumer unregistered" << endl;
    client.disconnect();
    cout << "client disconnected" << endl;
    return 0;
}