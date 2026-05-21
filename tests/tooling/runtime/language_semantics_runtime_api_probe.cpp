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
         snapshot.support_claim != nullptr && snapshot.semantic_surface != nullptr &&
         snapshot.metadata_key != nullptr && snapshot.runtime_anchor != nullptr &&
         snapshot.positive_fixture != nullptr &&
         snapshot.negative_fixture != nullptr && snapshot.replay_key != nullptr;
}

}  // namespace

int main() {
  objc3_runtime_language_semantics_surface_snapshot generic{};
  objc3_runtime_language_semantics_surface_snapshot protocol{};
  objc3_runtime_language_semantics_surface_snapshot ownership{};
  objc3_runtime_language_semantics_surface_snapshot concurrency{};
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
  const int invalid_status =
      objc3_runtime_copy_language_semantics_surface_by_kind(
          OBJC3_RUNTIME_LANGUAGE_SEMANTICS_SURFACE_INVALID, &invalid);

  const bool ok =
      objc3_runtime_language_semantics_api_abi_version() ==
          OBJC3_RUNTIME_LANGUAGE_SEMANTICS_ABI_VERSION &&
      objc3_runtime_language_semantics_surface_count() == 4u &&
      indexed_status == OBJC3_RUNTIME_LANGUAGE_SEMANTICS_STATUS_OK &&
      generic_status == OBJC3_RUNTIME_LANGUAGE_SEMANTICS_STATUS_OK &&
      protocol_status == OBJC3_RUNTIME_LANGUAGE_SEMANTICS_STATUS_OK &&
      ownership_status == OBJC3_RUNTIME_LANGUAGE_SEMANTICS_STATUS_OK &&
      concurrency_status == OBJC3_RUNTIME_LANGUAGE_SEMANTICS_STATUS_OK &&
      invalid_status == OBJC3_RUNTIME_LANGUAGE_SEMANTICS_STATUS_INVALID_QUERY &&
      SnapshotReady(indexed,
                    OBJC3_RUNTIME_LANGUAGE_SEMANTICS_SURFACE_GENERIC_RUNTIME_IDENTITY,
                    8160) &&
      SnapshotReady(generic,
                    OBJC3_RUNTIME_LANGUAGE_SEMANTICS_SURFACE_GENERIC_RUNTIME_IDENTITY,
                    8160) &&
      SnapshotReady(protocol,
                    OBJC3_RUNTIME_LANGUAGE_SEMANTICS_SURFACE_PROTOCOL_EXISTENTIAL_WITNESS,
                    8164) &&
      SnapshotReady(ownership,
                    OBJC3_RUNTIME_LANGUAGE_SEMANTICS_SURFACE_OWNERSHIP_MEMORY_EDGE,
                    8166) &&
      SnapshotReady(concurrency,
                    OBJC3_RUNTIME_LANGUAGE_SEMANTICS_SURFACE_CONCURRENCY_PUBLIC_API,
                    8167);

  std::printf("{");
  std::printf("\"surface_count\":%llu,",
              static_cast<unsigned long long>(
                  objc3_runtime_language_semantics_surface_count()));
  std::printf("\"indexed_status\":%d,", indexed_status);
  std::printf("\"generic_status\":%d,", generic_status);
  std::printf("\"protocol_status\":%d,", protocol_status);
  std::printf("\"ownership_status\":%d,", ownership_status);
  std::printf("\"concurrency_status\":%d,", concurrency_status);
  std::printf("\"invalid_status\":%d", invalid_status);
  std::printf("}\n");

  return ok ? 0 : 1;
}
