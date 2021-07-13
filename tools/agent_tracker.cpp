#include <habitat_cv.h>
#include <camera_array.h>
#include <input_handler.h>
#include <frame_rate.h>
#include <time_stamp.h>
#include <mask.h>
#include "opencv2/video.hpp"
#include <mutex>

using namespace habitat_cv;
using namespace cell_world;
using namespace std;
using namespace json_cpp;


struct Location_orientation {
    Point location{-1000,-1000};
    double theta;
};

struct  Colored_profile : Profile {
    Json_object_members(
            Add_member(agent_name);
            Add_member(area_lower_bound);
            Add_member(area_upper_bound);
            Add_member(color);
            );
    json_cpp::Json_vector<int> color;

    cv::Scalar get_color() {
        return cv::Scalar(color[0],color[1],color[2]);
    }
};

struct Colored_profile_list : Json_vector<Colored_profile> {
    Profile_list to_profiles (){
        Profile_list pl;
        for (Profile &profile : *this) {
            pl.push_back(profile);
        }
        return pl;
    }
};

struct Frame_detection : Json_object{
    Json_object_members(
            Add_member(frame);
            Add_member(time_stamp);
            Add_member(theta);
            Add_member(detection_coordinates);
            )
    unsigned int frame;
    double time_stamp;
    double theta;
    Detection_coordinates detection_coordinates;
};

Location_orientation get_location_orientation (Detection_location_list led_detections){
    double d1 = led_detections[0].location.dist(led_detections[1].location);
    double d2 = led_detections[1].location.dist(led_detections[2].location);
    double d3 = led_detections[2].location.dist(led_detections[0].location);

    Location back;
    Location front;
    if ( d1 < d2 && d1 < d3) {
        back = (led_detections[0].location + led_detections[1].location) ;
        back.x /= 2;
        back.y /= 2;
        front = led_detections[2].location;
    } else if ( d2 < d3 && d2 < d1) {
        back = (led_detections[1].location + led_detections[2].location) ;
        back.x /= 2;
        back.y /= 2;
        front = led_detections[0].location;
    } else {
        back = (led_detections[2].location + led_detections[0].location) ;
        back.x /= 2;
        back.y /= 2;
        front = led_detections[1].location;
    }
    Location_orientation lo;
    lo.location.x = ( front.x + back.x ) / 2;
    lo.location.y = ( front.y + back.y ) / 2;
    lo.theta = atan2( front.y - back.y, front.x - back.x);
    return lo;
}

string get_time(double time_stamp){
    ostringstream oss;
    unsigned int hour = (unsigned int)time_stamp / 60 / 60;
    unsigned int minute = (unsigned int)(time_stamp / 60) % 60;
    unsigned int second =(unsigned int)(time_stamp) % 60;
    unsigned int millisecond = (unsigned int)(time_stamp * 1000) % 1000;
    oss << setfill('0') << setw(2) << hour << ":"<<setw(2)<< minute<<":"<<setw(2)<<second<<"."<< setw(3)<< millisecond;
    return oss.str();
}

