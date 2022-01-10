#include <agent_tracking/cv_service.h>
#include <agent_tracking.h>
using namespace tcp_messages;
using namespace std;

namespace agent_tracking {
    bool Cv_service::new_experiment(const std::string &occlusions) {
        cout << "new_experiment" << endl;
        send_update(Message("new_experiment", New_experiment_message{occlusions}));
        return true;
    }

    bool Cv_service::new_episode(New_episode_message nem) {
        cout << "new_episode" << endl;
        agent_tracking::new_episode(nem);
        return true;
    }

    bool Cv_service::end_episode() {
        cout << "end_episode" << endl;
        agent_tracking::end_episode();
        return true;
    }

    bool Cv_service::update_puff() {
        cout << "update_puff" << endl;
        agent_tracking::update_puff();
        return true;
    }
}