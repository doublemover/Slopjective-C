#include "lower/objc3_lowering_contract.h"

#include "lower/metadata/lowering_metadata_helpers.h"

#include <sstream>
#include <string>

bool TryBuildObjc3RuntimeMetadataLayoutPolicy(
    const Objc3RuntimeMetadataLayoutPolicyInput &input,
    Objc3RuntimeMetadataLayoutPolicy &policy, std::string &error) {
  policy = Objc3RuntimeMetadataLayoutPolicy{};
  policy.abi_contract_id = input.abi_contract_id;
  policy.scaffold_contract_id = input.scaffold_contract_id;
  policy.object_format = HostRuntimeMetadataObjectFormat();
  policy.section_spelling_model = HostRuntimeMetadataSectionSpellingModel();
  policy.retention_anchor_model = HostRuntimeMetadataRetentionAnchorModel();
  policy.image_info_symbol = input.image_info_symbol;
  policy.logical_image_info_section = input.image_info_section;
  policy.emitted_image_info_section = MapRuntimeMetadataSectionForObjectFormat(
      policy.object_format, input.image_info_section);
  policy.descriptor_symbol_prefix = input.descriptor_symbol_prefix;
  policy.descriptor_linkage = input.descriptor_linkage;
  policy.aggregate_linkage = input.aggregate_linkage;
  policy.metadata_visibility = input.metadata_visibility;
  policy.retention_root = input.retention_root;
  policy.total_retained_global_count = input.total_retained_global_count;
  policy.fail_closed = input.scaffold_fail_closed;

  if (input.abi_contract_id.empty()) {
    error = "runtime metadata layout policy requires a non-empty ABI contract id";
    policy.failure_reason = error;
    return false;
  }
  if (input.scaffold_contract_id.empty()) {
    error =
        "runtime metadata layout policy requires a non-empty scaffold contract id";
    policy.failure_reason = error;
    return false;
  }
  if (!input.section_boundary_ready || !input.runtime_export_ready ||
      !input.scaffold_emitted || !input.scaffold_fail_closed ||
      !input.uses_llvm_used || !input.image_info_emitted) {
    error = "runtime metadata layout policy prerequisites are not ready";
    policy.failure_reason = error;
    return false;
  }
  if (input.image_info_symbol.empty() || input.image_info_section.empty() ||
      policy.emitted_image_info_section.empty()) {
    error =
        "runtime metadata layout policy requires image-info symbol and section";
    policy.failure_reason = error;
    return false;
  }
  if (!IsSupportedRuntimeMetadataObjectFormat(policy.object_format) ||
      policy.section_spelling_model.empty() ||
      policy.retention_anchor_model.empty()) {
    error =
        "runtime metadata layout policy requires a supported explicit object-format surface";
    policy.failure_reason = error;
    return false;
  }
  if (input.descriptor_symbol_prefix.empty()) {
    error =
        "runtime metadata layout policy requires a descriptor symbol prefix";
    policy.failure_reason = error;
    return false;
  }
  if (input.descriptor_linkage != "private") {
    error = "runtime metadata layout policy requires descriptor linkage private";
    policy.failure_reason = error;
    return false;
  }
  if (input.aggregate_linkage != "internal") {
    error = "runtime metadata layout policy requires aggregate linkage internal";
    policy.failure_reason = error;
    return false;
  }
  if (input.metadata_visibility != "hidden") {
    error =
        "runtime metadata layout policy requires metadata visibility hidden";
    policy.failure_reason = error;
    return false;
  }
  if (input.retention_root != "llvm.used") {
    error = "runtime metadata layout policy requires llvm.used retention root";
    policy.failure_reason = error;
    return false;
  }

  for (std::size_t i = 0; i < kCanonicalRuntimeMetadataFamilyOrder.size(); ++i) {
    const auto &family_input = input.families[i];
    auto &family = policy.families[i];
    family.kind = kCanonicalRuntimeMetadataFamilyOrder[i];
    family.logical_section_name = family_input.section_name;
    family.emitted_section_name = MapRuntimeMetadataSectionForObjectFormat(
        policy.object_format, family_input.section_name);
    family.aggregate_symbol_name = family_input.aggregate_symbol_name;
    family.descriptor_count = family_input.descriptor_count;

    if (family_input.kind != kCanonicalRuntimeMetadataFamilyOrder[i]) {
      error = "runtime metadata layout policy family order mismatch at index " +
              std::to_string(i) + ": expected " +
              kCanonicalRuntimeMetadataFamilyOrder[i] + " but saw " +
              family_input.kind;
      policy.failure_reason = error;
      return false;
    }
    if (family.logical_section_name.empty() ||
        family.emitted_section_name.empty() ||
        family.aggregate_symbol_name.empty()) {
      error =
          "runtime metadata layout policy requires non-empty family section and aggregate names";
      policy.failure_reason = error;
      return false;
    }
  }

  const std::size_t total_descriptor_count =
      CountRuntimeMetadataLayoutDescriptors(policy.families);
  if (input.total_retained_global_count != total_descriptor_count + 6u) {
    error =
        "runtime metadata layout policy retained-global count drifted from descriptor inventory";
    policy.failure_reason = error;
    return false;
  }

  policy.ready = true;
  policy.fail_closed = true;
  return true;
}

