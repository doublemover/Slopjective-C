#include "sema/objc3_semantic_passes.h"
#include "sema/objc3_semantic_collection_helpers.h"
#include "sema/objc3_semantic_context.h"
#include "sema/objc3_semantic_constant_evaluator.h"
#include "sema/objc3_semantic_feature_claims.h"
#include "sema/objc3_semantic_generic_argument_relations.h"
#include "sema/objc3_semantic_pass_helpers.h"
#include "sema/objc3_semantic_symbol_lookup.h"
#include "sema/objc3_semantic_type_factory.h"
#include "sema/objc3_semantic_type_predicates.h"
#include "sema/objc3_semantic_type_helpers.h"
#include "sema/objc3_semantic_type_relations.h"
#include "sema/objc3_static_analysis.h"
#include "sema/objc3_type_form_scaffold.h"
#include "pipeline/objc3_frontend_types.h"

#include "diag/objc3_diag_utils.h"
#include "support/objc3_concurrency_symbol_profiles.h"
#include "support/objc3_method_family.h"
#include "support/objc3_property_storage_profile_helpers.h"
#include "support/objc3_receiver_profile_helpers.h"
#include "support/objc3_string_predicates.h"
#include "support/objc3_type_profile_helpers.h"
#include "support/objc3_value_type_names.h"

#include <algorithm>
#include <limits>
#include <map>
#include <optional>
#include <sstream>
#include <type_traits>
#include <unordered_set>
#include <vector>

using objc3c::support::IsObjCReferenceAliasValueType;

struct Objc3ProtocolSemanticDefinition;

static Objc3SemanticCanonicalType BuildCanonicalSemanticTypeFromParam(
    const FuncParam &param);
static Objc3SemanticCanonicalType BuildCanonicalSemanticTypeFromFunctionReturn(
    const FunctionDecl &fn);
static Objc3SemanticCanonicalType BuildCanonicalSemanticTypeFromMethodReturn(
    const Objc3MethodDecl &method);
static Objc3SemanticCanonicalType BuildCanonicalSemanticTypeFromProperty(
    const Objc3PropertyDecl &property);

static SemanticTypeInfo MakeSemanticTypeFromParam(const FuncParam &param) {
  if (param.vector_spelling) {
    return MakeVectorSemanticType(param.type, param.vector_base_spelling, param.vector_lane_count);
  }
  SemanticTypeInfo info = MakeScalarSemanticType(param.type);
  info.canonical_type = BuildCanonicalSemanticTypeFromParam(param);
  if (param.object_pointer_type_spelling) {
    info.object_pointer_type_name = param.object_pointer_type_name;
  }
  if (IsObjCReferenceAliasValueType(param.type)) {
    if (param.ownership_is_weak_reference) {
      info.ownership_kind = SemanticOwnershipKind::Weak;
    } else if (param.ownership_is_unowned_reference) {
      info.ownership_kind = SemanticOwnershipKind::Unowned;
    } else {
      info.ownership_kind = SemanticOwnershipKind::Retained;
    }
    info.has_nullability_suffix = !param.nullability_suffix_tokens.empty();
  }
  return info;
}

static SemanticTypeInfo MakeSemanticTypeFromFunctionReturn(const FunctionDecl &fn) {
  if (fn.return_vector_spelling) {
    return MakeVectorSemanticType(fn.return_type, fn.return_vector_base_spelling, fn.return_vector_lane_count);
  }
  SemanticTypeInfo info = MakeScalarSemanticType(fn.return_type);
  info.canonical_type = BuildCanonicalSemanticTypeFromFunctionReturn(fn);
  if (fn.return_object_pointer_type_spelling) {
    info.object_pointer_type_name = fn.return_object_pointer_type_name;
  }
  if (IsObjCReferenceAliasValueType(fn.return_type)) {
    if (fn.return_ownership_is_weak_reference) {
      info.ownership_kind = SemanticOwnershipKind::Weak;
    } else if (fn.return_ownership_is_unowned_reference) {
      info.ownership_kind = SemanticOwnershipKind::Unowned;
    } else {
      info.ownership_kind = SemanticOwnershipKind::Retained;
    }
    info.has_nullability_suffix = !fn.return_nullability_suffix_tokens.empty();
  }
  return info;
}

static SemanticTypeInfo MakeSemanticTypeFromMethodReturn(const Objc3MethodDecl &method) {
  if (method.return_vector_spelling) {
    return MakeVectorSemanticType(method.return_type, method.return_vector_base_spelling, method.return_vector_lane_count);
  }
  SemanticTypeInfo info = MakeScalarSemanticType(method.return_type);
  info.canonical_type = BuildCanonicalSemanticTypeFromMethodReturn(method);
  if (method.return_object_pointer_type_spelling) {
    info.object_pointer_type_name = method.return_object_pointer_type_name;
  }
  if (IsObjCReferenceAliasValueType(method.return_type)) {
    if (method.return_ownership_is_weak_reference) {
      info.ownership_kind = SemanticOwnershipKind::Weak;
    } else if (method.return_ownership_is_unowned_reference) {
      info.ownership_kind = SemanticOwnershipKind::Unowned;
    } else {
      info.ownership_kind = SemanticOwnershipKind::Retained;
    }
    info.has_nullability_suffix = !method.return_nullability_suffix_tokens.empty();
  }
  return info;
}

