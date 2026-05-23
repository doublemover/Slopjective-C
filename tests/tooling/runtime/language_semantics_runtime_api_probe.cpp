#include "runtime/public/objc3_runtime_api.h"

#include <cstdio>

namespace {

bool SnapshotReady(
    const objc3_runtime_language_semantics_surface_snapshot &snapshot,
    int expected_kind,
    int expected_issue) {
  return snapshot.status == OBJC3_RUNTIME_LANGUAGE_SEMANTICS_STATUS_OK &&
         snapshot.surface_kind == expected_kind &&
         snapshot.issue_ref == expected_issue && snapshot.supported == 1 &&
         snapshot.runtime_metadata_evidence == 1 &&
         snapshot.sema_lowering_evidence == 1 &&
         snapshot.executable_fixture_evidence == 1 &&
         snapshot.fail_closed == 1 && snapshot.public_api_surface == 1 &&
         snapshot.associated_type_support == 0 &&
         snapshot.dynamic_existential_dispatch_support == 0 &&
         snapshot.support_claim != nullptr && snapshot.semantic_surface != nullptr &&
         snapshot.metadata_key != nullptr && snapshot.runtime_anchor != nullptr &&
         snapshot.witness_metadata_key != nullptr &&
         snapshot.conformance_metadata_key != nullptr &&
         snapshot.positive_fixture != nullptr &&
         snapshot.negative_fixture != nullptr &&
         snapshot.unsupported_associated_type_diagnostic != nullptr &&
         snapshot.unsupported_dynamic_dispatch_diagnostic != nullptr &&
         snapshot.unsupported_combination_diagnostic != nullptr &&
         snapshot.unsupported_policy != nullptr &&
         snapshot.combined_fixture != nullptr &&
         snapshot.combined_contract != nullptr &&
         snapshot.canonical_source_debug_map_bundle != nullptr &&
         snapshot.native_artifact_contract != nullptr &&
         snapshot.public_command != nullptr &&
         snapshot.replay_key != nullptr;
}

bool ProtocolSnapshotReady(
    const objc3_runtime_language_semantics_surface_snapshot &snapshot) {
  return SnapshotReady(
             snapshot,
             OBJC3_RUNTIME_LANGUAGE_SEMANTICS_SURFACE_PROTOCOL_EXISTENTIAL_WITNESS,
             8164) &&
         snapshot.witness_metadata_key[0] != '\0' &&
         snapshot.conformance_metadata_key[0] != '\0' &&
         snapshot.unsupported_associated_type_diagnostic[0] != '\0' &&
         snapshot.unsupported_dynamic_dispatch_diagnostic[0] != '\0';
}

bool AdvancedRuntimeClosureSnapshotReady(
    const objc3_runtime_language_semantics_surface_snapshot &snapshot) {
  return SnapshotReady(
             snapshot,
             OBJC3_RUNTIME_LANGUAGE_SEMANTICS_SURFACE_ADVANCED_RUNTIME_CLOSURE,
             8199) &&
         snapshot.combined_runtime_evidence == 1 &&
         snapshot.negative_combination_evidence == 1 &&
         snapshot.source_identity_evidence == 1 &&
         snapshot.umbrella_closure_support == 0 &&
         snapshot.canonical_source_debug_map_evidence == 1 &&
         snapshot.native_artifact_evidence == 1 &&
         snapshot.native_executable_umbrella_support == 0 &&
         snapshot.combined_runtime_state_record_count == 8u &&
         snapshot.canonical_source_map_record_count == 7u &&
         snapshot.canonical_debug_map_record_count == 7u &&
         snapshot.canonical_native_line_table_record_count == 7u &&
         snapshot.combined_interaction_record_count == 6u &&
         snapshot.combined_fixture[0] != '\0' &&
         snapshot.combined_contract[0] != '\0' &&
         snapshot.canonical_source_debug_map_bundle[0] != '\0' &&
         snapshot.native_artifact_contract[0] != '\0' &&
         snapshot.public_command[0] != '\0' &&
         snapshot.unsupported_combination_diagnostic[0] != '\0';
}

}  // namespace

