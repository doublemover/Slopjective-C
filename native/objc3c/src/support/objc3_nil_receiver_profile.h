#pragma once

namespace objc3c::support {

struct Objc3NilReceiverProfile {
  bool semantics_enabled = false;
  bool foldable = false;
  bool requires_runtime_dispatch = true;
  bool normalized = false;
};

Objc3NilReceiverProfile BuildNilReceiverProfile(
    bool receiver_is_nil_literal,
    bool existing_normalized,
    bool existing_enabled,
    bool existing_foldable,
    bool existing_requires_runtime_dispatch);

}  // namespace objc3c::support
