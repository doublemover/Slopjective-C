#pragma once

#include <string>

namespace objc3c::support {

struct Objc3MethodFamilyProfile {
  std::string name = "none";
  bool returns_retained_result = false;
  bool returns_related_result = false;
  bool normalized = false;
};

}  // namespace objc3c::support
