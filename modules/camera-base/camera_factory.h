#ifndef _CAMERA_FACTORY_H_
#define _CAMERA_FACTORY_H_

#include <unordered_map>

#include "3rdparty/easylogging++/easylogging++.h"

#include "camera_base.h"

/// \brief A macro to create a camera of specified type string.
///   For details, turn to class CameraFactory.
#define CF_CREATE_CAMERA(camera_type_name) CameraFactory::Instance().CreateCamera(camera_type_name)

/**
 * \brief Base class of camera registry.
 * \warning You should use its subclass CameraRegistry instead of this base class.
 */
class CameraRegistryBase {
public:
    virtual Camera *CreateCamera() = 0;

    CameraRegistryBase(const CameraRegistryBase &) = delete;

    CameraRegistryBase &operator=(const CameraRegistryBase &) = delete;

protected:
    CameraRegistryBase() = default;

    virtual ~CameraRegistryBase() = default;
};

/**
 * \brief Singleton camera factory.  \n
 *   Use CameraFactory::Instance() to get the only instance pointer.  \n
 *   Use macro CF_CREATE_CAMERA(registered_type) to create a camera instance.  \n
 *   Use CameraFactory::Instance::RegisterCamera(camera_type_name, *registry) to register a type of camera.
 * \warning
 *   Camera factory will not check whether CameraType is really subclass of Camera base class.
 *   (Thus, you should ensure that all callings of CameraRegistry constructor are completely under control.)
 */
class CameraFactory {
public:
    CameraFactory(const CameraFactory &) = delete;

    CameraFactory &operator=(const CameraFactory &) = delete;

    static CameraFactory &Instance() {
        static CameraFactory camera_factory;
        return camera_factory;
    }

    inline void RegisterCamera(const std::string &camera_type_name, CameraRegistryBase *registry) {
        camera_registry_[camera_type_name] = registry;
    }

    inline Camera *CreateCamera(const std::string &camera_type_name) {
        return camera_registry_.find(camera_type_name) != camera_registry_.end() ?
               camera_registry_[camera_type_name]->CreateCamera() : nullptr;
    }

private:
    CameraFactory() = default;

    ~CameraFactory() = default;

    std::unordered_map<std::string, CameraRegistryBase *> camera_registry_;
};

/**
 * \brief Templated camera registry class.
 * \tparam CameraType Camera type inherited from base class Camera.
 * \attention Once object is constructed, this type of camera will immediately be registered to camera factory.
 *   This means the constructed object is useless and should not appear in any other place.
 *   (Thus, template class though this is, it's better to be treated as a function.)
 * \warning
 *   Camera factory will not check whether CameraType is really subclass of Camera base class.
 *   (Thus, you should ensure that all callings of CameraRegistry constructor are completely under control.)
 */
template<class CameraType>
class CameraRegistry final : public CameraRegistryBase {
public:
    explicit CameraRegistry<CameraType>(const std::string &camera_type_name) {
        CameraFactory::Instance().RegisterCamera(camera_type_name, this);
    }

    inline Camera *CreateCamera() final { return new CameraType(); }
};

#endif  //_CAMERA_FACTORY_H_
