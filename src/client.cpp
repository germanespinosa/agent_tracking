#include <agent_tracking/client.h>
#include <agent_tracking/service.h>
#include <stdexcept>

using namespace cell_world;
using namespace std;
using namespace agent_tracking;
using namespace tcp_messages;

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
        return send_request(Message("new_episode",nem)).get_body<bool>();
    }

    bool Client::end_episode() {
        return send_request(Message("end_episode")).get_body<bool>();
    }

    bool Client::register_consumer() {
        registered_consumer = send_request(Message("end_episode")).get_body<bool>();
        return registered_consumer;
    }

    bool Client::unregister_consumer() {
        if (send_request(Message("end_episode")).get_body<bool>()) {
            registered_consumer = false;
            return true;
        }
        return false;
    }

    bool Client::update_puff() {
        return send_request(Message("update_puff")).get_body<bool>();
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

    World_info Client::get_world_info() {
        return send_request(Message("get_world_info")).get_body<World_info>();
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