#ifndef _IMAGE_PROVIDER_CAMERA_H_
#define _IMAGE_PROVIDER_CAMERA_H_

#include "modules/camera-base/camera_base.h"
#include "modules/image-provider-base/image_provider_base.h"

class ImageProviderCamera final : public ImageProvider {
public:
    ImageProviderCamera() : camera_(nullptr) {}

    ~ImageProviderCamera() final;

    bool Initialize(const std::string &) final;

    inline bool GetFrame(Frame &frame) final { return camera_->GetFrame(frame); }

private:
    Camera *camera_;
};

#endif  // _IMAGE_PROVIDER_CAMERA_H_
