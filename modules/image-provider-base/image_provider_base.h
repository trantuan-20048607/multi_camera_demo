#ifndef _IMAGE_PROVIDER_BASE_H_
#define _IMAGE_PROVIDER_BASE_H_

#include "frame.h"
#include "buffer.h"

class ImageProvider {
public:
    ImageProvider() = default;

    virtual ~ImageProvider() = default;

    ImageProvider(const ImageProvider &) = delete;

    ImageProvider &operator=(const ImageProvider &) = delete;

    /**
     * \brief Initialize by specified configuration file.
     * \param [in] file_path Configuration file path.
     * \return A boolean shows if initialization succeeded.
     */
    virtual bool Initialize(const std::string &file_path) = 0;

    /**
     * \brief Get a frame.
     * \param [out] frame OpenCV image reference.
     * \return A boolean shows if frame is complete.
     */
    virtual bool GetFrame(Frame &frame) = 0;

protected:
    cv::Mat intrinsic_mat_;
    cv::Mat distortion_mat_;
};

#endif  // _IMAGE_PROVIDER_BASE_H_
