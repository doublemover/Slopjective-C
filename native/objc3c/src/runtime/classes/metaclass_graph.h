#pragma once

namespace objc3c::runtime {

bool RuntimeMetaclassEdgeIsMaterializable(const char *class_owner_identity,
                                          const char *metaclass_owner_identity);

}  // namespace objc3c::runtime
