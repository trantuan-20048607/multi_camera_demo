#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include "3rdparty/easylogging++/easylogging++.h"

#include "modules/image-provider-base/image_provider_factory.h"

int main() {
    ImageProvider *ip;

    std::cout << "Input 'c' or 'C' for camera mode, otherwise for video mode: " << std::flush;
    char c;
    std::cin >> c;
    std::cin.get();
    ip = IPF_CREATE_IMAGE_PROVIDER((c == 'C' || c == 'c') ? "IPCamera" : "IPVideo");
    if (!ip->Initialize((c == 'C' || c == 'c') ? "../config/camera-init.yaml" : "../config/video-init.yaml"))
        return 0;

    for (Frame frame; cv::waitKey(1) != 'q';) {

        // An example for timing a scope, only available in debug mode.
#ifdef ELPP_FEATURE_PERFORMANCE_TRACKING
        TIMED_SCOPE(main_loop_timed_obj, "main");
#endif

        if (ip->GetFrame(frame)) {
            if (frame.time_stamp)
                cv::putText(frame.image,
                            "TIME STAMP: " + std::to_string(frame.time_stamp),
                            cv::Point(0, 24),
                            cv::FONT_HERSHEY_DUPLEX,
                            1,
                            cv::Scalar(0, 255, 0),
                            1,
                            false);
            cv::imshow("CAM", frame.image);
        }
    }

    delete ip;
}
