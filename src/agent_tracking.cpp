#include <thread>
#include <utility>
#include <agent_tracking.h>
#include <habitat_cv.h>
#include <layouts.h>
#include <background.h>
#include <easy_tcp.h>
#include <time_stamp.h>
#include <message.h>

#define SAFETY_MARGIN 50
#define PUFF_DURATION 10

using namespace habitat_cv;
using namespace cell_world;
using namespace easy_tcp;
using namespace std;

std::atomic<bool> tracking_running = false;
Camera_array *cameras = nullptr;
int camera_count = 4 ;

Camera_configuration camera_configuration = Resources::from("camera_configuration").key("default").get_resource<Camera_configuration>();
Composite composite(camera_configuration);

auto led_profile = Resources::from("profile").key("led").get_resource<Profile>();
auto mouse_profile = Resources::from("profile").key("mouse").get_resource<Profile>();

vector<std::reference_wrapper<Service>> consumers;
vector<int> consumers_id;
atomic<int> puff_state;
Cell_group occlusions;

Background *background = nullptr;
Screen_layout screen_layout;
Main_layout main_layout;
Raw_layout raw_layout;
Video *main_video = nullptr;
Video *raw_video = nullptr;
Video *mouse_video = nullptr;


void send_update(const Step &step){
    for (auto &consumer:consumers){
        string message = Message("step",step.to_json()).to_json();
        try {
            consumer.get().send_data(message.c_str(), message.size() + 1);
        } catch(...) {}
    }
}

void Agent_tracking::set_camera_file(const std::string &file_path) {
    if (cameras) delete cameras;
    cameras = new Camera_array(file_path, camera_count);
}

bool get_mouse_step(const Image& diff, Step &step, const Location &robot_location){
    auto mouse_candidates = Detection_list::get_detections(diff,35, 2).filter(mouse_profile);
    for (auto &mouse:mouse_candidates) {
        if (mouse.location.dist(robot_location) < SAFETY_MARGIN) continue;
        step.agent_name = "mouse";
        step.location = mouse.location;
        return true;
    }
    return false;
}

bool get_robot_step(const Image& image, Step &step){
    auto leds = Detection_list::get_detections(image,200, 1).filter(led_profile);
    if (leds.size() !=3) return false;
    double d1 = leds[0].location.dist(leds[1].location);
    double d2 = leds[1].location.dist(leds[2].location);
    double d3 = leds[2].location.dist(leds[0].location);
    Location back;
    Location front;
    if ( d1 < d2 && d1 < d3) {
        back = (leds[0].location + leds[1].location) ;
        back.x /= 2;
        back.y /= 2;
        front = leds[2].location;
    } else if ( d2 < d3 && d2 < d1) {
        back = (leds[1].location + leds[2].location) ;
        back.x /= 2;
        back.y /= 2;
        front = leds[0].location;
    } else {
        back = (leds[2].location + leds[0].location) ;
        back.x /= 2;
        back.y /= 2;
        front = leds[1].location;
    }
    step.location.x = ( front.x + back.x ) / 2;
    step.location.y = ( front.y + back.y ) / 2;
    step.rotation = -to_degrees(atan2( front.y - back.y, front.x - back.x) - M_PI / 2);
    step.agent_name = "robot";
    return true;
}

void Agent_tracking::initialize_background() {
    cameras->capture();
    auto composite_image = composite.get_composite(cameras->images);
    background->update(composite_image, composite.warped);
    cameras->images.save(background->path,{"raw_0.png","raw_1.png","raw_2.png","raw_3.png"});
}

