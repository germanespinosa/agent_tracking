#include <habitat_cv.h>

struct Background {
    void set_path(const std::string &);
    habitat_cv::Image composite;
    habitat_cv::Images cameras;
    bool load ();
    bool update (habitat_cv::Image, habitat_cv::Images);
    std::string path;
};