static SemanticTypeInfo MakeSemanticTypeFromFunctionInfoParam(const FunctionInfo &fn, std::size_t index) {
  if (index >= fn.param_types.size()) {
    return MakeScalarSemanticType(ValueType::Unknown);
  }

  if (index < fn.param_is_vector.size() && fn.param_is_vector[index]) {
    const std::string base_spelling =
        index < fn.param_vector_base_spelling.size() ? fn.param_vector_base_spelling[index] : "";
    const unsigned lane_count = index < fn.param_vector_lane_count.size() ? fn.param_vector_lane_count[index] : 1u;
    return MakeVectorSemanticType(fn.param_types[index], base_spelling, lane_count);
  }
  SemanticTypeInfo info = MakeScalarSemanticType(fn.param_types[index]);
  if (index < fn.param_canonical_types.size()) {
    info.canonical_type = fn.param_canonical_types[index];
  }
  if (IsObjCReferenceAliasValueType(fn.param_types[index])) {
    if (index < fn.param_ownership_is_weak_reference.size() &&
        fn.param_ownership_is_weak_reference[index]) {
      info.ownership_kind = SemanticOwnershipKind::Weak;
    } else if (index < fn.param_ownership_is_unowned_reference.size() &&
               fn.param_ownership_is_unowned_reference[index]) {
      info.ownership_kind = SemanticOwnershipKind::Unowned;
    } else {
      info.ownership_kind = SemanticOwnershipKind::Retained;
    }
    info.has_nullability_suffix =
        index < fn.param_has_nullability_suffix.size() &&
        fn.param_has_nullability_suffix[index];
  }
  return info;
}

static SemanticTypeInfo MakeSemanticTypeFromFunctionInfoReturn(const FunctionInfo &fn) {
  if (fn.return_is_vector) {
    return MakeVectorSemanticType(fn.return_type, fn.return_vector_base_spelling, fn.return_vector_lane_count);
  }
  SemanticTypeInfo info = MakeScalarSemanticType(fn.return_type);
  info.canonical_type = fn.return_canonical_type;
  if (IsObjCReferenceAliasValueType(fn.return_type)) {
    if (fn.return_ownership_is_weak_reference) {
      info.ownership_kind = SemanticOwnershipKind::Weak;
    } else if (fn.return_ownership_is_unowned_reference) {
      info.ownership_kind = SemanticOwnershipKind::Unowned;
    } else {
      info.ownership_kind = SemanticOwnershipKind::Retained;
    }
    info.has_nullability_suffix = fn.return_has_nullability_suffix;
  }
  return info;
}

static SemanticTypeInfo MakeSemanticTypeFromMethodInfoParam(const Objc3MethodInfo &method,
                                                            std::size_t index) {
  if (index >= method.param_types.size()) {
    return MakeScalarSemanticType(ValueType::Unknown);
  }
  if (index < method.param_is_vector.size() && method.param_is_vector[index]) {
    const std::string base_spelling =
        index < method.param_vector_base_spelling.size()
            ? method.param_vector_base_spelling[index]
            : "";
    const unsigned lane_count =
        index < method.param_vector_lane_count.size()
            ? method.param_vector_lane_count[index]
            : 1u;
    return MakeVectorSemanticType(method.param_types[index], base_spelling,
                                  lane_count);
  }
  SemanticTypeInfo info = MakeScalarSemanticType(method.param_types[index]);
  if (index < method.param_canonical_types.size()) {
    info.canonical_type = method.param_canonical_types[index];
  }
  if (IsObjCReferenceAliasValueType(method.param_types[index])) {
    if (index < method.param_ownership_is_weak_reference.size() &&
        method.param_ownership_is_weak_reference[index]) {
      info.ownership_kind = SemanticOwnershipKind::Weak;
    } else if (index < method.param_ownership_is_unowned_reference.size() &&
               method.param_ownership_is_unowned_reference[index]) {
      info.ownership_kind = SemanticOwnershipKind::Unowned;
    } else {
      info.ownership_kind = SemanticOwnershipKind::Retained;
    }
    info.has_nullability_suffix =
        index < method.param_has_nullability_suffix.size() &&
        method.param_has_nullability_suffix[index];
  }
  return info;
}

static SemanticTypeInfo MakeSemanticTypeFromMethodInfoReturn(
    const Objc3MethodInfo &method) {
  if (method.return_is_vector) {
    return MakeVectorSemanticType(method.return_type,
                                  method.return_vector_base_spelling,
                                  method.return_vector_lane_count);
  }
  SemanticTypeInfo info = MakeScalarSemanticType(method.return_type);
  info.canonical_type = method.return_canonical_type;
  if (IsObjCReferenceAliasValueType(method.return_type)) {
    if (method.return_ownership_is_weak_reference) {
      info.ownership_kind = SemanticOwnershipKind::Weak;
    } else if (method.return_ownership_is_unowned_reference) {
      info.ownership_kind = SemanticOwnershipKind::Unowned;
    } else {
      info.ownership_kind = SemanticOwnershipKind::Retained;
    }
    info.has_nullability_suffix = method.return_has_nullability_suffix;
  }
  return info;
}

static SemanticTypeInfo MakeSemanticTypeFromPropertyInfo(
    const Objc3PropertyInfo &property) {
  if (property.is_vector) {
    return MakeVectorSemanticType(property.type, property.vector_base_spelling,
                                  property.vector_lane_count);
  }
  SemanticTypeInfo info = MakeScalarSemanticType(property.type);
  info.canonical_type = property.canonical_type;
  if (IsObjCReferenceAliasValueType(property.type)) {
    if (property.ownership_is_weak_reference) {
      info.ownership_kind = SemanticOwnershipKind::Weak;
    } else if (property.ownership_is_unowned_reference) {
      info.ownership_kind = SemanticOwnershipKind::Unowned;
    } else {
      info.ownership_kind = SemanticOwnershipKind::Retained;
    }
    info.has_nullability_suffix = property.has_nullability_suffix;
  }
  return info;
}

