#include "runtime/images/multi_image_ordering.h"

#include <string>

namespace objc3c::runtime {

int CompareRuntimeImageRegistrationOrder(
    std::uint64_t lhs_ordinal, const char *lhs_identity,
    std::uint64_t rhs_ordinal, const char *rhs_identity) {
  if (lhs_ordinal < rhs_ordinal) {
    return -1;
  }
  if (lhs_ordinal > rhs_ordinal) {
    return 1;
  }
  const std::string lhs = lhs_identity != nullptr ? lhs_identity : "";
  const std::string rhs = rhs_identity != nullptr ? rhs_identity : "";
  if (lhs < rhs) {
    return -1;
  }
  if (lhs > rhs) {
    return 1;
  }
  return 0;
}

}  // namespace objc3c::runtime
