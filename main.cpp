#include <unistd.h>

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include "3rdparty/easylogging++/easylogging++.h"

#include "modules/camera-base/camera_factory.h"

int main() {
    auto *hik_cam = CF_CREATE_CAMERA("HikCamera");
    auto *dh_cam = CF_CREATE_CAMERA("DHCamera");

    el::Loggers::addFlag(el::LoggingFlag::ColoredTerminalOutput);
    el::Loggers::addFlag(el::LoggingFlag::MultiLoggerSupport);

    LOG(INFO) << "Waiting for camera...";

    while (!hik_cam->OpenCamera("00D59818552", "../config/cameras/MV-CA003-21UC_00D59818552.mfs"))
        sleep(1);

    while (!dh_cam->OpenCamera("KE0210010102", "../config/cameras/KE0210010102.txt"))
        sleep(1);

    hik_cam->StartStream();
    dh_cam->StartStream();
    Frame hik_frame, dh_frame;
    do {
        if (hik_cam->GetFrame(hik_frame)) {
            cv::putText(hik_frame.image,
                        "TIME STAMP: " + std::to_string(hik_frame.time_stamp),
                        cv::Point(0, 24),
                        cv::FONT_HERSHEY_DUPLEX,
                        1,
                        cv::Scalar(0, 255, 0),
                        1,
                        false);
            cv::imshow("HikCam", hik_frame.image);
        }

        if (dh_cam->GetFrame(dh_frame)) {
            cv::putText(dh_frame.image,
                        "TIME STAMP: " + std::to_string(dh_frame.time_stamp),
                        cv::Point(0, 24),
                        cv::FONT_HERSHEY_DUPLEX,
                        1,
                        cv::Scalar(0, 255, 0),
                        1,
                        false);
            cv::imshow("DHCam", dh_frame.image);
        }
    } while (cv::waitKey(1) != 'q');
    hik_frame.image.release();
    hik_cam->StopStream();
    hik_cam->CloseCamera();

    dh_frame.image.release();
    dh_cam->StopStream();
    dh_cam->CloseCamera();
    return 0;
}
