#include "artifacts/json/capability_support_schema_records.h"

#include <array>

namespace objc3::artifacts::json {
namespace {

constexpr std::string_view kCapabilitySupportSchemaOwner =
    "native/objc3c/src/artifacts/json/capability_support_schema_records.cpp";

constexpr std::array<CapabilitySupportSchemaRecord, 2>
    kCapabilitySupportSchemaRecords{{
        {{"objc3c-capability-matrix-v1",
          "schema_version",
          "objc3c-capability-matrix-v1",
          "https://objc3c.dev/schemas/objc3c-capability-matrix-v1.schema.json",
          "schemas/objc3c-capability-matrix-v1.schema.json",
          kCapabilitySupportSchemaOwner,
          "capability-support"},
         "docs/support/capability_matrix.json",
         "docs/support/capability_matrix.md",
         "capability-matrix"},
        {{"objc3c-capability-evidence-map-v1",
          "schema_version",
          "objc3c-capability-evidence-map-v1",
          "https://objc3c.dev/schemas/objc3c-capability-evidence-map-v1.schema.json",
          "schemas/objc3c-capability-evidence-map-v1.schema.json",
          kCapabilitySupportSchemaOwner,
          "capability-support"},
         "docs/support/evidence_map.json",
         "docs/support/evidence_map.md",
         "evidence-map"},
    }};

constexpr std::array<ArtifactSchemaContract, 2>
    kCapabilitySupportSchemaContracts{{
        kCapabilitySupportSchemaRecords[0].contract,
        kCapabilitySupportSchemaRecords[1].contract,
    }};

}  // namespace

std::span<const CapabilitySupportSchemaRecord>
CapabilitySupportSchemaRecords() {
  return std::span<const CapabilitySupportSchemaRecord>(
      kCapabilitySupportSchemaRecords.data(),
      kCapabilitySupportSchemaRecords.size());
}

std::span<const ArtifactSchemaContract> CapabilitySupportSchemaContracts() {
  return std::span<const ArtifactSchemaContract>(
      kCapabilitySupportSchemaContracts.data(),
      kCapabilitySupportSchemaContracts.size());
}

}  // namespace objc3::artifacts::json
