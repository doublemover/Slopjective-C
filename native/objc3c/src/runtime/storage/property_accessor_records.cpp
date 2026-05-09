#include "runtime/storage/property_accessor_records.h"

#include "runtime/metadata/runtime_emitted_records.h"
#include "runtime/metadata/runtime_realized_records.h"
#include "runtime/storage/property_accessors.h"

#include <tuple>

namespace objc3c::runtime {

bool RuntimePropertyDescriptorHasRealizableAccessorShape(
    const EmittedPropertyDescriptor &descriptor) {
  return descriptor.declaration_owner_identity != nullptr &&
         descriptor.property_name != nullptr &&
         descriptor.effective_getter_selector != nullptr &&
         RuntimePropertyAccessorSelectorIsMaterializable(
             descriptor.effective_getter_selector);
}

bool RuntimePropertyDescriptorDeclaresSynthesizedStorageBinding(
    const EmittedPropertyDescriptor &descriptor) {
  return descriptor.synthesized_binding_symbol != nullptr &&
         descriptor.synthesized_binding_symbol[0] != '\0';
}

bool BuildRuntimePropertyAccessorRecord(
    const EmittedPropertyDescriptor &descriptor,
    const EmittedIvarDescriptor &ivar_descriptor,
    RealizedPropertyAccessor &accessor) {
  accessor = RealizedPropertyAccessor{};
  accessor.property_descriptor = &descriptor;
  accessor.ivar_descriptor = &ivar_descriptor;
  accessor.getter_return_kind =
      ClassifyRuntimePropertyAccessorReturnType(descriptor);
  accessor.getter_owner_identity = BuildRuntimePropertyAccessorOwnerIdentity(
      descriptor.declaration_owner_identity,
      descriptor.effective_getter_selector);
  if (descriptor.effective_setter_available &&
      descriptor.effective_setter_selector != nullptr &&
      descriptor.effective_setter_selector[0] != '\0') {
    if (!RuntimePropertyAccessorSelectorIsMaterializable(
            descriptor.effective_setter_selector) ||
        !RuntimePropertySetterHasSupportedArity(1)) {
      return false;
    }
    accessor.setter_owner_identity = BuildRuntimePropertyAccessorOwnerIdentity(
        descriptor.declaration_owner_identity,
        descriptor.effective_setter_selector);
  }
  return true;
}

std::string BuildRuntimePropertyAccessorOwnerIdentity(
    const char *declaration_owner_identity,
    const char *selector) {
  const std::string owner =
      declaration_owner_identity != nullptr ? declaration_owner_identity : "";
  const std::string selector_text = selector != nullptr ? selector : "";
  return owner + "::instance_method:" + selector_text;
}

RuntimeMethodReturnKind ClassifyRuntimePropertyAccessorReturnType(
    const EmittedPropertyDescriptor &descriptor) {
  return ClassifyRuntimeReturnType(descriptor.type_name);
}

bool RuntimePropertyAccessorSortsBefore(
    const RealizedPropertyAccessor &lhs,
    const RealizedPropertyAccessor &rhs) {
  const std::string lhs_owner =
      lhs.property_descriptor != nullptr &&
              lhs.property_descriptor->declaration_owner_identity != nullptr
          ? lhs.property_descriptor->declaration_owner_identity
          : "";
  const std::string rhs_owner =
      rhs.property_descriptor != nullptr &&
              rhs.property_descriptor->declaration_owner_identity != nullptr
          ? rhs.property_descriptor->declaration_owner_identity
          : "";
  const std::string lhs_name =
      lhs.property_descriptor != nullptr &&
              lhs.property_descriptor->property_name != nullptr
          ? lhs.property_descriptor->property_name
          : "";
  const std::string rhs_name =
      rhs.property_descriptor != nullptr &&
              rhs.property_descriptor->property_name != nullptr
          ? rhs.property_descriptor->property_name
          : "";
  return std::tie(lhs_owner, lhs_name, lhs.getter_owner_identity,
                  lhs.setter_owner_identity) <
         std::tie(rhs_owner, rhs_name, rhs.getter_owner_identity,
                  rhs.setter_owner_identity);
}

}  // namespace objc3c::runtime
