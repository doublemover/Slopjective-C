#include "support/objc3_nil_receiver_profile.h"

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

}  // namespace objc3c::support
