#ifndef _DH_CAMERA_H_
#define _DH_CAMERA_H_

#include <string>

#include <opencv2/core.hpp>
#include "GxIAPI.h"
#include "DxImageProc.h"

#include "modules/camera-base/camera.h"
#include "3rdparty/easylogging++/easylogging++.h"
#include "modules/image-provider-base/buffer.h"
#include "modules/image-provider-base/frame.h"

/**
 * \brief This macro is used to check if the device is successfully initialized.
 * \attention !! DO NOT use this macro in other place !!
 */
#define GX_OPEN_CAMERA_CHECK_STATUS(status_code)          \
    if ((status_code) != GX_STATUS_SUCCESS) {             \
        LOG(ERROR) << GetErrorInfo(status_code);          \
        (status_code) = GXCloseDevice(device_);           \
        if ((status_code) != GX_STATUS_SUCCESS)           \
            LOG(ERROR) << GetErrorInfo(status_code);      \
        device_ = nullptr;                                \
        serial_number_ = "";                              \
        if (!camera_count_) {                             \
            (status_code) = GXCloseLib();                 \
            if ((status_code) != GX_STATUS_SUCCESS)       \
                LOG(ERROR) << GetErrorInfo(status_code);  \
        }                                                 \
    return false;                                         \
}

/**
 * \brief This macro is used to check if parameters are successfully modified or set.
 * \attention !! DO NOT use this macro in other place !!
 */
#define GX_CHECK_STATUS(status_code)           \
    if ((status_code) != GX_STATUS_SUCCESS) {  \
    LOG(ERROR) << GetErrorInfo(status_code);   \
    return false;                              \
}

/**
 * \brief This macro is used to check if the stream is successfully opened or closed.
 * \attention !! DO NOT use this macro in other place !!
 */
#define GX_START_STOP_STREAM_CHECK_STATUS(status_code)  \
if ((status_code) != GX_STATUS_SUCCESS) {               \
    if (raw_16_to_8_cache_ != nullptr) {                \
        delete[] raw_16_to_8_cache_;                    \
        raw_16_to_8_cache_ = nullptr;                   \
    }                                                   \
    if (raw_8_to_rgb_24_cache_ != nullptr) {            \
        delete[] raw_8_to_rgb_24_cache_;                \
        raw_8_to_rgb_24_cache_ = nullptr;               \
    }                                                   \
    LOG(ERROR) << GetErrorInfo(status_code);            \
    return false;                                       \
}

class DHCamera : public Camera {
public:
    DHCamera() : device_(nullptr),
                 color_filter_(GX_COLOR_FILTER_NONE),
                 payload_size_(0),
                 raw_8_to_rgb_24_cache_(nullptr),
                 raw_16_to_8_cache_(nullptr) {};

    DHCamera(const DHCamera &) = delete;

    DHCamera(DHCamera &&) = delete;

    DHCamera &operator=(const DHCamera &) = delete;

    DHCamera &operator=(const DHCamera &&) = delete;

    ~DHCamera() override = default;

    bool OpenCamera(const std::string &serial_number, const std::string &config_file) override;

    bool CloseCamera() override;

    bool StartStream() override;

    bool StopStream() override;

    bool IsConnected() override;

    inline bool GetImage(cv::Mat &image) override { return buffer_.Pop(image); }

    inline bool ExportConfigurationFile(const std::string &file_path) override {
        GX_STATUS status_code = GXExportConfigFile(device_, file_path.c_str());
        GX_CHECK_STATUS(status_code)
        LOG(INFO) << "Saved " << serial_number_ << "'s configuration to " << file_path << ".";
        return true;
    }

    inline bool ImportConfigurationFile(const std::string &file_path) override {
        GX_STATUS status_code = GXImportConfigFile(device_, file_path.c_str());
        GX_CHECK_STATUS(status_code)
        LOG(INFO) << "Imported " << serial_number_ << "'s configuration from " << file_path << ".";
        return true;
    }

    inline bool SetExposureTime(uint32_t exposure_time) override {
        if (!SetExposureMode(GX_EXPOSURE_MODE_TIMED)) {
            LOG(ERROR) << "Failed to set " << serial_number_ << "'s exposure mode to timed.";
            return false;
        }
        if (!SetExposureTimeMode(GX_EXPOSURE_TIME_MODE_STANDARD)) {
            LOG(ERROR) << "Failed to set " << serial_number_ << "'s exposure time mode to standard.";
            return false;
        }
        return SetExposureTimeDHImplementation((int64_t) exposure_time);
    }

    inline bool SetGainValue(float gain_value) override {
        if (!SetGainAuto(GX_GAIN_AUTO_OFF)) {
            LOG(ERROR) << "Failed to turn " << serial_number_ << "'s gain auto mode off.";
            return false;
        }
        return SetGainValueDHImplementation((double) gain_value);
    }

private:
    inline bool RegisterCaptureCallback(GXCaptureCallBack callback) {
        GX_STATUS status_code;
        status_code = GXRegisterCaptureCallback(device_,
                                                this,
                                                callback);
        GX_CHECK_STATUS(status_code)
        LOG(DEBUG) << "Registered " << serial_number_ << "'s capture callback.";
        return true;
    }

    inline bool UnregisterCaptureCallback() {
        GX_STATUS status_code;
        status_code = GXUnregisterCaptureCallback(device_);
        GX_CHECK_STATUS(status_code)
        LOG(DEBUG) << "Unregistered " << serial_number_ << "'s capture callback.";
        return true;
    }

