#pragma once

#include <string>
#include <string_view>

namespace objc3c::support {

struct Objc3NilReceiverProfile {
  bool semantics_enabled = false;
  bool foldable = false;
  bool requires_runtime_dispatch = true;
  bool normalized = false;
};

struct Objc3SuperDispatchProfile {
  bool enabled = false;
  bool requires_class_context = false;
  bool normalized = false;
};

struct Objc3MethodFamilyProfile {
  std::string name = "none";
  bool returns_retained_result = false;
  bool returns_related_result = false;
  bool normalized = false;
};

Objc3NilReceiverProfile BuildNilReceiverProfile(
    bool receiver_is_nil_literal,
    bool existing_normalized,
    bool existing_enabled,
    bool existing_foldable,
    bool existing_requires_runtime_dispatch);
Objc3SuperDispatchProfile BuildSuperDispatchProfile(
    bool receiver_is_super_identifier,
    bool existing_normalized,
    bool existing_enabled,
    bool existing_requires_class_context);
bool MethodFamilyReturnsRetainedResult(std::string_view family_name);
bool MethodFamilyReturnsRelatedResult(std::string_view family_name);
bool IsKnownMethodFamilyName(std::string_view family_name);
Objc3MethodFamilyProfile BuildMethodFamilyProfile(
    std::string_view selector,
    bool existing_normalized,
    const std::string &existing_family_name,
    bool existing_returns_retained_result,
    bool existing_returns_related_result);

}  // namespace objc3c::support