#define NOLOCATION Location(-1000,-1000)
enum Screen_image{
    main,
    difference,
    raw,
    mouse,
    cam0,
    cam1,
    cam2,
    cam3
};
void Agent_tracking::tracking_process ()
{
    Time_stamp ts;
    tracking_running = true;
    puff_state = false;
    Step mouse;
    mouse.location = NOLOCATION;
    Step robot;
    robot.location = NOLOCATION;
    int robot_counter = 0;
    ts.reset();
    int robot_best_cam = -1;
    bool new_robot_data;
    string screen_text;
    Screen_image screen_image = Screen_image::main;
    while (tracking_running) {
        cameras->capture();
        auto composite_image_gray = composite.get_composite(cameras->images);
        auto composite_image_rgb = composite_image_gray.to_rgb();
        auto diff = composite_image_gray.diff(background->composite);
        if (robot_best_cam == -1) {
            new_robot_data =get_robot_step(composite_image_gray, robot);
        } else {
            new_robot_data = get_robot_step(composite.warped[robot_best_cam], robot);
            if (!new_robot_data) {
                new_robot_data =get_robot_step(composite_image_gray, robot);
            }
        }
        unsigned int frame_number = 0;
        if (main_video) {
            frame_number = main_video->frame_count;
        }
        if ( new_robot_data ) {
            if ((robot.location.x < 500 || robot.location.x > 580) &&
                (robot.location.y < 500 || robot.location.y > 580)){
                int cam_row = robot.location.y > 540 ? 0 : 1;
                int cam_col = robot.location.x > 540 ? 1 : 0;
                robot_best_cam = camera_configuration.order[cam_row][cam_col];
            }
            auto cell_id = composite.map.cells.find(robot.location);
            robot.coordinates = composite.map.cells[cell_id].coordinates;
            robot.time_stamp = ts.to_seconds();
            robot.frame = frame_number;

            auto color_robot = cv::Scalar ({255,0,255});
            if (puff_state) {
                robot.data = "puff";
                color_robot = cv::Scalar ({0,0,255});
                puff_state--;
            }else{
                robot.data = "";
            }

            composite_image_rgb.circle(robot.location, 5, color_robot,true);
            auto cell_polygon = composite.get_polygon(robot.coordinates);
            composite_image_rgb.polygon(cell_polygon,color_robot);
            composite_image_rgb.arrow(robot.location, to_radians(robot.rotation), 50, color_robot);

            send_update(robot);
            robot_counter = 30;
        } else {
            if (robot_counter) robot_counter --;
            else robot.location = NOLOCATION;
        }
        if ( get_mouse_step(composite_image_gray, mouse, robot.location)) {
            auto cell_id = composite.map.cells.find(mouse.location);
            mouse.coordinates = composite.map.cells[cell_id].coordinates;
            mouse.time_stamp = ts.to_seconds();
            mouse.frame = frame_number;

            send_update(mouse);
            composite_image_rgb.circle(mouse.location, 5, {255,0,0},true);
            auto cell_polygon = composite.get_polygon(mouse.coordinates);
            composite_image_rgb.polygon(cell_polygon,{255,0,0});
        }
        auto main_frame = main_layout.get_frame(composite_image_rgb, frame_number);
        for (auto &occlusion:occlusions){
            composite_image_rgb.circle(occlusion.get().location,3,{0,128,255},true);
        }
        auto raw_frame = raw_layout.get_frame(cameras->images);
        Location l1 = mouse.location - Location(50,50);
        if (l1.x < 0) l1.x = 0;
        if (l1.y < 0) l1.y = 0;
        if (l1.x > 980) l1.x = 980;
        if (l1.y > 980) l1.x = 980;
        Images mouse_cut;
        for (auto &image: composite.warped) {
            Content_crop cut(l1,l1 + Location{100,100},Image::Type::gray);
            cut = image;
            mouse_cut.emplace_back(cut);
        }
        auto mouse_frame = raw_layout.get_frame(mouse_cut);
        Image screen_frame;
        switch (screen_image){
            case Screen_image::main :
                screen_frame = screen_layout.get_frame(composite_image_rgb, "main");
                break;
            case Screen_image::difference :
                screen_frame = screen_layout.get_frame(diff, "difference");
                break;
            case Screen_image::mouse :
                screen_frame = screen_layout.get_frame(mouse_frame, "mouse");
                break;
            case Screen_image::raw :
                screen_frame = screen_layout.get_frame(raw_frame, "raw");
                break;
            case Screen_image::cam0 :
                screen_frame = screen_layout.get_frame( cameras->images[0], "cam0");
                break;
            case Screen_image::cam1 :
                screen_frame = screen_layout.get_frame( cameras->images[1], "cam1");
                break;
            case Screen_image::cam2 :
                screen_frame = screen_layout.get_frame( cameras->images[2], "cam2");
                break;
            case Screen_image::cam3 :
                screen_frame = screen_layout.get_frame( cameras->images[3], "cam3");
                break;
        }
        if (main_video) screen_frame.circle({20,20},10,{0,0,255},true);
        cv::imshow("Agent Tracking", screen_frame);
        auto key = cv::waitKey(1);
        switch (key){
            case 'Q':
                tracking_running = false;
                break;
            case 'R':
                reset_cameras();
                break;
            case 'U':
                initialize_background();
                break;
            case '\t':
                if (screen_image == Screen_image::cam3)
                    screen_image = Screen_image::main;
                else
                    screen_image = static_cast<Screen_image>(screen_image + 1);
                cout << "change_screen_output to " << screen_image << endl;
                break;
        }

        if (mouse.location == NOLOCATION) continue; // starts recording when mouse crosses the door

        // write videos
        if (main_video) main_video->add_frame(main_frame);
        else mouse.location = NOLOCATION;
        if (raw_video) raw_video->add_frame(raw_frame);
        if (mouse_video) mouse_video->add_frame(mouse_frame);
    }

}

void Agent_tracking::new_episode(const New_episode_message &nem) {
    if (main_video) end_episode();
    occlusions.clear();
    try {
        occlusions = composite.world.create_cell_group(
                Resources::from("cell_group").key("hexagonal").key(nem.occlusions).key("occlusions").get_resource<Cell_group_builder>());
    } catch (...) {}
    cout << "Video destination folder: " + nem.destination_folder << endl;
    main_layout.new_episode(nem.subject, nem.experiment, nem.episode, nem.occlusions);
    main_video = new Video(nem.destination_folder + "/main.mp4", main_layout.size(), Image::rgb );
    raw_video = new Video(nem.destination_folder + "/raw.mp4", raw_layout.size(), Image::gray);
    mouse_video = new Video(nem.destination_folder + "/mouse.mp4", raw_layout.size(), Image::gray);
}

void Agent_tracking::set_background_path(const std::string &path) {
    if (background) delete background;
    background = new Background(path);
}

unsigned int consumers_count = 0;

int Agent_tracking::register_consumer(Service &consumer) {
    consumers.emplace_back(consumer);
    consumers_id.push_back(consumers_count);
    return consumers_count++;
}

void Agent_tracking::deregister_consumer(int consumer_id) {
    vector<std::reference_wrapper<Service>> new_consumers;
    vector<int> new_consumers_id;
    for (unsigned int i=0 ;i<consumers_id.size();i++){
        if (consumers_id[i] != consumer_id) {
            new_consumers.emplace_back(consumers[i]);
            new_consumers_id.emplace_back(consumers_id[i]);
        }
    }
    consumers = new_consumers;
    consumers_id = new_consumers_id;
}

void Agent_tracking::end_episode() {
    delete main_video;
    delete mouse_video;
    delete raw_video;
    main_video = nullptr;
    mouse_video = nullptr;
    raw_video = nullptr;
}

void Agent_tracking::update_puff() {
    puff_state = PUFF_DURATION;
}

void Agent_tracking::reset_cameras() {
    cameras->reset();
}




