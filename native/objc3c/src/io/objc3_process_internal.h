#pragma once
#include "io/objc3_process.h"

#include "io/json/json_parser.h"
#include "io/json/json_writer.h"
#include "io/objc3_manifest_artifacts.h"
#include "io/objc3_json.h"
#include "lower/objc3_lowering_contract.h"
#include "support/objc3_identifier_safe_suffix.h"

#if defined(_WIN32)
#include <process.h>
#else
#include <spawn.h>
#include <sys/wait.h>
#endif

#include <algorithm>
#include <array>
#include <cctype>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <optional>
#include <sstream>
#include <string>
#include <string_view>
#include <unordered_set>
#include <utility>
#include <vector>


using objc3::io::EscapeJsonString;
using objc3::io::json::JsonObjectWriter;
using objc3::io::json::JsonValue;
#if !defined(_WIN32)
extern char **environ;
#endif

// live task runtime anchor: packaged task-runtime probes continue to
// use the existing runtime library path and linked execution surface; no
// separate public scheduler/runtime package is introduced here.
// hardening anchor: replay/hardening probes for the same task
// runtime continue to package against that existing runtime archive path.
// system-helper/runtime-contract anchor: Part 8 runtime/helper proof
// likewise stays on the same packaged runtime archive path. The driver/process
// layer does not introduce a separate cleanup/resource runtime package or a
// dedicated borrowed-pointer import surface for this freeze; there is still no
// dedicated borrowed-pointer import surface here.
// live cleanup/runtime integration anchor: the linked Part 8 runtime
// probe also packages against that same runtime archive path plus the emitted
// module object; there is still no separate resource-runtime package boundary.
// bridge-packaging/toolchain anchor: Part 11 now freezes the same
// packaged runtime archive and sidecar topology as the truthful toolchain-
// visible interop boundary. The process layer validates imported Part 11
// preservation packets through the mixed-module link plan, but it still does
// not claim live header/module/bridge generation here.

enum class ProducedObjectFormat : std::uint8_t {
  kUnknown = 0,
  kCoff = 1,
  kElf = 2,
  kMachO = 3,
};

inline constexpr const char *kObjc3RuntimeBootstrapTableConsumptionContractId =
    "objc3c.runtime.bootstrap.table.consumption.freeze.v1";
inline constexpr const char *kObjc3RuntimeBootstrapTableConsumptionModel =
    "next-public-register-call-consumes-staged-registration-table-once";
inline constexpr const char *kObjc3RuntimeBootstrapTableDeduplicationModel =
    "translation-unit-identity-key-rejection-before-registration-state-advance";
inline constexpr const char
    *kObjc3RuntimeBootstrapTableImageStatePublicationModel =
        "image-walk-snapshot-publishes-module-identity-root-counts-and-staged-table-usage";
inline constexpr const char *kObjc3RuntimeLiveRegistrationDiscoveryReplayContractId =
    "objc3c.runtime.live.registration.discovery.replay.v1";
inline constexpr const char *kObjc3RuntimeLiveRegistrationModel =
    "emitted-metadata-images-register-through-native-runtime-and-retained-bootstrap-catalog";
inline constexpr const char *kObjc3RuntimeLiveDiscoveryTrackingModel =
    "image-walk-snapshot-tracks-last-discovered-root-and-descriptor-families";
inline constexpr const char *kObjc3RuntimeLiveReplayTrackingModel =
    "reset-replay-state-snapshot-tracks-retained-images-reset-clears-and-last-replayed-identity";
inline constexpr const char *kObjc3RuntimeLiveRestartHardeningContractId =
    "objc3c.runtime.live.restart.hardening.v1";
inline constexpr const char *kObjc3RuntimeLiveIdempotenceModel =
    "second-live-replay-without-reset-fails-closed-and-preserves-live-runtime-state";
inline constexpr const char *kObjc3RuntimeLiveTeardownModel =
    "reset-clears-live-state-zeroes-image-local-init-cells-and-retains-bootstrap-catalog";
