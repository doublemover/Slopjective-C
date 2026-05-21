#pragma once

#include <string>
#include <vector>

namespace objc3c::sema {

inline constexpr const char *kObjc3PublicLanguageSemanticsModelContractId =
    "objc3c.language.semantics.public-model.v1";
inline constexpr const char *kObjc3PublicLanguageSemanticsModelSurfacePath =
    "frontend.pipeline.semantic_surface.public_language_semantics_model";
inline constexpr const char *kObjc3PublicLanguageSemanticsAuthorityModel =
    "typed-sema-owned-language-contracts-with-runtime-lowering-handoffs";

enum class Objc3PublicLanguageSemanticCapabilityState {
  kSupported,
  kRejected,
  kReserved,
  kInternal,
};

enum class Objc3OwnershipTransferKind {
  kStrong,
  kWeak,
  kOwned,
  kBorrowed,
  kConsumed,
  kUnowned,
};

enum class Objc3ConcurrencyIsolationKind {
  kNone,
  kActor,
  kExecutor,
  kTaskGroup,
  kContinuation,
};

enum class Objc3ModuleVisibilityKind {
  kPublic,
  kPrivate,
  kInternal,
  kReexport,
};

enum class Objc3InteropLanguageKind {
  kC,
  kObjc2,
  kSwift,
  kCpp,
};

struct Objc3GenericTypeParameterSurface {
  std::string parameter_name;
  std::string variance_spelling;
  std::vector<std::string> upper_bound_protocols_lexicographic;
  std::string canonical_constraint_key;
  Objc3PublicLanguageSemanticCapabilityState public_state =
      Objc3PublicLanguageSemanticCapabilityState::kSupported;
  bool runtime_metadata_identity_required = true;
  bool higher_kinded_type_parameter_allowed = false;
};

struct Objc3GenericSpecializationSurface {
  std::string owner_canonical_name;
  std::vector<std::string> arguments_source_order;
  std::string substitution_replay_key;
  Objc3PublicLanguageSemanticCapabilityState public_state =
      Objc3PublicLanguageSemanticCapabilityState::kSupported;
  bool invariant_assignment_required = true;
  bool generic_collection_abi_claimed = false;
};

struct Objc3ProtocolExistentialWitnessSurface {
  std::string existential_canonical_spelling;
  std::vector<std::string> protocols_lexicographic;
  std::vector<std::string> witness_table_key_fields;
  std::string conformance_lookup_replay_key;
  Objc3PublicLanguageSemanticCapabilityState public_state =
      Objc3PublicLanguageSemanticCapabilityState::kSupported;
  bool witness_table_shape_published = true;
  bool associated_type_inference_claimed = false;
  bool swift_protocol_bridge_claimed = false;
};

struct Objc3OwnershipFlowSurface {
  std::string source_symbol;
  Objc3OwnershipTransferKind transfer_kind =
      Objc3OwnershipTransferKind::kStrong;
  std::vector<std::string> cleanup_paths_source_order;
  std::string escape_policy;
  Objc3PublicLanguageSemanticCapabilityState public_state =
      Objc3PublicLanguageSemanticCapabilityState::kSupported;
  bool runtime_result_contract_required = true;
  bool cleanup_order_is_deterministic = true;
};

struct Objc3ConcurrencyEffectSurface {
  std::string effect_spelling;
  Objc3ConcurrencyIsolationKind isolation_kind =
      Objc3ConcurrencyIsolationKind::kNone;
  std::string executor_policy;
  std::string cancellation_policy;
  std::string continuation_policy;
  Objc3PublicLanguageSemanticCapabilityState public_state =
      Objc3PublicLanguageSemanticCapabilityState::kSupported;
  bool sendability_boundary_required = true;
  bool async_cleanup_uses_ownership_model = true;
};

struct Objc3ModuleVisibilitySurface {
  std::string module_name;
  std::string module_identity_key;
  Objc3ModuleVisibilityKind visibility_kind = Objc3ModuleVisibilityKind::kPublic;
  std::vector<std::string> imported_module_identities_lexicographic;
  std::string deterministic_rebuild_key;
  Objc3PublicLanguageSemanticCapabilityState public_state =
      Objc3PublicLanguageSemanticCapabilityState::kSupported;
  bool package_lock_identity_required = true;
  bool hidden_import_access_rejected = true;
  bool visibility_drift_invalidates_rebuild = true;
};

struct Objc3InteropLaneSurface {
  Objc3InteropLanguageKind language_kind = Objc3InteropLanguageKind::kC;
  std::string surface_id;
  std::string bridge_metadata_key;
  std::vector<std::string> evidence_anchors_source_order;
  Objc3PublicLanguageSemanticCapabilityState public_state =
      Objc3PublicLanguageSemanticCapabilityState::kReserved;
  bool fail_closed = true;
  bool ownership_policy_explicit = true;
  bool async_policy_explicit = true;
};

struct Objc3PublicLanguageSemanticsModelSummary {
  std::string contract_id = kObjc3PublicLanguageSemanticsModelContractId;
  std::string frontend_surface_path =
      kObjc3PublicLanguageSemanticsModelSurfacePath;
  std::string authority_model = kObjc3PublicLanguageSemanticsAuthorityModel;
  std::vector<Objc3GenericTypeParameterSurface> generic_type_parameters;
  std::vector<Objc3GenericSpecializationSurface> generic_specializations;
  std::vector<Objc3ProtocolExistentialWitnessSurface>
      protocol_existential_witnesses;
  std::vector<Objc3OwnershipFlowSurface> ownership_flows;
  std::vector<Objc3ConcurrencyEffectSurface> concurrency_effects;
  std::vector<Objc3ModuleVisibilitySurface> module_visibility_surfaces;
  std::vector<Objc3InteropLaneSurface> interop_lane_surfaces;
  bool fail_closed = true;
  bool schema_fixture_published = false;
  bool typed_model_surface_published = true;
  bool generic_constraints_bound_to_protocol_existentials = false;
  bool ownership_model_bound_to_async_cleanup = false;
  bool concurrency_model_bound_to_actor_witness_checks = false;
  bool module_visibility_bound_to_package_identity = false;
  bool interop_lanes_bound_to_bridge_metadata = false;
  bool fallback_or_compatibility_shim_allowed = false;
  bool ready_for_lowering_handoff = false;
  std::string replay_key;
  std::string failure_reason;
};

inline bool IsReadyObjc3PublicLanguageSemanticsModelSummary(
    const Objc3PublicLanguageSemanticsModelSummary &summary) {
  return summary.contract_id == kObjc3PublicLanguageSemanticsModelContractId &&
         !summary.frontend_surface_path.empty() &&
         !summary.authority_model.empty() &&
         !summary.generic_type_parameters.empty() &&
         !summary.generic_specializations.empty() &&
         !summary.protocol_existential_witnesses.empty() &&
         !summary.ownership_flows.empty() &&
         !summary.concurrency_effects.empty() && summary.fail_closed &&
         !summary.module_visibility_surfaces.empty() &&
         !summary.interop_lane_surfaces.empty() &&
         summary.schema_fixture_published &&
         summary.typed_model_surface_published &&
         summary.generic_constraints_bound_to_protocol_existentials &&
         summary.ownership_model_bound_to_async_cleanup &&
         summary.concurrency_model_bound_to_actor_witness_checks &&
         summary.module_visibility_bound_to_package_identity &&
         summary.interop_lanes_bound_to_bridge_metadata &&
         !summary.fallback_or_compatibility_shim_allowed &&
         summary.ready_for_lowering_handoff && !summary.replay_key.empty() &&
         summary.failure_reason.empty();
}

}  // namespace objc3c::sema
