#pragma once

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
std::vector<const EmittedClassBundle *> CollectPreferredClassBundlesForImage(
    const RegisteredImageMetadata &record, const std::string &class_name);
std::string ResolveInterfaceOwnerIdentityForClass(
    const RegisteredImageMetadata &record, const std::string &class_name,
    const std::string &default_owner_identity);

}  // namespace objc3c::runtime