int main(int argc, char **argv){
    if (argc==1){
        cerr << "missing camera configuration parameter." << endl;
        exit(1);
    }
    string cam_config(argv[1]);
    Frame_rate fr;
    Profile_list led_profile;
    Profile led_p;
    led_p.agent_name = "led";
    led_p.area_lower_bound = 3;
    led_p.area_upper_bound = 100;
    led_profile.push_back(led_p);

    Camera_order co = Json_from_file<Camera_order>("/maze/agent_tracking/config/camera_order.config");
    auto cas = Json_from_file<Cameras_associations>("/maze/agent_tracking/config/associations.config");
    auto profiles = Json_from_file<Colored_profile_list>("/maze/agent_tracking/config/profiles.config").to_profiles();
    Camera_array ca("/maze/agent_tracking/config/EPIX_" + cam_config + ".fmt",co);
    string bg_path = "/maze/agent_tracking/backgrounds/" + cam_config + "/composite.png";
    cv::Mat back_ground = cv::imread(bg_path);
    Time_stamp ts;
    auto size = cv::Size(980,862);
    unsigned int title_row = 35;
    auto video_size = cv::Size(980,862 + title_row);

    cv::Rect rect_t(0,0, size.width, title_row);
    cv::Rect rect_s(0,0, size.width, size.height);
    cv::Rect rect_d(0, title_row, size.width,size.height );

    int fourcc = cv::VideoWriter::fourcc('m', 'p', '4', 'v');
    cv::VideoWriter writer;
    Detection detection;
    Composite composite(size, co, cas);
    Input_handler input;
    vector<Detection_coordinates> agents_coordinates;
    for (auto&profile:profiles){
        Detection_coordinates dc;
        dc.coordinates=Coordinates {-1000,-1000};
        dc.detection_location.profile = profile;
        agents_coordinates.push_back(dc);
    }
    habitat_cv::Cleaner cleaner(35, 2);
    habitat_cv::Bg_subtraction bg_subtraction;
    cv::Mat screen_s;
    cv::Mat screen(size.height + title_row, size.width,CV_8UC3, cv::Scalar(0,0,0));
    Mask mask(size);
    bg_subtraction.set_background(back_ground);
    int frame = -1;
    string experiment = "";
    Location_orientation robot_location_orientation;
    while (true) {
        fr.new_frame();
        ca.capture();
        double time_stamp = ts.to_seconds();
        auto comp = composite.get_composite(ca.images).clone();
        if (input.ready){
            auto s = input.get_string();
            if ( s == "exit"){
                break;
            } else if (s == "end"){
                writer.release();
                ca.reset();
                frame = -1;
            } else if (s == "update_background"){
                cv::imwrite(bg_path, composite.composite);
                bg_subtraction.set_background(composite.composite);
            } else{
                experiment = s.substr(s.find_last_of('/') + 1,255);
                string filename = s + ".mp4";
                writer = cv::VideoWriter (filename, fourcc, 30, video_size, true);
                frame = 0;
                ts.reset();
            }
        }

        auto sub = bg_subtraction.subtract(comp);
        auto clean = cleaner.clean(sub);

        cv::Mat leds = (comp > 200 ) & mask.mask ;
        auto leds_in_image = detection.get_detections(leds,led_profile);

        unsigned int ic = 0;
        while (leds_in_image.size() != 3 && ic < 4) {
            leds = (composite.warped[ic++] > 200) & mask.mask;
            leds_in_image = detection.get_detections(leds, led_profile);
        }

        auto &masked = mask.apply(clean);
        cvtColor(comp, screen_s, cv::COLOR_GRAY2RGB);
        screen_s(rect_s).copyTo(screen(rect_d));
        screen(rect_t) = 0;

        if (leds_in_image.size() == 3){
            robot_location_orientation = get_location_orientation(leds_in_image);
            Frame_detection fd;
            fd.detection_coordinates.detection_location.profile.agent_name = "robot";
            fd.frame = frame;
            fd.time_stamp = time_stamp;
            fd.detection_coordinates.detection_location.location = robot_location_orientation.location;
            fd.theta = robot_location_orientation.theta;
            auto center = robot_location_orientation.location.to<cv::Point>();
            auto robot_coordinates = composite.get_coordinates(center);
            fd.detection_coordinates.coordinates = robot_coordinates;
            fd.detection_coordinates.detection_location.location.x /= size.width;
            fd.detection_coordinates.detection_location.location.y /= size.height;
            composite.draw_circle(screen,center,4,cv::Scalar(0,255,0));
            composite.draw_arrow(screen,center,fd.theta,cv::Scalar(0,255,0));
            composite.draw_cell(screen,robot_coordinates,cv::Scalar(0,255,0));
            cout << fd << endl;
        }

        auto detections = detection.get_detections(masked, profiles);
        for (auto &d:detections) {
            if (d.location.dist(robot_location_orientation.location) > 45) {
                auto center = d.location.to<cv::Point>();
                auto coordinates = composite.get_coordinates(center);
                Frame_detection fd;
                fd.frame = frame;
                fd.time_stamp = time_stamp;
                fd.detection_coordinates.detection_location = d;
                fd.detection_coordinates.coordinates = coordinates;
                fd.detection_coordinates.detection_location.location.x /= size.width;
                fd.detection_coordinates.detection_location.location.y /= size.height;
                fd.theta = 0;
                composite.draw_circle(screen, center, 4, cv::Scalar(255, 0, 0));
                composite.draw_cell(screen, coordinates, cv::Scalar(255, 0, 0));
                cout << fd << endl;
            }
        }

        if (frame >= 0 ) {
            frame++;
            cv::putText(screen,
                        get_time(time_stamp).c_str(),
                        cv::Point(size.width - 180,title_row / 3 * 2), // Coordinates
                        cv::FONT_HERSHEY_DUPLEX, // Font
                        .8, // Scale. 2.0 = 2x bigger
                        cv::Scalar(255,255,255), // BGR Color
                        1 // Line Thickness (Optional)
            );
            int baseline=0;
            auto text = to_string(frame).c_str();
            auto textsize = cv::getTextSize(text, cv::FONT_HERSHEY_DUPLEX, .8, 1,&baseline);
            cv::putText(screen,
                        text,
                        cv::Point(size.width - 200 - textsize.width,title_row / 3 * 2), // Coordinates
                        cv::FONT_HERSHEY_DUPLEX, // Font
                        .8, // Scale. 2.0 = 2x bigger
                        cv::Scalar(255,255,255), // BGR Color
                        1 // Line Thickness (Optional)
            );

            cv::putText(screen,
                        experiment.c_str(),
                        cv::Point(5,title_row / 3 * 2), // Coordinates
                        cv::FONT_HERSHEY_DUPLEX, // Font
                        .8, // Scale. 2.0 = 2x bigger
                        cv::Scalar(255,255,255), // BGR Color
                        1 // Line Thickness (Optional)
            );
            writer.write(screen);
        }
        cv::imshow("leds",leds);
        cv::imshow("sub",sub);
        cv::imshow("clean",masked);
        cv::imshow("screen",screen);
        if (cv::waitKey(1) == 'Q') break;
    }
    writer.release();
    exit(0);
}