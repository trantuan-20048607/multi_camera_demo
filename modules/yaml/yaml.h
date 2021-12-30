#ifndef _YAML_H_
#define _YAML_H_

#include <opencv2/core/core.hpp>

/// \brief YAML file operator based on OpenCV file storage.
class YAML {
public:
    enum FileMode {
        READ = cv::FileStorage::READ,
        WRITE = cv::FileStorage::WRITE,
        APPEND = cv::FileStorage::APPEND
    };

    /**
     * \brief YAML constructor.
     * \param file_name File name.
     * \param file_mode File opening method.
     */
    YAML(std::string file_name, FileMode file_mode);

    ~YAML();

    YAML(const YAML &) = delete;

    YAML &operator=(const YAML &) = delete;

    /**
     * \brief Open file.
     * \return A boolean shows if file exists or can be opened.
     */
    bool Open();

    /**
     * \brief Set data to YAML file.
     * \note All modifications will immediately take effect.
     * \tparam Key Type of key.
     * \tparam Value Type of value.
     * \param [in] key Key name.
     * \param [in] value Writing value.
     */
    template<typename Key, typename Value>
    void SetData(Key &key, Value &value);

    /**
     * \brief Get data from YAML file.
     * \tparam Key Type of key.
     * \tparam Value Type of value.
     * \param [in] key Key name.
     * \param [out] value Value of key.
     */
    template<typename Key, typename Value>
    void GetData(Key &key, Value &value);

private:
    cv::FileStorage file_storage_;  // OpenCV file object.
    std::string file_name_;         // File name.
    FileMode file_mode_;            // File opening mode.
};

template<typename Key, typename Value>
void YAML::SetData(Key &key, Value &value) {
    file_storage_ << key << value;
}

template<typename Key, typename Value>
void YAML::GetData(Key &key, Value &value) {
    file_storage_[key] >> value;
}

#endif // _YAML_H_
