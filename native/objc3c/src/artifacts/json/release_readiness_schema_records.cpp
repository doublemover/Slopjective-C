#include "artifacts/json/release_readiness_schema_records.h"

#include <array>

namespace objc3::artifacts::json {
namespace {

constexpr std::string_view kReleaseReadinessSchemaOwner =
    "native/objc3c/src/artifacts/json/release_readiness_schema_records.cpp";

constexpr std::array<ReleaseReadinessSchemaRecord, 3>
    kReleaseReadinessSchemaRecords{{
        {{"objc3c-tooling-release-evidence-operation-v1",
          "schema_id",
          "objc3c-tooling-release-evidence-operation-v1",
          "https://objc3c.dev/schemas/objc3c-tooling-release-evidence-operation-v1.schema.json",
          "schemas/objc3c-tooling-release-evidence-operation-v1.schema.json",
          kReleaseReadinessSchemaOwner,
          "release-evidence-operation"},
         "v0.11",
         ".objc3-release-evidence-operation.json",
         "release-evidence"},
        {{"objc3c-tooling-integrated-advanced-feature-gate-v1",
          "schema_id",
          "objc3c-tooling-integrated-advanced-feature-gate-v1",
          "https://objc3c.dev/schemas/objc3c-tooling-integrated-advanced-feature-gate-v1.schema.json",
          "schemas/objc3c-tooling-integrated-advanced-feature-gate-v1.schema.json",
          kReleaseReadinessSchemaOwner,
          "advanced-feature-gate"},
         "v0.11",
         ".objc3-advanced-feature-gate.json",
         "gate"},
        {{"objc3c-tooling-release-candidate-execution-matrix-v1",
          "schema_id",
          "objc3c-tooling-release-candidate-execution-matrix-v1",
          "https://objc3c.dev/schemas/objc3c-tooling-release-candidate-execution-matrix-v1.schema.json",
          "schemas/objc3c-tooling-release-candidate-execution-matrix-v1.schema.json",
          kReleaseReadinessSchemaOwner,
          "release-candidate-matrix"},
         "v0.11",
         ".objc3-release-candidate-matrix.json",
         "release-candidate"},
    }};

constexpr std::array<ArtifactSchemaContract, 3>
    kReleaseReadinessSchemaContracts{{
        kReleaseReadinessSchemaRecords[0].contract,
        kReleaseReadinessSchemaRecords[1].contract,
        kReleaseReadinessSchemaRecords[2].contract,
    }};

}  // namespace

std::span<const ReleaseReadinessSchemaRecord>
ReleaseReadinessSchemaRecords() {
  return std::span<const ReleaseReadinessSchemaRecord>(
      kReleaseReadinessSchemaRecords.data(),
      kReleaseReadinessSchemaRecords.size());
}

std::span<const ArtifactSchemaContract> ReleaseReadinessSchemaContracts() {
  return std::span<const ArtifactSchemaContract>(
      kReleaseReadinessSchemaContracts.data(),
      kReleaseReadinessSchemaContracts.size());
}

}  // namespace objc3::artifacts::json
