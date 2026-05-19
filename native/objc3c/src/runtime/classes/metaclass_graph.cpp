#include "runtime/classes/metaclass_graph.h"

namespace objc3c::runtime {

bool RuntimeMetaclassEdgeIsMaterializable(const char *class_owner_identity,
                                          const char *metaclass_owner_identity) {
  return class_owner_identity != nullptr && class_owner_identity[0] != '\0' &&
         metaclass_owner_identity != nullptr &&
         metaclass_owner_identity[0] != '\0';
}

}  // namespace objc3c::runtime
