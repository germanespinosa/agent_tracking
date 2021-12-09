#include <agent_tracking/client.h>
#include <agent_tracking/service.h>
#include <stdexcept>

using namespace cell_world;
using namespace std;
using namespace agent_tracking;

namespace agent_tracking {

    bool Client::connect() {
        return connect("127.0.0.1");
    }

    bool Client::connect(const std::string &ip) {
        return Message_client::connect(ip, Service::get_port());
    }

    bool Client::new_episode(const std::string &experiment, int episode, const std::string &subject,
                             const std::string &occlusions, const std::string &destination_folder) {
        New_episode_message nem;
        nem.experiment = experiment;
        nem.episode = episode;
        nem.subject = subject;
        nem.occlusions = occlusions;
        nem.destination_folder = destination_folder;
        if (!send_message(Message("new_episode",nem))) return false;
        return wait_for_result("new_episode").body == "ok";
    }

    bool Client::end_episode() {
        if (!send_message(Message("end_episode"))) return false;
        return wait_for_result("end_episode").body == "ok";
    }

    bool Client::register_consumer() {
        if (!send_message(cell_world::Message("register_consumer"))) return false;
        registered_consumer = wait_for_result("register_consumer").body == "ok";
        return registered_consumer;
    }

    bool Client::reset_cameras() {
        if (!send_message(Message("reset_cameras"))) return false;
        return wait_for_result("reset_cameras").body == "ok";
    }

    bool Client::unregister_consumer() {
        if (!send_message(Message("unregister_consumer"))) return false;
        return wait_for_result("unregister_consumer").body == "ok";
    }

    bool Client::update_background() {
        if (!send_message(Message("update_background"))) return false;
        return wait_for_result("update_background").body == "ok";
    }

    bool Client::update_puff() {
        if (!send_message(Message("update_puff"))) return false;
        return wait_for_result("update_puff").body == "ok";
    }

    cell_world::Message Client::wait_for_result(const std::string &operation, double time_out) {
        Timer ts(time_out);
        std::string header = operation + "_result";
        while (!contains(header) && !ts.time_out());
        return get_message(header);
    }

    Client::Client() {

    }

    cell_world::Step &Client::get_current_state(const string &agent_name) {
        return current_states[agent_name];
    }

    void Client::process_step(const cell_world::Step &step) {
        current_states[step.agent_name] = step;
    }

    bool Client::contains_agent_state(const string &agent_name) {
        return current_states.count(agent_name) != 0;
    }

    Client::~Client() {

    }

    void Filtered_client::process_step(const cell_world::Step &step) {
        if (agent_filters.count(step.agent_name))
            agent_filters[step.agent_name] = filter_template;

        auto filtered_step = step;
        filtered_step.location = agent_filters[step.agent_name].filtered_location(current_states[step.agent_name].location, step.location);
        current_states[step.agent_name] = filtered_step;
    }

    Filtered_client::Filtered_client(const Filter &filter_template) : filter_template(filter_template) {
    }

    cell_world::Location Filter::filtered_location(const cell_world::Location &previous_location, const cell_world::Location &current_reading) {
        Location new_location = previous_location * complementary_filter + current_reading * (1 - complementary_filter);
        auto dist = previous_location.dist(new_location);
        if (dist < outlier_distance || outlier_count == -1 || outlier_count > (int)outlier_threshold) {
            outlier_count = 0;
            return new_location;
        }
        return previous_location;
    }
}