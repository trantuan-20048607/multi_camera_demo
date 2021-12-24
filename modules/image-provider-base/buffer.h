#ifndef _BUFFER_H_
#define _BUFFER_H_

#define IP_BUFFER_SIZE 4

#include <mutex>

/**
 * \brief Ring buffer with mutex.
 * \tparam Type Type of elements in this ring buffer.
 * \tparam size Max size of this ring buffer.
 * \attention Length must be 2^N.
 */
template<typename Type, unsigned int size>
class Buffer {
private:
    Type data_[size];
    unsigned int head_;
    unsigned int tail_;
    std::mutex lock_[size];
    std::mutex head_lock_;
    const unsigned int and_to_mod_ = size - 1;

public:
    Buffer<Type, size>() : head_(0), tail_(0) {
        assert(size);
        assert(!(size & (size - 1)));
    }

    ~Buffer() = default;

    /**
     * \return Size of this buffer, which is specified when it is constructed.
     */
    [[maybe_unused]] [[nodiscard]] inline unsigned int Size() const { return size; }

    /**
     * \brief Is this buffer empty?
     * \return A boolean shows if ring buffer is empty.
     */
    [[maybe_unused]] [[nodiscard]] inline bool Empty() const { return head_ == tail_; }

    /**
     * \brief Push an element.
     * \param [in] obj Input element.
     */
    inline void Push(const Type &obj) {
        std::lock_guard<std::mutex> lock(lock_[tail_]);
        data_[tail_] = obj;
        ++tail_;
        tail_ &= and_to_mod_;

        if (head_ == tail_) {
            std::lock_guard<std::mutex> head_lock(head_lock_);
            ++head_;
            head_ &= and_to_mod_;
        }
    }

    /**
     * \brief Pop an element.
     * \param [out] obj Output element.
     * \return A boolean shows if ring buffer is not empty.
     */
    inline bool Pop(Type &obj) {
        if (head_ == tail_)
            return false;
        std::lock_guard<std::mutex> lock(lock_[head_]);
        obj = data_[head_];
        std::lock_guard<std::mutex> head_lock(head_lock_);
        ++head_;
        head_ &= and_to_mod_;
        return true;
    }

    [[maybe_unused]] [[nodiscard]] Type &operator[](unsigned int id) {
        while (tail_ + id < 0) id += size;
        return data_[(tail_ + id) & and_to_mod_];
    }
};

#endif  // _BUFFER_H_
