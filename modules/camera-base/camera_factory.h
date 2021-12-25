#ifndef _CAMERA_FACTORY_H_
#define _CAMERA_FACTORY_H_

#include <string>
#include <unordered_map>

#include "camera.h"
#include "3rdparty/easylogging++/easylogging++.h"

#define CF_CREATE_CAMERA(camera_type_name) CameraFactory::Instance().CreateCamera(camera_type_name)

class CameraRegistryBase {
public:
    virtual Camera *CreateCamera() = 0;

    CameraRegistryBase(const CameraRegistryBase &) = delete;

    CameraRegistryBase &operator=(const CameraRegistryBase &) = delete;

protected:
    CameraRegistryBase() = default;

    virtual ~CameraRegistryBase() = default;
};

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

template<class CameraType>
class CameraRegistry final : public CameraRegistryBase {
public:
    explicit CameraRegistry<CameraType>(const std::string &camera_type_name) {
        CameraFactory::Instance().RegisterCamera(camera_type_name, this);
    }

    inline Camera *CreateCamera() final { return new CameraType(); }
};

#endif  //_CAMERA_FACTORY_H_
