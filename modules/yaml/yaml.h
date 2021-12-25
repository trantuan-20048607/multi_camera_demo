#ifndef _YAML_H_
#define _YAML_H_

#include <opencv2/core/core.hpp>

class YAML {
public:
    enum FileMode {
        READ = cv::FileStorage::READ,
        WRITE = cv::FileStorage::WRITE,
        APPEND = cv::FileStorage::APPEND
    };

    YAML(std::string file_name, FileMode file_mode = FileMode::READ);

    ~YAML();

    YAML(const YAML &) = delete;

    YAML(YAML &&) = delete;

    YAML &operator=(const YAML &) = delete;

    YAML &operator=(const YAML &&) = delete;

    bool Open();

    template<typename Key, typename Value>
    void SetData(Key &key, Value &value);

    template<typename Key, typename Value>
    void GetData(Key &key, Value &value);

private:
    cv::FileStorage file_storage_;
    std::string file_name_;
    FileMode file_mode_;
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