static bool AreObjCReferenceTypesAssignmentCompatible(
    const SemanticTypeInfo &target, const SemanticTypeInfo &value,
    const Objc3SemanticIntegrationSurface *surface = nullptr);
static std::optional<bool> AreGenericSpecializationsVarianceAssignmentCompatible(
    const SemanticTypeInfo &target, const SemanticTypeInfo &value,
    const Objc3SemanticIntegrationSurface *surface);

static bool IsUnsafeNullableToNonnullFlow(const SemanticTypeInfo &target,
                                          const SemanticTypeInfo &value,
                                          const Objc3SemanticIntegrationSurface *surface = nullptr) {
  return IsNonnullDestinationObjCReferenceSemanticType(target) &&
         IsNullableObjCReferenceSemanticType(value) &&
         AreObjCReferenceTypesAssignmentCompatible(target, value, surface);
}

static bool AreObjCReferenceTypesAssignmentCompatible(const SemanticTypeInfo &target,
                                                      const SemanticTypeInfo &value,
                                                      const Objc3SemanticIntegrationSurface *surface) {
  if (!IsCanonicalObjc3TypeFormScaffoldReady()) {
    return false;
  }
  if (!IsObjCReferenceSemanticType(target) || !IsObjCReferenceSemanticType(value)) {
    return false;
  }
  if (const std::optional<bool> variance_compatibility =
          AreGenericSpecializationsVarianceAssignmentCompatible(target, value,
                                                               surface)) {
    return *variance_compatibility;
  }
  if (target.type == value.type) {
    return true;
  }
  // Canonical ObjC bridge-top forms are assignment-compatible by design.
  if (IsObjc3CanonicalBridgeTopReferenceTypeForm(target.type) ||
      IsObjc3CanonicalBridgeTopReferenceTypeForm(value.type)) {
    return true;
  }
  return false;
}

static std::string BuildBootstrapLegalityFailureContractReplayKey(
    const Objc3BootstrapLegalityFailureContractSummary &summary) {
  std::ostringstream out;
  out << summary.contract_id << ";surface_path=" << summary.surface_path
      << ";duplicate_registration_policy="
      << summary.duplicate_registration_policy
      << ";image_registration_order_invariant="
      << summary.image_registration_order_invariant
      << ";failure_mode=" << summary.failure_mode
      << ";restart_lifecycle_model=" << summary.restart_lifecycle_model
      << ";replay_order_model=" << summary.replay_order_model
      << ";image_local_init_reset_model="
      << summary.image_local_init_reset_model
      << ";catalog_retention_model=" << summary.catalog_retention_model
      << ";runtime_state_snapshot_symbol="
      << summary.runtime_state_snapshot_symbol;
  return out.str();
}

static Objc3BootstrapLegalityFailureContractSummary
BuildBootstrapLegalityFailureContractSummaryFromIntegrationSurface(
    const Objc3SemanticIntegrationSurface &surface) {
  Objc3BootstrapLegalityFailureContractSummary summary;
  // semantic migration anchor: canonical-mode legacy literal diagnostics stays
  // truthful only if the live fail-closed legacy-literal rejection surface is
  // preserved through the compatibility/strictness summary exported here.
  summary.fail_closed = true;
  summary.duplicate_registration_policy_frozen = true;
  summary.image_order_invariant_frozen = true;
  summary.bootstrap_rejection_frozen = true;
  summary.restart_boundary_frozen = true;
  summary.semantic_diagnostics_required = true;
  summary.ready_for_lowering_and_runtime =
      surface.interface_implementation_summary.deterministic;
  if (summary.ready_for_lowering_and_runtime) {
    summary.replay_key = BuildBootstrapLegalityFailureContractReplayKey(summary);
  }
  if (!IsReadyObjc3BootstrapLegalityFailureContractSummary(summary)) {
    summary.failure_reason =
        "bootstrap legality duplicate/order/failure contract summary is incomplete";
  }
  return summary;
}

static std::string BuildBootstrapLegalitySemanticsReplayKey(
    const Objc3BootstrapLegalitySemanticsSummary &summary) {
  std::ostringstream out;
  out << summary.contract_id
      << ";bootstrap_legality_failure_contract_id="
      << summary.bootstrap_legality_failure_contract_id
      << ";bootstrap_semantics_contract_id="
      << summary.bootstrap_semantics_contract_id
      << ";surface_path=" << summary.surface_path
      << ";duplicate_registration_policy="
      << summary.duplicate_registration_policy
      << ";image_registration_order_invariant="
      << summary.image_registration_order_invariant
      << ";cross_image_legality_model=" << summary.cross_image_legality_model
      << ";semantic_diagnostic_model=" << summary.semantic_diagnostic_model
      << ";bootstrap_legality_failure_replay_key="
      << summary.bootstrap_legality_failure_replay_key;
  return out.str();
}

