#ifndef _HIK_CAMERA_H_
#define _HIK_CAMERA_H_

#include <string>

#include <opencv2/core.hpp>
#include "MvCameraControl.h"

#include "modules/camera-base/camera.h"
#include "3rdparty/easylogging++/easylogging++.h"
#include "modules/image-provider-base/buffer.h"

class HikCamera final : public Camera {
public:
    HikCamera() : device_(nullptr) {}

    HikCamera(const HikCamera &) = delete;

    HikCamera(HikCamera &&) = delete;

    HikCamera &operator=(const HikCamera &) = delete;

    HikCamera &operator=(const HikCamera &&) = delete;

    ~HikCamera() final = default;

    bool OpenCamera(const std::string &, const std::string &) final;

    bool StartStream() final;

    bool GetImage(cv::Mat &) final;

    bool StopStream() final;

    bool CloseCamera() final;

    inline bool IsConnected() final {
        if (device_ == nullptr) return false;
        return MV_CC_IsDeviceConnected(device_);
    }

    inline bool ExportConfigurationFile(const std::string &file_path) final {
        auto status_code = MV_CC_FeatureSave(device_, file_path.c_str());
        if (status_code != MV_OK) {
            LOG(INFO) << "Failed to save " << serial_number_ << "'s configuration to "
                      << file_path << " with error " << status_code << ".";
            return false;
        }
        LOG(INFO) << "Saved " << serial_number_ << "'s configuration to " << file_path << ".";
        return true;
    }

    inline bool ImportConfigurationFile(const std::string &file_path) final {
        auto status_code = MV_CC_FeatureLoad(device_, file_path.c_str());
        if (status_code != MV_OK) {
            LOG(INFO) << "Failed to save " << serial_number_ << "'s configuration to "
                      << file_path << " with error " << status_code << ".";
            return false;
        }
        LOG(INFO) << "Imported " << serial_number_ << "'s configuration to " << file_path << ".";
        return true;
    }

    inline bool SetExposureTime(uint32_t exposure_time) final {
        return SetExposureTimeHikImplementation((float) exposure_time);
    }

    inline bool SetGainValue(float gain) final {
        if (MV_CC_SetFloatValue(device_, "Gain", gain) != MV_OK) {
            LOG(ERROR) << "Failed to set " << serial_number_ << "'s gain to "
                       << std::to_string(gain) << ".";
            return false;
        }
        LOG(DEBUG) << "Set " << serial_number_ << "'s gain to "
                   << std::to_string(gain) << ".";
        return true;
    }

private:
    void *device_;

    static void __stdcall ImageCallbackEx(unsigned char *, MV_FRAME_OUT_INFO_EX *, void *);

    static void *DaemonThreadFunction(void *);

    inline bool SetExposureTimeHikImplementation(float exposure_time) {
        if (MV_CC_SetFloatValue(device_, "ExposureTime", exposure_time) != MV_OK) {
            LOG(ERROR) << "Failed to set " << serial_number_ << "'s exposure time to "
                       << std::to_string(exposure_time) << ".";
            return false;
        }
        LOG(DEBUG) << "Set " << serial_number_ << "'s exposure time to "
                   << std::to_string(exposure_time) << ".";
        return true;
    }
};

#endif  // _HIK_CAMERA_H_