inline constexpr const char *kObjc3RuntimeLiveRestartEvidenceModel =
    "repeated-reset-replay-cycles-publish-monotonic-reset-and-replay-generations";
inline constexpr const char *kObjc3ToolchainConformanceClaimOperationsContractId =
    "objc3c.toolchain.conformance.claim.operations.v1";
inline constexpr const char *kObjc3ToolchainConformanceClaimValidationSchemaId =
    "objc3c-driver-conformance-validation-v1";
inline constexpr const char *kObjc3ToolchainConformanceClaimValidationModel =
    "driver-validates-versioned-conformance-report-and-publication-sidecars-before-toolchain-consumption";
inline constexpr const char *kObjc3ToolchainConformanceClaimConsumptionModel =
    "validation-consumes-json-sidecars-only-and-keeps-unsupported-profiles-fail-closed";
inline constexpr const char *kObjc3AdvancedFeatureOpsContractId =
    "objc3c.advanced.feature.ci.runbook.dashboard.contract.v1";
inline constexpr const char *kObjc3AdvancedFeatureReportingContractId =
    "objc3c.tooling.feature.aware.conformance.report.emission.v1";
inline constexpr const char *kObjc3AdvancedFeatureReleaseEvidenceContractId =
    "objc3c.tooling.corpus.sharding.release.evidence.packaging.v1";
inline constexpr const char *kObjc3AdvancedFeatureEvidenceGateScriptPath =
    "scripts/check_release_evidence.py";
inline constexpr const char *kObjc3AdvancedFeatureEvidenceRunbookPath =
    "spec/conformance/release_evidence_gate_maintenance.md";
inline constexpr const char *kObjc3AdvancedFeatureDashboardSchemaPath =
    "schemas/objc3-conformance-dashboard-status-v1.schema.json";
inline constexpr const char *kObjc3ReleaseEvidenceOperationContractId =
    "objc3c.tooling.release.evidence.toolchain.operations.v1";
inline constexpr const char *kObjc3ReleaseEvidenceOperationSchemaId =
    "objc3c-tooling-release-evidence-operation-v1";
inline constexpr const char *kObjc3DashboardStatusPublicationContractId =
    "objc3c.tooling.dashboard.status.publication.v1";
inline constexpr const char *kObjc3DashboardStatusSchemaId =
    "objc3-conformance-dashboard-status/v1";
inline constexpr const char *kObjc3DashboardVersion = "0.11.0";
inline constexpr const char *kObjc3DashboardReleaseId = "objc3c-v0.11";
inline constexpr const char *kObjc3AdvancedFeatureGateContractId =
    "objc3c.tooling.integrated.advanced.feature.gate.v1";
inline constexpr const char *kObjc3AdvancedFeatureGateSchemaId =
    "objc3c-tooling-integrated-advanced-feature-gate-v1";
inline constexpr const char *kObjc3ReleaseCandidateMatrixContractId =
    "objc3c.tooling.release.candidate.execution.matrix.v1";
inline constexpr const char *kObjc3ReleaseCandidateMatrixSchemaId =
    "objc3c-tooling-release-candidate-execution-matrix-v1";
inline constexpr const char *kObjc3AdvancedFeatureReleaseLabel = "v0.11";
inline constexpr const char *kObjc3DeterministicReplayTimestamp =
    "1970-01-01T00:00:00Z";
inline constexpr const char *kObjc3DeterministicSourceRevision = "0000000";
inline constexpr const char *kObjc3ConformanceProfileClaimPolicyModel =
    "core-profile-claimed-strict-profiles-targeted-for-release-evidence-and-fail-closed-until-runtime-backed";
inline constexpr const char *kObjc3ConformanceFormatClaimPolicyModel =
    "json-only-conformance-artifacts-remain-claimable-until-other-formats-gain-validation-and-publication-support";
