#include "support/objc3_receiver_dispatch_profiles.h"

namespace objc3c::support {

Objc3NilReceiverProfile BuildNilReceiverProfile(
    bool receiver_is_nil_literal,
    bool existing_normalized,
    bool existing_enabled,
    bool existing_foldable,
    bool existing_requires_runtime_dispatch) {
  Objc3NilReceiverProfile profile;
  profile.semantics_enabled =
      existing_normalized ? existing_enabled : receiver_is_nil_literal;
  profile.foldable =
      existing_normalized ? existing_foldable : profile.semantics_enabled;
  profile.requires_runtime_dispatch =
      existing_normalized ? existing_requires_runtime_dispatch
                          : !profile.foldable;
  profile.normalized =
      existing_normalized ||
      (profile.semantics_enabled == receiver_is_nil_literal &&
       profile.semantics_enabled == profile.foldable &&
       profile.requires_runtime_dispatch == !profile.foldable);
  return profile;
}

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
