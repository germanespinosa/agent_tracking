//
// Created by german on 4/30/21.
//

#include <mask.h>

using namespace std;


cv::Mat &Mask::apply(const cv::Mat &img) {
    cv::bitwise_and(img,mask,masked);
    return masked;
}

Mask::Mask(cv::Size size) :
        mask(size.height, size.width,CV_8UC1, cv::Scalar(0,0,0)){
        auto center_x = size.width/2;
        auto center_y = size.height/2;
        double radius = center_x;
        vector<cv::Point> points;
        float c = M_PI / 3;
        for (float i=0;i<6;i++){
            float x = cos(i*c) * radius + center_x;
            float y = sin(i*c) * radius + center_y;
            points.emplace_back(x,y);
        }
        cv::fillPoly(mask,points,cv::Scalar(255,255,255));
    }

