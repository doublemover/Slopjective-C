#pragma once

#include <string>
#include <string_view>

#include "support/objc3_method_family.h"

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

inline Objc3NilReceiverProfile BuildNilReceiverProfile(
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

inline Objc3SuperDispatchProfile BuildSuperDispatchProfile(
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

inline bool MethodFamilyReturnsRetainedResult(std::string_view family_name) {
  return family_name == "init" || family_name == "copy" ||
         family_name == "mutableCopy" || family_name == "new";
}

inline bool MethodFamilyReturnsRelatedResult(std::string_view family_name) {
  return family_name == "init";
}

inline bool IsKnownMethodFamilyName(std::string_view family_name) {
  return family_name == "init" || family_name == "copy" ||
         family_name == "mutableCopy" || family_name == "new" ||
         family_name == "none";
}

inline Objc3MethodFamilyProfile BuildMethodFamilyProfile(
    std::string_view selector,
    bool existing_normalized,
    const std::string &existing_family_name,
    bool existing_returns_retained_result,
    bool existing_returns_related_result) {
  Objc3MethodFamilyProfile profile;
  profile.name =
      existing_normalized && !existing_family_name.empty()
          ? existing_family_name
          : ClassifyMethodFamilyFromSelector(std::string(selector));
  profile.returns_retained_result =
      existing_normalized ? existing_returns_retained_result
                          : MethodFamilyReturnsRetainedResult(profile.name);
  profile.returns_related_result =
      existing_normalized ? existing_returns_related_result
                          : MethodFamilyReturnsRelatedResult(profile.name);
  profile.normalized =
      existing_normalized ||
      (IsKnownMethodFamilyName(profile.name) &&
       (!profile.returns_related_result || profile.name == "init"));
  return profile;
}

}  // namespace objc3c::support
