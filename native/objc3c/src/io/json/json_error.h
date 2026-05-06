#pragma once

#include <cstddef>
#include <string>

namespace objc3::io::json {

struct JsonError {
  std::string message;
  std::size_t offset = 0;

  [[nodiscard]] std::string Format() const;
};

}  // namespace objc3::io::json
