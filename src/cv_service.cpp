#include <agent_tracking/cv_service.h>
#include <agent_tracking.h>
using namespace tcp_messages;
using namespace std;

namespace agent_tracking {
    void Cv_service::new_experiment(const std::string &occlusions) {
        cout << "new_experiment" << endl;
        send_update(Message("new_experiment", New_experiment_message{occlusions}));
        send_message(Message("new_experiment_result", "ok"));
    }

    void Cv_service::new_episode(New_episode_message nem) {
        cout << "new_episode" << endl;
        agent_tracking::new_episode(nem);
        send_message(Message("new_episode_result", "ok"));
    }

    void Cv_service::end_episode() {
        cout << "end_episode" << endl;
        agent_tracking::end_episode();
        send_message(Message("end_episode_result", "ok"));
    }

    void Cv_service::update_background() {
        cout << "update_background" << endl;
        agent_tracking::initialize_background();
        send_message(Message("update_background_result", "ok"));
    }

    void Cv_service::reset_cameras() {
        cout << "reset_cameras" << endl;
        agent_tracking::reset_cameras();
        send_message(Message("reset_cameras_result", "ok"));
    }

    void Cv_service::update_puff() {
        cout << "update_puff" << endl;
        agent_tracking::update_puff();
        send_message(Message("update_puff_result", "ok"));
    }

    void Cv_service::show_occlusions(const string &occlusions) {
        cout << "show_occlusions" << endl;
        Service::set_occlusions(occlusions);
        agent_tracking::set_occlusions(occlusions);
        send_message(Message("show_occlusions_result", "ok"));
    }

    void Cv_service::hide_occlusions() {
        cout << "hide_occlusions" << endl;
        agent_tracking::set_occlusions("00_00");
        send_message(Message("hide_occlusions_result", "ok"));
    }
}