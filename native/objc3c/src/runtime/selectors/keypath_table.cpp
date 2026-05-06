#include "runtime/selectors/keypath_table.h"

namespace objc3c::runtime {

bool RuntimeKeyPathHandleIsValid(std::uint64_t stable_id) {
  return stable_id != 0;
}

bool RuntimeKeyPathDescriptorIsMaterializable(const char *root_name,
                                              const char *component_path) {
  return root_name != nullptr && root_name[0] != '\0' &&
         component_path != nullptr && component_path[0] != '\0';
}

}  // namespace objc3c::runtime
