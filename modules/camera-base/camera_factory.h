#ifndef _CAMERA_FACTORY_H_
#define _CAMERA_FACTORY_H_

#include <unordered_map>

#include "3rdparty/easylogging++/easylogging++.h"

#include "camera_base.h"

/// \brief A macro to create a camera of specified type name. For details, turn to class CameraFactory.
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
 * \brief Singleton camera factory.
 * \details
 *   Use CameraFactory::Instance() to get the only instance pointer.  \n
 *   Use macro CF_CREATE_CAMERA(camera_type_name) to create a camera instance.  \n
 *   Use CameraFactory::Instance::RegisterCamera(camera_type_name, *registry) to register a type of camera.
 * \warning
 *   Camera factory will not check whether CameraType is really subclass of Camera base class.  \n
 *   (Thus, you should ensure that all callings of CameraRegistry constructor are completely under control.)
 */
class CameraFactory {
public:
    CameraFactory(const CameraFactory &) = delete;

    CameraFactory &operator=(const CameraFactory &) = delete;

    /**
     * \brief Get the only instance of camera factory.
     * \return A camera factory object.
     */
    static CameraFactory &Instance() {
        static CameraFactory camera_factory;
        return camera_factory;
    }

    /**
     * \brief Register a camera type.
     * \param [in] camera_type_name Type name of camera.
     * \param [in] registry A registry object of camera.
     * \warning You may call this function only when you're programming for a new type of camera.
     */
    inline void RegisterCamera(const std::string &camera_type_name, CameraRegistryBase *registry) {
        camera_registry_[camera_type_name] = registry;
    }

    /**
     * \brief Create a camera whose type is registered to factory.
     * \param [in] camera_type_name Type name of camera.
     * \return A pointer to crated camera.
     * \note You may use macro CF_CREATE_CAMERA(camera_type_name) instead of call this function.
     */
    inline Camera *CreateCamera(const std::string &camera_type_name) {
        if (camera_registry_.find(camera_type_name) != camera_registry_.end()) {
            LOG(DEBUG) << "Created " << camera_type_name << " object.";
            return camera_registry_[camera_type_name]->CreateCamera();
        } else {
            LOG(ERROR) << "Camera type " << camera_type_name << " not found.";
            return nullptr;
        }
    }

private:
    CameraFactory() = default;

    ~CameraFactory() = default;

    std::unordered_map<std::string, CameraRegistryBase *> camera_registry_;  // Camera registry map.
};

/**
 * \brief Templated camera registry class.
 * \tparam CameraType Camera type inherited from base class Camera.
 * \attention Once object is constructed, this type of camera will immediately be registered to camera factory.  \n
 *   This means the constructed object is useless and should not appear in any other place.  \n
 *   (Thus, template class though this is, it's better to be treated as a function.)
 * \warning
 *   Camera factory will not check whether CameraType is really subclass of Camera base class.  \n
 *   (Thus, you should ensure that all callings of CameraRegistry constructor are completely under control.)
 */
template<class CameraType>
class CameraRegistry final : public CameraRegistryBase {
public:
    /**
     * \brief Constructor of camera registry.
     * \param [in] camera_type_name Type name of camera.
     */
    explicit CameraRegistry<CameraType>(const std::string &camera_type_name) {
        CameraFactory::Instance().RegisterCamera(camera_type_name, this);
    }

    /**
     * \brief Create a camera of this type.
     * \return A camera pointer.
     * \warning NEVER directly call this function. Instead, it should be called by camera factory.
     */
    inline Camera *CreateCamera() final { return new CameraType(); }
};

#endif  // _CAMERA_FACTORY_H_
