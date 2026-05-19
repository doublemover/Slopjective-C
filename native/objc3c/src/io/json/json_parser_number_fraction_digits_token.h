#pragma once

#include <cstddef>
#include <string_view>

namespace objc3::io::json {

bool ConsumeJsonNumberFractionDigits(std::string_view text,
                                     std::size_t &cursor);

}  // namespace objc3::io::json
