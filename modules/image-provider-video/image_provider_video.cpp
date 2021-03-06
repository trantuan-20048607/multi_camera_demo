#include "image_provider_video.h"

/**
 * \warning Image provider registry will be initialized before the program entering the main function!
 *   This means any error occurring here will not be caught unless you're using debugger.
 *   (Thus, do not use this variable in any other place and you should not modify it.)
 */
[[maybe_unused]] ImageProviderRegistry<ImageProviderVideo> ImageProviderVideo::registry_ =
        ImageProviderRegistry<ImageProviderVideo>("IPVideo");

ImageProviderVideo::~ImageProviderVideo() {
    if (video_.isOpened()) {
        video_.release();
        intrinsic_mat_.release();
        distortion_mat_.release();
    }
}

bool ImageProviderVideo::Initialize(const std::string &file_path) {
    // Load initialization configuration file.
    cv::FileStorage video_init_config;
    try { video_init_config.open(file_path, cv::FileStorage::READ); }
    catch (const std::exception &) {
        LOG(ERROR) << "Failed to open camera initialization file " << file_path << ".";
        return false;
    }

    // Load global cameras' configuration file.
    std::string all_cams_config_file;
    video_init_config["ALL_CAMS_CONFIG_FILE"] >> all_cams_config_file;
    if (all_cams_config_file.empty()) {
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
    video_init_config["ALL_LENS_CONFIG_FILE"] >> all_lens_config_file;
    if (all_lens_config_file.empty()) {
        LOG(ERROR) << "All lens' config file configuration not found.";
        return false;
    }
    cv::FileStorage all_lens_config;
    try { all_lens_config.open(all_lens_config_file, cv::FileStorage::READ); }
    catch (const std::exception &) {
        LOG(ERROR) << "Failed to open all lens' config file " << all_lens_config_file << ".";
        return false;
    }

    // Get matrix for PnP.
    std::string len_type;
    all_cams_config[video_init_config["CAMERA"]]["LEN"] >> len_type;
    all_lens_config[len_type]["IntrinsicMatrix"] >> intrinsic_mat_;
    all_lens_config[len_type]["DistortionMatrix"] >> distortion_mat_;
    if (intrinsic_mat_.empty() || distortion_mat_.empty()) {
        LOG(ERROR) << "Camera len configurations not found.";
        intrinsic_mat_.release();
        distortion_mat_.release();
        return false;
    }

    // Open video file.
    std::string video_file;
    video_init_config["VIDEO"] >> video_file;
    if (video_file.empty()) {
        LOG(ERROR) << "Video configuration not found.";
        intrinsic_mat_.release();
        distortion_mat_.release();
        return false;
    }
    try { video_.open(video_file); }
    catch (const std::exception &) {
        LOG(ERROR) << "Failed to open video file " << video_file << ".";
        intrinsic_mat_.release();
        distortion_mat_.release();
        return false;
    }

    return true;
}