inline constexpr const char *kObjc3ConformancePublicationFailClosedDiagnosticModel =
    "known-profiles-claimed-json-publication-remains-fail-closed-on-unsupported-formats-and-unknown-profiles";
// scheduler/executor runtime anchor: the driver/process layer still
// does not own scheduling itself, but emitted IR/object evidence now carries a
// frozen private task-runtime helper boundary that later runtime integration
// issues must consume without reconstructing helper or snapshot names ad hoc.

inline std::vector<std::string> BuildFixedStringVector(
    std::initializer_list<const char *> values) {
  return std::vector<std::string>(values.begin(), values.end());
}

inline bool IsRecognizedCoffMachine(std::uint16_t machine) {
  switch (machine) {
    case 0x014c:  // IMAGE_FILE_MACHINE_I386
    case 0x8664:  // IMAGE_FILE_MACHINE_AMD64
    case 0x01c0:  // IMAGE_FILE_MACHINE_ARM
    case 0xaa64:  // IMAGE_FILE_MACHINE_ARM64
      return true;
    default:
      return false;
  }
}

inline bool IsMachOMagic(std::uint32_t magic) {
  switch (magic) {
    case 0xfeedfaceu:
    case 0xcefaedfeu:
    case 0xfeedfacfu:
    case 0xcffaedfeu:
    case 0xcafebabeu:
    case 0xbebafecau:
      return true;
    default:
      return false;
  }
}

inline ProducedObjectFormat DetectProducedObjectFormat(
    const std::filesystem::path &object_out) {
  std::error_code file_size_error;
  const std::uintmax_t size =
      std::filesystem::file_size(object_out, file_size_error);
  if (file_size_error || size < 4) {
    return ProducedObjectFormat::kUnknown;
  }

  std::ifstream file(object_out, std::ios::binary);
  if (!file.is_open()) {
    return ProducedObjectFormat::kUnknown;
  }

  std::array<unsigned char, 8> header{};
  file.read(reinterpret_cast<char *>(header.data()),
            static_cast<std::streamsize>(header.size()));
  if (!file.good() && !file.eof()) {
    return ProducedObjectFormat::kUnknown;
  }
  if (file.gcount() < 4) {
    return ProducedObjectFormat::kUnknown;
  }

  if (header[0] == 0x7f && header[1] == 'E' && header[2] == 'L' &&
      header[3] == 'F') {
    return ProducedObjectFormat::kElf;
  }

  const std::uint32_t magic =
      static_cast<std::uint32_t>(header[0]) |
      (static_cast<std::uint32_t>(header[1]) << 8u) |
      (static_cast<std::uint32_t>(header[2]) << 16u) |
      (static_cast<std::uint32_t>(header[3]) << 24u);
  if (IsMachOMagic(magic)) {
    return ProducedObjectFormat::kMachO;
  }

  if (file.gcount() >= 2) {
    const std::uint16_t machine =
        static_cast<std::uint16_t>(header[0]) |
        (static_cast<std::uint16_t>(header[1]) << 8u);
    if (IsRecognizedCoffMachine(machine)) {
      return ProducedObjectFormat::kCoff;
    }
  }

  return ProducedObjectFormat::kUnknown;
}

inline void NormalizeCoffTimestamp(const std::filesystem::path &object_out) {
  std::error_code file_size_error;
  const std::uintmax_t size = std::filesystem::file_size(object_out, file_size_error);
  if (file_size_error || size < 8) {
    return;
  }

  std::fstream file(object_out, std::ios::in | std::ios::out | std::ios::binary);
  if (!file.is_open()) {
    return;
  }

  std::array<unsigned char, 8> header{};
  file.read(reinterpret_cast<char *>(header.data()), static_cast<std::streamsize>(header.size()));
  if (!file.good() && !file.eof()) {
    return;
  }
  if (file.gcount() != static_cast<std::streamsize>(header.size())) {
    return;
  }

  const std::uint16_t machine = static_cast<std::uint16_t>(header[0]) |
                                static_cast<std::uint16_t>(static_cast<std::uint16_t>(header[1]) << 8u);
  if (!IsRecognizedCoffMachine(machine)) {
    return;
  }

  const char zero_timestamp[4] = {0, 0, 0, 0};
  file.seekp(4, std::ios::beg);
  if (!file.good()) {
    return;
  }
  file.write(zero_timestamp, 4);
}

