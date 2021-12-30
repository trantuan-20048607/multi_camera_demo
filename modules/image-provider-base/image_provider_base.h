#ifndef _IMAGE_PROVIDER_BASE_H_
#define _IMAGE_PROVIDER_BASE_H_

#include "frame.h"
#include "buffer.h"

/**
 * \brief Image provider base class.
 * \note You cannot directly construct objects.  \n
 *   Instead, find camera types in subclass documents,
 *   include image_provider_factory.h and use IPF_CREATE_IMAGE_PROVIDER macro.
 */
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
    cv::Mat intrinsic_mat_;   // Intrinsic matrix for PnP.
    cv::Mat distortion_mat_;  // Distortion matrix for PnP.
};

#endif  // _IMAGE_PROVIDER_BASE_H_
