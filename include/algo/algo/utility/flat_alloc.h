#pragma once

#include <cassert>
#include <memory>

namespace algo::utility {

template <typename T, typename Allocator = std::allocator<T>>
class CappedFlatAllocator {
public:
    explicit CappedFlatAllocator(size_t capacity, Allocator alloc = {})
        : capacity(capacity), alloc(std::move(alloc)) {
        storage = alloc.allocate(capacity);
    }

    ~CappedFlatAllocator() noexcept {
        alloc.deallocate(storage, capacity);
    }

    // returns a non-owning pointer
    template <typename... Args>
    T* allocate(Args&&... args) {
        assert(size < capacity);
        T* ptr = static_cast<T*>(storage) + size;
        std::construct_at(ptr, std::forward<Args>(args)...);
        ++size;
        return ptr;
    }

private:
    size_t size = 0;
    size_t capacity;
    T* storage;
    [[no_unique_address]] Allocator alloc;
};
}  // namespace algo::utility