static Objc3BootstrapLegalitySemanticsSummary
BuildBootstrapLegalitySemanticsSummaryFromIntegrationSurface(
    const Objc3SemanticIntegrationSurface &surface) {
  Objc3BootstrapLegalitySemanticsSummary summary;
  summary.fail_closed = true;
  summary.bootstrap_legality_failure_contract_ready =
      IsReadyObjc3BootstrapLegalityFailureContractSummary(
          surface.bootstrap_legality_failure_contract_summary);
  if (summary.bootstrap_legality_failure_contract_ready) {
    summary.duplicate_registration_policy =
        surface.bootstrap_legality_failure_contract_summary
            .duplicate_registration_policy;
    summary.image_registration_order_invariant =
        surface.bootstrap_legality_failure_contract_summary
            .image_registration_order_invariant;
    summary.bootstrap_legality_failure_replay_key =
        surface.bootstrap_legality_failure_contract_summary.replay_key;
  }
  summary.duplicate_registration_semantics_landed =
      summary.bootstrap_legality_failure_contract_ready;
  summary.image_order_semantics_landed =
      summary.bootstrap_legality_failure_contract_ready;
  summary.cross_image_legality_semantics_landed =
      summary.bootstrap_legality_failure_contract_ready;
  summary.semantic_diagnostics_landed =
      summary.bootstrap_legality_failure_contract_ready &&
      surface.bootstrap_legality_failure_contract_summary
          .semantic_diagnostics_required;
  summary.ready_for_lowering_and_runtime =
      summary.bootstrap_legality_failure_contract_ready &&
      surface.bootstrap_legality_failure_contract_summary
          .ready_for_lowering_and_runtime;
  if (summary.ready_for_lowering_and_runtime) {
    summary.replay_key = BuildBootstrapLegalitySemanticsReplayKey(summary);
  }
  if (!IsReadyObjc3BootstrapLegalitySemanticsSummary(summary)) {
    summary.failure_reason =
        "bootstrap legality duplicate/order semantics summary is incomplete";
  }
  return summary;
}

static std::string BuildBootstrapFailureRestartSemanticsReplayKey(
    const Objc3BootstrapFailureRestartSemanticsSummary &summary) {
  std::ostringstream out;
  out << summary.contract_id
      << ";bootstrap_legality_semantics_contract_id="
      << summary.bootstrap_legality_semantics_contract_id
      << ";bootstrap_reset_contract_id=" << summary.bootstrap_reset_contract_id
      << ";bootstrap_semantics_contract_id="
      << summary.bootstrap_semantics_contract_id
      << ";surface_path=" << summary.surface_path
      << ";failure_mode=" << summary.failure_mode
      << ";restart_lifecycle_model=" << summary.restart_lifecycle_model
      << ";replay_order_model=" << summary.replay_order_model
      << ";image_local_init_reset_model="
      << summary.image_local_init_reset_model
      << ";catalog_retention_model=" << summary.catalog_retention_model
      << ";unsupported_topology_model=" << summary.unsupported_topology_model
      << ";bootstrap_legality_semantics_replay_key="
      << summary.bootstrap_legality_semantics_replay_key;
  return out.str();
}

static Objc3BootstrapFailureRestartSemanticsSummary
BuildBootstrapFailureRestartSemanticsSummaryFromIntegrationSurface(
    const Objc3SemanticIntegrationSurface &surface) {
  Objc3BootstrapFailureRestartSemanticsSummary summary;
  summary.fail_closed = true;
  summary.bootstrap_legality_semantics_ready =
      IsReadyObjc3BootstrapLegalitySemanticsSummary(
          surface.bootstrap_legality_semantics_summary);
  if (summary.bootstrap_legality_semantics_ready) {
    summary.failure_mode =
        surface.bootstrap_legality_failure_contract_summary.failure_mode;
    summary.restart_lifecycle_model =
        surface.bootstrap_legality_failure_contract_summary
            .restart_lifecycle_model;
    summary.replay_order_model =
        surface.bootstrap_legality_failure_contract_summary.replay_order_model;
    summary.image_local_init_reset_model =
        surface.bootstrap_legality_failure_contract_summary
            .image_local_init_reset_model;
    summary.catalog_retention_model =
        surface.bootstrap_legality_failure_contract_summary
            .catalog_retention_model;
    summary.bootstrap_legality_semantics_replay_key =
        surface.bootstrap_legality_semantics_summary.replay_key;
  }
  summary.failure_mode_semantics_landed =
      summary.bootstrap_legality_semantics_ready;
  summary.restart_semantics_landed = summary.bootstrap_legality_semantics_ready;
  summary.replay_semantics_landed = summary.bootstrap_legality_semantics_ready;
  summary.unsupported_topology_semantics_landed =
      summary.bootstrap_legality_semantics_ready;
  summary.deterministic_recovery_semantics_landed =
      summary.bootstrap_legality_semantics_ready;
  summary.ready_for_lowering_and_runtime =
      summary.bootstrap_legality_semantics_ready &&
      surface.bootstrap_legality_semantics_summary.ready_for_lowering_and_runtime;
  if (summary.ready_for_lowering_and_runtime) {
    summary.replay_key =
        BuildBootstrapFailureRestartSemanticsReplayKey(summary);
  }
  if (!IsReadyObjc3BootstrapFailureRestartSemanticsSummary(summary)) {
    summary.failure_reason =
        "bootstrap failure-mode and restart semantics summary is incomplete";
  }
  return summary;
}

