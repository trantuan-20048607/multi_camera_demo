#ifndef _IMAGE_PROVIDER_CAMERA_H_
#define _IMAGE_PROVIDER_CAMERA_H_

#include "modules/camera-base/camera_base.h"
#include "modules/image-provider-base/image_provider_factory.h"

/**
 * \brief Camera image provider class implementation.
 * \warning NEVER directly use this class to create image provider!  \n
 *   Instead, turn to ImageProviderFactory class and use IPF_CREATE_IMAGE_PROVIDER("IPCamera").
 */
class [[maybe_unused]] ImageProviderCamera final : public ImageProvider {
public:
    ImageProviderCamera() : camera_(nullptr) {}

    ~ImageProviderCamera() final;

    bool Initialize(const std::string &) final;

    inline bool GetFrame(Frame &frame) final { return camera_->GetFrame(frame); }

private:
    Camera *camera_;  /**< Camera pointer. */

    /** Own registry for image provider camera. */
    [[maybe_unused]] static ImageProviderRegistry<ImageProviderCamera> registry_;
};

#endif  // _IMAGE_PROVIDER_CAMERA_H_
