#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "modules/data-structures/circular_buffer.h"
#include "modules/data-structures/frame.h"

/**
 * \brief Camera base class.
 * \note You cannot directly construct objects.  \n
 *   Instead, find camera types in subclass documents,
 *   include camera_factory.h and use CF_CREATE_CAMERA macro.
 */
class Camera {
public:
    Camera() : stream_running_(false),
               daemon_thread_id_(0),
               stop_daemon_thread_flag_(false) {}

    virtual ~Camera() = default;

    /**
     * \brief Open a camera.
     * \param [in] serial_number Serial number of the camera you wanna open.
     * \param [in] config_file Will load config from this file.
     * \return A boolean shows whether the camera is successfully opened.
     */
    virtual bool OpenCamera(const std::string &serial_number, const std::string &config_file) = 0;

    /**
     * \brief Close the opened camera.
     * \return A boolean shows whether the camera is normally closed.
     * \attention No matter what is returned, the camera will not be valid.
     */
    virtual bool CloseCamera() = 0;

    /**
     * \brief Get a frame with image and time stamp from internal image buffer.
     * \param [out] frame Acquired frame will be stored here.
     * \return A boolean shows if buffer is not empty, or if you can successfully get an frame.
     */
    virtual bool GetFrame(Frame &frame) = 0;

    /**
     * \brief Start the stream.
     * \return Whether stream is started normally.
     * \attention This function will return false when stream is already started or camera is not opened.
     */
    virtual bool StartStream() = 0;

    /**
     * \brief Stop the stream.
     * \return Whether stream is stopped normally.
     * \attention This function will return false when stream is not started or camera is not opened.
     */
    virtual bool StopStream() = 0;

    /**
     * \brief Check if current device is connected.
     * \return A boolean shows current device is connected.
     */
    virtual bool IsConnected() = 0;

    /**
     * \brief Import current config to specified file.
     * \param [in] file_path File path.
     * \return A boolean shows if config is successfully imported.
     */
    virtual bool ImportConfigurationFile(const std::string &file_path) = 0;

    /**
     * \brief Export current config to specified file.
     * \param [in] file_path File path.
     * \return A boolean shows if config is successfully saved.
     */
    virtual bool ExportConfigurationFile(const std::string &file_path) = 0;

    /**
     * \brief Set exposure time.
     * \param exposure_time Exposure time, automatically converted to corresponding data type.
     * \return A boolean shows if exposure time is successfully set.
     */
    [[maybe_unused]] virtual bool SetExposureTime(uint32_t exposure_time) = 0;

    /**
     * \brief Set gain value.
     * \param gain Gain value, automatically converted to corresponding data type.
     * \return A boolean shows if gain value is successfully set.
     */
    [[maybe_unused]] virtual bool SetGainValue(float gain) = 0;

    /**
     * \brief Get serial number.
     * \return Serial number of this camera.
     */
    [[maybe_unused]] [[nodiscard]] inline std::string GetSerialNumber() { return serial_number_; }

protected:
    std::string serial_number_;             // Serial number.
    bool stream_running_;                   // Flag shows if stream is running.
    pthread_t daemon_thread_id_;            // Daemon thread id.
    bool stop_daemon_thread_flag_;          // Flag to stop daemon thread.
    CircularBuffer<Frame, IP_BUFFER_SIZE> buffer_;  // A ring buffer to store images.
};

#endif  // _CAMERA_H_
