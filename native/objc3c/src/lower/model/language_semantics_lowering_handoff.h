#pragma once

#include <array>
#include <string_view>

#include "sema/model/language_semantics_concrete_contracts.h"
#include "sema/model/language_semantics_public_model.h"

namespace objc3c::lower {

inline constexpr const char *kObjc3LanguageSemanticsLoweringHandoffContractId =
    "objc3c.language-semantics.lowering-handoff.v1";

struct Objc3LanguageSemanticsLoweringHandoffSurface {
  std::string_view contract_id =
      kObjc3LanguageSemanticsLoweringHandoffContractId;
  std::string_view public_model_contract_id =
      objc3c::sema::kObjc3PublicLanguageSemanticsModelContractId;
  std::array<std::string_view, 5> semantic_surfaces = {
      "generic-type-system",
      "protocol-existential-witness-model",
      "ownership-memory-model",
      "concurrency-usability-model",
      "runtime-api-facing-language-semantics",
  };
  std::array<std::string_view, 5> lowering_metadata_keys = {
      "generic-specialization-metadata",
      "protocol-witness-conformance-metadata",
      "ownership-cleanup-transfer-metadata",
      "concurrency-executor-effect-metadata",
      "public-runtime-language-semantics-snapshot",
  };
  std::array<std::string_view, 5> runtime_anchors = {
      "generic-runtime-identity-record",
      "QueryRealizedClassProtocolConformanceUnlocked",
      "RuntimeResultFailClosedOwnershipModel",
      "actor_mailbox_enqueue",
      "objc3_runtime_copy_language_semantics_surface",
  };
  bool generic_constraints_lower_as_metadata = true;
  bool protocol_witnesses_lower_as_typed_records = true;
  bool ownership_cleanup_paths_lower_as_ordered_edges = true;
  bool concurrency_effects_lower_as_executor_hops = true;
  bool runtime_api_snapshots_publish_handoff = true;
  bool source_fixtures_bound_to_handoff = true;
  bool fail_closed_for_unsupported_patterns = true;
  bool compatibility_shim_allowed = false;
  std::string_view replay_key =
      "language-semantics:generic+protocol+ownership+concurrency:lowering-v1";
  std::string_view failure_reason;
};

inline constexpr Objc3LanguageSemanticsLoweringHandoffSurface
    kObjc3LanguageSemanticsLoweringHandoffSurface{};

inline constexpr bool IsReadyObjc3LanguageSemanticsLoweringHandoffSurface(
    const Objc3LanguageSemanticsLoweringHandoffSurface &surface) {
  return surface.contract_id ==
             kObjc3LanguageSemanticsLoweringHandoffContractId &&
         surface.public_model_contract_id ==
             objc3c::sema::kObjc3PublicLanguageSemanticsModelContractId &&
         surface.generic_constraints_lower_as_metadata &&
         surface.protocol_witnesses_lower_as_typed_records &&
         surface.ownership_cleanup_paths_lower_as_ordered_edges &&
         surface.concurrency_effects_lower_as_executor_hops &&
         surface.runtime_api_snapshots_publish_handoff &&
         surface.source_fixtures_bound_to_handoff &&
         surface.fail_closed_for_unsupported_patterns &&
         !surface.compatibility_shim_allowed && !surface.replay_key.empty() &&
         surface.failure_reason.empty() &&
         objc3c::sema::
             AllObjc3LanguageSemanticsConcreteFixtureContractsReady();
}

}  // namespace objc3c::lower
