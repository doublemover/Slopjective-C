#include "support/objc3_super_dispatch_profile.h"

namespace objc3c::support {

Objc3SuperDispatchProfile BuildSuperDispatchProfile(
    bool receiver_is_super_identifier,
    bool existing_normalized,
    bool existing_enabled,
    bool existing_requires_class_context) {
  Objc3SuperDispatchProfile profile;
  profile.enabled =
      existing_normalized ? existing_enabled : receiver_is_super_identifier;
  profile.requires_class_context =
      existing_normalized ? existing_requires_class_context : profile.enabled;
  profile.normalized =
      existing_normalized ||
      (profile.enabled == receiver_is_super_identifier &&
       profile.requires_class_context == profile.enabled);
  return profile;
}

}  // namespace objc3c::support
