#include <thread>
#include <agent_tracking.h>
#include <habitat_cv.h>
#include <cell_world/timer.h>
#include <agent_tracking/tracking_service.h>
#include <agent_tracking/background.h>
#include <agent_tracking/camera_array.h>
#include <agent_tracking/frame_rate.h>
#include <agent_tracking/layouts.h>

#define SAFETY_MARGIN 75
#define PUFF_DURATION 15

using namespace habitat_cv;
using namespace cell_world;
using namespace easy_tcp;
using namespace std;
using namespace tcp_messages;

namespace agent_tracking {

    Timer ts;
    std::atomic<bool> tracking_running = false;
    Camera_array *cameras = nullptr;

    Camera_configuration camera_configuration = Resources::from("camera_configuration").key(
            "default").get_resource<Camera_configuration>();
    Composite composite(camera_configuration, 1);

    auto led_profile = Resources::from("profile").key("led").get_resource<Profile>();
    auto mouse_profile = Resources::from("profile").key("mouse").get_resource<Profile>();

    atomic<int> puff_state;
    Cell_group occlusions;

    Background background;
    Screen_layout screen_layout;
    Main_layout main_layout;
    Raw_layout raw_layout;

    Video main_video(main_layout.size(), Image::rgb);
    Video raw_video(raw_layout.size(), Image::gray);
    Video mouse_video(raw_layout.size(), Image::gray);

    unsigned int robot_threshold = 240;


    void set_camera_file(const std::string &file_path) {
        if (cameras) delete cameras;
        cameras = new Camera_array(file_path, camera_configuration.order.count());
    }

    bool get_mouse_step(const Image &diff, Step &step, const Location &robot_location) {
        auto mouse_candidates = Detection_list::get_detections(diff, 55, 2).filter(mouse_profile);
        for (auto &mouse: mouse_candidates) {
            if (mouse.location.dist(robot_location) < SAFETY_MARGIN) continue;
            step.agent_name = "prey";
            step.location = mouse.location;
            return true;
        }
        return false;
    }

    bool get_robot_step(const Image &image, Step &step) {
        auto leds = Detection_list::get_detections(image, robot_threshold, 0).filter(led_profile);
        if (leds.size() != 3) return false;
        double d1 = leds[0].location.dist(leds[1].location);
        double d2 = leds[1].location.dist(leds[2].location);
        double d3 = leds[2].location.dist(leds[0].location);
        Location back;
        Location front;
        if (d1 < d2 && d1 < d3) {
            back = (leds[0].location + leds[1].location);
            back.x /= 2;
            back.y /= 2;
            front = leds[2].location;
        } else if (d2 < d3 && d2 < d1) {
            back = (leds[1].location + leds[2].location);
            back.x /= 2;
            back.y /= 2;
            front = leds[0].location;
        } else {
            back = (leds[2].location + leds[0].location);
            back.x /= 2;
            back.y /= 2;
            front = leds[1].location;
        }
        step.location.x = (front.x + back.x) / 2;
        step.location.y = (front.y + back.y) / 2;
        step.rotation = -to_degrees(atan2(front.y - back.y, front.x - back.x) - M_PI / 2);
        step.agent_name = "predator";
        return true;
    }

    void initialize_background() {
        cameras->capture();
        auto composite_image = composite.get_composite(cameras->images);
        background.update(composite_image, composite.warped);
    }

#define NOLOCATION Location(-1000,-1000)
    enum Screen_image {
        main,
        difference,
        led,
        led0,
        led1,
        led2,
        led3,
        raw,
        mouse,
        cam0,
        cam1,
        cam2,
        cam3,
        warped0,
        warped1,
        warped2,
        warped3,
        composite_image,
        large
    };

