#include <catch.h>
#include <fstream>
#include <habitat_cv.h>
#include <agent_tracking/layouts.h>
#include <agent_tracking/camera_array.h>

using namespace std;
using namespace habitat_cv;
using namespace cell_world;
using namespace agent_tracking;


TEST_CASE("capture") {
    Camera_configuration camera_configuration = Resources::from("camera_configuration").key(
            "default").get_resource<Camera_configuration>();
    Composite composite(camera_configuration, 1);
    Camera_array cameras("../../config/EPIX_IRbest.fmt", 4);

    Timer timer;
    cameras.capture();
    composite.get_composite(cameras.images);
    cout << timer.to_seconds() * 1000 <<  endl;
}