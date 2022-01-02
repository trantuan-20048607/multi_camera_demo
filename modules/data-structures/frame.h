#ifndef _FRAME_H_
#define _FRAME_H_

#include <opencv2/core/core.hpp>

/**
 * \brief Single frame structure.
 * \details 2 ways of initializing method provided:  \n
 *   (Default) Directly use Frame() to initialize an empty and useless frame.  \n
 *   (Manual) Use Frame(_image, _time_stamp) to initialize a complete frame.
 */
struct Frame {
    cv::Mat image;        /**< OpenCV style image matrix. */
    uint64_t time_stamp;  /**< Time stamp in DEC nanoseconds. */

    Frame(cv::Mat &_image, uint64_t _time_stamp) : image(_image), time_stamp(_time_stamp) {}

    Frame() : time_stamp(0) {}
};

#endif  // _FRAME_H_
