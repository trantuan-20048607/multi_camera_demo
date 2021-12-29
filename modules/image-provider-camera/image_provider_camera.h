#ifndef _IMAGE_PROVIDER_CAMERA_H_
#define _IMAGE_PROVIDER_CAMERA_H_

#include "modules/image-provider-base/image_provider_base.h"

class ImageProviderCamera final : public ImageProvider {
public:
    ~ImageProviderCamera() final = default;

    bool Initialize(const std::string &file_path) final {};

    bool GetFrame(Frame &) final {};
};

#endif  // _IMAGE_PROVIDER_CAMERA_H_
