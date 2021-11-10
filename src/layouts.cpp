#include <string>
#include <layouts.h>
#include <opencv2/opencv.hpp>

using namespace habitat_cv;
using namespace std;

Main_layout::Main_layout() :
    Layout(1125, 1080,Image::Type::rgb),
    composite({0.0,45.0}, {1080.0,1030.0}, Image::Type::rgb),
    date_time ({540,45},Image::Type::rgb,{255,255,255},{30,30,30},.8,0,1),
    occlusions({540,45},Image::Type::rgb,{255,255,255},{30,30,30},.8,2,1),
    subject ({540,45},Image::Type::rgb,{255,255,255},{30,30,30},.8,0,1),
    experiment ({540,45},Image::Type::rgb,{255,255,255},{30,30,30},.8,0,1),
    episode ({540,45},Image::Type::rgb,{255,255,255},{30,30,30},.8,2,1),
    frame ({540,45},Image::Type::rgb,{255,255,255},{30,30,30},.8,2,1){
    add_place_holder(composite, {0,0});
    add_place_holder(date_time, {0,1080});
    add_place_holder(subject, {0,1035});
    add_place_holder(experiment, {0,990});
    add_place_holder(occlusions, {540,1080});
    add_place_holder(episode, {540,1035});
    add_place_holder(frame, {540,990});
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


habitat_cv::Image Main_layout::get_frame(const Image &image, unsigned int frame_count) {
    composite = image;
    frame = "Frame: " + to_string(frame_count) + "  ";
    json_cpp::Json_date d = json_cpp::Json_date::now();
    auto dt = d.to_json();
    dt = dt.substr(1, dt.size()-2);
    date_time = "  " + dt;
    return get_image();
}

void Main_layout::new_episode(std::string subject_name, std::string experiment_identifier, int episode_number, std::string occlusions_name) {
    subject = "  Subject: " + subject_name;
    experiment = "  Experiment: " + experiment_identifier;
    episode = "Episode: " + to_string(episode_number) + "  ";
    occlusions = "World: " + occlusions_name + "  ";
}

Raw_layout::Raw_layout() :
    Layout(1080, 1080,Image::Type::gray),
    panel0({540,540},Image::Type::gray),
    panel1({540,540},Image::Type::gray),
    panel2({540,540},Image::Type::gray),
    panel3({540,540},Image::Type::gray)
    {
    add_place_holder(panel0,{0,0});
    add_place_holder(panel1,{540,0});
    add_place_holder(panel2,{0,540});
    add_place_holder(panel3,{540,540});
}

habitat_cv::Image Raw_layout::get_frame(const Images &images) {
    panel0 = images[0];
    panel1 = images[1];
    panel2 = images[2];
    panel3 = images[3];
    return get_image();
}

Screen_layout::Screen_layout():
    Layout(830,800,Image::rgb),
    screen({800,800},Image::rgb),
    screen_text({800,30},Image::rgb,{255,255,255},{30,30,30},1,1,1){
    add_place_holder(screen,{0,0});
    add_place_holder(screen_text,{0, 800});
}

habitat_cv::Image Screen_layout::get_frame(const Image &image, const string &text) {
    screen = image.to_rgb();
    screen_text = text;
    return get_image();
}