static std::string BuildCompatibilityStrictnessClaimSemanticsReplayKey(
    const Objc3CompatibilityStrictnessClaimSemanticsSummary &summary) {
  std::ostringstream out;
  out << summary.contract_id
      << ";inventory_contract_id="
      << summary.runnable_feature_claim_inventory_contract_id
      << ";truth_surface_contract_id="
      << summary.feature_claim_truth_surface_contract_id
      << ";surface_path=" << summary.surface_path
      << ";semantic_model=" << summary.semantic_model
      << ";downgrade_model=" << summary.downgrade_model
      << ";rejection_model=" << summary.rejection_model
      << ";canonical_interface_truth_model="
      << summary.canonical_interface_truth_model
      << ";separate_compilation_macro_truth_model="
      << summary.separate_compilation_macro_truth_model
      << ";canonical_interface_payload_mode="
      << summary.canonical_interface_payload_mode
      << ";language_profile=" << summary.effective_language_profile
      << ";valid_language_profiles="
      << summary.valid_language_profile_count
      << ";live_selection_surfaces=" << summary.live_selection_surface_count
      << ";valid_selection_combinations="
      << summary.valid_selection_combination_count
      << ";runnable_claims=" << summary.runnable_feature_claim_count
      << ";downgraded_source_only_claims="
      << summary.downgraded_source_only_claim_count
      << ";rejected_feature_claims="
      << summary.rejected_unsupported_feature_claim_count
      << ";live_unsupported_feature_families="
      << summary.live_unsupported_feature_family_count
      << ";live_unsupported_feature_sites="
      << summary.live_unsupported_feature_site_count
      << ";live_unsupported_feature_diagnostics="
      << summary.live_unsupported_feature_diagnostic_count
      << ";throws_source_rejections="
      << summary.throws_source_rejection_site_count
      << ";blocks_source_rejections="
      << summary.blocks_source_rejection_site_count
      << ";arc_source_rejections="
      << summary.arc_source_rejection_site_count
      << ";rejected_selection_surfaces="
      << summary.rejected_selection_surface_count
      << ";suppressed_macro_claims="
      << summary.suppressed_macro_claim_count
      << ";suppressed_macro_claim_ids=";
  for (std::size_t index = 0; index < summary.suppressed_macro_claim_ids.size();
       ++index) {
    if (index > 0u) {
      out << ",";
    }
    out << summary.suppressed_macro_claim_ids[index];
  }
  return out.str();
}

static Objc3CompatibilityStrictnessClaimSemanticsSummary
BuildCompatibilityStrictnessClaimSemanticsSummaryFromIntegrationSurface(
    const Objc3SemanticIntegrationSurface &surface,
    const Objc3UnsupportedFeatureClaimEnforcementStats
        &unsupported_feature_enforcement) {
  Objc3CompatibilityStrictnessClaimSemanticsSummary summary;
  summary.effective_language_profile = "canonical";
  summary.valid_language_profile_count =
      kObjc3CompatibilityStrictnessClaimValidLanguageProfileCount;
  summary.live_selection_surface_count =
      kObjc3CompatibilityStrictnessClaimLiveSelectionSurfaceCount;
  summary.valid_selection_combination_count =
      kObjc3CompatibilityStrictnessClaimValidSelectionCombinationCount;
  summary.runnable_feature_claim_count =
      kObjc3CompatibilityStrictnessClaimRunnableFeatureCount;
  summary.downgraded_source_only_claim_count =
      kObjc3CompatibilityStrictnessClaimSourceOnlyFeatureCount;
  summary.rejected_unsupported_feature_claim_count =
      kObjc3CompatibilityStrictnessClaimRejectedFeatureCount;
  summary.rejected_selection_surface_count =
      kObjc3CompatibilityStrictnessClaimRejectedSelectionSurfaceCount;
  summary.suppressed_macro_claim_count =
      kObjc3CompatibilityStrictnessClaimSuppressedMacroClaimCount;
  summary.live_unsupported_feature_family_count =
      unsupported_feature_enforcement.live_unsupported_feature_family_count;
  summary.live_unsupported_feature_site_count =
      unsupported_feature_enforcement.live_unsupported_feature_site_count;
  summary.live_unsupported_feature_diagnostic_count =
      unsupported_feature_enforcement.live_unsupported_feature_diagnostic_count;
  summary.throws_source_rejection_site_count =
      unsupported_feature_enforcement.throws_source_rejection_site_count;
  summary.blocks_source_rejection_site_count =
      unsupported_feature_enforcement.blocks_source_rejection_site_count;
  summary.arc_source_rejection_site_count =
      unsupported_feature_enforcement.arc_source_rejection_site_count;
  summary.suppressed_macro_claim_ids = {
      kObjc3SuppressedMacroClaimStrictnessLevel,
      kObjc3SuppressedMacroClaimConcurrencyMode,
      kObjc3SuppressedMacroClaimConcurrencyStrict,
  };
  summary.fail_closed = true;
  summary.language_profile_semantics_landed = true;
  summary.canonical_literal_rejection_semantics_landed = true;
  summary.source_only_claim_downgrade_semantics_landed = true;
  summary.unsupported_feature_claim_rejection_semantics_landed = true;
  summary.live_unsupported_feature_source_rejection_landed = true;
  summary.strictness_selection_rejection_semantics_landed = true;
  summary.feature_macro_claim_suppression_semantics_landed = true;
  summary.canonical_interface_truth_semantics_landed = true;
  summary.separate_compilation_macro_truth_semantics_landed = true;
  summary.selected_configuration_valid = true;
  summary.selected_configuration_downgraded = false;
  summary.selected_configuration_rejected = false;
  // semantic truth anchor: the current native toolchain still has
  // no standalone textual-interface payload, so canonical interface truth is
  // fail-closed by explicitly publishing that absence and the suppressed macro
  // claim set any future separate-compilation surface must preserve.
  summary.ready_for_lowering_and_runtime =
      surface.interface_implementation_summary.deterministic &&
      unsupported_feature_enforcement.live_unsupported_feature_site_count == 0u &&
      unsupported_feature_enforcement.live_unsupported_feature_diagnostic_count ==
          unsupported_feature_enforcement.live_unsupported_feature_site_count;
  if (summary.ready_for_lowering_and_runtime) {
    summary.replay_key =
        BuildCompatibilityStrictnessClaimSemanticsReplayKey(summary);
  }
  if (!IsReadyObjc3CompatibilityStrictnessClaimSemanticsSummary(summary)) {
    summary.failure_reason =
        "compatibility/strictness/claim semantics summary is incomplete";
  }
  return summary;
}

