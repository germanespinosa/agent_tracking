#include <background.h>

using namespace habitat_cv;
Background::Background(const std::string &path):
        composite(Image::read(path, "composite.png").to_gray()),
        cameras(Images::read(path, {"background_0.png","background_1.png","background_2.png","background_3.png"}).to_gray()),
        path(path){
    
}

void Background::update(habitat_cv::Image new_composite, habitat_cv::Images new_cameras) {
    composite = new_composite;
    new_composite.save(path, "composite.png");
    cameras = new_cameras;
    new_cameras.save(path, {"background_0.png","background_1.png","background_2.png","background_3.png"});
}
