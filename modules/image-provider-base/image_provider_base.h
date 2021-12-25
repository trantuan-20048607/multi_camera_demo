#ifndef _IMAGE_PROVIDER_BASE_H_
#define _IMAGE_PROVIDER_BASE_H_

#include "frame.h"
#include "buffer.h"

class ImageProvider {
public:
    virtual ~ImageProvider() = default;

    /**
     * \brief Initialize by specified configuration file.
     * \param file_path Configuration file path.
     * \return A boolean shows if initialization succeeded.
     */
    virtual bool Initialize(const std::string &file_path) = 0;

    virtual bool GetFrame(Frame &) = 0;

protected:
    Buffer<Frame, IP_BUFFER_SIZE> buffer_;
};

#endif  // _IMAGE_PROVIDER_BASE_H_
