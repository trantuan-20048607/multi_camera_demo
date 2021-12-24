#include <string>
#include <unistd.h>

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

#include "3rdparty/easylogging++/easylogging++.h"
#include "camera_hik.h"

bool HikCamera::OpenCamera(const std::string &serial_number, const std::string &config_file) {
    // Will not check stream status for recovery.
    // if (!stream_running_) return false;
    if (device_ != nullptr) return false;

    // Enumerate device list.
    MV_CC_DEVICE_INFO_LIST device_list;
    memset(&device_list, 0, sizeof(MV_CC_DEVICE_INFO_LIST));
    auto status_code = MV_CC_EnumDevices(MV_GIGE_DEVICE | MV_USB_DEVICE, &device_list);

    if (MV_OK != status_code) {
        LOG(ERROR) << "Failed to enumerate devices with error code " << status_code << ".";
        return false;
    }

    // Find device with specified serial number.
    if (device_list.nDeviceNum > 0) {
        for (unsigned int i = 0; i < device_list.nDeviceNum; i++) {
            MV_CC_DEVICE_INFO *device_info = device_list.pDeviceInfo[i];
            std::string sn;

            // Check device transport layer protocol type. Only GigE and USB are supported.
            if (device_info->nTLayerType == MV_GIGE_DEVICE) {
                sn = std::string(reinterpret_cast<char *>(device_info->SpecialInfo.stGigEInfo.chSerialNumber));
                LOG(DEBUG) << "GigE device with serial number " << sn << " found.";
            } else if (device_info->nTLayerType == MV_USB_DEVICE) {
                sn = std::string(reinterpret_cast<char *>(device_info->SpecialInfo.stUsb3VInfo.chSerialNumber));
                LOG(DEBUG) << "USB device with serial number " << sn << " found.";
            } else {
                LOG(WARNING) << "Device with unsupported device transport layer protocol type " << sn << "found.";
            }

            // Compare sn with serial_number.
            if (sn == serial_number) {
                LOG(INFO) << "Found device with serial number " << serial_number << ".";

                // Create device handle.
                status_code = MV_CC_CreateHandle(&device_, device_info);
                if (MV_OK != status_code) {
                    LOG(ERROR) << "Failed to create device handle with error " << status_code << ".";
                    return false;
                }
                break;
            }
        }
    } else {
        LOG(ERROR) << "No device found.";
        return false;
    }

    if (device_ == nullptr) {
        LOG(ERROR) << "No device found.";
        return false;
    }

    // Open device.
    status_code = MV_CC_OpenDevice(device_, MV_ACCESS_Exclusive, 1);
    if (MV_OK != status_code) {
        LOG(ERROR) << "Failed to open device with error " << status_code << ".";
        status_code = MV_CC_DestroyHandle(device_);
        if (MV_OK != status_code)
            LOG(ERROR) << "Failed to destroy device handle with error " << status_code << ".";
        return false;
    }

    // Get device info.
    MV_CC_DEVICE_INFO device_info;
    status_code = MV_CC_GetDeviceInfo(device_, &device_info);
    if (MV_OK != status_code) {
        LOG(ERROR) << "Failed to get device info with error " << status_code << ".";
        status_code = MV_CC_CloseDevice(device_);
        if (MV_OK != status_code)
            LOG(ERROR) << "Failed to close device with error " << status_code << ".";
        status_code = MV_CC_DestroyHandle(device_);
        if (MV_OK != status_code)
            LOG(ERROR) << "Failed to destroy device handle with error " << status_code << ".";
        return false;
    }

    // (Only for GigE) Detect network optimal package size.
    if (device_info.nTLayerType == MV_GIGE_DEVICE) {
        int nPacketSize = MV_CC_GetOptimalPacketSize(device_);
        if (nPacketSize > 0) {
            status_code = MV_CC_SetIntValue(device_, "GevSCPSPacketSize", nPacketSize);
            if (status_code != MV_OK) {
                LOG(WARNING) << "Failed to set packet size with error " << status_code << ".";
            }
        } else {
            LOG(WARNING) << "Failed to set packet size with error " << status_code << ".";
        }
    }

    // Load configurations.
    if (!ImportConfigurationFile(config_file)) {
        status_code = MV_CC_CloseDevice(device_);
        if (MV_OK != status_code)
            LOG(ERROR) << "Failed to close device with error " << status_code << ".";
        status_code = MV_CC_DestroyHandle(device_);
        if (MV_OK != status_code)
            LOG(ERROR) << "Failed to destroy device handle with error " << status_code << ".";
        return false;
    }

    // Register image callback.
    status_code = MV_CC_RegisterImageCallBackEx(device_, ImageCallbackEx, this);
    if (MV_OK != status_code) {
        LOG(ERROR) << "Failed to register image callback with error " << status_code << ".";
        status_code = MV_CC_CloseDevice(device_);
        if (MV_OK != status_code)
            LOG(ERROR) << "Failed to close device with error " << status_code << ".";
        status_code = MV_CC_DestroyHandle(device_);
        if (MV_OK != status_code)
            LOG(ERROR) << "Failed to destroy device handle with error " << status_code << ".";
        return false;
    } else {
        LOG(DEBUG) << "Registered " << serial_number_ << "'s capture callback.";
    }

    // Create daemon thread.
    if (!daemon_thread_id_) {
        stop_daemon_thread_flag_ = false;
        pthread_create(&daemon_thread_id_, nullptr, DaemonThreadFunction, this);
        LOG(DEBUG) << serial_number_ << "'s daemon thread " << std::to_string(daemon_thread_id_) << " started.";
    }

    serial_number_ = serial_number;

    LOG(INFO) << "Opened camera " << serial_number << ".";
    return true;
}

