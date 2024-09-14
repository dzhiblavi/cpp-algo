#pragma once

#include "test/utility/random.h"

#include <string>

namespace test::string {

inline std::string generateRandomString(size_t length, size_t alph_size) {
    constexpr std::string_view CHARACTERS = "abcdefghijklmnopqrstuvwxyz";
    std::string random_string;
    random_string.reserve(length);

    for (size_t i = 0; i < length; ++i) {
        random_string += CHARACTERS[utility::random::uniform(size_t(0), alph_size - 1)];
    }

    return random_string;
}

}  // namespace test::string
