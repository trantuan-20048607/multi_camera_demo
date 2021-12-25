#ifndef _IMAGE_PROVIDER_BASE_H_
#define _IMAGE_PROVIDER_BASE_H_

#include "frame.h"
#include "buffer.h"

class ImageProvider {
public:
    virtual ~ImageProvider() = default;

    virtual bool Initialize() = 0;

    virtual bool GetFrame(Frame &) = 0;

protected:
    Buffer<Frame, IP_BUFFER_SIZE> buffer_;
};

#endif  // _IMAGE_PROVIDER_BASE_H_