int main() {
  objc3_runtime_language_semantics_surface_snapshot generic{};
  objc3_runtime_language_semantics_surface_snapshot protocol{};
  objc3_runtime_language_semantics_surface_snapshot ownership{};
  objc3_runtime_language_semantics_surface_snapshot concurrency{};
  objc3_runtime_language_semantics_surface_snapshot advanced{};
  objc3_runtime_language_semantics_surface_snapshot invalid{};
  objc3_runtime_language_semantics_surface_snapshot indexed{};

  const int indexed_status =
      objc3_runtime_copy_language_semantics_surface(0u, &indexed);
  const int generic_status =
      objc3_runtime_copy_language_semantics_surface_by_kind(
          OBJC3_RUNTIME_LANGUAGE_SEMANTICS_SURFACE_GENERIC_RUNTIME_IDENTITY,
          &generic);
  const int protocol_status =
      objc3_runtime_copy_language_semantics_surface_by_kind(
          OBJC3_RUNTIME_LANGUAGE_SEMANTICS_SURFACE_PROTOCOL_EXISTENTIAL_WITNESS,
          &protocol);
  const int ownership_status =
      objc3_runtime_copy_language_semantics_surface_by_kind(
          OBJC3_RUNTIME_LANGUAGE_SEMANTICS_SURFACE_OWNERSHIP_MEMORY_EDGE,
          &ownership);
  const int concurrency_status =
      objc3_runtime_copy_language_semantics_surface_by_kind(
          OBJC3_RUNTIME_LANGUAGE_SEMANTICS_SURFACE_CONCURRENCY_PUBLIC_API,
          &concurrency);
  const int advanced_status =
      objc3_runtime_copy_language_semantics_surface_by_kind(
          OBJC3_RUNTIME_LANGUAGE_SEMANTICS_SURFACE_ADVANCED_RUNTIME_CLOSURE,
          &advanced);
  const int invalid_status =
      objc3_runtime_copy_language_semantics_surface_by_kind(
          OBJC3_RUNTIME_LANGUAGE_SEMANTICS_SURFACE_INVALID, &invalid);

  const bool ok =
      objc3_runtime_language_semantics_api_abi_version() ==
          OBJC3_RUNTIME_LANGUAGE_SEMANTICS_ABI_VERSION &&
      objc3_runtime_language_semantics_surface_count() == 5u &&
      indexed_status == OBJC3_RUNTIME_LANGUAGE_SEMANTICS_STATUS_OK &&
      generic_status == OBJC3_RUNTIME_LANGUAGE_SEMANTICS_STATUS_OK &&
      protocol_status == OBJC3_RUNTIME_LANGUAGE_SEMANTICS_STATUS_OK &&
      ownership_status == OBJC3_RUNTIME_LANGUAGE_SEMANTICS_STATUS_OK &&
      concurrency_status == OBJC3_RUNTIME_LANGUAGE_SEMANTICS_STATUS_OK &&
      advanced_status == OBJC3_RUNTIME_LANGUAGE_SEMANTICS_STATUS_OK &&
      invalid_status == OBJC3_RUNTIME_LANGUAGE_SEMANTICS_STATUS_INVALID_QUERY &&
      SnapshotReady(indexed,
                    OBJC3_RUNTIME_LANGUAGE_SEMANTICS_SURFACE_GENERIC_RUNTIME_IDENTITY,
                    8160) &&
      SnapshotReady(generic,
                    OBJC3_RUNTIME_LANGUAGE_SEMANTICS_SURFACE_GENERIC_RUNTIME_IDENTITY,
                    8160) &&
      ProtocolSnapshotReady(protocol) &&
      SnapshotReady(ownership,
                    OBJC3_RUNTIME_LANGUAGE_SEMANTICS_SURFACE_OWNERSHIP_MEMORY_EDGE,
                    8166) &&
      SnapshotReady(concurrency,
                    OBJC3_RUNTIME_LANGUAGE_SEMANTICS_SURFACE_CONCURRENCY_PUBLIC_API,
                    8167) &&
      AdvancedRuntimeClosureSnapshotReady(advanced);

  std::printf("{");
  std::printf("\"surface_count\":%llu,",
              static_cast<unsigned long long>(
                  objc3_runtime_language_semantics_surface_count()));
  std::printf("\"indexed_status\":%d,", indexed_status);
  std::printf("\"generic_status\":%d,", generic_status);
  std::printf("\"protocol_status\":%d,", protocol_status);
  std::printf("\"ownership_status\":%d,", ownership_status);
  std::printf("\"concurrency_status\":%d,", concurrency_status);
  std::printf("\"advanced_status\":%d,", advanced_status);
  std::printf("\"advanced_combined_runtime_evidence\":%d,",
              advanced.combined_runtime_evidence);
  std::printf("\"advanced_negative_combination_evidence\":%d,",
              advanced.negative_combination_evidence);
  std::printf("\"advanced_source_identity_evidence\":%d,",
              advanced.source_identity_evidence);
  std::printf("\"advanced_umbrella_closure_support\":%d,",
              advanced.umbrella_closure_support);
  std::printf("\"advanced_canonical_source_debug_map_evidence\":%d,",
              advanced.canonical_source_debug_map_evidence);
  std::printf("\"advanced_native_artifact_evidence\":%d,",
              advanced.native_artifact_evidence);
  std::printf("\"advanced_native_executable_umbrella_support\":%d,",
              advanced.native_executable_umbrella_support);
  std::printf("\"advanced_canonical_source_map_record_count\":%u,",
              advanced.canonical_source_map_record_count);
  std::printf("\"advanced_canonical_debug_map_record_count\":%u,",
              advanced.canonical_debug_map_record_count);
  std::printf("\"advanced_canonical_native_line_table_record_count\":%u,",
              advanced.canonical_native_line_table_record_count);
  std::printf("\"invalid_status\":%d", invalid_status);
  std::printf("}\n");

  return ok ? 0 : 1;
}
