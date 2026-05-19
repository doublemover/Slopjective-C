#include "runtime/reflection/property_entry_descriptor_snapshot_fields.h"

#include "runtime/metadata/runtime_emitted_records.h"
#include "runtime/metadata/runtime_realized_records.h"
#include "runtime/strings/borrowed_string.h"

#include <string_view>

namespace objc3c::runtime {

namespace {

bool PropertyAttributeProfileHas(std::string_view profile,
                                 std::string_view token) {
  const std::size_t position = profile.find(token);
  if (position == std::string_view::npos) {
    return false;
  }
  const std::size_t end = position + token.size();
  const bool starts_token = position == 0 || profile[position - 1] == ';';
  const bool ends_token = end == profile.size() || profile[end] == ';';
  return starts_token && ends_token;
}

std::uint64_t CountRenderedPropertyAttributes(std::string_view profile) {
  constexpr std::string_view kAttributesPrefix = "attributes=";
  const std::size_t position = profile.find(kAttributesPrefix);
  if (position == std::string_view::npos) {
    return 0;
  }

  std::string_view attributes =
      profile.substr(position + kAttributesPrefix.size());
  const std::size_t terminator = attributes.find(';');
  if (terminator != std::string_view::npos) {
    attributes = attributes.substr(0, terminator);
  }
  if (attributes.empty()) {
    return 0;
  }

  std::uint64_t count = 1;
  for (const char character : attributes) {
    if (character == ',') {
      ++count;
    }
  }
  return count;
}

bool HasExplicitSelector(const char *selector) {
  return selector != nullptr && selector[0] != '\0';
}

}  // namespace

void PopulateRuntimePropertyEntryDescriptorSnapshotFields(
    const RealizedPropertyAccessor &accessor,
    const EmittedPropertyDescriptor &descriptor,
    objc3_runtime_property_entry_snapshot &snapshot) {
  const std::string_view property_attribute_profile =
      descriptor.property_attribute_profile != nullptr
          ? std::string_view(descriptor.property_attribute_profile)
          : std::string_view();
  snapshot.setter_available = descriptor.effective_setter_available ? 1 : 0;
  snapshot.has_runtime_getter = 1;
  snapshot.has_runtime_setter =
      !accessor.setter_owner_identity.empty() ? 1 : 0;
  snapshot.attribute_count =
      CountRenderedPropertyAttributes(property_attribute_profile);
  snapshot.is_readonly =
      PropertyAttributeProfileHas(property_attribute_profile, "readonly=1") ? 1
                                                                           : 0;
  snapshot.is_nonatomic =
      PropertyAttributeProfileHas(property_attribute_profile, "nonatomic=1") ? 1
                                                                            : 0;
  snapshot.is_strong =
      PropertyAttributeProfileHas(property_attribute_profile, "strong=1") ? 1
                                                                         : 0;
  snapshot.is_assign =
      PropertyAttributeProfileHas(property_attribute_profile, "assign=1") ? 1
                                                                         : 0;
  snapshot.is_weak =
      PropertyAttributeProfileHas(property_attribute_profile, "weak=1") ? 1
                                                                       : 0;
  snapshot.is_copy =
      PropertyAttributeProfileHas(property_attribute_profile, "copy=1") ? 1
                                                                       : 0;
  snapshot.has_custom_getter =
      HasExplicitSelector(descriptor.getter_selector) ? 1 : 0;
  snapshot.has_custom_setter =
      HasExplicitSelector(descriptor.setter_selector) ? 1 : 0;
  snapshot.property_name = descriptor.property_name;
  snapshot.declaration_owner_identity =
      descriptor.declaration_owner_identity != nullptr
          ? descriptor.declaration_owner_identity
          : nullptr;
  snapshot.export_owner_identity =
      descriptor.export_owner_identity != nullptr
          ? descriptor.export_owner_identity
          : nullptr;
  snapshot.getter_selector =
      descriptor.getter_selector != nullptr ? descriptor.getter_selector
                                            : nullptr;
  snapshot.setter_selector =
      descriptor.setter_selector != nullptr ? descriptor.setter_selector
                                            : nullptr;
  snapshot.effective_getter_selector =
      descriptor.effective_getter_selector != nullptr
          ? descriptor.effective_getter_selector
          : nullptr;
  snapshot.effective_setter_selector =
      descriptor.effective_setter_selector != nullptr
          ? descriptor.effective_setter_selector
          : nullptr;
  snapshot.ivar_binding_symbol =
      descriptor.ivar_binding_symbol != nullptr ? descriptor.ivar_binding_symbol
                                                : nullptr;
  snapshot.synthesized_binding_symbol =
      descriptor.synthesized_binding_symbol != nullptr
          ? descriptor.synthesized_binding_symbol
          : nullptr;
  snapshot.ivar_layout_symbol =
      descriptor.ivar_layout_symbol != nullptr ? descriptor.ivar_layout_symbol
                                               : nullptr;
  snapshot.ivar_layout_replay_key =
      accessor.ivar_descriptor != nullptr &&
              accessor.ivar_descriptor->layout_replay_key != nullptr
          ? accessor.ivar_descriptor->layout_replay_key
          : descriptor.ivar_layout_replay_key;
  snapshot.property_attribute_profile =
      descriptor.property_attribute_profile != nullptr
          ? descriptor.property_attribute_profile
          : nullptr;
  snapshot.ownership_lifetime_profile =
      descriptor.ownership_lifetime_profile != nullptr
          ? descriptor.ownership_lifetime_profile
          : nullptr;
  snapshot.ownership_runtime_hook_profile =
      descriptor.ownership_runtime_hook_profile != nullptr
          ? descriptor.ownership_runtime_hook_profile
          : nullptr;
  snapshot.accessor_ownership_profile =
      descriptor.accessor_ownership_profile != nullptr
          ? descriptor.accessor_ownership_profile
          : nullptr;
  snapshot.getter_owner_identity =
      BorrowRuntimeCString(accessor.getter_owner_identity);
  snapshot.setter_owner_identity =
      BorrowRuntimeCString(accessor.setter_owner_identity);
}

}  // namespace objc3c::runtime