bool IsReadyObjc3RuntimeMetadataLayoutPolicy(
    const Objc3RuntimeMetadataLayoutPolicy &policy) {
  if (!policy.ready || !policy.fail_closed || policy.contract_id.empty() ||
      policy.abi_contract_id.empty() || policy.scaffold_contract_id.empty() ||
      policy.object_format_surface_contract_id.empty() ||
      !IsSupportedRuntimeMetadataObjectFormat(policy.object_format) ||
      policy.section_spelling_model.empty() ||
      policy.retention_anchor_model.empty() || policy.image_info_symbol.empty() ||
      policy.logical_image_info_section.empty() ||
      policy.emitted_image_info_section.empty() ||
      policy.descriptor_symbol_prefix.empty() ||
      policy.descriptor_linkage != "private" ||
      policy.aggregate_linkage != "internal" ||
      policy.metadata_visibility != "hidden" ||
      policy.retention_root != "llvm.used" ||
      policy.family_ordering_model !=
          kObjc3RuntimeMetadataLayoutFamilyOrderingModel ||
      policy.descriptor_ordering_model !=
          kObjc3RuntimeMetadataDescriptorOrderingModel ||
      policy.aggregate_relocation_policy !=
          kObjc3RuntimeMetadataAggregateRelocationPolicy ||
      policy.comdat_policy != kObjc3RuntimeMetadataComdatPolicy ||
      policy.visibility_spelling_policy !=
          kObjc3RuntimeMetadataVisibilitySpellingPolicy ||
      policy.retention_ordering_model !=
          kObjc3RuntimeMetadataRetentionOrderingModel ||
      policy.object_format_policy_model !=
          kObjc3RuntimeMetadataObjectFormatPolicyModel ||
      !policy.failure_reason.empty()) {
    return false;
  }

  for (std::size_t i = 0; i < kCanonicalRuntimeMetadataFamilyOrder.size(); ++i) {
    const auto &family = policy.families[i];
    if (family.kind != kCanonicalRuntimeMetadataFamilyOrder[i] ||
        family.logical_section_name.empty() ||
        family.emitted_section_name.empty() ||
        family.aggregate_symbol_name.empty()) {
      return false;
    }
  }

  return policy.total_retained_global_count ==
         CountRuntimeMetadataLayoutDescriptors(policy.families) + 6u;
}

std::string Objc3RuntimeMetadataLayoutPolicyReplayKey(
    const Objc3RuntimeMetadataLayoutPolicy &policy) {
  std::ostringstream out;
  // normalized layout policy anchor: replay proof now serializes the
  // canonical normalized metadata layout decision rather than relying on
  // emitter-local hardcoded family ordering or relocation semantics.
  // object-format policy expansion anchor: replay proof now also
  // serializes the explicit host-format surface, including emitted section
  // spellings and retention-anchor behavior.
  out << "contract=" << policy.contract_id
      << ";abi_contract=" << policy.abi_contract_id
      << ";scaffold_contract=" << policy.scaffold_contract_id
      << ";object_format_contract="
      << policy.object_format_surface_contract_id
      << ";ready=" << BoolToken(policy.ready)
      << ";fail_closed=" << BoolToken(policy.fail_closed)
      << ";family_order=" << policy.family_ordering_model
      << ";descriptor_order=" << policy.descriptor_ordering_model
      << ";aggregate_relocation=" << policy.aggregate_relocation_policy
      << ";comdat=" << policy.comdat_policy
      << ";visibility_spelling=" << policy.visibility_spelling_policy
      << ";retention_order=" << policy.retention_ordering_model
      << ";object_format_model=" << policy.object_format_policy_model
      << ";object_format=" << policy.object_format
      << ";section_spelling_model=" << policy.section_spelling_model
      << ";retention_anchor_model=" << policy.retention_anchor_model
      << ";image_info=" << policy.image_info_symbol << "@"
      << policy.logical_image_info_section
      << ";image_info_emitted=" << policy.image_info_symbol << "@"
      << policy.emitted_image_info_section
      << ";descriptor_linkage=" << policy.descriptor_linkage
      << ";aggregate_linkage=" << policy.aggregate_linkage
      << ";metadata_visibility=" << policy.metadata_visibility
      << ";retention_root=" << policy.retention_root
      << ";total_retained_globals=" << policy.total_retained_global_count;
  for (const auto &family : policy.families) {
    out << ";family=" << family.kind << "|" << family.logical_section_name
        << "|" << family.aggregate_symbol_name << "|" << family.descriptor_count
        << ";family_emitted=" << family.kind << "|"
        << family.emitted_section_name << "|" << family.aggregate_symbol_name
        << "|" << family.descriptor_count;
  }
  if (!policy.failure_reason.empty()) {
    out << ";failure=" << policy.failure_reason;
  }
  return out.str();
}
