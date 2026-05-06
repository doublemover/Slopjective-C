#include "runtime/classes/protocol_conformance.h"

namespace objc3c::runtime {

bool RuntimeProtocolConformanceEdgeIsMaterializable(const char *class_name,
                                                    const char *protocol_name) {
  return class_name != nullptr && class_name[0] != '\0' &&
         protocol_name != nullptr && protocol_name[0] != '\0';
}

}  // namespace objc3c::runtime
