#include <string>
#include <iomanip>
#include <chrono>
#include <utility>

#include "modules/yaml/yaml.h"
#include "3rdparty/easylogging++/easylogging++.h"

YAML::YAML(std::string file_name, FileMode file_mode) :
        file_name_(std::move(file_name)),
        file_mode_(file_mode) {}

YAML::~YAML() { file_storage_.release(); }

bool YAML::Open() {
    if (!file_storage_.open(file_name_, file_mode_)) {
        LOG(ERROR) << "Failed to open yaml file " << file_name_ << ".";
        return false;
    }

    if (file_mode_ == WRITE) {
        auto current_time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        std::stringstream date_str;
        date_str << std::put_time(std::localtime(&current_time), "%Y-%m-%d");
        file_storage_ << "Last Modification Date" << date_str.str();
        date_str.clear();
    }

    LOG(DEBUG) << "Opened yaml file " << file_name_ << ".";
    return true;
}