inline void NormalizeObjectDeterminism(const std::filesystem::path &object_out) {
  switch (DetectProducedObjectFormat(object_out)) {
    case ProducedObjectFormat::kCoff:
      NormalizeCoffTimestamp(object_out);
      return;
    case ProducedObjectFormat::kElf:
    case ProducedObjectFormat::kMachO:
    case ProducedObjectFormat::kUnknown:
    default:
      return;
  }
}

inline std::string ProducedObjectFormatName(ProducedObjectFormat format) {
  switch (format) {
    case ProducedObjectFormat::kCoff:
      return kObjc3RuntimeMetadataObjectFormatCoff;
    case ProducedObjectFormat::kElf:
      return kObjc3RuntimeMetadataObjectFormatElf;
    case ProducedObjectFormat::kMachO:
      return kObjc3RuntimeMetadataObjectFormatMachO;
    case ProducedObjectFormat::kUnknown:
    default:
      return "";
  }
}

inline bool ExtractBoundaryTokenValue(const std::string &line,
                               const std::string &key,
                               std::string &value) {
  const std::string token = key + "=";
  const std::size_t start = line.find(token);
  if (start == std::string::npos) {
    return false;
  }
  const std::size_t value_start = start + token.size();
  std::size_t value_end = line.find(';', value_start);
  if (value_end == std::string::npos) {
    value_end = line.size();
  }
  value = line.substr(value_start, value_end - value_start);
  return !value.empty();
}

inline int DecodeHexNibble(char ch) {
  if (ch >= '0' && ch <= '9') {
    return ch - '0';
  }
  if (ch >= 'a' && ch <= 'f') {
    return 10 + (ch - 'a');
  }
  if (ch >= 'A' && ch <= 'F') {
    return 10 + (ch - 'A');
  }
  return -1;
}

inline bool DecodeHexString(const std::string &text, std::string &decoded) {
  if ((text.size() % 2u) != 0u) {
    return false;
  }
  decoded.clear();
  decoded.reserve(text.size() / 2u);
  for (std::size_t i = 0; i < text.size(); i += 2u) {
    const int high = DecodeHexNibble(text[i]);
    const int low = DecodeHexNibble(text[i + 1u]);
    if (high < 0 || low < 0) {
      decoded.clear();
      return false;
    }
    decoded.push_back(static_cast<char>((high << 4) | low));
  }
  return true;
}

inline bool ExtractHexBoundaryTokenValue(const std::string &line,
                                  const std::string &key,
                                  std::string &value) {
  std::string encoded;
  if (!ExtractBoundaryTokenValue(line, key, encoded)) {
    return false;
  }
  return DecodeHexString(encoded, value) && !value.empty();
}

inline std::string BuildIndentedStringArrayJson(const std::vector<std::string> &values,
                                         const std::string &indent) {
  (void)indent;
  JsonValue::Array array;
  array.reserve(values.size());
  for (const std::string &value : values) {
    array.push_back(JsonValue::String(value));
  }
  return objc3::io::json::RenderJson(JsonValue::ArrayValue(std::move(array)));
}

inline bool TryParseJsonObjectText(const std::string &text,
                            const std::string &label,
                            JsonValue &value,
                            std::string &error) {
  objc3::io::json::JsonParseResult parsed = objc3::io::json::ParseJson(text);
  if (!parsed.ok()) {
    error = label + " is not valid JSON: " + parsed.error->Format();
    return false;
  }
  if (!parsed.value.IsObject()) {
    error = label + " is not a JSON object";
    return false;
  }
  value = std::move(parsed.value);
  return true;
}

