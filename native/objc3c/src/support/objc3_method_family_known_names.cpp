#include "support/objc3_method_family_known_names.h"

namespace objc3c::support {

bool IsKnownMethodFamilyName(std::string_view family_name) {
  return family_name == "alloc" || family_name == "init" || family_name == "copy" ||
         family_name == "mutableCopy" || family_name == "new" ||
         family_name == "none";
}

}  // namespace objc3c::support
