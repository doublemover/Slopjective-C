#include "runtime/public/objc3_runtime_language_semantics.h"

#include "runtime/classes/protocol_conformance.h"

#include <cstddef>
#include <cstdint>

namespace objc3c::runtime {
namespace {

struct RuntimeLanguageSemanticsSurfaceRecord {
  int surface_kind;
  int issue_ref;
  bool supported;
  bool runtime_metadata_evidence;
  bool sema_lowering_evidence;
  bool executable_fixture_evidence;
  bool fail_closed;
  bool public_api_surface;
  bool associated_type_support;
  bool dynamic_existential_dispatch_support;
  bool combined_runtime_evidence;
  bool negative_combination_evidence;
  bool source_identity_evidence;
  bool umbrella_closure_support;
  bool canonical_source_debug_map_evidence;
  std::uint32_t combined_runtime_state_record_count;
  std::uint32_t canonical_source_map_record_count;
  std::uint32_t canonical_debug_map_record_count;
  std::uint32_t canonical_native_line_table_record_count;
  std::uint32_t combined_interaction_record_count;
  const char *support_claim;
  const char *semantic_surface;
  const char *metadata_key;
  const char *runtime_anchor;
  const char *witness_metadata_key;
  const char *conformance_metadata_key;
  const char *positive_fixture;
  const char *negative_fixture;
  const char *diagnostic_code;
  const char *unsupported_associated_type_diagnostic;
  const char *unsupported_dynamic_dispatch_diagnostic;
  const char *unsupported_combination_diagnostic;
  const char *unsupported_policy;
  const char *combined_fixture;
  const char *combined_contract;
  const char *canonical_source_debug_map_bundle;
  const char *public_command;
  const char *replay_key;
};

constexpr const char *kUnsupportedPatternsPolicy =
    "unsupported patterns stay rejected or reserved until executable runtime "
    "metadata and diagnostics exist";

constexpr RuntimeLanguageSemanticsSurfaceRecord
    kRuntimeLanguageSemanticsSurfaces[] = {
        {OBJC3_RUNTIME_LANGUAGE_SEMANTICS_SURFACE_GENERIC_RUNTIME_IDENTITY,
         8160,
         true,
         true,
         true,
         true,
         true,
         true,
         false,
         false,
         false,
         false,
         false,
         false,
         false,
         0u,
         0u,
         0u,
         0u,
         0u,
         "objc3c.behavior.runtime.generics.cross-module-metadata",
         "generic-runtime-identity-record",
         "generic-specialization-metadata",
         "EmittedKeyPathDescriptor::generic_metadata_replay_key",
         "",
         "",
         "tests/tooling/fixtures/native/type_semantic_generic_method_substitution_positive.objc3",
         "tests/tooling/fixtures/native/recovery/negative/neg_param_type_generic.objc3",
         "O3S206",
         "",
         "",
         "",
         kUnsupportedPatternsPolicy,
         "",
         "",
         "",
         "npm run objc3c -- validate-type-conformance",
         "language-semantics:generic-runtime-identity:v1"},
        {OBJC3_RUNTIME_LANGUAGE_SEMANTICS_SURFACE_PROTOCOL_EXISTENTIAL_WITNESS,
         8164,
         true,
         true,
         true,
         true,
         true,
         true,
         false,
         false,
         false,
         false,
         false,
         false,
         false,
         0u,
         0u,
         0u,
         0u,
         0u,
         "objc3c.behavior.language.protocols.existential-witness-model",
         "protocol-existential-witness-record",
         objc3c::runtime::kObjc3ProtocolExistentialWitnessMetadataKey,
         "BuildRuntimeProtocolExistentialWitnessMetadata+QueryRealizedClassProtocolConformanceUnlocked",
         objc3c::runtime::kObjc3ProtocolExistentialWitnessMetadataKey,
         "ProtocolConformanceMatch",
         "tests/tooling/fixtures/native/execution/positive/id_protocol_qualifier_alias_signature.objc3",
         "tests/tooling/fixtures/native/recovery/negative/negative_protocol_existential_associated_type_rejected.objc3",
         "O3S216",
         objc3c::runtime::kObjc3ProtocolExistentialAssociatedTypeDiagnosticCode,
         objc3c::runtime::kObjc3ProtocolExistentialDynamicDispatchDiagnosticCode,
         "",
         kUnsupportedPatternsPolicy,
         "",
         "",
         "",
         "npm run objc3c -- validate-type-conformance",
         "language-semantics:protocol-existential-witness:v1"},
        {OBJC3_RUNTIME_LANGUAGE_SEMANTICS_SURFACE_OWNERSHIP_MEMORY_EDGE,
         8166,
         true,
         true,
         true,
         true,
         true,
         true,
         false,
         false,
         false,
         false,
         false,
         false,
         false,
         0u,
         0u,
         0u,
         0u,
         0u,
         "objc3c.behavior.language.ownership-memory-model",
         "ownership-runtime-edge",
         "ownership-cleanup-transfer-metadata",
         "RuntimeResultFailClosedOwnershipModel",
         "",
         "",
         "tests/tooling/fixtures/native/execution/positive/escaping_owned_object_block_copy_dispose.objc3",
         "tests/tooling/fixtures/native/recovery/negative/negative_language_semantics_conflicting_capture_ownership.objc3",
         "O3S301",
         "",
         "",
         "",
         kUnsupportedPatternsPolicy,
         "",
         "",
         "",
         "npm run objc3c -- test-runtime-acceptance-block-arc",
         "language-semantics:ownership-memory-edge:v1"},
        {OBJC3_RUNTIME_LANGUAGE_SEMANTICS_SURFACE_CONCURRENCY_PUBLIC_API,
         8167,
         true,
         true,
         true,
         true,
         true,
         true,
         false,
         false,
         false,
         false,
         false,
         false,
         false,
         0u,
         0u,
         0u,
         0u,
         0u,
         "objc3c.behavior.language.concurrency.public-usability-model",
         "concurrency-public-api-surface",
         "concurrency-executor-effect-metadata",
         "objc3_runtime_spawn_task_i32+objc3_runtime_executor_hop_i32+actor_mailbox_enqueue",
         "",
         "",
         "tests/tooling/fixtures/native/stdlib_concurrency_runtime_helper_surface_positive.objc3",
         "tests/tooling/fixtures/native/recovery/negative/negative_language_semantics_executor_on_sync_function.objc3",
         "O3S224",
         "",
         "",
         "",
         kUnsupportedPatternsPolicy,
         "",
         "",
         "",
         "npm run objc3c -- validate-concurrency-conformance",
         "language-semantics:concurrency-public-api-surface:v1"},
        {OBJC3_RUNTIME_LANGUAGE_SEMANTICS_SURFACE_ADVANCED_RUNTIME_CLOSURE,
         8199,
         true,
         true,
         true,
         true,
         true,
         true,
         false,
         false,
         true,
         true,
         true,
         false,
         true,
         8u,
         7u,
         7u,
         7u,
         6u,
         "objc3c.behavior.language.advanced-runtime-closure",
         "advanced-runtime-combined-closure",
         "advanced-runtime-combined-source-identity",
         "build_advanced_runtime_capability_split_contract+combined_runtime_identity_contract",
         "",
         "",
         "tests/native/runtime/advanced_closure/combined_positive.objc3",
         "tests/native/runtime/advanced_closure/negative_matrix.contract.json",
         "O3S336",
         "",
         "",
         "advanced-runtime.unsupported-combination",
         "combined runtime closure is bounded to checked ownership, blocks, "
         "errors, concurrency, property behavior, metaprogramming provenance, "
         "and package replay evidence; Swift ABI, distributed actors, broad "
         "scheduler guarantees, and arbitrary macro host execution remain "
         "reserved or rejected",
         "tests/native/runtime/advanced_closure/combined_positive.objc3",
         "tests/tooling/fixtures/advanced_runtime_closure/combined_runtime_identity_contract.json",
         "tests/tooling/fixtures/advanced_runtime_closure/combined_runtime_source_debug_map.json",
         "npm run objc3c -- validate-advanced-runtime-closure",
         "language-semantics:advanced-runtime-combined-closure:v1"},
};

constexpr std::uint64_t RuntimeLanguageSemanticsSurfaceCount() {
  return sizeof(kRuntimeLanguageSemanticsSurfaces) /
         sizeof(kRuntimeLanguageSemanticsSurfaces[0]);
}

void InitializeRuntimeLanguageSemanticsSnapshot(
    objc3_runtime_language_semantics_surface_snapshot &snapshot) {
  snapshot = {};
  snapshot.abi_version = OBJC3_RUNTIME_LANGUAGE_SEMANTICS_ABI_VERSION;
  snapshot.snapshot_size =
      sizeof(objc3_runtime_language_semantics_surface_snapshot);
  snapshot.status = OBJC3_RUNTIME_LANGUAGE_SEMANTICS_STATUS_NOT_FOUND;
}

void PopulateRuntimeLanguageSemanticsSnapshot(
    const RuntimeLanguageSemanticsSurfaceRecord &record,
    objc3_runtime_language_semantics_surface_snapshot &snapshot) {
  snapshot.status = OBJC3_RUNTIME_LANGUAGE_SEMANTICS_STATUS_OK;
  snapshot.surface_kind = record.surface_kind;
  snapshot.issue_ref = record.issue_ref;
  snapshot.supported = record.supported ? 1 : 0;
  snapshot.runtime_metadata_evidence =
      record.runtime_metadata_evidence ? 1 : 0;
  snapshot.sema_lowering_evidence = record.sema_lowering_evidence ? 1 : 0;
  snapshot.executable_fixture_evidence =
      record.executable_fixture_evidence ? 1 : 0;
  snapshot.fail_closed = record.fail_closed ? 1 : 0;
  snapshot.public_api_surface = record.public_api_surface ? 1 : 0;
  snapshot.associated_type_support =
      record.associated_type_support ? 1 : 0;
  snapshot.dynamic_existential_dispatch_support =
      record.dynamic_existential_dispatch_support ? 1 : 0;
  snapshot.combined_runtime_evidence =
      record.combined_runtime_evidence ? 1 : 0;
  snapshot.negative_combination_evidence =
      record.negative_combination_evidence ? 1 : 0;
  snapshot.source_identity_evidence =
      record.source_identity_evidence ? 1 : 0;
  snapshot.umbrella_closure_support =
      record.umbrella_closure_support ? 1 : 0;
  snapshot.canonical_source_debug_map_evidence =
      record.canonical_source_debug_map_evidence ? 1 : 0;
  snapshot.combined_runtime_state_record_count =
      record.combined_runtime_state_record_count;
  snapshot.canonical_source_map_record_count =
      record.canonical_source_map_record_count;
  snapshot.canonical_debug_map_record_count =
      record.canonical_debug_map_record_count;
  snapshot.canonical_native_line_table_record_count =
      record.canonical_native_line_table_record_count;
  snapshot.combined_interaction_record_count =
      record.combined_interaction_record_count;
  snapshot.support_claim = record.support_claim;
  snapshot.semantic_surface = record.semantic_surface;
  snapshot.metadata_key = record.metadata_key;
  snapshot.runtime_anchor = record.runtime_anchor;
  snapshot.witness_metadata_key = record.witness_metadata_key;
  snapshot.conformance_metadata_key = record.conformance_metadata_key;
  snapshot.positive_fixture = record.positive_fixture;
  snapshot.negative_fixture = record.negative_fixture;
  snapshot.diagnostic_code = record.diagnostic_code;
  snapshot.unsupported_associated_type_diagnostic =
      record.unsupported_associated_type_diagnostic;
  snapshot.unsupported_dynamic_dispatch_diagnostic =
      record.unsupported_dynamic_dispatch_diagnostic;
  snapshot.unsupported_combination_diagnostic =
      record.unsupported_combination_diagnostic;
  snapshot.unsupported_policy = record.unsupported_policy;
  snapshot.combined_fixture = record.combined_fixture;
  snapshot.combined_contract = record.combined_contract;
  snapshot.canonical_source_debug_map_bundle =
      record.canonical_source_debug_map_bundle;
  snapshot.public_command = record.public_command;
  snapshot.replay_key = record.replay_key;
}

}  // namespace
}  // namespace objc3c::runtime

