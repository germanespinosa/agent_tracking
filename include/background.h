#include <habitat_cv.h>

struct Background {
    Background (const std::string &);
    habitat_cv::Image composite;
    habitat_cv::Images cameras;
    void update (habitat_cv::Image, habitat_cv::Images);
    std::string path;
};