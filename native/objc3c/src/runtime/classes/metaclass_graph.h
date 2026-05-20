#pragma once

#include <string>

namespace objc3c::runtime {

bool RuntimeMetaclassEdgeIsMaterializable(const char *class_owner_identity,
                                          const char *metaclass_owner_identity);

bool RuntimeMetaclassMetadataIsConsistent(
    const char *class_name, const char *class_owner_identity,
    const char *metaclass_owner_identity,
    const char *super_class_owner_identity,
    const char *super_metaclass_owner_identity, bool is_root_class,
    std::string *failure_reason);

bool RuntimeMetaclassSuperclassLinkIsConsistent(
    const char *class_name, const char *super_class_owner_identity,
    const char *super_metaclass_owner_identity,
    const char *resolved_super_class_owner_identity,
    const char *resolved_super_metaclass_owner_identity,
    std::string *failure_reason);

}  // namespace objc3c::runtime
