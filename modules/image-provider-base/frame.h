#ifndef _FRAME_H_
#define _FRAME_H_

#include <opencv2/core/core.hpp>

struct Frame {
    cv::Mat image;
    uint64_t time_stamp;

    Frame(cv::Mat &_image, uint64_t _time_stamp) : image(_image), time_stamp(_time_stamp) {}

    Frame() : time_stamp(0) {}
};

#endif
