#pragma once
#include <habitat_cv.h>

namespace agent_tracking {
    struct Main_layout : habitat_cv::Layout {
        Main_layout();

        void new_episode(std::string subject_name, std::string experiment_identifier, int episode_number,
                         std::string occlusions);

        habitat_cv::Content_crop composite;
        habitat_cv::Content_text date_time;
        habitat_cv::Content_text occlusions;
        habitat_cv::Content_text subject;
        habitat_cv::Content_text experiment;
        habitat_cv::Content_text episode;
        habitat_cv::Content_text frame;

        habitat_cv::Image get_frame(const habitat_cv::Image &, unsigned int frame_count);
    };

    struct Raw_layout : habitat_cv::Layout {
        Raw_layout();

        habitat_cv::Content_resize panel0;
        habitat_cv::Content_resize panel1;
        habitat_cv::Content_resize panel2;
        habitat_cv::Content_resize panel3;

        habitat_cv::Image get_frame(const habitat_cv::Images &);
    };

    struct Screen_layout : habitat_cv::Layout {
        Screen_layout();

        habitat_cv::Content_resize screen;
        habitat_cv::Content_text screen_text;

        habitat_cv::Image get_frame(const habitat_cv::Image &, const std::string &);
    };
}