static void DiagnoseEscapingBlockRuntimeHandleCaptureLegality(
    unsigned line, unsigned column, const SemanticTypeInfo &value,
    std::vector<std::string> &diagnostics) {
  (void)line;
  (void)column;
  (void)diagnostics;
  if (!value.callable_block_runtime_handle_candidate) {
    return;
  }
}

static void DiagnoseUnsupportedFeatureClaimsInExpr(
    const Expr *expr,
    std::vector<std::string> &diagnostics,
    Objc3UnsupportedFeatureClaimEnforcementStats &stats,
    const Objc3UnsupportedFeatureClaimContext &context);

static bool BlockLiteralUsesRunnableC003Subset(const Expr &expr) {
  // byref-cell/copy-helper/dispose-helper anchor: the runnable block
  // slice now admits non-escaping byref and ownership-sensitive captures at the
  // unsupported-feature gate as soon as the source model is normalized. The
  // gate intentionally remains parser/source-truth driven so later sema and
  // lowering passes can consume the real byref/helper lowering path without
  // depending on post-sema runtime counters. Heap-promotion remains deferred
  // to later lowering/runtime checks instead of this source admission step.
  return expr.block_literal_is_normalized &&
         expr.block_source_model_is_normalized &&
         expr.block_parameter_count <= 4u &&
         expr.block_source_storage_annotations_are_normalized;
}

static bool BlockLiteralUsesRunnableC002Subset(const Expr &expr) {
  // executable-block-object/invoke-thunk anchor: preserve the
  // original runnable-block gate name as a compatibility surface for the C002
  // readiness contract while C003 widens the admitted non-escaping subset.
  return BlockLiteralUsesRunnableC003Subset(expr);
}

static bool ExprContainsBlockLiteral(const Expr *expr);

static bool StmtContainsBlockLiteral(const Stmt *stmt) {
  if (stmt == nullptr) {
    return false;
  }

  switch (stmt->kind) {
    case Stmt::Kind::Let:
      return stmt->let_stmt != nullptr &&
             ExprContainsBlockLiteral(stmt->let_stmt->value.get());
    case Stmt::Kind::Assign:
      return stmt->assign_stmt != nullptr &&
             ExprContainsBlockLiteral(stmt->assign_stmt->value.get());
    case Stmt::Kind::Return:
      return stmt->return_stmt != nullptr &&
             ExprContainsBlockLiteral(stmt->return_stmt->value.get());
    case Stmt::Kind::If:
      if (stmt->if_stmt == nullptr) {
        return false;
      }
      if (ExprContainsBlockLiteral(stmt->if_stmt->condition.get())) {
        return true;
      }
      for (const auto &then_stmt : stmt->if_stmt->then_body) {
        if (StmtContainsBlockLiteral(then_stmt.get())) {
          return true;
        }
      }
      for (const auto &else_stmt : stmt->if_stmt->else_body) {
        if (StmtContainsBlockLiteral(else_stmt.get())) {
          return true;
        }
      }
      return false;
    case Stmt::Kind::DoWhile:
      if (stmt->do_while_stmt == nullptr) {
        return false;
      }
      for (const auto &body_stmt : stmt->do_while_stmt->body) {
        if (StmtContainsBlockLiteral(body_stmt.get())) {
          return true;
        }
      }
      return ExprContainsBlockLiteral(stmt->do_while_stmt->condition.get());
    case Stmt::Kind::For:
      if (stmt->for_stmt == nullptr) {
        return false;
      }
      if (ExprContainsBlockLiteral(stmt->for_stmt->init.value.get()) ||
          ExprContainsBlockLiteral(stmt->for_stmt->condition.get()) ||
          ExprContainsBlockLiteral(stmt->for_stmt->step.value.get())) {
        return true;
      }
      for (const auto &body_stmt : stmt->for_stmt->body) {
        if (StmtContainsBlockLiteral(body_stmt.get())) {
          return true;
        }
      }
      return false;
    case Stmt::Kind::Switch:
      if (stmt->switch_stmt == nullptr) {
        return false;
      }
      if (ExprContainsBlockLiteral(stmt->switch_stmt->condition.get())) {
        return true;
      }
      for (const auto &switch_case : stmt->switch_stmt->cases) {
        for (const auto &case_stmt : switch_case.body) {
          if (StmtContainsBlockLiteral(case_stmt.get())) {
            return true;
          }
        }
      }
      return false;
    case Stmt::Kind::While:
      if (stmt->while_stmt == nullptr) {
        return false;
      }
      if (ExprContainsBlockLiteral(stmt->while_stmt->condition.get())) {
        return true;
      }
      for (const auto &body_stmt : stmt->while_stmt->body) {
        if (StmtContainsBlockLiteral(body_stmt.get())) {
          return true;
        }
      }
      return false;
    case Stmt::Kind::Block:
  case Stmt::Kind::Defer:
      if (stmt->block_stmt == nullptr) {
        return false;
      }
      for (const auto &body_stmt : stmt->block_stmt->body) {
        if (StmtContainsBlockLiteral(body_stmt.get())) {
          return true;
        }
      }
      return false;
    case Stmt::Kind::Expr:
      return stmt->expr_stmt != nullptr &&
             ExprContainsBlockLiteral(stmt->expr_stmt->value.get());
    case Stmt::Kind::Break:
    case Stmt::Kind::Continue:
    case Stmt::Kind::Empty:
      return false;
  }
  return false;
}

