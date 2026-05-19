#include "io/json/json_equivalence.h"

#include "io/json/json_equivalence_nodes.h"

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
    case JsonValue::Kind::kArray:
      return JsonArraysEqual(left.AsArray(), right.AsArray());
    case JsonValue::Kind::kObject:
      return JsonObjectsEqual(left.AsObject(), right.AsObject());
  }
  return false;
}

}  // namespace objc3::io::json
