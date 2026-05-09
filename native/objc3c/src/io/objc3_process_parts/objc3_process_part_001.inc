#include "io/objc3_process.h"

#include "io/json/json_parser.h"
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
#include <unordered_set>
#include <utility>
#include <vector>

namespace {

using objc3::io::EscapeJsonString;
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

std::vector<std::string> BuildFixedStringVector(
    std::initializer_list<const char *> values) {
  return std::vector<std::string>(values.begin(), values.end());
}

bool IsRecognizedCoffMachine(std::uint16_t machine) {
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

bool IsMachOMagic(std::uint32_t magic) {
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

ProducedObjectFormat DetectProducedObjectFormat(
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

void NormalizeCoffTimestamp(const std::filesystem::path &object_out) {
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

void NormalizeObjectDeterminism(const std::filesystem::path &object_out) {
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

std::string ProducedObjectFormatName(ProducedObjectFormat format) {
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

bool ExtractBoundaryTokenValue(const std::string &line,
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

int DecodeHexNibble(char ch) {
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

bool DecodeHexString(const std::string &text, std::string &decoded) {
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

bool ExtractHexBoundaryTokenValue(const std::string &line,
                                  const std::string &key,
                                  std::string &value) {
  std::string encoded;
  if (!ExtractBoundaryTokenValue(line, key, encoded)) {
    return false;
  }
  return DecodeHexString(encoded, value) && !value.empty();
}

std::string BuildIndentedStringArrayJson(const std::vector<std::string> &values,
                                         const std::string &indent) {
  std::ostringstream out;
  out << "[\n";
  for (std::size_t index = 0; index < values.size(); ++index) {
    out << indent;
    objc3::io::WriteJsonString(out, values[index]);
    if (index + 1u < values.size()) {
      out << ",";
    }
    out << "\n";
  }
  out << indent.substr(0, indent.size() >= 2u ? indent.size() - 2u : 0u)
      << "]";
  return out.str();
}

std::string ComputeFnv1a64Hex(const std::string &text) {
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

std::string ComputeSha256ShapedContentDigest(const std::string &text) {
  return ComputeFnv1a64Hex("objc3c-dashboard-digest-1:" + text) +
         ComputeFnv1a64Hex("objc3c-dashboard-digest-2:" + text) +
         ComputeFnv1a64Hex("objc3c-dashboard-digest-3:" + text) +
         ComputeFnv1a64Hex("objc3c-dashboard-digest-4:" + text);
}

bool TryExtractJsonStringField(const std::string &text,
                               const std::string &field,
                               std::string &value) {
  const objc3::io::json::JsonParseResult parsed = objc3::io::json::ParseJson(text);
  if (!parsed.ok()) {
    return false;
  }
  const std::optional<std::string> field_value = parsed.value.GetString(field);
  if (!field_value.has_value()) {
    return false;
  }
  value = *field_value;
  return true;
}

bool TryExtractJsonBoolField(const std::string &text,
                             const std::string &field,
                             bool &value) {
  const objc3::io::json::JsonParseResult parsed = objc3::io::json::ParseJson(text);
  if (!parsed.ok()) {
    return false;
  }
  const std::optional<bool> field_value = parsed.value.GetBool(field);
  if (!field_value.has_value()) {
    return false;
  }
  value = *field_value;
  return true;
}

bool TryExtractJsonStringArrayField(const std::string &text,
                                    const std::string &field,
                                    std::vector<std::string> &values) {
  const objc3::io::json::JsonParseResult parsed = objc3::io::json::ParseJson(text);
  if (!parsed.ok()) {
    return false;
  }
  const objc3::io::json::JsonValue *field_value = parsed.value.Find(field);
  if (field_value == nullptr || !field_value->IsArray()) {
    return false;
  }
  std::vector<std::string> array_values;
  array_values.reserve(field_value->AsArray().size());
  for (const objc3::io::json::JsonValue &entry : field_value->AsArray()) {
    if (!entry.IsString()) {
      return false;
    }
    array_values.push_back(entry.AsString());
  }
  values = std::move(array_values);
  return true;
}

}  // namespace

std::vector<std::string> BuildObjc3ClaimedConformanceProfileIds() {
  return BuildFixedStringVector(
      {"core", "strict", "strict-concurrency", "strict-system"});
}

std::vector<std::string> BuildObjc3RejectedConformanceProfileIds() {
  return {};
}

std::vector<std::string> BuildObjc3ReleaseTargetedProfileIds() {
  return BuildFixedStringVector({"strict", "strict-concurrency", "strict-system"});
}

std::vector<std::filesystem::path>
BuildObjc3DeprecatedClaimCompatibilityArtifactPaths(
    const std::filesystem::path &out_dir,
    const std::string &emit_prefix) {
  return {
      out_dir / (emit_prefix + ".objc3-release-runtime-claim-matrix.json"),
      out_dir / (emit_prefix + ".objc3-dashboard-ready-summary.json"),
      out_dir / (emit_prefix + ".objc3-toolchain-runtime-ga-operations-scaffold.json"),
  };
}

bool DiagnoseObjc3DeprecatedClaimCompatibilityArtifacts(
    const std::filesystem::path &out_dir,
    const std::string &emit_prefix,
    std::string &error) {
  error.clear();
  std::vector<std::string> deprecated_artifacts;
  for (const auto &path :
       BuildObjc3DeprecatedClaimCompatibilityArtifactPaths(out_dir, emit_prefix)) {
    if (std::filesystem::exists(path)) {
      deprecated_artifacts.push_back(path.filename().string());
    }
  }
  if (deprecated_artifacts.empty()) {
    return true;
  }

  std::ostringstream out;
  out << "deprecated claim/scaffold compatibility sidecar(s) detected next to the live release artifacts: ";
  for (std::size_t index = 0; index < deprecated_artifacts.size(); ++index) {
    if (index != 0u) {
      out << ", ";
    }
    out << deprecated_artifacts[index];
  }
  out << " (remove them; the canonical release surface is the integrated "
         ".objc3-conformance-publication.json, "
         ".objc3-conformance-validation.json, "
         ".objc3-release-evidence-operation.json, "
         ".objc3-dashboard-status.json, "
         ".objc3-advanced-feature-gate.json, and "
         ".objc3-release-candidate-matrix.json artifacts in the active output directory, while reports and other transient evidence belong under tmp/)";
  error = out.str();
  return false;
}

bool IsObjc3ClaimedConformanceProfile(const std::string &profile_id) {
  return profile_id == "core" || profile_id == "strict" ||
         profile_id == "strict-concurrency" || profile_id == "strict-system";
}

bool IsObjc3SupportedConformanceFormat(const std::string &format) {
  return format == "json";
}

std::string BuildUnsupportedObjc3ConformanceProfileSelectionDiagnostic(
    const std::string &profile_id) {
  std::ostringstream out;
  out << "unsupported --objc3-conformance-profile selection: " << profile_id
      << " (claimed profiles: core, strict, strict-concurrency, strict-system; targeted release-evidence profiles: strict, strict-concurrency, strict-system; policy="
      << kObjc3ConformanceProfileClaimPolicyModel << ")";
  return out.str();
}

std::string BuildUnsupportedObjc3ConformanceFormatSelectionDiagnostic(
    const std::string &format) {
  std::ostringstream out;
  out << "unsupported --emit-objc3-conformance-format selection: " << format
      << " (claimed publication format: json; targeted release-evidence profiles: strict, strict-concurrency, strict-system; policy="
      << kObjc3ConformanceFormatClaimPolicyModel << ")";
  return out.str();
}

int RunProcess(const std::string &executable, const std::vector<std::string> &args) {
  std::vector<std::string> owned_argv;
  owned_argv.reserve(args.size() + 1);
  std::string argv0 = executable;
  const std::filesystem::path executable_path(executable);
  if (executable_path.has_filename()) {
    const std::string filename = executable_path.filename().string();
    if (!filename.empty()) {
      argv0 = filename;
    }
  }
  owned_argv.push_back(argv0);
  for (const auto &arg : args) {
    owned_argv.push_back(arg);
  }

  std::vector<const char *> argv;
  argv.reserve(owned_argv.size() + 1);
  for (const auto &arg : owned_argv) {
    argv.push_back(arg.c_str());
  }
  argv.push_back(nullptr);

#if defined(_WIN32)
  const bool has_explicit_path =
      executable.find('\\') != std::string::npos || executable.find('/') != std::string::npos ||
      executable.find(':') != std::string::npos;
  const int status = has_explicit_path ? _spawnv(_P_WAIT, executable.c_str(), argv.data())
                                       : _spawnvp(_P_WAIT, executable.c_str(), argv.data());
  if (status == -1) {
    return 127;
  }
  return status;
#else
  std::vector<char *> mutable_argv;
  mutable_argv.reserve(owned_argv.size() + 1);
  for (auto &arg : owned_argv) {
    mutable_argv.push_back(arg.data());
  }
  mutable_argv.push_back(nullptr);

  const bool has_explicit_path = executable.find('/') != std::string::npos;
  pid_t child_pid = 0;
  const int spawn_status =
      has_explicit_path ? posix_spawn(&child_pid, executable.c_str(), nullptr, nullptr, mutable_argv.data(), environ)
                        : posix_spawnp(&child_pid, executable.c_str(), nullptr, nullptr, mutable_argv.data(), environ);
  if (spawn_status != 0) {
    return 127;
  }

  int wait_status = 0;
  if (waitpid(child_pid, &wait_status, 0) < 0) {
    return 127;
  }
  if (WIFEXITED(wait_status)) {
    return WEXITSTATUS(wait_status);
  }
  if (WIFSIGNALED(wait_status)) {
    return 128 + WTERMSIG(wait_status);
  }
  return 127;
#endif
}

int RunObjectiveCCompile(const std::filesystem::path &clang_path,
                         const std::filesystem::path &input,
                         const std::filesystem::path &object_out) {
  const std::string clang_exe = clang_path.string();
  const int syntax_status =
      RunProcess(clang_exe, {"-x", "objective-c", "-std=gnu11", "-fsyntax-only", input.string()});
  if (syntax_status != 0) {
    return syntax_status;
  }

  const int compile_status = RunProcess(clang_exe, {"-x", "objective-c", "-std=gnu11", "-c", input.string(), "-o",
                                                    object_out.string(), "-fno-color-diagnostics"});
  if (compile_status == 0) {
    NormalizeObjectDeterminism(object_out);
  }
  return compile_status;
}

int RunIRCompile(const std::filesystem::path &clang_path,
                 const std::filesystem::path &ir_path,
                 const std::filesystem::path &object_out) {
  const std::string clang_exe = clang_path.string();
  const int compile_status = RunProcess(clang_exe, {"-x", "ir", "-c", ir_path.string(), "-o", object_out.string(),
                                                    "-fno-color-diagnostics"});
  if (compile_status == 0) {
    NormalizeObjectDeterminism(object_out);
  }
  return compile_status;
}

int RunIRCompileLLVMDirect(const std::filesystem::path &llc_path,
                           const std::filesystem::path &ir_path,
                           const std::filesystem::path &object_out,
                           std::string &error) {
#if defined(OBJC3C_ENABLE_LLVM_DIRECT_OBJECT_EMISSION)
  // emitted metadata inventory freeze anchor: the llvm-direct path
  // must preserve the IR-emitted runtime metadata section inventory exactly.
  // This backend boundary may not rewrite, rename, or silently substitute a
  // different metadata inventory model.
  // source-to-section matrix anchor: only image-info plus
  // class/protocol/category/property/ivar descriptor sections are materialized
  // today, while unsupported standalone rows stay explicit in the published
  // completeness matrix until later work lands.
  // layout/visibility policy anchor: llvm-direct object emission must preserve
  // emitted global order, section family order, the local-linkage/no-COMDAT policy,
  // and llvm.used retention order exactly. Backend execution may not inject
  // exported visibility or object-format-specific rewrites before the next runtime step lands.
  // normalized layout policy anchor: llvm-direct object emission now
  // consumes an IR surface that already encodes one normalized metadata layout replay key.
  // The backend may not reorder, relax, or reinterpret that
  // semantic finalization boundary.
  // object-format policy expansion anchor: llvm-direct object
  // emission now normalizes post-write determinism according to the produced
  // object format instead of assuming COFF-only behavior.
  // metadata section emission freeze anchor: the backend consumes a
  // real emitted metadata-section scaffold today, but it may not invent
  // richer payload bytes or new metadata families while lane-C is still frozen
  // at the placeholder-emission boundary.
  // class/metaclass data emission anchor: once the IR carries real
  // class descriptor bundles, llvm-direct object emission must preserve those
  // inline class/metaclass/name/method-ref payloads verbatim instead of
  // re-synthesizing or collapsing them back into placeholder bytes.
  // protocol/category data emission anchor: the same llvm-direct
  // path must preserve emitted protocol/category descriptor bundles,
  // inherited/adopted protocol-ref lists, and owner-identity attachment lists
  // verbatim instead of collapsing them back into placeholder bytes.
  // member-table data emission anchor: llvm-direct object emission
  // must preserve method/property/ivar payloads verbatim, including adjacent
  // owner-scoped method-table globals plus real property/ivar descriptor bytes
  // exactly as emitted in IR. The
  // selector/string pool expansion anchor extends that requirement to
  // canonical selector and string pool sections, including their stable ordinal
  // aggregates and pooled cstring payload bytes, without collapsing back to the
  // older selector-only global scheme.
  // binary inspection harness expansion anchor: llvm-direct object
  // emission must also preserve section families, relocation counts, and
  // aggregate symbol inventories in a form that stays inspectable through one
  // shared llvm-readobj/llvm-objdump corpus. Compile-failure cases must remain
  // fail-closed and produce no synthesized object-inspection artifacts.
  // object-packaging/retention freeze anchor: this same produced
  // object boundary is now frozen as the lane-D packaging handoff, rooted in
  // module.obj, @llvm.used retention, and retained __objc3_sec_* aggregate
  // symbols. Later archive/link/startup registration work may extend the
  // pipeline, but it may not replace or silently bypass these current anchors.
  // live-optional-send-and-keypath-runtime-support anchor: the same
  // llvm-direct object path must preserve retained keypath descriptor sections
  // and runtime-link sidecars because the runtime now consumes those
  // descriptors into a private keypath registry while optional sends keep
  // executing through the public lookup/dispatch path.
  // cross-module type-surface preservation anchor: imported runtime
  // surfaces now preserve the same optional/key-path packets, so the cross-
  // module link-plan path must carry those artifacts forward without silently
  // degrading them into generic metadata-only packaging.
  // backend may not drop, pool, or reshape those member records opportunistically.
  const int llc_status =
      RunProcess(llc_path.string(), {"-filetype=obj", "-o", object_out.string(), ir_path.string()});
  if (llc_status == 0) {
    NormalizeObjectDeterminism(object_out);
    return 0;
  }
  if (llc_status == 127) {
    error = "llvm-direct object emission failed: llc executable not found: " + llc_path.string();
    return 125;
  }
  error = "llvm-direct object emission failed: llc exited with status " + std::to_string(llc_status) + " for " +
          ir_path.string();
  return llc_status;
#else
  (void)llc_path;
  (void)ir_path;
  (void)object_out;
  error = "llvm-direct object emission backend unavailable in this build (enable OBJC3C_ENABLE_LLVM_DIRECT_OBJECT_EMISSION).";
  return 125;
#endif
}

bool TryBuildObjc3RuntimeMetadataLinkerRetentionArtifacts(
    const std::filesystem::path &ir_path,
    const std::filesystem::path &object_out,
    Objc3RuntimeMetadataLinkerRetentionArtifacts &artifacts,
    std::string &error) {
  // metadata-emission gate anchor: lane-E consumes the object-level
  // linker-retention/discovery artifacts published on this path together with
  // the C006 binary-inspection corpus and the D003 merged-discovery proof.
  // Any drift here must fail closed before later cross-lane closeout runs.
  // cross-lane object-emission closeout anchor: the same emitted
  // response/discovery artifacts are now replayed on integrated native class,
  // category, and message-send object probes, so this path must stay stable
  // enough for later startup-registration work to trust the produced objects.
  // translation-unit registration surface freeze: startup
  // registration must consume the linker-response/discovery sidecars derived
  // here without re-deriving translation-unit identity or renaming the public
  // discovery/linker-anchor boundary emitted by the earlier path.
  artifacts = Objc3RuntimeMetadataLinkerRetentionArtifacts{};
  error.clear();

  const ProducedObjectFormat produced_format =
      DetectProducedObjectFormat(object_out);
  artifacts.object_format = ProducedObjectFormatName(produced_format);
  artifacts.object_artifact_relative_path = object_out.filename().generic_string();
  if (artifacts.object_format.empty()) {
    error = "unable to determine produced object format for runtime metadata "
            "linker retention artifacts: " +
            object_out.string();
    return false;
  }

  std::ifstream ir_stream(ir_path, std::ios::binary);
  if (!ir_stream.is_open()) {
    error = "unable to open IR for runtime metadata linker retention artifacts: " +
            ir_path.string();
    return false;
  }

  std::string boundary_line;
  for (std::string line; std::getline(ir_stream, line);) {
    if (line.rfind("; runtime_metadata_linker_retention = ", 0) == 0) {
      boundary_line = std::move(line);
      break;
    }
  }
  if (boundary_line.empty()) {
    error = "runtime metadata linker retention boundary line not found in IR: " +
            ir_path.string();
    return false;
  }

  if (!ExtractBoundaryTokenValue(boundary_line, "linker_anchor_symbol",
                                 artifacts.linker_anchor_symbol) ||
      !ExtractBoundaryTokenValue(boundary_line, "discovery_root_symbol",
                                 artifacts.discovery_root_symbol) ||
      !ExtractBoundaryTokenValue(boundary_line,
                                 "linker_anchor_logical_section",
                                 artifacts.linker_anchor_logical_section) ||
      !ExtractBoundaryTokenValue(boundary_line,
                                 "discovery_root_logical_section",
                                 artifacts.discovery_root_logical_section) ||
      !ExtractBoundaryTokenValue(boundary_line,
                                 "linker_response_artifact_suffix",
                                 artifacts.linker_response_artifact_suffix) ||
      !(ExtractHexBoundaryTokenValue(boundary_line,
                                     "translation_unit_identity_key_hex",
                                     artifacts.translation_unit_identity_key) ||
        ExtractBoundaryTokenValue(boundary_line,
                                  "translation_unit_identity_key",
                                  artifacts.translation_unit_identity_key)) ||
      !ExtractBoundaryTokenValue(boundary_line, "discovery_artifact_suffix",
                                 artifacts.discovery_artifact_suffix)) {
    error = "runtime metadata linker retention boundary line is missing one or "
            "more required tokens: " +
            ir_path.string();
    return false;
  }

  artifacts.driver_linker_flag =
      Objc3RuntimeMetadataDriverLinkerRetentionFlagForObjectFormat(
          artifacts.object_format, artifacts.linker_anchor_symbol);
  if (artifacts.driver_linker_flag.empty()) {
    error = "no linker-retention driver flag available for produced object "
            "format " +
            artifacts.object_format;
    return false;
  }
  artifacts.linker_response_file_payload = artifacts.driver_linker_flag + "\n";

  artifacts.linker_anchor_emitted_section =
      Objc3RuntimeMetadataSectionForObjectFormat(
          artifacts.object_format, artifacts.linker_anchor_logical_section);
  artifacts.discovery_root_emitted_section =
      Objc3RuntimeMetadataSectionForObjectFormat(
          artifacts.object_format, artifacts.discovery_root_logical_section);
  artifacts.translation_unit_identity_model =
      kObjc3RuntimeArchiveStaticLinkTranslationUnitIdentityModel;

  std::ostringstream discovery;
  discovery << "{\n"
            << "  \"contract_id\": \""
            << EscapeJsonString(kObjc3RuntimeLinkerRetentionContractId)
            << "\",\n"
            << "  \"object_format\": \"" << EscapeJsonString(artifacts.object_format)
            << "\",\n"
            << "  \"object_artifact\": \""
            << EscapeJsonString(artifacts.object_artifact_relative_path)
            << "\",\n"
            << "  \"linker_anchor_symbol\": \""
            << EscapeJsonString(artifacts.linker_anchor_symbol) << "\",\n"
            << "  \"discovery_root_symbol\": \""
            << EscapeJsonString(artifacts.discovery_root_symbol) << "\",\n"
            << "  \"linker_anchor_logical_section\": \""
            << EscapeJsonString(artifacts.linker_anchor_logical_section) << "\",\n"
            << "  \"discovery_root_logical_section\": \""
            << EscapeJsonString(artifacts.discovery_root_logical_section) << "\",\n"
            << "  \"linker_anchor_emitted_section\": \""
            << EscapeJsonString(artifacts.linker_anchor_emitted_section) << "\",\n"
            << "  \"discovery_root_emitted_section\": \""
            << EscapeJsonString(artifacts.discovery_root_emitted_section) << "\",\n"
            << "  \"linker_response_artifact_suffix\": \""
            << EscapeJsonString(artifacts.linker_response_artifact_suffix) << "\",\n"
            << "  \"discovery_artifact_suffix\": \""
            << EscapeJsonString(artifacts.discovery_artifact_suffix) << "\",\n"
            << "  \"translation_unit_identity_model\": \""
            << EscapeJsonString(artifacts.translation_unit_identity_model)
            << "\",\n"
            << "  \"translation_unit_identity_key\": \""
            << EscapeJsonString(artifacts.translation_unit_identity_key) << "\",\n"
            << "  \"driver_linker_flags\": [\"" << EscapeJsonString(artifacts.driver_linker_flag)
            << "\"]\n"
            << "}\n";
  artifacts.discovery_json = discovery.str();
  return true;
}

bool TryBuildObjc3RuntimeTranslationUnitRegistrationManifestArtifact(
    const Objc3RuntimeTranslationUnitRegistrationManifestArtifactInputs &inputs,
    const Objc3RuntimeMetadataLinkerRetentionArtifacts &linker_retention_artifacts,
    std::size_t runtime_metadata_binary_byte_count,
    std::string &manifest_json,
    std::string &error) {
  manifest_json.clear();
  error.clear();

  if (inputs.contract_id.empty() ||
      inputs.translation_unit_registration_contract_id.empty() ||
      inputs.runtime_support_library_link_wiring_contract_id.empty() ||
      inputs.manifest_payload_model.empty() ||
      inputs.manifest_artifact_relative_path.empty() ||
      inputs.runtime_owned_payload_artifacts.size() != 3u ||
      inputs.runtime_support_library_archive_relative_path.empty() ||
      inputs.constructor_root_symbol.empty() ||
      inputs.constructor_root_ownership_model.empty() ||
      inputs.manifest_authority_model.empty() ||
      inputs.constructor_init_stub_symbol_prefix.empty() ||
      inputs.constructor_init_stub_ownership_model.empty() ||
      inputs.constructor_priority_policy.empty() ||
      inputs.registration_entrypoint_symbol.empty() ||
      inputs.translation_unit_identity_model.empty() ||
      inputs.launch_integration_contract_id.empty() ||
      inputs.runtime_library_resolution_model.empty() ||
      inputs.driver_linker_flag_consumption_model.empty() ||
      inputs.compile_wrapper_command_surface.empty() ||
      inputs.compile_proof_command_surface.empty() ||
      inputs.execution_smoke_command_surface.empty() ||
      inputs.registration_descriptor_source_contract_id.empty() ||
      inputs.registration_descriptor_source_surface_path.empty() ||
      inputs.registration_descriptor_pragma_name.empty() ||
      inputs.image_root_pragma_name.empty() ||
      inputs.module_identity_source.empty() ||
      inputs.registration_descriptor_identifier.empty() ||
      inputs.registration_descriptor_identity_source.empty() ||
      inputs.image_root_identifier.empty() ||
      inputs.image_root_identity_source.empty() ||
      inputs.bootstrap_visible_metadata_ownership_model.empty() ||
      inputs.total_descriptor_count !=
          inputs.class_descriptor_count + inputs.protocol_descriptor_count +
              inputs.category_descriptor_count +
              inputs.property_descriptor_count + inputs.ivar_descriptor_count ||
      inputs.bootstrap_semantics_contract_id.empty() ||
      inputs.duplicate_registration_policy.empty() ||
      inputs.realization_order_policy.empty() ||
      inputs.failure_mode.empty() ||
      inputs.registration_result_model.empty() ||
      inputs.registration_order_ordinal_model.empty() ||
      inputs.runtime_state_snapshot_symbol.empty() ||
      inputs.bootstrap_runtime_api_contract_id.empty() ||
      inputs.bootstrap_runtime_api_public_header_path.empty() ||
      inputs.bootstrap_runtime_api_archive_relative_path.empty() ||
      inputs.bootstrap_runtime_api_registration_status_enum_type.empty() ||
      inputs.bootstrap_runtime_api_image_descriptor_type.empty() ||
      inputs.bootstrap_runtime_api_selector_handle_type.empty() ||
      inputs.bootstrap_runtime_api_registration_snapshot_type.empty() ||
      inputs.bootstrap_runtime_api_registration_entrypoint_symbol.empty() ||
      inputs.bootstrap_runtime_api_selector_lookup_symbol.empty() ||
      inputs.bootstrap_runtime_api_dispatch_entrypoint_symbol.empty() ||
      inputs.bootstrap_runtime_api_state_snapshot_symbol.empty() ||
      inputs.bootstrap_runtime_api_reset_for_testing_symbol.empty() ||
      inputs.bootstrap_reset_contract_id.empty() ||
      inputs.bootstrap_reset_internal_header_path.empty() ||
      inputs.bootstrap_reset_replay_registered_images_symbol.empty() ||
      inputs.bootstrap_reset_reset_replay_state_snapshot_symbol.empty() ||
      inputs.bootstrap_reset_lifecycle_model.empty() ||
      inputs.bootstrap_reset_replay_order_model.empty() ||
      inputs.bootstrap_reset_image_local_init_state_reset_model.empty() ||
      inputs.bootstrap_reset_bootstrap_catalog_retention_model.empty() ||
      inputs.bootstrap_lowering_contract_id.empty() ||
      inputs.bootstrap_lowering_boundary_model.empty() ||
      inputs.bootstrap_global_ctor_list_model.empty() ||
      inputs.bootstrap_registration_table_layout_model.empty() ||
      inputs.bootstrap_image_local_initialization_model.empty() ||
      inputs.bootstrap_constructor_root_emission_state.empty() ||
      inputs.bootstrap_init_stub_emission_state.empty() ||
      inputs.bootstrap_registration_table_emission_state.empty() ||
      inputs.bootstrap_registration_table_symbol_prefix.empty() ||
      inputs.bootstrap_image_local_init_state_symbol_prefix.empty() ||
      inputs.bootstrap_registration_table_abi_version == 0 ||
      inputs.bootstrap_registration_table_pointer_field_count == 0 ||
      inputs.translation_unit_registration_order_ordinal == 0 ||
      inputs.object_artifact_relative_path.empty() ||
      inputs.backend_artifact_relative_path.empty()) {
    error =
        "translation-unit registration manifest inputs are incomplete";
    return false;
  }
  if (linker_retention_artifacts.translation_unit_identity_key.empty() ||
      linker_retention_artifacts.translation_unit_identity_model.empty() ||
      linker_retention_artifacts.driver_linker_flag.empty() ||
      linker_retention_artifacts.object_format.empty() ||
      linker_retention_artifacts.linker_anchor_symbol.empty() ||
      linker_retention_artifacts.discovery_root_symbol.empty()) {
    error =
        "translation-unit registration manifest requires populated linker-retention artifacts";
    return false;
  }
  if (linker_retention_artifacts.translation_unit_identity_model !=
      inputs.translation_unit_identity_model) {
    error =
        "translation-unit registration manifest identity model drifted from linker-retention artifacts";
    return false;
  }

  const std::string constructor_init_stub_symbol =
      inputs.constructor_init_stub_symbol_prefix +
      objc3c::support::MakeIdentifierSafeSuffix(
          linker_retention_artifacts.translation_unit_identity_key, "translation_unit");
  const std::string bootstrap_registration_table_symbol =
      inputs.bootstrap_registration_table_symbol_prefix +
      objc3c::support::MakeIdentifierSafeSuffix(
          linker_retention_artifacts.translation_unit_identity_key, "translation_unit");
  const std::string bootstrap_image_local_init_state_symbol =
      inputs.bootstrap_image_local_init_state_symbol_prefix +
      objc3c::support::MakeIdentifierSafeSuffix(
          linker_retention_artifacts.translation_unit_identity_key, "translation_unit");
  // bootstrap-invariant anchor: later startup registration must
  // preserve one init-stub/root identity per translation unit, reject
  // duplicate registration on the same identity key, and fail closed before
  // user entry if bootstrap materialization cannot honor that contract.
  // live bootstrap semantics anchor: the emitted registration
  // manifest now carries the exact duplicate-registration, order, failure-mode,
  // and runtime-status-code contract consumed by the real runtime library and
  // probe harness. Drift between the emitted manifest and runtime behavior must
  // fail closed before later constructor-root automation lands.
  // bootstrap-lowering anchor: the emitted registration manifest now
  // also freezes the lowering-owned ctor-root/init-stub/registration-table
  // materialization boundary. This artifact may publish the canonical names
  // and non-goal states, but it may not synthesize bootstrap globals on its
  // own ahead of the later lowering implementation issue.
  // constructor/init-stub emission anchor: once lowering emits the
  // real bootstrap globals, this manifest must publish the exact derived
  // init-stub and registration-table symbols from the full translation-unit
  // identity key, not a truncated semicolon-split fragment.
  // registration-table/image-local-init anchor: once lowering
  // expands that emitted boundary, the manifest must also publish the
  // self-describing registration-table layout contract and exact derived
  // image-local init-state symbol from the same translation-unit identity key.
  // runtime-bootstrap-api anchor: the same manifest also freezes the
  // runtime-owned bootstrap header/archive/entrypoint/reset surface so later
  // registrar/image-walk work consumes one canonical API contract instead of
  // re-deriving launch-path behavior from scattered runtime details.
  // launch-integration anchor: compile, proof, and execution-smoke
  // command surfaces must all consume this emitted registration manifest as the
  // authoritative runtime launch contract instead of guessing archive paths or
  // linker flags from implicit heuristics.
  // live catch/bridge/runtime integration anchor: runnable Part 6
  // probes keep using this same emitted runtime-library archive path and
  // linker-response topology; lane-D must prove linked error/bridge execution
  // through the packaged runtime rather than inventing a special-case driver
  // path for throws or catch handling.
  // startup-registration gate anchor: lane-E closes over this same
  // emitted manifest plus the replay-stable bootstrap evidence chain from
  // A002/B002/C003/D003/D004, so drift here must fail closed before E002.
  // runbook-closeout anchor: the operator runbook must stay bound to this emitted launch contract
  // and prove the same integrated path end to end.

  std::ostringstream out;
  out << "{\n"
      << "  \"contract_id\": \"" << EscapeJsonString(inputs.contract_id)
      << "\",\n"
      << "  \"launch_integration_contract_id\": \""
      << EscapeJsonString(inputs.launch_integration_contract_id) << "\",\n"
      << "  \"translation_unit_registration_contract_id\": \""
      << EscapeJsonString(inputs.translation_unit_registration_contract_id)
      << "\",\n"
      << "  \"runtime_support_library_link_wiring_contract_id\": \""
      << EscapeJsonString(
             inputs.runtime_support_library_link_wiring_contract_id)
      << "\",\n"
      << "  \"manifest_payload_model\": \""
      << EscapeJsonString(inputs.manifest_payload_model) << "\",\n"
      << "  \"manifest_artifact\": \""
      << EscapeJsonString(inputs.manifest_artifact_relative_path) << "\",\n"
      << "  \"object_artifact\": \""
      << EscapeJsonString(inputs.object_artifact_relative_path) << "\",\n"
      << "  \"backend_artifact\": \""
      << EscapeJsonString(inputs.backend_artifact_relative_path) << "\",\n"
      << "  \"runtime_owned_payload_artifacts\": [\n"
      << "    \""
      << EscapeJsonString(inputs.runtime_owned_payload_artifacts[0])
      << "\",\n"
      << "    \""
      << EscapeJsonString(inputs.runtime_owned_payload_artifacts[1])
      << "\",\n"
      << "    \""
      << EscapeJsonString(inputs.runtime_owned_payload_artifacts[2])
      << "\"\n"
      << "  ],\n"
      << "  \"runtime_metadata_binary_byte_count\": "
      << runtime_metadata_binary_byte_count << ",\n"
      << "  \"runtime_support_library_archive_relative_path\": \""
      << EscapeJsonString(inputs.runtime_support_library_archive_relative_path)
      << "\",\n"
      << "  \"registration_entrypoint_symbol\": \""
      << EscapeJsonString(inputs.registration_entrypoint_symbol) << "\",\n"
      << "  \"constructor_root_symbol\": \""
      << EscapeJsonString(inputs.constructor_root_symbol) << "\",\n"
      << "  \"constructor_root_ownership_model\": \""
      << EscapeJsonString(inputs.constructor_root_ownership_model) << "\",\n"
      << "  \"manifest_authority_model\": \""
      << EscapeJsonString(inputs.manifest_authority_model) << "\",\n"
      << "  \"constructor_init_stub_symbol\": \""
      << EscapeJsonString(constructor_init_stub_symbol) << "\",\n"
      << "  \"constructor_init_stub_ownership_model\": \""
      << EscapeJsonString(inputs.constructor_init_stub_ownership_model)
      << "\",\n"
      << "  \"constructor_priority_policy\": \""
      << EscapeJsonString(inputs.constructor_priority_policy) << "\",\n"
      << "  \"translation_unit_identity_model\": \""
      << EscapeJsonString(inputs.translation_unit_identity_model) << "\",\n"
      << "  \"runtime_library_resolution_model\": \""
      << EscapeJsonString(inputs.runtime_library_resolution_model) << "\",\n"
      << "  \"cleanup_unwind_runtime_link_model\": \""
      << "linker-response-plus-runtime-support-archive-sidecars-provide-runnable-cleanup-executable-link-inputs\",\n"
      << "  \"driver_linker_flag_consumption_model\": \""
      << EscapeJsonString(inputs.driver_linker_flag_consumption_model)
      << "\",\n"
      << "  \"compile_wrapper_command_surface\": \""
      << EscapeJsonString(inputs.compile_wrapper_command_surface)
      << "\",\n"
      << "  \"compile_proof_command_surface\": \""
      << EscapeJsonString(inputs.compile_proof_command_surface) << "\",\n"
      << "  \"execution_smoke_command_surface\": \""
      << EscapeJsonString(inputs.execution_smoke_command_surface)
      << "\",\n"
      << "  \"dispatch_accessor_runtime_abi_surface\": {\n"
      << "    \"contract_id\": \""
      << EscapeJsonString(inputs.dispatch_accessor_runtime_abi_contract_id)
      << "\",\n"
      << "    \"abi_boundary_model\": \""
      << EscapeJsonString(inputs.dispatch_accessor_runtime_abi_boundary_model)
      << "\",\n"
      << "    \"public_header_path\": \""
      << EscapeJsonString(inputs.dispatch_accessor_public_header_path)
      << "\",\n"
      << "    \"private_header_path\": \""
      << EscapeJsonString(inputs.dispatch_accessor_private_header_path)
      << "\",\n"
      << "    \"runtime_dispatch_symbol\": \""
      << EscapeJsonString(inputs.dispatch_accessor_runtime_dispatch_symbol)
      << "\",\n"
      << "    \"dispatch_state_snapshot_symbol\": \""
      << EscapeJsonString(
             inputs.dispatch_accessor_dispatch_state_snapshot_symbol)
      << "\",\n"
      << "    \"method_cache_state_snapshot_symbol\": \""
      << EscapeJsonString(
             inputs.dispatch_accessor_method_cache_state_snapshot_symbol)
      << "\",\n"
      << "    \"method_cache_entry_snapshot_symbol\": \""
      << EscapeJsonString(
             inputs.dispatch_accessor_method_cache_entry_snapshot_symbol)
      << "\",\n"
      << "    \"property_registry_state_snapshot_symbol\": \""
      << EscapeJsonString(
             inputs.dispatch_accessor_property_registry_state_snapshot_symbol)
      << "\",\n"
      << "    \"property_entry_snapshot_symbol\": \""
      << EscapeJsonString(inputs.dispatch_accessor_property_entry_snapshot_symbol)
      << "\",\n"
      << "    \"arc_debug_state_snapshot_symbol\": \""
      << EscapeJsonString(
             inputs.dispatch_accessor_arc_debug_state_snapshot_symbol)
      << "\",\n"
      << "    \"current_property_read_symbol\": \""
      << EscapeJsonString(inputs.dispatch_accessor_current_property_read_symbol)
      << "\",\n"
      << "    \"current_property_write_symbol\": \""
      << EscapeJsonString(inputs.dispatch_accessor_current_property_write_symbol)
      << "\",\n"
      << "    \"current_property_exchange_symbol\": \""
      << EscapeJsonString(
             inputs.dispatch_accessor_current_property_exchange_symbol)
      << "\",\n"
      << "    \"bind_current_property_context_symbol\": \""
      << EscapeJsonString(
             inputs.dispatch_accessor_bind_current_property_context_symbol)
      << "\",\n"
      << "    \"clear_current_property_context_symbol\": \""
      << EscapeJsonString(
             inputs.dispatch_accessor_clear_current_property_context_symbol)
      << "\",\n"
      << "    \"weak_current_property_load_symbol\": \""
      << EscapeJsonString(
             inputs.dispatch_accessor_weak_current_property_load_symbol)
      << "\",\n"
      << "    \"weak_current_property_store_symbol\": \""
      << EscapeJsonString(
             inputs.dispatch_accessor_weak_current_property_store_symbol)
      << "\",\n"
      << "    \"retain_symbol\": \""
      << EscapeJsonString(inputs.dispatch_accessor_retain_symbol) << "\",\n"
      << "    \"release_symbol\": \""
      << EscapeJsonString(inputs.dispatch_accessor_release_symbol) << "\",\n"
      << "    \"autorelease_symbol\": \""
      << EscapeJsonString(inputs.dispatch_accessor_autorelease_symbol)
      << "\",\n"
      << "    \"private_testing_surface_only\": "
      << (inputs.dispatch_accessor_private_testing_surface_only ? "true"
                                                               : "false")
      << ",\n"
      << "    \"deterministic\": "
      << (inputs.dispatch_accessor_deterministic ? "true" : "false") << "\n"
      << "  },\n"
      << "  \"storage_accessor_runtime_abi_surface\": {\n"
      << "    \"contract_id\": \""
      << EscapeJsonString(inputs.storage_accessor_runtime_abi_contract_id)
      << "\",\n"
      << "    \"abi_boundary_model\": \""
      << EscapeJsonString(inputs.storage_accessor_runtime_abi_boundary_model)
      << "\",\n"
      << "    \"public_header_path\": \""
      << EscapeJsonString(inputs.storage_accessor_public_header_path)
      << "\",\n"
      << "    \"private_header_path\": \""
      << EscapeJsonString(inputs.storage_accessor_private_header_path)
      << "\",\n"
      << "    \"property_registry_state_snapshot_symbol\": \""
      << EscapeJsonString(
             inputs.storage_accessor_property_registry_state_snapshot_symbol)
      << "\",\n"
      << "    \"property_entry_snapshot_symbol\": \""
      << EscapeJsonString(inputs.storage_accessor_property_entry_snapshot_symbol)
      << "\",\n"
      << "    \"current_property_read_symbol\": \""
      << EscapeJsonString(inputs.storage_accessor_current_property_read_symbol)
      << "\",\n"
      << "    \"current_property_write_symbol\": \""
      << EscapeJsonString(inputs.storage_accessor_current_property_write_symbol)
      << "\",\n"
      << "    \"current_property_exchange_symbol\": \""
      << EscapeJsonString(
             inputs.storage_accessor_current_property_exchange_symbol)
      << "\",\n"
      << "    \"bind_current_property_context_symbol\": \""
      << EscapeJsonString(
             inputs.storage_accessor_bind_current_property_context_symbol)
      << "\",\n"
      << "    \"clear_current_property_context_symbol\": \""
      << EscapeJsonString(
             inputs.storage_accessor_clear_current_property_context_symbol)
      << "\",\n"
      << "    \"weak_current_property_load_symbol\": \""
      << EscapeJsonString(
             inputs.storage_accessor_weak_current_property_load_symbol)
      << "\",\n"
      << "    \"weak_current_property_store_symbol\": \""
      << EscapeJsonString(
             inputs.storage_accessor_weak_current_property_store_symbol)
      << "\",\n"
      << "    \"private_testing_surface_only\": "
      << (inputs.storage_accessor_private_testing_surface_only ? "true"
                                                               : "false")
      << ",\n"
      << "    \"deterministic\": "
      << (inputs.storage_accessor_deterministic ? "true" : "false") << "\n"
      << "  },\n"
      << "  \"registration_descriptor_source_contract_id\": \""
      << EscapeJsonString(inputs.registration_descriptor_source_contract_id)
      << "\",\n"
      << "  \"registration_descriptor_source_surface_path\": \""
      << EscapeJsonString(inputs.registration_descriptor_source_surface_path)
      << "\",\n"
      << "  \"registration_descriptor_pragma_name\": \""
      << EscapeJsonString(inputs.registration_descriptor_pragma_name)
      << "\",\n"
      << "  \"image_root_pragma_name\": \""
      << EscapeJsonString(inputs.image_root_pragma_name) << "\",\n"
      << "  \"module_identity_source\": \""
      << EscapeJsonString(inputs.module_identity_source) << "\",\n"
      << "  \"registration_descriptor_identifier\": \""
      << EscapeJsonString(inputs.registration_descriptor_identifier)
      << "\",\n"
      << "  \"registration_descriptor_identity_source\": \""
      << EscapeJsonString(inputs.registration_descriptor_identity_source)
      << "\",\n"
      << "  \"image_root_identifier\": \""
      << EscapeJsonString(inputs.image_root_identifier) << "\",\n"
      << "  \"image_root_identity_source\": \""
      << EscapeJsonString(inputs.image_root_identity_source) << "\",\n"
      << "  \"bootstrap_visible_metadata_ownership_model\": \""
      << EscapeJsonString(inputs.bootstrap_visible_metadata_ownership_model)
      << "\",\n"
      << "  \"class_descriptor_count\": " << inputs.class_descriptor_count
      << ",\n"
      << "  \"protocol_descriptor_count\": "
      << inputs.protocol_descriptor_count << ",\n"
      << "  \"category_descriptor_count\": "
      << inputs.category_descriptor_count << ",\n"
      << "  \"property_descriptor_count\": "
      << inputs.property_descriptor_count << ",\n"
      << "  \"ivar_descriptor_count\": " << inputs.ivar_descriptor_count
      << ",\n"
      << "  \"total_descriptor_count\": " << inputs.total_descriptor_count
      << ",\n"
      << "  \"bootstrap_semantics_contract_id\": \""
      << EscapeJsonString(inputs.bootstrap_semantics_contract_id) << "\",\n"
      << "  \"duplicate_registration_policy\": \""
      << EscapeJsonString(inputs.duplicate_registration_policy) << "\",\n"
      << "  \"realization_order_policy\": \""
      << EscapeJsonString(inputs.realization_order_policy) << "\",\n"
      << "  \"failure_mode\": \"" << EscapeJsonString(inputs.failure_mode)
      << "\",\n"
      << "  \"registration_result_model\": \""
      << EscapeJsonString(inputs.registration_result_model) << "\",\n"
      << "  \"registration_order_ordinal_model\": \""
      << EscapeJsonString(inputs.registration_order_ordinal_model) << "\",\n"
      << "  \"runtime_state_snapshot_symbol\": \""
      << EscapeJsonString(inputs.runtime_state_snapshot_symbol) << "\",\n"
      << "  \"bootstrap_runtime_api_contract_id\": \""
      << EscapeJsonString(inputs.bootstrap_runtime_api_contract_id)
      << "\",\n"
      << "  \"bootstrap_runtime_api_public_header_path\": \""
      << EscapeJsonString(inputs.bootstrap_runtime_api_public_header_path)
      << "\",\n"
      << "  \"bootstrap_runtime_api_archive_relative_path\": \""
      << EscapeJsonString(inputs.bootstrap_runtime_api_archive_relative_path)
      << "\",\n"
      << "  \"bootstrap_runtime_api_registration_status_enum_type\": \""
      << EscapeJsonString(
             inputs.bootstrap_runtime_api_registration_status_enum_type)
      << "\",\n"
      << "  \"bootstrap_runtime_api_image_descriptor_type\": \""
      << EscapeJsonString(inputs.bootstrap_runtime_api_image_descriptor_type)
      << "\",\n"
      << "  \"bootstrap_runtime_api_selector_handle_type\": \""
      << EscapeJsonString(inputs.bootstrap_runtime_api_selector_handle_type)
      << "\",\n"
      << "  \"bootstrap_runtime_api_registration_snapshot_type\": \""
      << EscapeJsonString(
             inputs.bootstrap_runtime_api_registration_snapshot_type)
      << "\",\n"
      << "  \"bootstrap_runtime_api_registration_entrypoint_symbol\": \""
      << EscapeJsonString(
             inputs.bootstrap_runtime_api_registration_entrypoint_symbol)
      << "\",\n"
      << "  \"bootstrap_runtime_api_selector_lookup_symbol\": \""
      << EscapeJsonString(inputs.bootstrap_runtime_api_selector_lookup_symbol)
      << "\",\n"
      << "  \"bootstrap_runtime_api_dispatch_entrypoint_symbol\": \""
      << EscapeJsonString(
             inputs.bootstrap_runtime_api_dispatch_entrypoint_symbol)
      << "\",\n"
      << "  \"bootstrap_runtime_api_state_snapshot_symbol\": \""
      << EscapeJsonString(inputs.bootstrap_runtime_api_state_snapshot_symbol)
      << "\",\n"
      << "  \"bootstrap_runtime_api_reset_for_testing_symbol\": \""
      << EscapeJsonString(
             inputs.bootstrap_runtime_api_reset_for_testing_symbol)
      << "\",\n"
      << "  \"bootstrap_registrar_contract_id\": \""
      << EscapeJsonString(inputs.bootstrap_registrar_contract_id)
      << "\",\n"
      << "  \"bootstrap_registrar_internal_header_path\": \""
      << EscapeJsonString(inputs.bootstrap_registrar_internal_header_path)
      << "\",\n"
      << "  \"bootstrap_registrar_stage_registration_table_symbol\": \""
      << EscapeJsonString(
             inputs.bootstrap_registrar_stage_registration_table_symbol)
      << "\",\n"
      << "  \"bootstrap_registrar_image_walk_snapshot_symbol\": \""
      << EscapeJsonString(
             inputs.bootstrap_registrar_image_walk_snapshot_symbol)
      << "\",\n"
      << "  \"bootstrap_registrar_image_walk_model\": \""
      << EscapeJsonString(inputs.bootstrap_registrar_image_walk_model)
      << "\",\n"
      << "  \"bootstrap_registrar_discovery_root_validation_model\": \""
      << EscapeJsonString(
             inputs.bootstrap_registrar_discovery_root_validation_model)
      << "\",\n"
      << "  \"bootstrap_registrar_selector_pool_interning_model\": \""
      << EscapeJsonString(
             inputs.bootstrap_registrar_selector_pool_interning_model)
      << "\",\n"
      << "  \"bootstrap_registrar_realization_staging_model\": \""
      << EscapeJsonString(
             inputs.bootstrap_registrar_realization_staging_model)
      << "\",\n"
      << "  \"bootstrap_table_consumption_contract_id\": \""
      << EscapeJsonString(kObjc3RuntimeBootstrapTableConsumptionContractId)
      << "\",\n"
      << "  \"bootstrap_table_consumption_model\": \""
      << EscapeJsonString(kObjc3RuntimeBootstrapTableConsumptionModel)
      << "\",\n"
      << "  \"bootstrap_table_deduplication_model\": \""
      << EscapeJsonString(kObjc3RuntimeBootstrapTableDeduplicationModel)
      << "\",\n"
      << "  \"bootstrap_table_image_state_publication_model\": \""
      << EscapeJsonString(
             kObjc3RuntimeBootstrapTableImageStatePublicationModel)
      << "\",\n"
      << "  \"bootstrap_table_stage_registration_table_symbol\": \""
      << EscapeJsonString(
             inputs.bootstrap_registrar_stage_registration_table_symbol)
      << "\",\n"
      << "  \"bootstrap_table_image_walk_snapshot_symbol\": \""
      << EscapeJsonString(
             inputs.bootstrap_registrar_image_walk_snapshot_symbol)
      << "\",\n"
      << "  \"bootstrap_live_registration_contract_id\": \""
      << EscapeJsonString(kObjc3RuntimeLiveRegistrationDiscoveryReplayContractId)
      << "\",\n"
      << "  \"bootstrap_live_registration_model\": \""
      << EscapeJsonString(kObjc3RuntimeLiveRegistrationModel) << "\",\n"
