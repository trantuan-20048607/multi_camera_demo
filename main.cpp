#include <execution>

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include "3rdparty/easylogging++/easylogging++.h"

#include "modules/image-provider-camera/image_provider_camera.h"
#include "modules/image-provider-video/image_provider_video.h"

int main() {
    el::Loggers::addFlag(el::LoggingFlag::ColoredTerminalOutput);
    el::Loggers::addFlag(el::LoggingFlag::MultiLoggerSupport);

    ImageProvider *ip;

    std::cout << "Input 'c' or 'C' for camera mode, otherwise for video mode: " << std::flush;
    char c;
    std::cin >> c;
    std::cin.get();
    if (c == 'C' || c == 'c') {
        ip = new ImageProviderCamera;
        while (!ip->Initialize("../config/camera-init.yaml")) sleep(1);
    } else {
        ip = new ImageProviderVideo;
        ip->Initialize("../config/video-init.yaml");
    }

    for (Frame frame; cv::waitKey(1) != 'q';) {
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
    return 0;
}
