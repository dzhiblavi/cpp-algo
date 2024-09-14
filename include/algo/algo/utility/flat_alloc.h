#pragma once

#include <cassert>
#include <memory>

namespace algo::utility {

template <typename T, typename Allocator = std::allocator<T>>
class FixedCapacityStorage {
public:
    explicit FixedCapacityStorage(size_t capacity, Allocator alloc = {})
        : capacity(capacity), alloc(std::move(alloc)) {
        storage = alloc.allocate(capacity);
    }

    ~FixedCapacityStorage() noexcept {
        alloc.deallocate(storage, capacity);
    }

    template <typename... Args>
    void emplace_back(Args&&... args) {
        assert(size < capacity);
        T* ptr = static_cast<T*>(storage) + size;
        std::construct_at(ptr, std::forward<Args>(args)...);
        size++;
    }

    T* operator*(size_t i) {
        assert(0 <= i && i < size);
        return storage + i;
    }

private:
    size_t size = 0;
    size_t capacity;
    T* storage;
    [[no_unique_address]] Allocator alloc;
};
}  // namespace algo::utility