    inline bool SetExposureTimeDHImplementation(int64_t exposure_time) {
        GX_STATUS status_code;
        status_code = GXSetEnum(device_,
                                GX_FLOAT_EXPOSURE_TIME,
                                exposure_time);
        GX_CHECK_STATUS(status_code)
        LOG(DEBUG) << "Set " << serial_number_ << "'s exposure time to "
                   << std::to_string(exposure_time) << ".";
        return true;
    }

    /*
     * GX_EXPOSURE_MODE_TIMED         = 1  Control exposure time through exposure time register
     * GX_EXPOSURE_MODE_TRIGGERWIDTH  = 2  Control exposure time through trigger signal width
     */
    inline bool SetExposureMode(GX_EXPOSURE_MODE_ENTRY gx_exposure_mode_entry) {
        GX_STATUS status_code;
        status_code = GXSetEnum(device_,
                                GX_ENUM_EXPOSURE_MODE,
                                gx_exposure_mode_entry);
        GX_CHECK_STATUS(status_code)
        LOG(DEBUG) << "Set " << serial_number_ << "'s exposure mode to "
                   << std::to_string(gx_exposure_mode_entry) << ".";
        return true;
    }

    /*
     * GX_EXPOSURE_TIME_MODE_ULTRASHORT  = 0  Ultra short mode
     * GX_EXPOSURE_TIME_MODE_STANDARD    = 1  Standard mode
     */
    inline bool SetExposureTimeMode(GX_EXPOSURE_TIME_MODE_ENTRY gx_exposure_time_mode_entry) {
        GX_STATUS status_code;
        status_code = GXSetEnum(device_,
                                GX_ENUM_EXPOSURE_TIME_MODE,
                                gx_exposure_time_mode_entry);
        GX_CHECK_STATUS(status_code)
        LOG(DEBUG) << "Set " << serial_number_ << "'s exposure time mode to "
                   << std::to_string(gx_exposure_time_mode_entry) << ".";
        return true;
    }

    inline bool SetGainValueDHImplementation(double gain) {
        GX_STATUS status_code;
        status_code = GXSetEnum(device_,
                                GX_ENUM_GAIN_SELECTOR,
                                GX_GAIN_SELECTOR_ALL);
        GX_CHECK_STATUS(status_code)
        status_code = GXSetFloat(device_,
                                 GX_FLOAT_GAIN,
                                 (float) gain);
        GX_CHECK_STATUS(status_code)
        double value = 0;
        status_code = GXGetFloat(device_,
                                 GX_FLOAT_GAIN,
                                 &value);
        GX_CHECK_STATUS(status_code)
        LOG(DEBUG) << "Set " << serial_number_ << "'s gain value to "
                   << std::to_string(gain) << ".";
        return true;
    }

    /*
     * Valid parameters:
     * GX_GAIN_AUTO_OFF         = 0             => Switch off automatic gain
     * GX_GAIN_AUTO_CONTINUOUS  = 1             => Continuous automatic gain
     * GX_GAIN_AUTO_ONCE        = 2             => Single automatic gain
     */
    inline bool SetGainAuto(GX_GAIN_AUTO_ENTRY gx_gain_auto_entry) {
        GX_STATUS status_code;
        status_code = GXSetEnum(device_,
                                GX_ENUM_GAIN_AUTO,
                                gx_gain_auto_entry);
        GX_CHECK_STATUS(status_code)
        LOG(DEBUG) << "Set " << serial_number_ << "'s gain to auto mode "
                   << std::to_string(gx_gain_auto_entry) << ".";
        return true;
    }

    /**
     * \brief Get error details for output.
     * \param error_status_code A status code returned from GxI API.
     * \return A string with error details.
     */
    inline static std::string GetErrorInfo(GX_STATUS error_status_code) {
        size_t str_size = 0;
        if (GXGetLastError(&error_status_code,
                           nullptr,
                           &str_size) != GX_STATUS_SUCCESS) {
            return "{?}{Unknown error}";
        }
        char *error_info = new char[str_size];
        if (GXGetLastError(&error_status_code,
                           error_info,
                           &str_size) != GX_STATUS_SUCCESS) {
            delete[] error_info;
            return "{?}{Unknown error}";
        }
        return error_info;
    }

    /**
     * \brief Convert RAW 8/16 pixel formats to an RGB 24 one.
     * \param [in] frame_callback A frame buffer acquired directly from camera.
     * \return A boolean shows whether pixel format is normally converted.
     */
    bool Raw8Raw16ToRGB24(GX_FRAME_CALLBACK_PARAM *frame_callback);

    /**
     * \brief Internal capture callback function.
     * \param [in] frame_callback Frame callback pointer.
     */
    static void GX_STDC DefaultCaptureCallback(GX_FRAME_CALLBACK_PARAM *frame_callback);

    /**
     * \brief Daemon thread main function.
     * \param [in] obj Camera itself.
     * \attention !! Do NOT use this function in another place !!
     */
    static void *DaemonThreadFunction(void *obj);

    static uint16_t camera_count_;

    GX_DEV_HANDLE device_;
    int64_t color_filter_;
    int64_t payload_size_;

    unsigned char *raw_8_to_rgb_24_cache_;
    unsigned char *raw_16_to_8_cache_;
};

#endif  // _DH_CAMERA_H_