inline bool TryGetJsonStringField(const JsonValue &object,
                           std::string_view field,
                           std::string &value) {
  const std::optional<std::string> field_value = object.GetString(field);
  if (!field_value.has_value()) {
    return false;
  }
  value = *field_value;
  return true;
}

inline bool TryGetJsonBoolField(const JsonValue &object,
                         std::string_view field,
                         bool &value) {
  const std::optional<bool> field_value = object.GetBool(field);
  if (!field_value.has_value()) {
    return false;
  }
  value = *field_value;
  return true;
}

inline bool TryGetJsonStringArrayField(const JsonValue &object,
                                std::string_view field,
                                std::vector<std::string> &values) {
  const JsonValue *field_value = object.Find(field);
  if (field_value == nullptr || !field_value->IsArray()) {
    return false;
  }
  std::vector<std::string> array_values;
  array_values.reserve(field_value->AsArray().size());
  for (const JsonValue &entry : field_value->AsArray()) {
    if (!entry.IsString()) {
      return false;
    }
    array_values.push_back(entry.AsString());
  }
  values = std::move(array_values);
  return true;
}

const JsonValue *FindJsonPath(
    const JsonValue &root,
    std::initializer_list<std::string_view> field_path) {
  const JsonValue *cursor = &root;
  for (std::string_view field : field_path) {
    if (cursor == nullptr) {
      return nullptr;
    }
    cursor = cursor->Find(field);
  }
  return cursor;
}

inline bool HasJsonField(const JsonValue &root,
                  std::initializer_list<std::string_view> field_path) {
  return FindJsonPath(root, field_path) != nullptr;
}

inline bool JsonStringFieldEquals(
    const JsonValue &root,
    std::initializer_list<std::string_view> field_path,
    std::string_view expected) {
  const JsonValue *field = FindJsonPath(root, field_path);
  return field != nullptr && field->IsString() && field->AsString() == expected;
}

inline std::string FinishJsonObject(JsonObjectWriter &object, std::ostringstream &out) {
  object.End();
  out << '\n';
  return out.str();
}

inline std::string ComputeFnv1a64Hex(const std::string &text) {
  std::uint64_t hash = 14695981039346656037ull;
  for (unsigned char c : text) {
    hash ^= static_cast<std::uint64_t>(c);
    hash *= 1099511628211ull;
  }
  std::ostringstream out;
  out << std::hex;
  out.width(16);
  out.fill('0');
  out << hash;
  return out.str();
}

inline std::string ComputeSha256ShapedContentDigest(const std::string &text) {
  return ComputeFnv1a64Hex("objc3c-dashboard-digest-1:" + text) +
         ComputeFnv1a64Hex("objc3c-dashboard-digest-2:" + text) +
         ComputeFnv1a64Hex("objc3c-dashboard-digest-3:" + text) +
         ComputeFnv1a64Hex("objc3c-dashboard-digest-4:" + text);
}

inline bool TryExtractJsonStringField(const std::string &text,
                               const std::string &field,
                               std::string &value) {
  JsonValue parsed;
  std::string parse_error;
  if (!TryParseJsonObjectText(text, "JSON object", parsed, parse_error)) {
    return false;
  }
  return TryGetJsonStringField(parsed, field, value);
}

inline bool TryExtractJsonBoolField(const std::string &text,
                             const std::string &field,
                             bool &value) {
  JsonValue parsed;
  std::string parse_error;
  if (!TryParseJsonObjectText(text, "JSON object", parsed, parse_error)) {
    return false;
  }
  return TryGetJsonBoolField(parsed, field, value);
}

inline bool TryExtractJsonStringArrayField(const std::string &text,
                                    const std::string &field,
                                    std::vector<std::string> &values) {
  JsonValue parsed;
  std::string parse_error;
  if (!TryParseJsonObjectText(text, "JSON object", parsed, parse_error)) {
    return false;
  }
  return TryGetJsonStringArrayField(parsed, field, values);
}