static bool ExprContainsBlockLiteral(const Expr *expr) {
  if (expr == nullptr) {
    return false;
  }
  if (expr->kind == Expr::Kind::BlockLiteral) {
    return true;
  }

  if (ExprContainsBlockLiteral(expr->left.get()) ||
      ExprContainsBlockLiteral(expr->right.get()) ||
      ExprContainsBlockLiteral(expr->third.get()) ||
      ExprContainsBlockLiteral(expr->receiver.get())) {
    return true;
  }
  for (const auto &arg : expr->args) {
    if (ExprContainsBlockLiteral(arg.get())) {
      return true;
    }
  }
  return false;
}

static bool FunctionBodyContainsBlockLiteral(const FunctionDecl &fn) {
  for (const auto &stmt : fn.body) {
    if (StmtContainsBlockLiteral(stmt.get())) {
      return true;
    }
  }
  return false;
}

static bool MethodBodyContainsBlockLiteral(const Objc3MethodDecl &method) {
  for (const auto &stmt : method.body) {
    if (StmtContainsBlockLiteral(stmt.get())) {
      return true;
    }
  }
  return false;
}

static void DiagnoseUnsupportedFeatureClaimsInStmt(
    const Stmt *stmt,
    std::vector<std::string> &diagnostics,
    Objc3UnsupportedFeatureClaimEnforcementStats &stats,
    const Objc3UnsupportedFeatureClaimContext &context) {
  if (stmt == nullptr) {
    return;
  }

  switch (stmt->kind) {
  case Stmt::Kind::Let:
    if (stmt->let_stmt != nullptr) {
      DiagnoseUnsupportedFeatureClaimsInExpr(
          stmt->let_stmt->value.get(), diagnostics, stats, context);
    }
    return;
  case Stmt::Kind::Assign:
    if (stmt->assign_stmt != nullptr) {
      DiagnoseUnsupportedFeatureClaimsInExpr(
          stmt->assign_stmt->value.get(), diagnostics, stats, context);
    }
    return;
  case Stmt::Kind::Return:
    if (stmt->return_stmt != nullptr) {
      DiagnoseUnsupportedFeatureClaimsInExpr(
          stmt->return_stmt->value.get(), diagnostics, stats, context);
    }
    return;
  case Stmt::Kind::If:
    if (stmt->if_stmt == nullptr) {
      return;
    }
    DiagnoseUnsupportedFeatureClaimsInExpr(
        stmt->if_stmt->condition.get(), diagnostics, stats, context);
    for (const auto &then_stmt : stmt->if_stmt->then_body) {
      DiagnoseUnsupportedFeatureClaimsInStmt(
          then_stmt.get(), diagnostics, stats, context);
    }
    for (const auto &else_stmt : stmt->if_stmt->else_body) {
      DiagnoseUnsupportedFeatureClaimsInStmt(
          else_stmt.get(), diagnostics, stats, context);
    }
    return;
  case Stmt::Kind::DoWhile:
    if (stmt->do_while_stmt == nullptr) {
      return;
    }
    for (const auto &body_stmt : stmt->do_while_stmt->body) {
      DiagnoseUnsupportedFeatureClaimsInStmt(
          body_stmt.get(), diagnostics, stats, context);
    }
    DiagnoseUnsupportedFeatureClaimsInExpr(
        stmt->do_while_stmt->condition.get(), diagnostics, stats, context);
    return;
  case Stmt::Kind::For:
    if (stmt->for_stmt == nullptr) {
      return;
    }
    DiagnoseUnsupportedFeatureClaimsInExpr(
        stmt->for_stmt->init.value.get(), diagnostics, stats, context);
    DiagnoseUnsupportedFeatureClaimsInExpr(
        stmt->for_stmt->condition.get(), diagnostics, stats, context);
    DiagnoseUnsupportedFeatureClaimsInExpr(
        stmt->for_stmt->step.value.get(), diagnostics, stats, context);
    for (const auto &body_stmt : stmt->for_stmt->body) {
      DiagnoseUnsupportedFeatureClaimsInStmt(
          body_stmt.get(), diagnostics, stats, context);
    }
    return;
  case Stmt::Kind::Switch:
    if (stmt->switch_stmt == nullptr) {
      return;
    }
    DiagnoseUnsupportedFeatureClaimsInExpr(
        stmt->switch_stmt->condition.get(), diagnostics, stats, context);
    for (const auto &switch_case : stmt->switch_stmt->cases) {
      for (const auto &case_stmt : switch_case.body) {
        DiagnoseUnsupportedFeatureClaimsInStmt(
            case_stmt.get(), diagnostics, stats, context);
      }
    }
    return;
  case Stmt::Kind::While:
    if (stmt->while_stmt == nullptr) {
      return;
    }
    DiagnoseUnsupportedFeatureClaimsInExpr(
        stmt->while_stmt->condition.get(), diagnostics, stats, context);
    for (const auto &body_stmt : stmt->while_stmt->body) {
      DiagnoseUnsupportedFeatureClaimsInStmt(
          body_stmt.get(), diagnostics, stats, context);
    }
    return;
  case Stmt::Kind::Block:
    if (stmt->block_stmt == nullptr) {
      return;
    }
    if (stmt->block_stmt->is_autoreleasepool_scope) {
      // autoreleasepool/destruction-order semantic expansion anchor:
      // lane-B identified the ownership-sensitive destruction-order surface so
      // later runtime work would not need to recover it from source.
      // runtime-memory-management implementation anchor: native mode
      // now accepts `@autoreleasepool` blocks and relies on emitted runtime
      // push/pop helpers plus live refcount/weak-table support instead of the
      // earlier fail-closed diagnostic path.
      // ownership-runtime-gate freeze anchor: lane-E now freezes
      // this accepted autoreleasepool slice as part of the supported ownership
      // runtime baseline and leaves ARC/block/public-ABI widening as explicit
      // non-goals for later milestones.
      // ownership-smoke closeout anchor: the runnable closeout matrix
      // consumes this admitted ownership-sensitive autoreleasepool slice
      // unchanged and may not widen it during the earlier tranche closeout.
    }
    for (const auto &body_stmt : stmt->block_stmt->body) {
      DiagnoseUnsupportedFeatureClaimsInStmt(
          body_stmt.get(), diagnostics, stats, context);
    }
    return;
  case Stmt::Kind::Defer:
    if (stmt->block_stmt == nullptr) {
      return;
    }
    if (ShouldRejectDeferFeatureClaim(context)) {
      diagnostics.push_back(MakeDiag(
          stmt->line,
          stmt->column,
          "O3S221",
          "unsupported feature claim: defer statements are not yet runnable in Objective-C 3 native mode"));
      ++stats.live_unsupported_feature_site_count;
      ++stats.live_unsupported_feature_diagnostic_count;
      return;
    }
    for (const auto &body_stmt : stmt->block_stmt->body) {
      DiagnoseUnsupportedFeatureClaimsInStmt(
          body_stmt.get(), diagnostics, stats, context);
    }
    return;
  case Stmt::Kind::Expr:
    if (stmt->expr_stmt != nullptr) {
      DiagnoseUnsupportedFeatureClaimsInExpr(
          stmt->expr_stmt->value.get(), diagnostics, stats, context);
    }
    return;
  case Stmt::Kind::Break:
  case Stmt::Kind::Continue:
  case Stmt::Kind::Empty:
    return;
  }
}

