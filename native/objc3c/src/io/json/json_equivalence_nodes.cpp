#include "io/json/json_equivalence_nodes.h"

#include <cstddef>

#include "io/json/json_equivalence.h"

namespace objc3::io::json {

bool JsonArraysEqual(const JsonValue::Array &left,
                     const JsonValue::Array &right) {
  if (left.size() != right.size()) {
    return false;
  }
  for (std::size_t i = 0; i < left.size(); ++i) {
    if (!JsonEquals(left[i], right[i])) {
      return false;
    }
  }
  return true;
}

bool JsonObjectsEqual(const JsonValue::Object &left,
                      const JsonValue::Object &right) {
  if (left.size() != right.size()) {
    return false;
  }
  for (const auto &[key, left_value] : left) {
    const auto right_value = right.find(key);
    if (right_value == right.end() ||
        !JsonEquals(left_value, right_value->second)) {
      return false;
    }
  }
  return true;
}

}  // namespace objc3::io::json