bool HikCamera::StartStream() {
    if (device_ == nullptr) return false;

    // Save temporary configuration file to cache.
    ExportConfigurationFile("../cache/" + serial_number_ + ".txt");

    // Start stream.
    auto status_code = MV_CC_StartGrabbing(device_);
    if (MV_OK != status_code) {
        LOG(ERROR) << "Failed to start stream with error " << status_code << ".";
        return false;
    }

    stream_running_ = true;
    LOG(INFO) << serial_number_ << "'s stream started.";
    return true;
}

void HikCamera::ImageCallbackEx(unsigned char *image_data, MV_FRAME_OUT_INFO_EX *frame_info, void *obj) {
    auto self = (HikCamera *) obj;
    cv::Mat frame;

    if (frame_info->enPixelType == PixelType_Gvsp_RGB8_Packed) {
        frame = cv::Mat(frame_info->nHeight, frame_info->nWidth, CV_8UC3, image_data);
        self->buffer_.Push(frame);
    } else if (frame_info->enPixelType == PixelType_Gvsp_BayerRG8) {
        frame = cv::Mat(frame_info->nHeight, frame_info->nWidth, CV_8UC1, image_data);
        cv::cvtColor(frame, frame, cv::COLOR_BayerRG2RGB);
        self->buffer_.Push(frame);
    }
}

bool HikCamera::GetImage(cv::Mat &image) {
    if (device_ == nullptr) return false;
    if (!stream_running_) return false;

    if (!buffer_.Empty())
        image.release();
    return buffer_.Pop(image);
}

bool HikCamera::StopStream() {
    if (device_ == nullptr) return false;
    if (!stream_running_) return false;

    stream_running_ = false;

    // Stop stream.
    auto status_code = MV_CC_StopGrabbing(device_);
    if (MV_OK != status_code) {
        LOG(ERROR) << "Failed to stop stream with error " << status_code << ".";
        return false;
    }

    LOG(INFO) << serial_number_ << "'s stream stopped.";
    return true;
}

bool HikCamera::CloseCamera() {
    if (stream_running_) return false;
    if (device_ == nullptr) return false;

    // Stop daemon thread.
    stop_daemon_thread_flag_ = true;
    pthread_join(daemon_thread_id_, nullptr);
    stop_daemon_thread_flag_ = false;

    LOG(DEBUG) << serial_number_ << "'s daemon thread " << std::to_string(daemon_thread_id_) << " stopped.";

    // Close device.
    auto status_code = MV_CC_CloseDevice(device_);
    if (MV_OK != status_code)
        LOG(ERROR) << "Failed to close camera with error " << status_code << ".";

    // Destroy device handle.
    status_code = MV_CC_DestroyHandle(device_);
    if (MV_OK != status_code)
        LOG(ERROR) << "Failed to destroy handle with error " << status_code << ".";

    LOG(INFO) << "Closed camera " << serial_number_ << ".";

    // Clear cache.
    serial_number_ = "";
    device_ = nullptr;
    daemon_thread_id_ = 0;

    return true;
}

void *HikCamera::DaemonThreadFunction(void *obj) {
    auto self = (HikCamera *) obj;

    while (!self->stop_daemon_thread_flag_) {
        sleep(1);
        if (!MV_CC_IsDeviceConnected(self->device_)) {
            // Print information.
            LOG(ERROR) << self->serial_number_ << " is disconnected unexpectedly.";
            LOG(INFO) << "Preparing for reconnection...";

            // Stop the stream.
            MV_CC_StopGrabbing(self->device_);

            // Close device.
            MV_CC_CloseDevice(self->device_);

            // Clean cache.
            MV_CC_DestroyHandle(self->device_);
            self->device_ = nullptr;

            // Reopen camera.
            while (!self->OpenCamera(self->serial_number_, "../cache/" + self->serial_number_ + ".txt"))
                sleep(1);

            LOG(INFO) << self->serial_number_ << " is successfully reconnected.";

            if (self->stream_running_)
                self->StartStream();
        }
    }
    return nullptr;
}