    void tracking_process() {
        Frame_rate fr;
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
            fr.new_frame();
            cout << fr.filtered_fps << "                   \r";
            Timer p;
            auto composite_image_gray = composite.get_composite(cameras->images);
            cout << "this is the time " << p.to_seconds() * 1000 << endl;
            auto composite_image_rgb = composite_image_gray.to_rgb();

            auto diff = composite_image_gray.diff(background.composite);

            if (robot_best_cam == -1) {
                new_robot_data = get_robot_step(composite_image_gray, robot);
            } else {
                new_robot_data = get_robot_step(composite.warped[robot_best_cam], robot);
                if (!new_robot_data) {
                    new_robot_data = get_robot_step(composite_image_gray, robot);
                }
            }
            unsigned int frame_number = 0;
            if (main_video.is_open()) {
                frame_number = main_video.frame_count;
            }
            if (new_robot_data) {
                if ((robot.location.x < 500 || robot.location.x > 580) &&
                    (robot.location.y < 500 || robot.location.y > 580)) {
                    int cam_row = robot.location.y > 540 ? 0 : 1;
                    int cam_col = robot.location.x > 540 ? 1 : 0;
                    robot_best_cam = camera_configuration.order[cam_row][cam_col];
                }
                auto color_robot = cv::Scalar({255, 0, 255});
                if (puff_state) {
                    robot.data = "puff";
                    color_robot = cv::Scalar({0, 0, 255});
                    puff_state--;
                } else {
                    robot.data = "";
                }
                thread([frame_number](Step &robot, Composite &composite, Timer &ts){
                    auto cell_id = composite.map.cells.find(robot.location);
                    robot.coordinates = composite.map.cells[cell_id].coordinates;
                    robot.time_stamp = ts.to_seconds();
                    robot.frame = frame_number;
                    Service::send_step(robot);
                }, reference_wrapper(robot), reference_wrapper(composite), reference_wrapper(ts)).detach();

                composite_image_rgb.circle(robot.location, 5, color_robot, true);
                auto cell_polygon = composite.get_polygon(robot.coordinates);
                composite_image_rgb.polygon(cell_polygon, color_robot);
                composite_image_rgb.arrow(robot.location, to_radians(robot.rotation), 50, color_robot);

                robot_counter = 30;
            } else {
                if (robot_counter) robot_counter--;
                else robot.location = NOLOCATION;
            }
            if (get_mouse_step(diff, mouse, robot.location)) {
                thread([frame_number](Step &mouse, Composite &composite, Timer &ts){
                    auto cell_id = composite.map.cells.find(mouse.location);
                    mouse.coordinates = composite.map.cells[cell_id].coordinates;
                    mouse.time_stamp = ts.to_seconds();
                    mouse.frame = frame_number;
                    Service::send_step(mouse);
                }, reference_wrapper(mouse), reference_wrapper(composite), reference_wrapper(ts)).detach();

                composite_image_rgb.circle(mouse.location, 5, {255, 0, 0}, true);
                auto cell_polygon = composite.get_polygon(mouse.coordinates);
                composite_image_rgb.polygon(cell_polygon, {255, 0, 0});
            }

            auto main_frame = main_layout.get_frame(composite_image_rgb, frame_number);

            auto raw_frame = raw_layout.get_frame(cameras->images);
            Location l1 = (mouse.location - Location(50, 50 )) * composite.resize_factor;
            if (l1.x < 0) l1.x = 0;
            if (l1.y < 0) l1.y = 0;
            if (l1.x > 980 * composite.resize_factor) l1.x = 980 * composite.resize_factor;
            if (l1.y > 980 * composite.resize_factor) l1.y = 980 * composite.resize_factor;
            Images mouse_cut;
            for (auto &image: composite.warped) {
                Content_crop cut(l1, l1 + Location(100 , 100) * composite.resize_factor, Image::Type::gray);
                cut = image;
                mouse_cut.emplace_back(cut);
            }

            auto mouse_frame = raw_layout.get_frame(mouse_cut);
            Image screen_frame;
            switch (screen_image) {
                case Screen_image::main :
                    for (auto &occlusion: occlusions) {
                        composite_image_rgb.circle(occlusion.get().location, 10, {0, 255, 0}, true);
                    }
                    screen_frame = screen_layout.get_frame(composite_image_rgb, "main");
                    break;
                case Screen_image::difference :
                    screen_frame = screen_layout.get_frame(diff, "difference");
                    break;
                case Screen_image::led :
                    if (robot_best_cam == -1)
                        screen_frame = screen_layout.get_frame(Image(cv::Mat(composite_image_gray > robot_threshold),""), "LEDs");
                    else
                        screen_frame = screen_layout.get_frame(Image(cv::Mat(composite.warped[robot_best_cam] > robot_threshold),""), "LEDs");
                    break;
                case Screen_image::led0 :
                    screen_frame = screen_layout.get_frame(Image(cv::Mat(composite.warped[0] > robot_threshold),""), "LED0");
                    break;
                case Screen_image::led1 :
                    screen_frame = screen_layout.get_frame(Image(cv::Mat(composite.warped[1] > robot_threshold),""), "LED1");
                    break;
                case Screen_image::led2 :
                    screen_frame = screen_layout.get_frame(Image(cv::Mat(composite.warped[2] > robot_threshold),""), "LED2");
                    break;
                case Screen_image::led3 :
                    screen_frame = screen_layout.get_frame(Image(cv::Mat(composite.warped[3] > robot_threshold),""), "LED3");
                    break;
                case Screen_image::mouse :
                    screen_frame = screen_layout.get_frame(mouse_frame, "mouse");
                    break;
                case Screen_image::raw :
                    screen_frame = screen_layout.get_frame(raw_frame, "raw");
                    break;
                case Screen_image::cam0 :
                    screen_frame = screen_layout.get_frame(cameras->images[0], "cam0");
                    break;
                case Screen_image::cam1 :
                    screen_frame = screen_layout.get_frame(cameras->images[1], "cam1");
                    break;
                case Screen_image::cam2 :
                    screen_frame = screen_layout.get_frame(cameras->images[2], "cam2");
                    break;
                case Screen_image::cam3 :
                    screen_frame = screen_layout.get_frame(cameras->images[3], "cam3");
                    break;
                case Screen_image::warped0 :
                    screen_frame = screen_layout.get_frame(composite.warped[0], "warped0");
                    break;
                case Screen_image::warped1 :
                    screen_frame = screen_layout.get_frame(composite.warped[1], "warped1");
                    break;
                case Screen_image::warped2 :
                    screen_frame = screen_layout.get_frame(composite.warped[2], "warped2");
                    break;
                case Screen_image::warped3 :
                    screen_frame = screen_layout.get_frame(composite.warped[3], "warped3");
                    break;
                case Screen_image::composite_image :
                    screen_frame = screen_layout.get_frame(composite.composite, "composite");
                    break;
                case Screen_image::large :
                    screen_frame = screen_layout.get_frame(composite.composite_large, "large");
                    break;
            }
            if (main_video.is_open()) screen_frame.circle({20, 20}, 10, {0, 0, 255}, true);
            cv::imshow("Agent Tracking", screen_frame);
            auto key = cv::waitKey(1);
            switch (key) {
                case 'V':
                    // start video recording
                    main_video.new_video("main.mp4");
                    raw_video.new_video("raw.mp4");
                    mouse_video.new_video("mouse.mp4");
                    break;
                case 'B':
                    // end video recording
                    main_video.close();
                    mouse_video.close();
                    raw_video.close();
                    break;
                case 'Q':
                    tracking_running = false;
                    break;
                case 'M':
                    screen_image = main;
                    break;
                case 'R':
                    reset_cameras();
                    break;
                case '[':
                    robot_threshold++;
                    cout << "robot threshold set to " << robot_threshold << endl;
                    break;
                case ']':
                    robot_threshold--;
                    cout << "robot threshold set to " << robot_threshold << endl;
                    break;
                case 'U':
                    initialize_background();
                    break;
                case '\t':
                    if (screen_image == Screen_image::large)
                        screen_image = Screen_image::main;
                    else
                        screen_image = static_cast<Screen_image>(screen_image + 1);
                    cout << "change_screen_output to " << screen_image << endl;
                    break;
                case ' ':
                    if (screen_image == Screen_image::main)
                        screen_image = Screen_image::large;
                    else
                        screen_image = static_cast<Screen_image>(screen_image - 1);
                    cout << "change_screen_output to " << screen_image << endl;
                    break;
            }

            if (mouse.location == NOLOCATION) continue; // starts recording when mouse crosses the door

            thread t([main_frame, raw_frame, mouse_frame]() {
                main_video.add_frame(main_frame);
                raw_video.add_frame(raw_frame);
                mouse_video.add_frame(mouse_frame);
            });
            t.detach();
            if (!main_video.is_open()) mouse.location = NOLOCATION;
            // write videos
        }

    }

    void new_episode(const New_episode_message &nem) {
        if (main_video.is_open()) end_episode();
        cout << "Video destination folder: " + nem.destination_folder << endl;
        main_layout.new_episode(nem.subject, nem.experiment, nem.episode, nem.occlusions);
        main_video.new_video(nem.destination_folder + "/main.mp4");
        raw_video.new_video(nem.destination_folder + "/raw.mp4");
        mouse_video.new_video(nem.destination_folder + "/mouse.mp4");
        ts.reset();
    }

    void set_background_path(const std::string &path) {
        background.set_path(path);
        if (!background.load()) {
            cameras->capture();
            composite.get_composite(cameras->images);
            background.update(composite.composite, composite.warped);
        }
    }

    void end_episode() {
        main_video.close();
        mouse_video.close();
        raw_video.close();
//reset_cameras();
    }

    void update_puff() {
        puff_state = PUFF_DURATION;
    }

    void reset_cameras() {
        cameras->reset();
    }

    void set_occlusions(const string &occlusions_name) {
        occlusions.clear();
        try {
            occlusions = composite.world.create_cell_group(
                    Resources::from("cell_group").key("hexagonal").key(occlusions_name).key(
                            "occlusions").get_resource<Cell_group_builder>());
        } catch (...) {}
    }

}