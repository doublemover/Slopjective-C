#include "support/objc3_method_family_related_result.h"

namespace objc3c::support {

bool MethodFamilyReturnsRelatedResult(std::string_view family_name) {
  return family_name == "init";
}

}  // namespace objc3c::support
