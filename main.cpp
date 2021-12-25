#include <unistd.h>

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include "3rdparty/easylogging++/easylogging++.h"

#include "modules/camera-base/camera_factory.h"

int main() {
    auto *cam = CF_CREATE_CAMERA("DHCamera");

    el::Loggers::addFlag(el::LoggingFlag::ColoredTerminalOutput);
    el::Loggers::addFlag(el::LoggingFlag::MultiLoggerSupport);

    LOG(INFO) << "Waiting for camera...";

    while (!cam->OpenCamera("KE0210010102", "../config/cameras/KE0210010102.txt"))
        sleep(1);

    cam->StartStream();
    Frame frame;
    uint32_t frame_count = 0;
    uint32_t last_second_frame_cont = 0;
    time_t start_time = time(nullptr), end_time;
    for (uint32_t fps = 0; cv::waitKey(1) != 'q'; ++frame_count) {
        frame.image.release();
        if (!cam->GetFrame(frame)) {
            --frame_count;
            continue;
        }
        cv::putText(frame.image,
                    "FRAME:" + std::to_string(frame_count) + " FPS: " + std::to_string(fps),
                    cv::Point(0, 24),
                    cv::FONT_HERSHEY_DUPLEX,
                    1,
                    cv::Scalar(0, 255, 0),
                    1,
                    false);

        cv::putText(frame.image,
                    "TIME STAMP: " + std::to_string(frame.time_stamp),
                    cv::Point(0, 48),
                    cv::FONT_HERSHEY_DUPLEX,
                    1,
                    cv::Scalar(0, 255, 0),
                    1,
                    false);
        cv::imshow("CAM", frame.image);
        end_time = time(nullptr);
        if (end_time - start_time == 1) {
            fps = frame_count - last_second_frame_cont;
            last_second_frame_cont = frame_count;
            start_time = time(nullptr);
        } else if (end_time - start_time > 1) {
            last_second_frame_cont = frame_count;
            start_time = time(nullptr);
        }
    }
    frame.image.release();
    cam->StopStream();
    cam->CloseCamera();
    return 0;
}
