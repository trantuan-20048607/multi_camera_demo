#ifndef _IMAGE_PROVIDER_VIDEO_H_
#define _IMAGE_PROVIDER_VIDEO_H_

#include <opencv2/videoio.hpp>

#include "modules/image-provider-base/image_provider_factory.h"

/**
 * \brief Video image provider class implementation.
 * \warning NEVER directly use this class to create image provider!  \n
 *   Instead, turn to ImageProviderFactory class and use IPF_CREATE_IMAGE_PROVIDER("IPVideo").
 */
class ImageProviderVideo final : public ImageProvider {
public:
    ImageProviderVideo() = default;

    ~ImageProviderVideo() final;

    bool Initialize(const std::string &) final;

    inline bool GetFrame(Frame &frame) final { return video_.read(frame.image); }

private:
    cv::VideoCapture video_;  // Video object.

    // Own registry for image provider video.
    [[maybe_unused]] static ImageProviderRegistry<ImageProviderVideo> registry_;
};

#endif  // _IMAGE_PROVIDER_VIDEO_H_