extern "C" uint32_t objc3_runtime_language_semantics_api_abi_version(void) {
  return OBJC3_RUNTIME_LANGUAGE_SEMANTICS_ABI_VERSION;
}

extern "C" uint64_t objc3_runtime_language_semantics_surface_count(void) {
  return objc3c::runtime::RuntimeLanguageSemanticsSurfaceCount();
}

extern "C" int objc3_runtime_copy_language_semantics_surface(
    uint64_t index,
    objc3_runtime_language_semantics_surface_snapshot *snapshot) {
  if (snapshot == nullptr) {
    return OBJC3_RUNTIME_LANGUAGE_SEMANTICS_STATUS_INVALID_OUTPUT;
  }
  objc3c::runtime::InitializeRuntimeLanguageSemanticsSnapshot(*snapshot);
  if (index >= objc3c::runtime::RuntimeLanguageSemanticsSurfaceCount()) {
    return OBJC3_RUNTIME_LANGUAGE_SEMANTICS_STATUS_NOT_FOUND;
  }
  objc3c::runtime::PopulateRuntimeLanguageSemanticsSnapshot(
      objc3c::runtime::kRuntimeLanguageSemanticsSurfaces[index], *snapshot);
  return OBJC3_RUNTIME_LANGUAGE_SEMANTICS_STATUS_OK;
}

extern "C" int objc3_runtime_copy_language_semantics_surface_by_kind(
    int surface_kind,
    objc3_runtime_language_semantics_surface_snapshot *snapshot) {
  if (snapshot == nullptr) {
    return OBJC3_RUNTIME_LANGUAGE_SEMANTICS_STATUS_INVALID_OUTPUT;
  }
  objc3c::runtime::InitializeRuntimeLanguageSemanticsSnapshot(*snapshot);
  if (surface_kind <= OBJC3_RUNTIME_LANGUAGE_SEMANTICS_SURFACE_INVALID) {
    return OBJC3_RUNTIME_LANGUAGE_SEMANTICS_STATUS_INVALID_QUERY;
  }
  for (const auto &record :
       objc3c::runtime::kRuntimeLanguageSemanticsSurfaces) {
    if (record.surface_kind == surface_kind) {
      objc3c::runtime::PopulateRuntimeLanguageSemanticsSnapshot(record,
                                                                *snapshot);
      return OBJC3_RUNTIME_LANGUAGE_SEMANTICS_STATUS_OK;
    }
  }
  return OBJC3_RUNTIME_LANGUAGE_SEMANTICS_STATUS_NOT_FOUND;
}
