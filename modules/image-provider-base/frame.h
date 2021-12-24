#ifndef _FRAME_H_
#define _FRAME_H_

#include <opencv2/core/core.hpp>

struct Frame {
    cv::Mat image;
    unsigned long long time_stamp;

    Frame(cv::Mat &img, unsigned int time_stamp_low, unsigned int time_stamp_high) : image(img) {
        time_stamp = time_stamp_high;
        time_stamp <<= 32;
        time_stamp += time_stamp_low;
    }

    Frame() : time_stamp(0) {}
};

#endif
