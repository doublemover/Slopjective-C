#include "support/objc3_method_family_retained_result.h"

namespace objc3c::support {

bool MethodFamilyReturnsRetainedResult(std::string_view family_name) {
  return family_name == "init" || family_name == "copy" ||
         family_name == "mutableCopy" || family_name == "new";
}

}  // namespace objc3c::support
