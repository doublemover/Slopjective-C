#include "runtime/dispatch/dispatch_family.h"

namespace objc3c::runtime {

bool RuntimeDispatchFamilyIsValid(DispatchFamily family) {
  return family == DispatchFamily::Instance || family == DispatchFamily::Class;
}

const char *RuntimeDispatchFamilyName(DispatchFamily family) {
  switch (family) {
    case DispatchFamily::Instance:
      return "instance";
    case DispatchFamily::Class:
      return "class";
    case DispatchFamily::Invalid:
      return "invalid";
  }
  return "invalid";
}

}  // namespace objc3c::runtime
