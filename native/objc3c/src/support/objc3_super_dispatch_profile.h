#pragma once

namespace objc3c::support {

struct Objc3SuperDispatchProfile {
  bool enabled = false;
  bool requires_class_context = false;
  bool normalized = false;
};

Objc3SuperDispatchProfile BuildSuperDispatchProfile(
    bool receiver_is_super_identifier,
    bool existing_normalized,
    bool existing_enabled,
    bool existing_requires_class_context);

}  // namespace objc3c::support
