#include "3rdparty/easylogging++/easylogging++.h"

#include "image_provider_video.h"

ImageProviderVideo::~ImageProviderVideo() {
    if (video_.isOpened()) {
        video_.release();
        intrinsic_mat_.release();
        distortion_mat_.release();
    }
}

bool ImageProviderVideo::Initialize(const std::string &file_path) {
    cv::FileStorage video_init_config;
    try { video_init_config.open(file_path, cv::FileStorage::READ); }
    catch (const std::exception &) {
        LOG(ERROR) << "Failed to open camera initialization file " << file_path << ".";
        return false;
    }

    std::string all_cams_config_file;
    try { video_init_config["ALL_CAMS_CONFIG_FILE"] >> all_cams_config_file; }
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

    std::string all_lens_config_file;
    try { video_init_config["ALL_LENS_CONFIG_FILE"] >> all_lens_config_file; }
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

    std::string len_type;
    try {
        all_cams_config[video_init_config["CAMERA"]]["LEN"] >> len_type;
        all_lens_config[len_type]["IntrinsicMatrix"] >> intrinsic_mat_;
        all_lens_config[len_type]["DistortionMatrix"] >> distortion_mat_;
    } catch (const std::exception &) {
        LOG(ERROR) << "Camera len configurations not found.";
        return false;
    }

    std::string video_file;
    try { video_init_config["VIDEO"] >> video_file; }
    catch (const std::exception &) {
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
