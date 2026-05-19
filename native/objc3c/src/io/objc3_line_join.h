#pragma once

#include <string>
#include <vector>

namespace objc3::io {

[[nodiscard]] std::string JoinLinesWithTrailingNewline(
    const std::vector<std::string> &lines);

}  // namespace objc3::io
