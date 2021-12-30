#include "image_provider_camera.h"
#include "modules/camera-base/camera_factory.h"

/**
 * \warning Image provider registry will be initialized before the program entering the main function!
 *   This means any error occurring here will not be caught unless you're using debugger.
 *   (Thus, do not use this variable in any other place and you should not modify it.)
 */
[[maybe_unused]] ImageProviderRegistry<ImageProviderCamera> ImageProviderCamera::registry_ =
        ImageProviderRegistry<ImageProviderCamera>("IPCamera");

ImageProviderCamera::~ImageProviderCamera() {
    if (camera_) {
        camera_->StopStream();
        camera_->CloseCamera();
        delete camera_;
        intrinsic_mat_.release();
        distortion_mat_.release();
    }
}

bool ImageProviderCamera::Initialize(const std::string &file_path) {
    // Load initialization configuration file.
    cv::FileStorage camera_init_config;
    try { camera_init_config.open(file_path, cv::FileStorage::READ); }
    catch (const std::exception &) {
        LOG(ERROR) << "Failed to open camera initialization file " << file_path << ".";
        return false;
    }

    // Load global cameras' configuration file.
    std::string all_cams_config_file;
    try { camera_init_config["ALL_CAMS_CONFIG_FILE"] >> all_cams_config_file; }
    catch (const std::exception &) {
        LOG(ERROR) << "All cameras' config file configuration not found.";
        return false;
    }
    cv::FileStorage all_cams_config;
    try { all_cams_config.open(all_cams_config_file, cv::FileStorage::READ); }
    catch (const std::exception &) {
        LOG(ERROR) << "Failed to open all cameras' config file " << all_cams_config_file << ".";
        return false;
    }

    // Load global lens' configuration file.
    std::string all_lens_config_file;
    try { camera_init_config["ALL_LENS_CONFIG_FILE"] >> all_lens_config_file; }
    catch (const std::exception &) {
        LOG(ERROR) << "All lens' config file configuration not found.";
        return false;
    }
    cv::FileStorage all_lens_config;
    try { all_lens_config.open(all_lens_config_file, cv::FileStorage::READ); }
    catch (const std::exception &) {
        LOG(ERROR) << "Failed to open all lens' config file " << all_lens_config_file << ".";
        return false;
    }

    // Create a camera.
    std::string camera_type;
    try { all_cams_config[camera_init_config["CAMERA"]]["TYPE"] >> camera_type; }
    catch (const std::exception &) {
        LOG(ERROR) << "Camera type configuration not found.";
        return false;
    }
    camera_ = CF_CREATE_CAMERA(camera_type);
    if (!camera_) {
        LOG(ERROR) << "Failed to create camera object of type " << camera_type << ".";
        return false;
    }

    // Open camera.
    std::string serial_number, camera_config_file;
    try {
        all_cams_config[camera_init_config["CAMERA"]]["SN"] >> serial_number;
        all_cams_config[camera_init_config["CAMERA"]]["CONFIG"] >> camera_config_file;
    } catch (const std::exception &) {
        LOG(ERROR) << "Camera configurations not found.";
        return false;
    }
    if (!camera_->OpenCamera(serial_number, camera_config_file)) {
        delete camera_;
        return false;
    }

    // Get matrix for PnP.
    std::string len_type;
    try {
        all_cams_config[camera_init_config["CAMERA"]]["LEN"] >> len_type;
        all_lens_config[len_type]["IntrinsicMatrix"] >> intrinsic_mat_;
        all_lens_config[len_type]["DistortionMatrix"] >> distortion_mat_;
    } catch (const std::exception &) {
        LOG(ERROR) << "Camera len configurations not found.";
        return false;
    }

    // Start stream.
    if (!camera_->StartStream()) {
        camera_->CloseCamera();
        delete camera_;
        intrinsic_mat_.release();
        distortion_mat_.release();
        return false;
    }

    return true;
}
