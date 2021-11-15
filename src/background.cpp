#include <cell_world.h>
#include <agent_tracking/background.h>

using namespace habitat_cv;
using namespace cell_world;

namespace agent_tracking {
    bool Background::update(habitat_cv::Image new_composite, habitat_cv::Images new_cameras) {
        new_composite.save(path, "composite.png");
        new_cameras.save(path, {"background_0.png", "background_1.png", "background_2.png", "background_3.png"});
        return load();
    }

    bool Background::load() {
        if (file_exists(path, {"composite.png", "background_0.png", "background_1.png", "background_2.png",
                               "background_3.png"})) {
            composite = Image::read(path, "composite.png").to_gray();
            cameras = Images::read(path, {"background_0.png", "background_1.png", "background_2.png",
                                          "background_3.png"}).to_gray();
            return true;
        }
        return false;
    }

    void Background::set_path(const std::string &new_path) {
        path = new_path;
    }
}