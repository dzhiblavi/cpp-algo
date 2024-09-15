#pragma once

#include "algo/utility/for_each_macro.h"

#include <cstddef>
#include <vector>

namespace algo::utility {

#define _SOA_PROXY_SETTER(Type, Name)                                            \
    template <typename T = Type>                                                 \
    std::enable_if_t<std::is_same_v<T, Descriptor>> set_##Name(const T& value) { \
        this->soa->Name##_store_[this->idx] = value.idx;                         \
    }                                                                            \
    template <typename T = Type>                                                 \
    std::enable_if_t<!std::is_same_v<T, Descriptor>> set_##Name(T value) {       \
        this->soa->Name##_store_[this->idx] = std::move(value);                  \
    }

#define _SOA_PROXY_GETTER(Type, Name)                                         \
    template <typename T = Type>                                              \
    std::enable_if_t<std::is_same_v<T, Descriptor>, T> Name() {               \
        return Descriptor{soa, this->soa->Name##_store_[this->idx]};          \
    }                                                                         \
    template <typename T = Type>                                              \
    std::enable_if_t<!std::is_same_v<T, Descriptor>, T&> Name() {             \
        return this->soa->Name##_store_[this->idx];                           \
    }                                                                         \
    template <typename T = Type>                                              \
    std::enable_if_t<!std::is_same_v<T, Descriptor>, const T&> Name() const { \
        return this->soa->Name##_store_[this->idx];                           \
    }

#define _SOA_PROXY_FIELD(Field) _SOA_PROXY_SETTER Field _SOA_PROXY_GETTER Field

#define _SOA_STORAGE_IMPL(Type, Name) std::vector<StoredType<Type>> Name##_store_{};
#define _SOA_STORAGE(Field) _SOA_STORAGE_IMPL Field

#define _SOA_STORAGE_APPEND_IMPL(_, Name) Name##_store_.emplace_back();
#define _SOA_STORAGE_APPEND(Field) _SOA_STORAGE_APPEND_IMPL Field

#define _SOA_STORAGE_RESERVE_IMPL(_, Name) Name##_store_.reserve(capacity);
#define _SOA_STORAGE_RESERVE(Field) _SOA_STORAGE_RESERVE_IMPL Field

#define _SOA_STORAGE_SIZE_IMPL(_, Name) Name##_store_.size()
#define _SOA_STORAGE_SIZE(First, ...) _SOA_STORAGE_SIZE_IMPL First

#define SOA(Class, ...)                                                                    \
    template <typename SizeType>                                                           \
    class Class {                                                                          \
    public:                                                                                \
        class Descriptor {                                                                 \
            static constexpr SizeType kNull = -1;                                          \
                                                                                           \
        public:                                                                            \
            friend class Class;                                                            \
            Descriptor() = default;                                                        \
            Descriptor(const Descriptor&) = default;                                       \
            Descriptor& operator=(const Descriptor&) = default;                            \
            FOR_EACH(_SOA_PROXY_FIELD, __VA_ARGS__)                                        \
                                                                                           \
            bool isNull() const {                                                          \
                return idx == kNull;                                                       \
            }                                                                              \
                                                                                           \
        private:                                                                           \
            Descriptor(Class* soa, std::convertible_to<SizeType> auto index)               \
                : soa(soa), idx(static_cast<SizeType>(index)) {}                           \
                                                                                           \
            Class* soa;                                                                    \
            SizeType idx = kNull;                                                          \
        };                                                                                 \
                                                                                           \
        Descriptor create() {                                                              \
            size_t size = _SOA_STORAGE_SIZE(__VA_ARGS__);                                  \
            FOR_EACH(_SOA_STORAGE_APPEND, __VA_ARGS__)                                     \
            return {this, size};                                                           \
        }                                                                                  \
                                                                                           \
        void reserve(size_t capacity) {                                                    \
            FOR_EACH(_SOA_STORAGE_RESERVE, __VA_ARGS__)                                    \
        }                                                                                  \
                                                                                           \
    private:                                                                               \
        template <typename T>                                                              \
        using StoredType = std::conditional_t<std::is_same_v<T, Descriptor>, SizeType, T>; \
        FOR_EACH(_SOA_STORAGE, __VA_ARGS__)                                                \
    }

}  // namespace algo::utility
