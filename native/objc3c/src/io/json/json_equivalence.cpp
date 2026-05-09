#include "io/json/json_equivalence.h"

#include <cstddef>

namespace objc3::io::json {

bool JsonEquals(const JsonValue &left, const JsonValue &right) {
  if (left.kind() != right.kind()) {
    return false;
  }
  switch (left.kind()) {
    case JsonValue::Kind::kNull:
      return true;
    case JsonValue::Kind::kBool:
      return left.AsBool() == right.AsBool();
    case JsonValue::Kind::kNumber:
      return left.AsNumber() == right.AsNumber();
    case JsonValue::Kind::kString:
      return left.AsString() == right.AsString();
    case JsonValue::Kind::kArray: {
      const JsonValue::Array &left_array = left.AsArray();
      const JsonValue::Array &right_array = right.AsArray();
      if (left_array.size() != right_array.size()) {
        return false;
      }
      for (std::size_t i = 0; i < left_array.size(); ++i) {
        if (!JsonEquals(left_array[i], right_array[i])) {
          return false;
        }
      }
      return true;
    }
    case JsonValue::Kind::kObject: {
      const JsonValue::Object &left_object = left.AsObject();
      const JsonValue::Object &right_object = right.AsObject();
      if (left_object.size() != right_object.size()) {
        return false;
      }
      for (const auto &[key, left_value] : left_object) {
        const JsonValue *right_value = right.Find(key);
        if (right_value == nullptr || !JsonEquals(left_value, *right_value)) {
          return false;
        }
      }
      return true;
    }
  }
  return false;
}

}  // namespace objc3::io::json