static void DiagnoseUnsupportedFeatureClaimsInExpr(
    const Expr *expr,
    std::vector<std::string> &diagnostics,
    Objc3UnsupportedFeatureClaimEnforcementStats &stats,
    const Objc3UnsupportedFeatureClaimContext &context) {
  if (expr == nullptr) {
    return;
  }

  switch (expr->kind) {
  // executable-block-source-closure anchor: once block literals
  // parse successfully they still fail closed here until the next runtime step and later
  // runtime issues make them runnable.
  case Expr::Kind::BlockLiteral:
    // block-source-model-completion anchor: source-only frontend
    // projection runs may admit block literals through sema so manifest-only
    // tooling can prove the completed source model, while native emit paths
    // continue to fail closed until later lowering/runtime work lands.
    // block-runtime-semantic-rules freeze anchor: current semantic
    // behavior is intentionally split between source-only admission and native
    // fail-closed rejection, with no runnable block object semantics implied.
    if (ShouldRejectBlockLiteralFeatureClaim(
            context,
            BlockLiteralUsesRunnableC002Subset(*expr))) {
      RecordUnsupportedFeatureClaimDiagnostic(
          stats.blocks_source_rejection_site_count,
          expr->line,
          expr->column,
          "unsupported feature claim: block literals are not yet runnable in Objective-C 3 native mode",
          diagnostics,
          stats);
    }
    return;
  case Expr::Kind::Call:
    for (const auto &arg : expr->args) {
      DiagnoseUnsupportedFeatureClaimsInExpr(arg.get(), diagnostics, stats, context);
    }
    return;
  case Expr::Kind::MessageSend:
    DiagnoseUnsupportedFeatureClaimsInExpr(
        expr->receiver.get(), diagnostics, stats, context);
    for (const auto &arg : expr->args) {
      DiagnoseUnsupportedFeatureClaimsInExpr(arg.get(), diagnostics, stats, context);
    }
    return;
  case Expr::Kind::Binary:
    DiagnoseUnsupportedFeatureClaimsInExpr(
        expr->left.get(), diagnostics, stats, context);
    DiagnoseUnsupportedFeatureClaimsInExpr(
        expr->right.get(), diagnostics, stats, context);
    return;
  case Expr::Kind::Conditional:
    DiagnoseUnsupportedFeatureClaimsInExpr(
        expr->left.get(), diagnostics, stats, context);
    DiagnoseUnsupportedFeatureClaimsInExpr(
        expr->right.get(), diagnostics, stats, context);
    DiagnoseUnsupportedFeatureClaimsInExpr(
        expr->third.get(), diagnostics, stats, context);
    return;
  case Expr::Kind::BoolLiteral:
  case Expr::Kind::Identifier:
  case Expr::Kind::NilLiteral:
  case Expr::Kind::Number:
  default:
    return;
  }
}
