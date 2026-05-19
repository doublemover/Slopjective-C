#pragma once

#include "runtime/state/runtime_bootstrap_contracts.h"

#include <string>
#include <vector>

namespace objc3c::runtime {

struct EmittedClassBundle;
struct RegisteredImageMetadata;
struct RuntimeState;

bool RuntimeImplementationOwnerIdentity(const char *owner_identity);
std::vector<const RegisteredImageMetadata *> OrderedClassGraphImages(
    const RuntimeState &state);
bool CollectSortedImageClassNames(const RegisteredImageMetadata &record,
                                  std::vector<std::string> &class_names);
bool RuntimeClassMetadataTableIsSupported(
    const RuntimeState &state,
    const objc3_runtime_registration_table *registration_table,
    std::string &diagnostic_reason);
std::vector<const EmittedClassBundle *> CollectPreferredClassBundlesForImage(
    const RegisteredImageMetadata &record, const std::string &class_name);
std::string ResolveInterfaceOwnerIdentityForClass(
    const RegisteredImageMetadata &record, const std::string &class_name,
    const std::string &default_owner_identity);

}  // namespace objc3c::runtime
