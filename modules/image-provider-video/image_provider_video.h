#ifndef _IMAGE_PROVIDER_VIDEO_H_
#define _IMAGE_PROVIDER_VIDEO_H_

#include <opencv2/videoio.hpp>

#include "modules/image-provider-base/image_provider_base.h"

class ImageProviderVideo final : public ImageProvider {
public:
    ImageProviderVideo() = default;

    ~ImageProviderVideo() final;

    bool Initialize(const std::string &) final;

    inline bool GetFrame(Frame &frame) final { return video_.read(frame.image); }

private:
    cv::VideoCapture video_;
};

#endif  // _IMAGE_PROVIDER_VIDEO_H_
