#include "io/objc3_process.h"

#include "io/objc3_manifest_artifacts.h"
#include "lower/objc3_lowering_contract.h"

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
#include <sstream>
#include <string>
#include <unordered_set>
#include <vector>

namespace {
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
    "objc3c-runtime-bootstrap-table-consumption-freeze/m263-d001-v1";
inline constexpr const char *kObjc3RuntimeBootstrapTableConsumptionModel =
    "next-public-register-call-consumes-staged-registration-table-once";
inline constexpr const char *kObjc3RuntimeBootstrapTableDeduplicationModel =
    "translation-unit-identity-key-rejection-before-registration-state-advance";
inline constexpr const char
    *kObjc3RuntimeBootstrapTableImageStatePublicationModel =
        "image-walk-snapshot-publishes-module-identity-root-counts-and-staged-table-usage";
inline constexpr const char *kObjc3RuntimeLiveRegistrationDiscoveryReplayContractId =
    "objc3c-runtime-live-registration-discovery-replay/m263-d002-v1";
inline constexpr const char *kObjc3RuntimeLiveRegistrationModel =
    "emitted-metadata-images-register-through-native-runtime-and-retained-bootstrap-catalog";
inline constexpr const char *kObjc3RuntimeLiveDiscoveryTrackingModel =
    "image-walk-snapshot-tracks-last-discovered-root-and-descriptor-families";
inline constexpr const char *kObjc3RuntimeLiveReplayTrackingModel =
    "reset-replay-state-snapshot-tracks-retained-images-reset-clears-and-last-replayed-identity";
inline constexpr const char *kObjc3RuntimeLiveRestartHardeningContractId =
    "objc3c-runtime-live-restart-hardening/m263-d003-v1";
inline constexpr const char *kObjc3RuntimeLiveIdempotenceModel =
    "second-live-replay-without-reset-fails-closed-and-preserves-live-runtime-state";
inline constexpr const char *kObjc3RuntimeLiveTeardownModel =
    "reset-clears-live-state-zeroes-image-local-init-cells-and-retains-bootstrap-catalog";
inline constexpr const char *kObjc3RuntimeLiveRestartEvidenceModel =
    "repeated-reset-replay-cycles-publish-monotonic-reset-and-replay-generations";
inline constexpr const char *kObjc3ToolchainConformanceClaimOperationsContractId =
    "objc3c-toolchain-conformance-claim-operations/m264-d002-v1";
inline constexpr const char *kObjc3ToolchainConformanceClaimValidationSchemaId =
    "objc3c-driver-conformance-validation-v1";
inline constexpr const char *kObjc3ToolchainConformanceClaimValidationModel =
    "driver-validates-versioned-conformance-report-and-publication-sidecars-before-toolchain-consumption";
inline constexpr const char *kObjc3ToolchainConformanceClaimConsumptionModel =
    "validation-consumes-json-sidecars-only-and-keeps-unsupported-profiles-fail-closed";
inline constexpr const char *kObjc3AdvancedFeatureOpsContractId =
    "objc3c-advanced-feature-ci-runbook-dashboard-contract/m275-d001-v1";
inline constexpr const char *kObjc3AdvancedFeatureReportingContractId =
    "objc3c-part12-feature-aware-conformance-report-emission/m275-c002-v1";
inline constexpr const char *kObjc3AdvancedFeatureReleaseEvidenceContractId =
    "objc3c-part12-corpus-sharding-release-evidence-packaging/m275-c003-v1";
inline constexpr const char *kObjc3AdvancedFeatureEvidenceGateScriptPath =
    "scripts/check_release_evidence.py";
inline constexpr const char *kObjc3AdvancedFeatureEvidenceRunbookPath =
    "spec/conformance/release_evidence_gate_maintenance.md";
inline constexpr const char *kObjc3AdvancedFeatureDashboardSchemaPath =
    "schemas/objc3-conformance-dashboard-status-v1.schema.json";
inline constexpr const char *kObjc3ReleaseEvidenceOperationContractId =
    "objc3c-part12-release-evidence-toolchain-operations/m275-d002-v1";
inline constexpr const char *kObjc3ReleaseEvidenceOperationSchemaId =
    "objc3c-part12-release-evidence-operation-v1";
inline constexpr const char *kObjc3DashboardStatusPublicationContractId =
    "objc3c-part12-dashboard-status-publication/m275-d002-v1";
inline constexpr const char *kObjc3DashboardStatusPublicationSchemaId =
    "objc3c-part12-dashboard-status-publication-v1";
inline constexpr const char *kObjc3AdvancedFeatureGateContractId =
    "objc3c-part12-integrated-advanced-feature-gate/m275-e001-v1";
inline constexpr const char *kObjc3AdvancedFeatureGateSchemaId =
    "objc3c-part12-integrated-advanced-feature-gate-v1";
inline constexpr const char *kObjc3ReleaseCandidateMatrixContractId =
    "objc3c-part12-release-candidate-execution-matrix/m275-e002-v1";
inline constexpr const char *kObjc3ReleaseCandidateMatrixSchemaId =
    "objc3c-part12-release-candidate-execution-matrix-v1";
inline constexpr const char *kObjc3AdvancedFeatureReleaseLabel = "v0.11";
inline constexpr const char *kObjc3DeterministicReplayTimestamp =
    "1970-01-01T00:00:00Z";
inline constexpr const char *kObjc3DeterministicSourceRevision = "0000000";
// scheduler/executor runtime anchor: the driver/process layer still
// does not own scheduling itself, but emitted IR/object evidence now carries a
// frozen private task-runtime helper boundary that later runtime integration
// issues must consume without reconstructing helper or snapshot names ad hoc.

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

std::string EscapeJsonString(const std::string &text) {
  std::ostringstream out;
  for (unsigned char c : text) {
    switch (c) {
      case '\\':
        out << "\\\\";
        break;
      case '"':
        out << "\\\"";
        break;
      case '\b':
        out << "\\b";
        break;
      case '\f':
        out << "\\f";
        break;
      case '\n':
        out << "\\n";
        break;
      case '\r':
        out << "\\r";
        break;
      case '\t':
        out << "\\t";
        break;
      default:
        if (c < 0x20) {
          out << "\\u";
          constexpr char kHex[] = "0123456789abcdef";
          out << "00" << kHex[(c >> 4u) & 0x0f] << kHex[c & 0x0f];
        } else {
          out << static_cast<char>(c);
        }
        break;
    }
  }
  return out.str();
}

std::string MakeIdentifierSafeSuffix(const std::string &text) {
  std::string suffix;
  suffix.reserve(text.size());
  for (unsigned char c : text) {
    if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
        (c >= '0' && c <= '9') || c == '_') {
      suffix.push_back(static_cast<char>(c));
    } else {
      suffix.push_back('_');
    }
  }
  if (suffix.empty()) {
    suffix = "translation_unit";
  }
  return suffix;
}

std::string BuildIndentedStringArrayJson(const std::vector<std::string> &values,
                                         const std::string &indent) {
  std::ostringstream out;
  out << "[\n";
  for (std::size_t index = 0; index < values.size(); ++index) {
    out << indent << "\"" << EscapeJsonString(values[index]) << "\"";
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

void SkipJsonWhitespace(const std::string &text, std::size_t &index) {
  while (index < text.size() &&
         std::isspace(static_cast<unsigned char>(text[index])) != 0) {
    ++index;
  }
}

bool TryFindJsonFieldValueStart(const std::string &text,
                                const std::string &field,
                                std::size_t &value_start) {
  const std::string key = "\"" + field + "\"";
  const std::size_t key_pos = text.find(key);
  if (key_pos == std::string::npos) {
    return false;
  }
  const std::size_t colon_pos = text.find(':', key_pos + key.size());
  if (colon_pos == std::string::npos) {
    return false;
  }
  value_start = colon_pos + 1u;
  SkipJsonWhitespace(text, value_start);
  return value_start < text.size();
}

bool TryExtractJsonStringField(const std::string &text,
                               const std::string &field,
                               std::string &value) {
  std::size_t index = 0;
  if (!TryFindJsonFieldValueStart(text, field, index) || text[index] != '"') {
    return false;
  }
  ++index;
  std::string decoded;
  while (index < text.size()) {
    const char ch = text[index++];
    if (ch == '\\') {
      if (index >= text.size()) {
        return false;
      }
      decoded.push_back(text[index++]);
      continue;
    }
    if (ch == '"') {
      value = decoded;
      return true;
    }
    decoded.push_back(ch);
  }
  return false;
}

bool TryExtractJsonBoolField(const std::string &text,
                             const std::string &field,
                             bool &value) {
  std::size_t index = 0;
  if (!TryFindJsonFieldValueStart(text, field, index)) {
    return false;
  }
  if (text.compare(index, 4u, "true") == 0) {
    value = true;
    return true;
  }
  if (text.compare(index, 5u, "false") == 0) {
    value = false;
    return true;
  }
  return false;
}

bool TryExtractJsonStringArrayField(const std::string &text,
                                    const std::string &field,
                                    std::vector<std::string> &values) {
  std::size_t index = 0;
  if (!TryFindJsonFieldValueStart(text, field, index) || text[index] != '[') {
    return false;
  }
  ++index;
  SkipJsonWhitespace(text, index);
  std::vector<std::string> parsed;
  while (index < text.size() && text[index] != ']') {
    if (text[index] != '"') {
      return false;
    }
    ++index;
    std::string decoded;
    while (index < text.size()) {
      const char ch = text[index++];
      if (ch == '\\') {
        if (index >= text.size()) {
          return false;
        }
        decoded.push_back(text[index++]);
        continue;
      }
      if (ch == '"') {
        break;
      }
      decoded.push_back(ch);
    }
    parsed.push_back(decoded);
    SkipJsonWhitespace(text, index);
    if (index < text.size() && text[index] == ',') {
      ++index;
      SkipJsonWhitespace(text, index);
    }
  }
  if (index >= text.size() || text[index] != ']') {
    return false;
  }
  values = parsed;
  return true;
}

}  // namespace

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
  // completeness matrix until later M253 work lands.
  // layout/visibility policy anchor: llvm-direct object emission must preserve
  // emitted global order, section family order, the local-linkage/no-COMDAT policy,
  // and llvm.used retention order exactly. Backend execution may not inject
  // exported visibility or object-format-specific rewrites before M253-B003 lands.
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
  // discovery/linker-anchor boundary emitted by the M253 path.
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
      MakeIdentifierSafeSuffix(
          linker_retention_artifacts.translation_unit_identity_key);
  const std::string bootstrap_registration_table_symbol =
      inputs.bootstrap_registration_table_symbol_prefix +
      MakeIdentifierSafeSuffix(
          linker_retention_artifacts.translation_unit_identity_key);
  const std::string bootstrap_image_local_init_state_symbol =
      inputs.bootstrap_image_local_init_state_symbol_prefix +
      MakeIdentifierSafeSuffix(
          linker_retention_artifacts.translation_unit_identity_key);
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
  // linker flags from ad hoc fallback heuristics.
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
      << "  \"bootstrap_live_discovery_tracking_model\": \""
      << EscapeJsonString(kObjc3RuntimeLiveDiscoveryTrackingModel)
      << "\",\n"
      << "  \"bootstrap_live_replay_tracking_model\": \""
      << EscapeJsonString(kObjc3RuntimeLiveReplayTrackingModel) << "\",\n"
      << "  \"bootstrap_live_replay_registered_images_symbol\": \""
      << EscapeJsonString(
             inputs.bootstrap_reset_replay_registered_images_symbol)
      << "\",\n"
      << "  \"bootstrap_live_reset_replay_state_snapshot_symbol\": \""
      << EscapeJsonString(
             inputs.bootstrap_reset_reset_replay_state_snapshot_symbol)
      << "\",\n"
      << "  \"bootstrap_live_restart_hardening_contract_id\": \""
      << EscapeJsonString(kObjc3RuntimeLiveRestartHardeningContractId)
      << "\",\n"
      << "  \"bootstrap_live_idempotence_model\": \""
      << EscapeJsonString(kObjc3RuntimeLiveIdempotenceModel) << "\",\n"
      << "  \"bootstrap_live_teardown_model\": \""
      << EscapeJsonString(kObjc3RuntimeLiveTeardownModel) << "\",\n"
      << "  \"bootstrap_live_restart_evidence_model\": \""
      << EscapeJsonString(kObjc3RuntimeLiveRestartEvidenceModel)
      << "\",\n"
      << "  \"bootstrap_live_restart_reset_for_testing_symbol\": \""
      << EscapeJsonString(inputs.bootstrap_runtime_api_reset_for_testing_symbol)
      << "\",\n"
      << "  \"bootstrap_live_restart_replay_registered_images_symbol\": \""
      << EscapeJsonString(
             inputs.bootstrap_reset_replay_registered_images_symbol)
      << "\",\n"
      << "  \"bootstrap_live_restart_reset_replay_state_snapshot_symbol\": \""
      << EscapeJsonString(
             inputs.bootstrap_reset_reset_replay_state_snapshot_symbol)
      << "\",\n"
      << "  \"bootstrap_reset_contract_id\": \""
      << EscapeJsonString(inputs.bootstrap_reset_contract_id) << "\",\n"
      << "  \"bootstrap_reset_internal_header_path\": \""
      << EscapeJsonString(inputs.bootstrap_reset_internal_header_path)
      << "\",\n"
      << "  \"bootstrap_reset_replay_registered_images_symbol\": \""
      << EscapeJsonString(
             inputs.bootstrap_reset_replay_registered_images_symbol)
      << "\",\n"
      << "  \"bootstrap_reset_reset_replay_state_snapshot_symbol\": \""
      << EscapeJsonString(
             inputs.bootstrap_reset_reset_replay_state_snapshot_symbol)
      << "\",\n"
      << "  \"bootstrap_reset_lifecycle_model\": \""
      << EscapeJsonString(inputs.bootstrap_reset_lifecycle_model) << "\",\n"
      << "  \"bootstrap_reset_replay_order_model\": \""
      << EscapeJsonString(inputs.bootstrap_reset_replay_order_model)
      << "\",\n"
      << "  \"bootstrap_reset_image_local_init_state_reset_model\": \""
      << EscapeJsonString(
             inputs.bootstrap_reset_image_local_init_state_reset_model)
      << "\",\n"
      << "  \"bootstrap_reset_bootstrap_catalog_retention_model\": \""
      << EscapeJsonString(
             inputs.bootstrap_reset_bootstrap_catalog_retention_model)
      << "\",\n"
      << "  \"bootstrap_lowering_contract_id\": \""
      << EscapeJsonString(inputs.bootstrap_lowering_contract_id) << "\",\n"
      << "  \"bootstrap_lowering_boundary_model\": \""
      << EscapeJsonString(inputs.bootstrap_lowering_boundary_model)
      << "\",\n"
      << "  \"bootstrap_global_ctor_list_model\": \""
      << EscapeJsonString(inputs.bootstrap_global_ctor_list_model)
      << "\",\n"
      << "  \"bootstrap_registration_table_layout_model\": \""
      << EscapeJsonString(inputs.bootstrap_registration_table_layout_model)
      << "\",\n"
      << "  \"bootstrap_image_local_initialization_model\": \""
      << EscapeJsonString(inputs.bootstrap_image_local_initialization_model)
      << "\",\n"
      << "  \"bootstrap_constructor_root_emission_state\": \""
      << EscapeJsonString(inputs.bootstrap_constructor_root_emission_state)
      << "\",\n"
      << "  \"bootstrap_init_stub_emission_state\": \""
      << EscapeJsonString(inputs.bootstrap_init_stub_emission_state)
      << "\",\n"
      << "  \"bootstrap_registration_table_emission_state\": \""
      << EscapeJsonString(
             inputs.bootstrap_registration_table_emission_state)
      << "\",\n"
      << "  \"bootstrap_registration_table_symbol_prefix\": \""
      << EscapeJsonString(inputs.bootstrap_registration_table_symbol_prefix)
      << "\",\n"
      << "  \"bootstrap_image_local_init_state_symbol_prefix\": \""
      << EscapeJsonString(
             inputs.bootstrap_image_local_init_state_symbol_prefix)
      << "\",\n"
      << "  \"bootstrap_registration_table_symbol\": \""
      << EscapeJsonString(bootstrap_registration_table_symbol) << "\",\n"
      << "  \"bootstrap_image_local_init_state_symbol\": \""
      << EscapeJsonString(bootstrap_image_local_init_state_symbol)
      << "\",\n"
      << "  \"bootstrap_registration_table_abi_version\": "
      << inputs.bootstrap_registration_table_abi_version << ",\n"
      << "  \"bootstrap_registration_table_pointer_field_count\": "
      << inputs.bootstrap_registration_table_pointer_field_count << ",\n"
      << "  \"success_status_code\": " << inputs.success_status_code << ",\n"
      << "  \"invalid_descriptor_status_code\": "
      << inputs.invalid_descriptor_status_code << ",\n"
      << "  \"duplicate_registration_status_code\": "
      << inputs.duplicate_registration_status_code << ",\n"
      << "  \"out_of_order_status_code\": "
      << inputs.out_of_order_status_code << ",\n"
      << "  \"translation_unit_registration_order_ordinal\": "
      << inputs.translation_unit_registration_order_ordinal << ",\n"
      << "  \"translation_unit_identity_key\": \""
      << EscapeJsonString(
             linker_retention_artifacts.translation_unit_identity_key)
      << "\",\n"
      << "  \"object_format\": \""
      << EscapeJsonString(linker_retention_artifacts.object_format)
      << "\",\n"
      << "  \"linker_anchor_symbol\": \""
      << EscapeJsonString(linker_retention_artifacts.linker_anchor_symbol)
      << "\",\n"
      << "  \"discovery_root_symbol\": \""
      << EscapeJsonString(linker_retention_artifacts.discovery_root_symbol)
      << "\",\n"
      << "  \"driver_linker_flags\": [\n"
      << "    \""
      << EscapeJsonString(linker_retention_artifacts.driver_linker_flag)
      << "\"\n"
      << "  ],\n"
      << "  \"launch_integration_ready\": true,\n"
      << "  \"ready_for_lowering_init_stub_emission\": true,\n"
      << "  \"ready_for_bootstrap_lowering_materialization\": true,\n"
      << "  \"ready_for_runtime_bootstrap_enforcement\": true,\n"
      << "  \"ready_for_runtime_bootstrap_table_consumption\": true,\n"
      << "  \"ready_for_live_registration_discovery_replay\": true,\n"
      << "  \"ready_for_live_restart_hardening\": true\n"
      << "}\n";
  manifest_json = out.str();
  return true;
}

bool TryBuildObjc3RuntimeRegistrationDescriptorArtifact(
    const Objc3RuntimeRegistrationDescriptorArtifactInputs &inputs,
    const Objc3RuntimeMetadataLinkerRetentionArtifacts &linker_retention_artifacts,
    std::string &descriptor_json,
    std::string &error) {
  descriptor_json.clear();
  error.clear();
  if (inputs.contract_id.empty() ||
      inputs.registration_manifest_contract_id.empty() ||
      inputs.source_surface_contract_id.empty() ||
      inputs.payload_model.empty() ||
      inputs.artifact_relative_path.empty() || inputs.authority_model.empty() ||
      inputs.translation_unit_identity_model.empty() ||
      inputs.payload_ownership_model.empty() ||
      inputs.runtime_support_library_archive_relative_path.empty() ||
      inputs.registration_entrypoint_symbol.empty() ||
      inputs.registration_descriptor_pragma_name.empty() ||
      inputs.image_root_pragma_name.empty() || inputs.module_identity_source.empty() ||
      inputs.registration_descriptor_identifier.empty() ||
      inputs.registration_descriptor_identity_source.empty() ||
      inputs.image_root_identifier.empty() ||
      inputs.image_root_identity_source.empty() ||
      inputs.bootstrap_visible_metadata_ownership_model.empty() ||
      inputs.constructor_root_symbol.empty() ||
      inputs.constructor_init_stub_symbol_prefix.empty() ||
      inputs.bootstrap_registration_table_symbol_prefix.empty() ||
      inputs.bootstrap_image_local_init_state_symbol_prefix.empty() ||
      inputs.total_descriptor_count !=
          inputs.class_descriptor_count + inputs.protocol_descriptor_count +
              inputs.category_descriptor_count +
              inputs.property_descriptor_count + inputs.ivar_descriptor_count ||
      inputs.translation_unit_registration_order_ordinal == 0 ||
      inputs.object_artifact_relative_path.empty() ||
      inputs.backend_artifact_relative_path.empty() ||
      linker_retention_artifacts.translation_unit_identity_key.empty() ||
      linker_retention_artifacts.object_format.empty() ||
      linker_retention_artifacts.linker_anchor_symbol.empty() ||
      linker_retention_artifacts.discovery_root_symbol.empty()) {
    error =
        "runtime registration descriptor artifact inputs incomplete for " +
        inputs.artifact_relative_path;
    return false;
  }

  const std::string safe_identity_suffix =
      MakeIdentifierSafeSuffix(linker_retention_artifacts.translation_unit_identity_key);
  const std::string constructor_init_stub_symbol =
      inputs.constructor_init_stub_symbol_prefix + safe_identity_suffix;
  const std::string bootstrap_registration_table_symbol =
      inputs.bootstrap_registration_table_symbol_prefix + safe_identity_suffix;
  const std::string bootstrap_image_local_init_state_symbol =
      inputs.bootstrap_image_local_init_state_symbol_prefix + safe_identity_suffix;

  std::ostringstream out;
  out << "{\n"
      << "  \"contract_id\": \"" << EscapeJsonString(inputs.contract_id)
      << "\",\n"
      << "  \"registration_manifest_contract_id\": \""
      << EscapeJsonString(inputs.registration_manifest_contract_id)
      << "\",\n"
      << "  \"source_surface_contract_id\": \""
      << EscapeJsonString(inputs.source_surface_contract_id) << "\",\n"
      << "  \"payload_model\": \""
      << EscapeJsonString(inputs.payload_model) << "\",\n"
      << "  \"artifact\": \""
      << EscapeJsonString(inputs.artifact_relative_path) << "\",\n"
      << "  \"authority_model\": \""
      << EscapeJsonString(inputs.authority_model) << "\",\n"
      << "  \"translation_unit_identity_model\": \""
      << EscapeJsonString(inputs.translation_unit_identity_model) << "\",\n"
      << "  \"translation_unit_identity_key\": \""
      << EscapeJsonString(linker_retention_artifacts.translation_unit_identity_key)
      << "\",\n"
      << "  \"payload_ownership_model\": \""
      << EscapeJsonString(inputs.payload_ownership_model) << "\",\n"
      << "  \"runtime_support_library_archive_relative_path\": \""
      << EscapeJsonString(inputs.runtime_support_library_archive_relative_path)
      << "\",\n"
      << "  \"registration_entrypoint_symbol\": \""
      << EscapeJsonString(inputs.registration_entrypoint_symbol) << "\",\n"
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
      << "  \"constructor_root_symbol\": \""
      << EscapeJsonString(inputs.constructor_root_symbol) << "\",\n"
      << "  \"constructor_init_stub_symbol\": \""
      << EscapeJsonString(constructor_init_stub_symbol) << "\",\n"
      << "  \"bootstrap_registration_table_symbol\": \""
      << EscapeJsonString(bootstrap_registration_table_symbol) << "\",\n"
      << "  \"bootstrap_image_local_init_state_symbol\": \""
      << EscapeJsonString(bootstrap_image_local_init_state_symbol)
      << "\",\n"
      << "  \"class_descriptor_count\": "
      << inputs.class_descriptor_count << ",\n"
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
      << "  \"translation_unit_registration_order_ordinal\": "
      << inputs.translation_unit_registration_order_ordinal << ",\n"
      << "  \"object_artifact\": \""
      << EscapeJsonString(inputs.object_artifact_relative_path) << "\",\n"
      << "  \"backend_artifact\": \""
      << EscapeJsonString(inputs.backend_artifact_relative_path) << "\",\n"
      << "  \"object_format\": \""
      << EscapeJsonString(linker_retention_artifacts.object_format)
      << "\",\n"
      << "  \"linker_anchor_symbol\": \""
      << EscapeJsonString(linker_retention_artifacts.linker_anchor_symbol)
      << "\",\n"
      << "  \"discovery_root_symbol\": \""
      << EscapeJsonString(linker_retention_artifacts.discovery_root_symbol)
      << "\",\n"
      << "  \"ready_for_descriptor_artifact_emission\": true,\n"
      << "  \"ready_for_registration_descriptor_lowering\": true\n"
      << "}\n";
  descriptor_json = out.str();
  return true;
}

bool TryBuildObjc3Part10MacroHostProcessCacheArtifact(
    const Objc3Part10MacroHostProcessCacheArtifactInputs &inputs,
    const std::filesystem::path &source_input_path,
    std::string &artifact_json,
    std::string &error) {
  artifact_json.clear();
  error.clear();

  if (inputs.contract_id.empty() || inputs.source_contract_id.empty() ||
      inputs.surface_path.empty() || inputs.artifact_relative_path.empty() ||
      inputs.host_executable_relative_path.empty() ||
      inputs.cache_root_relative_path.empty() || inputs.host_model.empty() ||
      inputs.toolchain_model.empty() || inputs.cache_model.empty() ||
      inputs.fail_closed_model.empty() || inputs.replay_key.empty()) {
    error = "part10 macro host process/cache artifact inputs are incomplete";
    return false;
  }
  if (!std::filesystem::exists(source_input_path)) {
    error = "part10 macro host process/cache source input not found: " +
            source_input_path.generic_string();
    return false;
  }

  const std::filesystem::path host_executable =
      std::filesystem::path(inputs.host_executable_relative_path);
  if (!std::filesystem::exists(host_executable)) {
    error = "part10 macro host process/cache host executable not found: " +
            host_executable.generic_string();
    return false;
  }

  const std::string cache_key = ComputeFnv1a64Hex(inputs.replay_key);
  const std::filesystem::path cache_root =
      std::filesystem::path(inputs.cache_root_relative_path);
  const std::filesystem::path cache_entry = cache_root / cache_key;
  const std::filesystem::path cache_summary_path =
      cache_entry / "module.host-summary.json";
  const std::filesystem::path cache_runtime_import_path =
      BuildRuntimeAwareImportModuleArtifactPath(cache_entry, "module");
  const std::filesystem::path cache_manifest_path =
      BuildManifestArtifactPath(cache_entry, "module");

  bool cache_hit = std::filesystem::exists(cache_summary_path) &&
                   std::filesystem::exists(cache_runtime_import_path) &&
                   std::filesystem::exists(cache_manifest_path);
  bool launch_attempted = false;
  int host_process_exit_code = 0;

  if (!cache_hit) {
    launch_attempted = true;
    std::error_code create_error;
    std::filesystem::create_directories(cache_entry, create_error);
    if (create_error) {
      error = "failed to create Part 10 cache entry directory: " +
              cache_entry.generic_string() + ": " + create_error.message();
      return false;
    }
    const std::vector<std::string> args = {
        source_input_path.generic_string(),
        "--out-dir",
        cache_entry.generic_string(),
        "--emit-prefix",
        "module",
        "--summary-out",
        cache_summary_path.generic_string(),
        "--no-emit-ir",
        "--no-emit-object"};
    host_process_exit_code = RunProcess(host_executable.generic_string(), args);
    if (host_process_exit_code != 0) {
      error = "part10 macro host process launch failed with exit code " +
              std::to_string(host_process_exit_code);
      return false;
    }
    cache_hit = std::filesystem::exists(cache_summary_path) &&
                std::filesystem::exists(cache_runtime_import_path) &&
                std::filesystem::exists(cache_manifest_path);
    if (!cache_hit) {
      error =
          "part10 macro host process launch completed but cache artifacts are incomplete";
      return false;
    }
  }

  std::ostringstream out;
  out << "{\n"
      << "  \"contract_id\": \"" << EscapeJsonString(inputs.contract_id)
      << "\",\n"
      << "  \"source_contract_id\": \""
      << EscapeJsonString(inputs.source_contract_id) << "\",\n"
      << "  \"surface_path\": \"" << EscapeJsonString(inputs.surface_path)
      << "\",\n"
      << "  \"artifact\": \"" << EscapeJsonString(inputs.artifact_relative_path)
      << "\",\n"
      << "  \"host_executable_relative_path\": \""
      << EscapeJsonString(inputs.host_executable_relative_path) << "\",\n"
      << "  \"cache_root_relative_path\": \""
      << EscapeJsonString(inputs.cache_root_relative_path) << "\",\n"
      << "  \"cache_key\": \"" << EscapeJsonString(cache_key) << "\",\n"
      << "  \"cache_entry_relative_path\": \""
      << EscapeJsonString(cache_entry.generic_string()) << "\",\n"
      << "  \"cache_summary_relative_path\": \""
      << EscapeJsonString(cache_summary_path.generic_string()) << "\",\n"
      << "  \"cache_runtime_import_surface_relative_path\": \""
      << EscapeJsonString(cache_runtime_import_path.generic_string())
      << "\",\n"
      << "  \"cache_manifest_relative_path\": \""
      << EscapeJsonString(cache_manifest_path.generic_string()) << "\",\n"
      << "  \"host_model\": \"" << EscapeJsonString(inputs.host_model)
      << "\",\n"
      << "  \"toolchain_model\": \""
      << EscapeJsonString(inputs.toolchain_model) << "\",\n"
      << "  \"cache_model\": \"" << EscapeJsonString(inputs.cache_model)
      << "\",\n"
      << "  \"fail_closed_model\": \""
      << EscapeJsonString(inputs.fail_closed_model) << "\",\n"
      << "  \"source_input_path\": \""
      << EscapeJsonString(source_input_path.generic_string()) << "\",\n"
      << "  \"launch_attempted\": " << (launch_attempted ? "true" : "false")
      << ",\n"
      << "  \"cache_hit\": " << (cache_hit ? "true" : "false") << ",\n"
      << "  \"host_process_exit_code\": " << host_process_exit_code << ",\n"
      << "  \"deterministic\": " << (inputs.deterministic ? "true" : "false")
      << ",\n"
      << "  \"replay_key\": \"" << EscapeJsonString(inputs.replay_key)
      << "\"\n"
      << "}\n";
  artifact_json = out.str();
  return true;
}

bool TryBuildObjc3CrossModuleRuntimeLinkPlanArtifact(
    const Objc3CrossModuleRuntimeLinkPlanArtifactInputs &inputs,
    std::string &plan_json,
    std::string &linker_response_payload,
    std::string &error) {
  plan_json.clear();
  linker_response_payload.clear();
  error.clear();

  if (inputs.contract_id.empty() ||
      inputs.source_orchestration_contract_id.empty() ||
      inputs.import_surface_contract_id.empty() ||
      inputs.registration_manifest_contract_id.empty() ||
      inputs.payload_model.empty() ||
      inputs.artifact_relative_path.empty() ||
      inputs.linker_response_artifact_relative_path.empty() ||
      inputs.authority_model.empty() ||
      inputs.packaging_model.empty() ||
      inputs.registration_scope_model.empty() ||
      inputs.link_object_order_model.empty() ||
      inputs.local_module_name.empty() ||
      inputs.local_import_surface_artifact_relative_path.empty() ||
      inputs.local_registration_manifest_artifact_relative_path.empty() ||
      inputs.local_object_artifact_relative_path.empty() ||
      inputs.runtime_support_library_archive_relative_path.empty() ||
      inputs.object_format.empty() ||
      inputs.local_translation_unit_identity_model.empty() ||
      inputs.local_translation_unit_identity_key.empty() ||
      inputs.local_translation_unit_registration_order_ordinal == 0 ||
      inputs.expected_part6_contract_id.empty() ||
      inputs.expected_part6_source_contract_id.empty() ||
      inputs.expected_part7_actor_contract_id.empty() ||
      inputs.expected_part7_actor_source_contract_id.empty() ||
      inputs.expected_part11_ffi_contract_id.empty() ||
      inputs.expected_part11_ffi_source_contract_id.empty() ||
      inputs.expected_part11_ffi_preservation_contract_id.empty() ||
      inputs.expected_part11_header_module_bridge_contract_id.empty() ||
      inputs.expected_part11_header_module_bridge_source_contract_id.empty() ||
      inputs.expected_part11_header_module_bridge_preservation_contract_id
          .empty() ||
      inputs.expected_part11_bridge_header_artifact_relative_path.empty() ||
      inputs.expected_part11_bridge_module_artifact_relative_path.empty() ||
      inputs.expected_part11_bridge_artifact_relative_path.empty() ||
      inputs.expected_part10_host_cache_contract_id.empty() ||
      inputs.expected_part10_host_cache_source_contract_id.empty() ||
      inputs.expected_part10_host_cache_executable_relative_path.empty() ||
      inputs.expected_part10_host_cache_root_relative_path.empty() ||
      inputs.local_driver_linker_flags.empty() ||
      inputs.direct_import_surface_artifact_paths.empty() ||
      inputs.imported_inputs.empty()) {
    error = "cross-module runtime link-plan artifact inputs are incomplete";
    return false;
  }
  // cleanup-unwind integration anchor: runnable cleanup/unwind
  // proofs stay toolchain-visible through the linker-response sidecar plus the
  // emitted runtime-support archive path that native executable probes consume.
  // runtime-fast-path-integration anchor: Part 9 keeps imported
  // direct-surface artifact paths visible in the cross-module link plan so the
  // runtime/cache lane can prove exactly which imported modules participate in
  // dispatch-boundary preservation before D002 widens the live fast path.
  // live-dispatch-fast-path anchor: imported direct-surface artifact paths feed the runtime cache seeding model once live direct/final/sealed fast paths are materialized after registration.
  if (inputs.direct_import_surface_artifact_paths.size() !=
      inputs.imported_inputs.size()) {
    error =
        "cross-module runtime link-plan direct import surface count does not "
        "match imported input count";
    return false;
  }

  std::vector<Objc3CrossModuleRuntimeLinkPlanImportedInput> imported_inputs =
      inputs.imported_inputs;
  std::sort(imported_inputs.begin(), imported_inputs.end(),
            [](const auto &lhs, const auto &rhs) {
              if (lhs.translation_unit_registration_order_ordinal !=
                  rhs.translation_unit_registration_order_ordinal) {
                return lhs.translation_unit_registration_order_ordinal <
                       rhs.translation_unit_registration_order_ordinal;
              }
              if (lhs.translation_unit_identity_key !=
                  rhs.translation_unit_identity_key) {
                return lhs.translation_unit_identity_key <
                       rhs.translation_unit_identity_key;
              }
              return lhs.module_name < rhs.module_name;
            });

  std::unordered_set<std::string> seen_translation_unit_identity_keys;
  std::unordered_set<std::uint64_t> seen_registration_ordinals;
  std::unordered_set<std::string> seen_driver_linker_flags;
  std::unordered_set<std::string> seen_direct_import_surface_paths;
  std::unordered_set<std::string> seen_part6_replay_keys;
  std::unordered_set<std::string> seen_part7_actor_replay_keys;
  std::unordered_set<std::string> seen_part11_ffi_replay_keys;
  std::unordered_set<std::string> seen_part11_header_module_bridge_replay_keys;
  std::unordered_set<std::string> seen_part10_host_cache_replay_keys;
  std::vector<std::string> ordered_link_object_artifacts;
  std::vector<std::string> merged_driver_linker_flags;
  std::vector<std::string> imported_part6_module_names;
  std::vector<std::string> imported_part7_actor_module_names;
  std::vector<std::string> imported_part11_ffi_module_names;
  std::vector<std::string> imported_part11_header_module_bridge_module_names;
  std::vector<std::string> imported_part10_host_cache_module_names;
  std::vector<std::string> direct_import_surface_artifact_paths =
      inputs.direct_import_surface_artifact_paths;
  std::sort(direct_import_surface_artifact_paths.begin(),
            direct_import_surface_artifact_paths.end());

  seen_translation_unit_identity_keys.insert(
      inputs.local_translation_unit_identity_key);
  seen_registration_ordinals.insert(
      inputs.local_translation_unit_registration_order_ordinal);
  for (const auto &surface_path : direct_import_surface_artifact_paths) {
    if (surface_path.empty()) {
      error =
          "cross-module runtime link-plan direct import surface path missing";
      return false;
    }
    if (!seen_direct_import_surface_paths.insert(surface_path).second) {
      error =
          "cross-module runtime link-plan duplicate direct import surface "
          "path: " +
          surface_path;
      return false;
    }
  }
  for (const auto &local_flag : inputs.local_driver_linker_flags) {
    if (local_flag.empty()) {
      error =
          "cross-module runtime link-plan local driver-linker flag input missing";
      return false;
    }
  }

  struct OrderedLinkInput {
    std::uint64_t registration_order_ordinal = 0;
    std::string translation_unit_identity_key;
    std::string module_name;
    std::string object_artifact_path;
    std::vector<std::string> driver_linker_flags;
  };
  std::vector<OrderedLinkInput> ordered_link_inputs;
  ordered_link_inputs.reserve(imported_inputs.size() + 1u);

  for (const auto &imported_input : imported_inputs) {
    if (imported_input.module_name.empty() ||
        imported_input.import_surface_artifact_path.empty() ||
        imported_input.registration_manifest_artifact_path.empty() ||
        imported_input.object_artifact_path.empty() ||
        imported_input.discovery_artifact_path.empty() ||
        imported_input.linker_response_artifact_path.empty() ||
        imported_input.translation_unit_identity_model.empty() ||
        imported_input.translation_unit_identity_key.empty() ||
        imported_input.object_format.empty() ||
        imported_input.runtime_support_library_archive_relative_path.empty() ||
        imported_input.translation_unit_registration_order_ordinal == 0 ||
        imported_input.driver_linker_flags.empty()) {
      error =
          "cross-module runtime link-plan imported input is incomplete for " +
          imported_input.module_name;
      return false;
    }
    if (imported_input.object_format != inputs.object_format) {
      error = "cross-module runtime link-plan object-format mismatch for " +
              imported_input.module_name;
      return false;
    }
    // error-runtime/bridge-helper anchor: Part 6 lane-D still rides
    // the same packaged runtime-support archive path as the rest of the native
    // runtime helper cluster, so imported modules must agree on that archive
    // before later cross-module Part 6 execution claims can become truthful.
    // continuation/runtime-helper anchor: the first private Part 7
    // continuation helper cluster likewise stays inside the same packaged
    // runtime-support archive path. Mixed-module async/runtime-helper claims
    // therefore may not diverge on the runtime archive even before live
    // suspension integration lands.
    // live continuation/runtime integration anchor: the supported
    // direct-call await slice now links and executes through that same archive,
    // so runnable Part 7 helper traffic still depends on this runtime archive
    // equality across mixed-module link plans.
    // actor-runtime/executor-binding anchor: the private actor
    // runtime helper cluster also rides that same packaged runtime archive, so
    // actor-state/executor-binding claims cannot diverge on archive identity
    // across mixed-module link plans.
    // actor-mailbox/isolation-runtime anchor: live mailbox helper
    // traffic still links through that same packaged runtime archive, so mixed
    // actor-runtime link plans must keep one archive identity even after the
    // mailbox helpers become runnable.
    // system-helper/runtime-contract anchor: Part 8 cleanup/resource
    // and retainable-family runtime proof also stays on that same packaged
    // runtime archive path. Mixed-module link plans may not diverge on archive
    // identity while lane-D still reuses the existing private helper cluster.
    // expansion-host/runtime-boundary anchor: Part 10 does not launch
    // a macro host from the driver yet; the packaged objc3_runtime.lib archive
    // remains the only host-facing runtime handoff while macro execution and
    // runtime package loading stay fail-closed.
    if (imported_input.runtime_support_library_archive_relative_path !=
        inputs.runtime_support_library_archive_relative_path) {
      error =
          "cross-module runtime link-plan runtime library path mismatch for " +
          imported_input.module_name;
      return false;
    }
    for (const auto &flag : imported_input.driver_linker_flags) {
      if (flag.empty()) {
        error =
            "cross-module runtime link-plan imported linker flag is empty for " +
            imported_input.module_name;
        return false;
      }
    }
    if (!seen_translation_unit_identity_keys
             .insert(imported_input.translation_unit_identity_key)
             .second) {
      error =
          "cross-module runtime link-plan duplicate translation-unit identity key: " +
          imported_input.translation_unit_identity_key;
      return false;
    }
    if (!seen_registration_ordinals
             .insert(imported_input.translation_unit_registration_order_ordinal)
             .second) {
      error =
          "cross-module runtime link-plan duplicate registration order ordinal: " +
          std::to_string(
              imported_input.translation_unit_registration_order_ordinal);
      return false;
    }
    if (imported_input.part6_result_and_bridging_artifact_replay_present) {
      if (imported_input.part6_contract_id != inputs.expected_part6_contract_id) {
        error =
            "cross-module runtime link-plan Part 6 contract mismatch for " +
            imported_input.module_name;
        return false;
      }
      if (imported_input.part6_source_contract_id !=
          inputs.expected_part6_source_contract_id) {
        error =
            "cross-module runtime link-plan Part 6 source contract mismatch for " +
            imported_input.module_name;
        return false;
      }
      if (!imported_input.part6_binary_artifact_replay_ready ||
          !imported_input.part6_runtime_import_artifact_ready ||
          !imported_input.part6_separate_compilation_replay_ready ||
          imported_input.part6_result_and_bridging_artifact_replay_key.empty() ||
          imported_input.part6_part6_replay_key.empty() ||
          imported_input.part6_throws_replay_key.empty() ||
          imported_input.part6_result_like_replay_key.empty() ||
          imported_input.part6_ns_error_replay_key.empty() ||
          imported_input.part6_unwind_replay_key.empty()) {
        error =
            "cross-module runtime link-plan Part 6 replay surface incomplete for " +
            imported_input.module_name;
        return false;
      }
      if (!seen_part6_replay_keys.insert(imported_input.part6_part6_replay_key)
               .second) {
        error =
            "cross-module runtime link-plan duplicate imported Part 6 replay key: " +
            imported_input.part6_part6_replay_key;
        return false;
      }
      imported_part6_module_names.push_back(imported_input.module_name);
    }
    if (imported_input.part7_actor_mailbox_runtime_import_present) {
      // actor cross-module isolation-metadata hardening anchor:
      // imported actor-runtime surfaces must preserve one canonical private
      // mailbox/isolation replay contract and may not drift on contract ids or
      // replay keys across mixed-module link plans.
      if (imported_input.part7_actor_contract_id !=
          inputs.expected_part7_actor_contract_id) {
        error =
            "cross-module runtime link-plan Part 7 actor contract mismatch for " +
            imported_input.module_name;
        return false;
      }
      if (imported_input.part7_actor_source_contract_id !=
          inputs.expected_part7_actor_source_contract_id) {
        error =
            "cross-module runtime link-plan Part 7 actor source contract mismatch for " +
            imported_input.module_name;
        return false;
      }
      if (!imported_input.part7_actor_mailbox_runtime_ready ||
          !imported_input.part7_actor_mailbox_runtime_deterministic ||
          imported_input.part7_actor_mailbox_runtime_replay_key.empty() ||
          imported_input.part7_actor_lowering_replay_key.empty() ||
          imported_input.part7_actor_isolation_lowering_replay_key.empty()) {
        error =
            "cross-module runtime link-plan Part 7 actor replay surface incomplete for " +
            imported_input.module_name;
        return false;
      }
      if (!seen_part7_actor_replay_keys
               .insert(imported_input.part7_actor_mailbox_runtime_replay_key)
               .second) {
        error =
            "cross-module runtime link-plan duplicate imported Part 7 actor replay key: " +
            imported_input.part7_actor_mailbox_runtime_replay_key;
        return false;
      }
      imported_part7_actor_module_names.push_back(imported_input.module_name);
    }
    if (imported_input.part11_ffi_metadata_interface_preservation_present) {
      // bridge-packaging/toolchain anchor: imported Part 11 runtime-
      // import surfaces must preserve one canonical metadata/interface packet
      // across mixed-module link plans before D002 claims live header/module/
      // bridge generation from that packaging topology.
      if (imported_input.part11_ffi_contract_id !=
          inputs.expected_part11_ffi_contract_id) {
        error =
            "cross-module runtime link-plan Part 11 ffi contract mismatch for " +
            imported_input.module_name;
        return false;
      }
      if (imported_input.part11_ffi_source_contract_id !=
          inputs.expected_part11_ffi_source_contract_id) {
        error =
            "cross-module runtime link-plan Part 11 ffi source contract mismatch for " +
            imported_input.module_name;
        return false;
      }
      if (imported_input.part11_ffi_preservation_contract_id !=
          inputs.expected_part11_ffi_preservation_contract_id) {
        error =
            "cross-module runtime link-plan Part 11 ffi preservation contract mismatch for " +
            imported_input.module_name;
        return false;
      }
      if (!imported_input.part11_ffi_runtime_import_artifact_ready ||
          !imported_input.part11_ffi_separate_compilation_preservation_ready ||
          !imported_input.part11_ffi_deterministic ||
          imported_input.part11_ffi_replay_key.empty() ||
          imported_input.part11_ffi_lowering_replay_key.empty() ||
          imported_input.part11_ffi_preservation_replay_key.empty()) {
        error =
            "cross-module runtime link-plan Part 11 ffi preservation surface incomplete for " +
            imported_input.module_name;
        return false;
      }
      if (!seen_part11_ffi_replay_keys
               .insert(imported_input.part11_ffi_replay_key)
               .second) {
        error =
            "cross-module runtime link-plan duplicate imported Part 11 ffi replay key: " +
            imported_input.part11_ffi_replay_key;
        return false;
      }
      imported_part11_ffi_module_names.push_back(imported_input.module_name);
    }
    if (imported_input.part11_header_module_bridge_generation_present) {
      if (imported_input.part11_header_module_bridge_contract_id !=
          inputs.expected_part11_header_module_bridge_contract_id) {
        error =
            "cross-module runtime link-plan Part 11 bridge-generation contract mismatch for " +
            imported_input.module_name;
        return false;
      }
      if (imported_input.part11_header_module_bridge_source_contract_id !=
          inputs.expected_part11_header_module_bridge_source_contract_id) {
        error =
            "cross-module runtime link-plan Part 11 bridge-generation source contract mismatch for " +
            imported_input.module_name;
        return false;
      }
      if (imported_input.part11_header_module_bridge_preservation_contract_id !=
          inputs.expected_part11_header_module_bridge_preservation_contract_id) {
        error =
            "cross-module runtime link-plan Part 11 bridge-generation preservation contract mismatch for " +
            imported_input.module_name;
        return false;
      }
      if (!imported_input.part11_header_module_bridge_runtime_generation_ready ||
          !imported_input
               .part11_header_module_bridge_cross_module_packaging_ready ||
          !imported_input.part11_header_module_bridge_deterministic ||
          imported_input.part11_header_module_bridge_replay_key.empty() ||
          imported_input.part11_header_module_bridge_preservation_replay_key
              .empty() ||
          imported_input.part11_bridge_header_artifact_relative_path.empty() ||
          imported_input.part11_bridge_module_artifact_relative_path.empty() ||
          imported_input.part11_bridge_artifact_relative_path.empty()) {
        error =
            "cross-module runtime link-plan Part 11 bridge-generation surface incomplete for " +
            imported_input.module_name;
        return false;
      }
      if (imported_input.part11_bridge_header_artifact_relative_path !=
              inputs.expected_part11_bridge_header_artifact_relative_path ||
          imported_input.part11_bridge_module_artifact_relative_path !=
              inputs.expected_part11_bridge_module_artifact_relative_path ||
          imported_input.part11_bridge_artifact_relative_path !=
              inputs.expected_part11_bridge_artifact_relative_path) {
        error =
            "cross-module runtime link-plan Part 11 bridge-generation artifact path mismatch for " +
            imported_input.module_name;
        return false;
      }
      if (!seen_part11_header_module_bridge_replay_keys
               .insert(imported_input.part11_header_module_bridge_replay_key)
               .second) {
        error =
            "cross-module runtime link-plan duplicate imported Part 11 bridge-generation replay key: " +
            imported_input.part11_header_module_bridge_replay_key;
        return false;
      }
      imported_part11_header_module_bridge_module_names.push_back(
          imported_input.module_name);
    }
    if (imported_input.part10_macro_host_process_cache_runtime_integration_present) {
      if (imported_input.part10_macro_host_process_cache_contract_id !=
          inputs.expected_part10_host_cache_contract_id) {
        error =
            "cross-module runtime link-plan Part 10 host/cache contract mismatch for " +
            imported_input.module_name;
        return false;
      }
      if (imported_input.part10_macro_host_process_cache_source_contract_id !=
          inputs.expected_part10_host_cache_source_contract_id) {
        error =
            "cross-module runtime link-plan Part 10 host/cache source contract mismatch for " +
            imported_input.module_name;
        return false;
      }
      if (!imported_input.part10_macro_host_process_cache_runtime_ready ||
          !imported_input.part10_macro_host_process_cache_separate_compilation_ready ||
          !imported_input.part10_macro_host_process_cache_deterministic ||
          imported_input.part10_macro_host_process_cache_replay_key.empty() ||
          imported_input.part10_macro_host_process_cache_host_executable_relative_path.empty() ||
          imported_input.part10_macro_host_process_cache_root_relative_path.empty()) {
        error =
            "cross-module runtime link-plan Part 10 host/cache surface incomplete for " +
            imported_input.module_name;
        return false;
      }
      if (imported_input.part10_macro_host_process_cache_host_executable_relative_path !=
          inputs.expected_part10_host_cache_executable_relative_path) {
        error =
            "cross-module runtime link-plan Part 10 host/cache executable path mismatch for " +
            imported_input.module_name;
        return false;
      }
      if (imported_input.part10_macro_host_process_cache_root_relative_path !=
          inputs.expected_part10_host_cache_root_relative_path) {
        error =
            "cross-module runtime link-plan Part 10 host/cache root path mismatch for " +
            imported_input.module_name;
        return false;
      }
      if (!seen_part10_host_cache_replay_keys
               .insert(imported_input.part10_macro_host_process_cache_replay_key)
               .second) {
        error =
            "cross-module runtime link-plan duplicate imported Part 10 host/cache replay key: " +
            imported_input.part10_macro_host_process_cache_replay_key;
        return false;
      }
      imported_part10_host_cache_module_names.push_back(imported_input.module_name);
    }
    ordered_link_inputs.push_back(
        {imported_input.translation_unit_registration_order_ordinal,
         imported_input.translation_unit_identity_key,
         imported_input.module_name,
         imported_input.object_artifact_path,
         imported_input.driver_linker_flags});
  }

  ordered_link_inputs.push_back(
      {inputs.local_translation_unit_registration_order_ordinal,
       inputs.local_translation_unit_identity_key,
       inputs.local_module_name,
       inputs.local_object_artifact_relative_path,
       inputs.local_driver_linker_flags});
  std::sort(ordered_link_inputs.begin(), ordered_link_inputs.end(),
            [](const auto &lhs, const auto &rhs) {
              if (lhs.registration_order_ordinal != rhs.registration_order_ordinal) {
                return lhs.registration_order_ordinal <
                       rhs.registration_order_ordinal;
              }
              return lhs.translation_unit_identity_key <
                     rhs.translation_unit_identity_key;
            });
  for (const auto &ordered_input : ordered_link_inputs) {
    ordered_link_object_artifacts.push_back(ordered_input.object_artifact_path);
    for (const auto &flag : ordered_input.driver_linker_flags) {
      if (seen_driver_linker_flags.insert(flag).second) {
        merged_driver_linker_flags.push_back(flag);
      }
    }
  }
  if (merged_driver_linker_flags.empty()) {
    error =
        "cross-module runtime link-plan merged driver-linker flag list is empty";
    return false;
  }

  std::vector<std::string> module_names_lexicographic;
  module_names_lexicographic.reserve(imported_inputs.size() + 1u);
  module_names_lexicographic.push_back(inputs.local_module_name);
  for (const auto &imported_input : imported_inputs) {
    module_names_lexicographic.push_back(imported_input.module_name);
  }
  std::sort(module_names_lexicographic.begin(), module_names_lexicographic.end());

  std::ostringstream imported_modules_json;
  imported_modules_json << "[\n";
  for (std::size_t index = 0; index < imported_inputs.size(); ++index) {
    const auto &imported_input = imported_inputs[index];
    imported_modules_json
        << "    {\n"
        << "      \"module_name\": \""
        << EscapeJsonString(imported_input.module_name) << "\",\n"
        << "      \"import_surface_artifact_path\": \""
        << EscapeJsonString(imported_input.import_surface_artifact_path)
        << "\",\n"
        << "      \"registration_manifest_artifact_path\": \""
        << EscapeJsonString(imported_input.registration_manifest_artifact_path)
        << "\",\n"
        << "      \"object_artifact_path\": \""
        << EscapeJsonString(imported_input.object_artifact_path) << "\",\n"
        << "      \"discovery_artifact_path\": \""
        << EscapeJsonString(imported_input.discovery_artifact_path) << "\",\n"
        << "      \"linker_response_artifact_path\": \""
        << EscapeJsonString(imported_input.linker_response_artifact_path)
        << "\",\n"
        << "      \"translation_unit_identity_model\": \""
        << EscapeJsonString(imported_input.translation_unit_identity_model)
        << "\",\n"
        << "      \"translation_unit_identity_key\": \""
        << EscapeJsonString(imported_input.translation_unit_identity_key)
        << "\",\n"
        << "      \"translation_unit_registration_order_ordinal\": "
        << imported_input.translation_unit_registration_order_ordinal << ",\n"
        << "      \"object_format\": \""
        << EscapeJsonString(imported_input.object_format) << "\",\n"
        << "      \"runtime_support_library_archive_relative_path\": \""
        << EscapeJsonString(
               imported_input.runtime_support_library_archive_relative_path)
        << "\",\n"
        << "      \"part6_result_and_bridging_artifact_replay_present\": "
        << (imported_input.part6_result_and_bridging_artifact_replay_present
                ? "true"
                : "false")
        << ",\n"
        << "      \"part6_binary_artifact_replay_ready\": "
        << (imported_input.part6_binary_artifact_replay_ready ? "true"
                                                              : "false")
        << ",\n"
        << "      \"part6_runtime_import_artifact_ready\": "
        << (imported_input.part6_runtime_import_artifact_ready ? "true"
                                                               : "false")
        << ",\n"
        << "      \"part6_separate_compilation_replay_ready\": "
        << (imported_input.part6_separate_compilation_replay_ready ? "true"
                                                                   : "false")
        << ",\n"
        << "      \"part6_deterministic\": "
        << (imported_input.part6_deterministic ? "true" : "false")
        << ",\n"
        << "      \"part6_contract_id\": \""
        << EscapeJsonString(imported_input.part6_contract_id) << "\",\n"
        << "      \"part6_source_contract_id\": \""
        << EscapeJsonString(imported_input.part6_source_contract_id)
        << "\",\n"
        << "      \"part6_result_and_bridging_artifact_replay_key\": \""
        << EscapeJsonString(
               imported_input.part6_result_and_bridging_artifact_replay_key)
        << "\",\n"
        << "      \"part6_replay_key\": \""
        << EscapeJsonString(imported_input.part6_part6_replay_key) << "\",\n"
        << "      \"throws_replay_key\": \""
        << EscapeJsonString(imported_input.part6_throws_replay_key)
        << "\",\n"
        << "      \"result_like_replay_key\": \""
        << EscapeJsonString(imported_input.part6_result_like_replay_key)
        << "\",\n"
        << "      \"ns_error_replay_key\": \""
        << EscapeJsonString(imported_input.part6_ns_error_replay_key)
        << "\",\n"
        << "      \"unwind_replay_key\": \""
        << EscapeJsonString(imported_input.part6_unwind_replay_key)
        << "\",\n"
        << "      \"part7_actor_mailbox_runtime_import_present\": "
        << (imported_input.part7_actor_mailbox_runtime_import_present ? "true"
                                                                      : "false")
        << ",\n"
        << "      \"part7_actor_mailbox_runtime_ready\": "
        << (imported_input.part7_actor_mailbox_runtime_ready ? "true"
                                                             : "false")
        << ",\n"
        << "      \"part7_actor_mailbox_runtime_deterministic\": "
        << (imported_input.part7_actor_mailbox_runtime_deterministic ? "true"
                                                                     : "false")
        << ",\n"
        << "      \"part7_actor_contract_id\": \""
        << EscapeJsonString(imported_input.part7_actor_contract_id)
        << "\",\n"
        << "      \"part7_actor_source_contract_id\": \""
        << EscapeJsonString(imported_input.part7_actor_source_contract_id)
        << "\",\n"
        << "      \"part7_actor_mailbox_runtime_replay_key\": \""
        << EscapeJsonString(imported_input.part7_actor_mailbox_runtime_replay_key)
        << "\",\n"
        << "      \"part7_actor_lowering_replay_key\": \""
        << EscapeJsonString(imported_input.part7_actor_lowering_replay_key)
        << "\",\n"
        << "      \"part7_actor_isolation_lowering_replay_key\": \""
        << EscapeJsonString(
               imported_input.part7_actor_isolation_lowering_replay_key)
        << "\",\n"
        << "      \"part11_ffi_metadata_interface_preservation_present\": "
        << (imported_input.part11_ffi_metadata_interface_preservation_present
                ? "true"
                : "false")
        << ",\n"
        << "      \"part11_ffi_runtime_import_artifact_ready\": "
        << (imported_input.part11_ffi_runtime_import_artifact_ready ? "true"
                                                                    : "false")
        << ",\n"
        << "      \"part11_ffi_separate_compilation_preservation_ready\": "
        << (imported_input.part11_ffi_separate_compilation_preservation_ready
                ? "true"
                : "false")
        << ",\n"
        << "      \"part11_ffi_deterministic\": "
        << (imported_input.part11_ffi_deterministic ? "true" : "false")
        << ",\n"
        << "      \"part11_ffi_contract_id\": \""
        << EscapeJsonString(imported_input.part11_ffi_contract_id)
        << "\",\n"
        << "      \"part11_ffi_source_contract_id\": \""
        << EscapeJsonString(imported_input.part11_ffi_source_contract_id)
        << "\",\n"
        << "      \"part11_ffi_preservation_contract_id\": \""
        << EscapeJsonString(imported_input.part11_ffi_preservation_contract_id)
        << "\",\n"
        << "      \"part11_ffi_replay_key\": \""
        << EscapeJsonString(imported_input.part11_ffi_replay_key)
        << "\",\n"
        << "      \"part11_ffi_lowering_replay_key\": \""
        << EscapeJsonString(imported_input.part11_ffi_lowering_replay_key)
        << "\",\n"
        << "      \"part11_ffi_preservation_replay_key\": \""
        << EscapeJsonString(imported_input.part11_ffi_preservation_replay_key)
        << "\",\n"
        << "      \"part11_ffi_local_foreign_callable_count\": "
        << imported_input.part11_ffi_local_foreign_callable_count << ",\n"
        << "      \"part11_ffi_local_metadata_preservation_sites\": "
        << imported_input.part11_ffi_local_metadata_preservation_sites
        << ",\n"
        << "      \"part11_ffi_local_interface_annotation_sites\": "
        << imported_input.part11_ffi_local_interface_annotation_sites
        << ",\n"
        << "      \"part11_header_module_bridge_generation_present\": "
        << (imported_input.part11_header_module_bridge_generation_present
                ? "true"
                : "false")
        << ",\n"
        << "      \"part11_header_module_bridge_runtime_generation_ready\": "
        << (imported_input.part11_header_module_bridge_runtime_generation_ready
                ? "true"
                : "false")
        << ",\n"
        << "      \"part11_header_module_bridge_cross_module_packaging_ready\": "
        << (imported_input
                    .part11_header_module_bridge_cross_module_packaging_ready
                ? "true"
                : "false")
        << ",\n"
        << "      \"part11_header_module_bridge_deterministic\": "
        << (imported_input.part11_header_module_bridge_deterministic ? "true"
                                                                     : "false")
        << ",\n"
        << "      \"part11_header_module_bridge_contract_id\": \""
        << EscapeJsonString(
               imported_input.part11_header_module_bridge_contract_id)
        << "\",\n"
        << "      \"part11_header_module_bridge_source_contract_id\": \""
        << EscapeJsonString(
               imported_input.part11_header_module_bridge_source_contract_id)
        << "\",\n"
        << "      \"part11_header_module_bridge_preservation_contract_id\": \""
        << EscapeJsonString(imported_input
                                .part11_header_module_bridge_preservation_contract_id)
        << "\",\n"
        << "      \"part11_header_module_bridge_replay_key\": \""
        << EscapeJsonString(imported_input.part11_header_module_bridge_replay_key)
        << "\",\n"
        << "      \"part11_header_module_bridge_preservation_replay_key\": \""
        << EscapeJsonString(
               imported_input
                   .part11_header_module_bridge_preservation_replay_key)
        << "\",\n"
        << "      \"part11_bridge_header_artifact_relative_path\": \""
        << EscapeJsonString(
               imported_input.part11_bridge_header_artifact_relative_path)
        << "\",\n"
        << "      \"part11_bridge_module_artifact_relative_path\": \""
        << EscapeJsonString(
               imported_input.part11_bridge_module_artifact_relative_path)
        << "\",\n"
        << "      \"part11_bridge_artifact_relative_path\": \""
        << EscapeJsonString(imported_input.part11_bridge_artifact_relative_path)
        << "\",\n"
        << "      \"part11_header_module_bridge_local_foreign_callable_count\": "
        << imported_input.part11_header_module_bridge_local_foreign_callable_count
        << ",\n"
        << "      \"part10_macro_host_process_cache_runtime_integration_present\": "
        << (imported_input.part10_macro_host_process_cache_runtime_integration_present
                ? "true"
                : "false")
        << ",\n"
        << "      \"part10_macro_host_process_cache_runtime_ready\": "
        << (imported_input.part10_macro_host_process_cache_runtime_ready ? "true"
                                                                         : "false")
        << ",\n"
        << "      \"part10_macro_host_process_cache_separate_compilation_ready\": "
        << (imported_input.part10_macro_host_process_cache_separate_compilation_ready
                ? "true"
                : "false")
        << ",\n"
        << "      \"part10_macro_host_process_cache_deterministic\": "
        << (imported_input.part10_macro_host_process_cache_deterministic ? "true"
                                                                         : "false")
        << ",\n"
        << "      \"part10_macro_host_process_cache_contract_id\": \""
        << EscapeJsonString(imported_input.part10_macro_host_process_cache_contract_id)
        << "\",\n"
        << "      \"part10_macro_host_process_cache_source_contract_id\": \""
        << EscapeJsonString(imported_input.part10_macro_host_process_cache_source_contract_id)
        << "\",\n"
        << "      \"part10_macro_host_process_cache_replay_key\": \""
        << EscapeJsonString(imported_input.part10_macro_host_process_cache_replay_key)
        << "\",\n"
        << "      \"part10_macro_host_process_cache_host_executable_relative_path\": \""
        << EscapeJsonString(imported_input.part10_macro_host_process_cache_host_executable_relative_path)
        << "\",\n"
        << "      \"part10_macro_host_process_cache_root_relative_path\": \""
        << EscapeJsonString(imported_input.part10_macro_host_process_cache_root_relative_path)
        << "\",\n"
        << "      \"driver_linker_flags\": "
        << BuildIndentedStringArrayJson(imported_input.driver_linker_flags,
                                        "        ");
    imported_modules_json << "\n    }";
    if (index + 1u < imported_inputs.size()) {
      imported_modules_json << ",";
    }
    imported_modules_json << "\n";
  }
  imported_modules_json << "  ]";
  std::sort(imported_part6_module_names.begin(), imported_part6_module_names.end());
  std::sort(imported_part7_actor_module_names.begin(),
            imported_part7_actor_module_names.end());
  std::sort(imported_part11_ffi_module_names.begin(),
            imported_part11_ffi_module_names.end());
  std::sort(imported_part11_header_module_bridge_module_names.begin(),
            imported_part11_header_module_bridge_module_names.end());
  std::sort(imported_part10_host_cache_module_names.begin(),
            imported_part10_host_cache_module_names.end());

  std::ostringstream out;
  out << "{\n"
      << "  \"contract_id\": \"" << EscapeJsonString(inputs.contract_id)
      << "\",\n"
      << "  \"source_orchestration_contract_id\": \""
      << EscapeJsonString(inputs.source_orchestration_contract_id) << "\",\n"
      << "  \"import_surface_contract_id\": \""
      << EscapeJsonString(inputs.import_surface_contract_id) << "\",\n"
      << "  \"registration_manifest_contract_id\": \""
      << EscapeJsonString(inputs.registration_manifest_contract_id)
      << "\",\n"
      << "  \"payload_model\": \"" << EscapeJsonString(inputs.payload_model)
      << "\",\n"
      << "  \"artifact\": \"" << EscapeJsonString(inputs.artifact_relative_path)
      << "\",\n"
      << "  \"linker_response_artifact\": \""
      << EscapeJsonString(inputs.linker_response_artifact_relative_path)
      << "\",\n"
      << "  \"authority_model\": \""
      << EscapeJsonString(inputs.authority_model) << "\",\n"
      << "  \"packaging_model\": \""
      << EscapeJsonString(inputs.packaging_model) << "\",\n"
      << "  \"registration_scope_model\": \""
      << EscapeJsonString(inputs.registration_scope_model) << "\",\n"
      << "  \"link_object_order_model\": \""
      << EscapeJsonString(inputs.link_object_order_model) << "\",\n"
      << "  \"expected_part6_contract_id\": \""
      << EscapeJsonString(inputs.expected_part6_contract_id) << "\",\n"
      << "  \"expected_part6_source_contract_id\": \""
      << EscapeJsonString(inputs.expected_part6_source_contract_id) << "\",\n"
      << "  \"expected_part7_actor_contract_id\": \""
      << EscapeJsonString(inputs.expected_part7_actor_contract_id) << "\",\n"
      << "  \"expected_part7_actor_source_contract_id\": \""
      << EscapeJsonString(inputs.expected_part7_actor_source_contract_id)
      << "\",\n"
      << "  \"expected_part11_ffi_contract_id\": \""
      << EscapeJsonString(inputs.expected_part11_ffi_contract_id) << "\",\n"
      << "  \"expected_part11_ffi_source_contract_id\": \""
      << EscapeJsonString(inputs.expected_part11_ffi_source_contract_id)
      << "\",\n"
      << "  \"expected_part11_ffi_preservation_contract_id\": \""
      << EscapeJsonString(inputs.expected_part11_ffi_preservation_contract_id)
      << "\",\n"
      << "  \"expected_part11_header_module_bridge_contract_id\": \""
      << EscapeJsonString(
             inputs.expected_part11_header_module_bridge_contract_id)
      << "\",\n"
      << "  \"expected_part11_header_module_bridge_source_contract_id\": \""
      << EscapeJsonString(
             inputs.expected_part11_header_module_bridge_source_contract_id)
      << "\",\n"
      << "  \"expected_part11_header_module_bridge_preservation_contract_id\": \""
      << EscapeJsonString(
             inputs.expected_part11_header_module_bridge_preservation_contract_id)
      << "\",\n"
      << "  \"expected_part11_bridge_header_artifact_relative_path\": \""
      << EscapeJsonString(
             inputs.expected_part11_bridge_header_artifact_relative_path)
      << "\",\n"
      << "  \"expected_part11_bridge_module_artifact_relative_path\": \""
      << EscapeJsonString(
             inputs.expected_part11_bridge_module_artifact_relative_path)
      << "\",\n"
      << "  \"expected_part11_bridge_artifact_relative_path\": \""
      << EscapeJsonString(inputs.expected_part11_bridge_artifact_relative_path)
      << "\",\n"
      << "  \"expected_part10_host_cache_contract_id\": \""
      << EscapeJsonString(inputs.expected_part10_host_cache_contract_id)
      << "\",\n"
      << "  \"expected_part10_host_cache_source_contract_id\": \""
      << EscapeJsonString(inputs.expected_part10_host_cache_source_contract_id)
      << "\",\n"
      << "  \"expected_part10_host_cache_executable_relative_path\": \""
      << EscapeJsonString(
             inputs.expected_part10_host_cache_executable_relative_path)
      << "\",\n"
      << "  \"expected_part10_host_cache_root_relative_path\": \""
      << EscapeJsonString(inputs.expected_part10_host_cache_root_relative_path)
      << "\",\n"
      << "  \"module_names_lexicographic\": "
      << BuildIndentedStringArrayJson(module_names_lexicographic, "    ")
      << ",\n"
      << "  \"module_image_count\": " << module_names_lexicographic.size()
      << ",\n"
      << "  \"direct_import_input_count\": "
      << direct_import_surface_artifact_paths.size() << ",\n"
      << "  \"part6_imported_module_count\": "
      << imported_part6_module_names.size() << ",\n"
      << "  \"part7_actor_imported_module_count\": "
      << imported_part7_actor_module_names.size() << ",\n"
      << "  \"part11_ffi_imported_module_count\": "
      << imported_part11_ffi_module_names.size() << ",\n"
      << "  \"part11_header_module_bridge_imported_module_count\": "
      << imported_part11_header_module_bridge_module_names.size() << ",\n"
      << "  \"part10_host_cache_imported_module_count\": "
      << imported_part10_host_cache_module_names.size() << ",\n"
      << "  \"direct_import_surface_artifact_paths\": "
      << BuildIndentedStringArrayJson(direct_import_surface_artifact_paths,
                                      "    ")
      << ",\n"
      << "  \"part6_imported_module_names_lexicographic\": "
      << BuildIndentedStringArrayJson(imported_part6_module_names, "    ")
      << ",\n"
      << "  \"part7_actor_imported_module_names_lexicographic\": "
      << BuildIndentedStringArrayJson(imported_part7_actor_module_names, "    ")
      << ",\n"
      << "  \"part11_ffi_imported_module_names_lexicographic\": "
      << BuildIndentedStringArrayJson(imported_part11_ffi_module_names, "    ")
      << ",\n"
      << "  \"part11_header_module_bridge_imported_module_names_lexicographic\": "
      << BuildIndentedStringArrayJson(
             imported_part11_header_module_bridge_module_names, "    ")
      << ",\n"
      << "  \"part10_host_cache_imported_module_names_lexicographic\": "
      << BuildIndentedStringArrayJson(imported_part10_host_cache_module_names,
                                      "    ")
      << ",\n"
      << "  \"part6_cross_module_preservation_ready\": "
      << (!imported_part6_module_names.empty() ? "true" : "false") << ",\n"
      << "  \"part7_actor_cross_module_isolation_ready\": "
      << (!imported_part7_actor_module_names.empty() ? "true" : "false")
      << ",\n"
      << "  \"part11_ffi_cross_module_packaging_ready\": "
      << (!imported_part11_ffi_module_names.empty() ? "true" : "false")
      << ",\n"
      << "  \"part11_header_module_bridge_cross_module_packaging_ready\": "
      << (!imported_part11_header_module_bridge_module_names.empty() ? "true"
                                                                     : "false")
      << ",\n"
      << "  \"part10_host_cache_cross_module_preservation_ready\": "
      << (!imported_part10_host_cache_module_names.empty() ? "true" : "false")
      << ",\n"
      << "  \"cleanup_unwind_runtime_link_model\": "
      << "\"linker-response-plus-runtime-support-archive-sidecars-provide-runnable-cleanup-executable-link-inputs\",\n"
      << "  \"runtime_support_library_archive_relative_path\": \""
      << EscapeJsonString(inputs.runtime_support_library_archive_relative_path)
      << "\",\n"
      << "  \"object_format\": \"" << EscapeJsonString(inputs.object_format)
      << "\",\n"
      << "  \"local_module\": {\n"
      << "    \"module_name\": \""
      << EscapeJsonString(inputs.local_module_name) << "\",\n"
      << "    \"import_surface_artifact_relative_path\": \""
      << EscapeJsonString(inputs.local_import_surface_artifact_relative_path)
      << "\",\n"
      << "    \"registration_manifest_artifact_relative_path\": \""
      << EscapeJsonString(
             inputs.local_registration_manifest_artifact_relative_path)
      << "\",\n"
      << "    \"object_artifact_relative_path\": \""
      << EscapeJsonString(inputs.local_object_artifact_relative_path)
      << "\",\n"
      << "    \"translation_unit_identity_model\": \""
      << EscapeJsonString(inputs.local_translation_unit_identity_model)
      << "\",\n"
      << "    \"translation_unit_identity_key\": \""
      << EscapeJsonString(inputs.local_translation_unit_identity_key)
      << "\",\n"
      << "    \"translation_unit_registration_order_ordinal\": "
      << inputs.local_translation_unit_registration_order_ordinal << ",\n"
      << "    \"driver_linker_flags\": "
      << BuildIndentedStringArrayJson(inputs.local_driver_linker_flags,
                                      "      ")
      << "\n"
      << "  },\n"
      << "  \"imported_modules\": " << imported_modules_json.str() << ",\n"
      << "  \"link_object_artifacts\": "
      << BuildIndentedStringArrayJson(ordered_link_object_artifacts, "    ")
      << ",\n"
      << "  \"driver_linker_flags\": "
      << BuildIndentedStringArrayJson(merged_driver_linker_flags, "    ")
      << ",\n"
      << "  \"ready\": true\n"
      << "}\n";
  plan_json = out.str();

  std::ostringstream response_out;
  for (const auto &flag : merged_driver_linker_flags) {
    response_out << flag << "\n";
  }
  linker_response_payload = response_out.str();
  return true;
}

bool TryBuildObjc3ConformanceReportPublicationArtifact(
    const Objc3ConformanceReportPublicationArtifactInputs &inputs,
    std::string &artifact_json,
    std::string &error) {
  artifact_json.clear();
  error.clear();

  // D001 publication remains fail-closed until richer profiles are runnable:
  // core-profile-live-other-known-profiles-fail-closed-before-publication

  if (inputs.contract_id.empty() || inputs.schema_id.empty() ||
      inputs.selected_profile.empty() || inputs.supported_profile_ids.empty() ||
      inputs.rejected_profile_ids.empty() ||
      inputs.effective_compatibility_mode.empty() ||
      inputs.publication_model.empty() ||
      inputs.publication_surface_kind.empty() ||
      inputs.fail_closed_diagnostic_model.empty() ||
      inputs.lowered_report_contract_id.empty() ||
      inputs.runtime_capability_contract_id.empty() ||
      inputs.public_conformance_schema_id.empty() ||
      inputs.advanced_feature_ops_contract_id.empty() ||
      inputs.advanced_feature_reporting_contract_id.empty() ||
      inputs.advanced_feature_release_evidence_contract_id.empty() ||
      inputs.ci_release_evidence_gate_script_path.empty() ||
      inputs.runbook_reference_path.empty() ||
      inputs.dashboard_schema_path.empty() ||
      inputs.advanced_feature_targeted_profile_ids.empty() ||
      inputs.report_artifact_relative_path.empty()) {
    error = "conformance report publication artifact inputs are incomplete";
    return false;
  }

  std::ostringstream out;
  out << "{\n"
      << "  \"contract_id\": \"" << EscapeJsonString(inputs.contract_id)
      << "\",\n"
      << "  \"schema_id\": \"" << EscapeJsonString(inputs.schema_id)
      << "\",\n"
      << "  \"selected_profile\": \""
      << EscapeJsonString(inputs.selected_profile) << "\",\n"
      << "  \"selected_profile_supported\": "
      << (inputs.selected_profile_supported ? "true" : "false") << ",\n"
      << "  \"supported_profile_ids\": "
      << BuildIndentedStringArrayJson(inputs.supported_profile_ids, "    ")
      << ",\n"
      << "  \"rejected_profile_ids\": "
      << BuildIndentedStringArrayJson(inputs.rejected_profile_ids, "    ")
      << ",\n"
      << "  \"effective_compatibility_mode\": \""
      << EscapeJsonString(inputs.effective_compatibility_mode) << "\",\n"
      << "  \"migration_assist_enabled\": "
      << (inputs.migration_assist_enabled ? "true" : "false") << ",\n"
      << "  \"publication_model\": \""
      << EscapeJsonString(inputs.publication_model) << "\",\n"
      << "  \"publication_surface_kind\": \""
      << EscapeJsonString(inputs.publication_surface_kind) << "\",\n"
      << "  \"fail_closed_diagnostic_model\": \""
      << EscapeJsonString(inputs.fail_closed_diagnostic_model) << "\",\n"
      << "  \"lowered_report_contract_id\": \""
      << EscapeJsonString(inputs.lowered_report_contract_id) << "\",\n"
      << "  \"runtime_capability_contract_id\": \""
      << EscapeJsonString(inputs.runtime_capability_contract_id) << "\",\n"
      << "  \"public_conformance_schema_id\": \""
      << EscapeJsonString(inputs.public_conformance_schema_id) << "\",\n"
      << "  \"advanced_feature_ops_contract_id\": \""
      << EscapeJsonString(inputs.advanced_feature_ops_contract_id) << "\",\n"
      << "  \"advanced_feature_reporting_contract_id\": \""
      << EscapeJsonString(inputs.advanced_feature_reporting_contract_id)
      << "\",\n"
      << "  \"advanced_feature_release_evidence_contract_id\": \""
      << EscapeJsonString(inputs.advanced_feature_release_evidence_contract_id)
      << "\",\n"
      << "  \"advanced_feature_targeted_profile_ids\": "
      << BuildIndentedStringArrayJson(inputs.advanced_feature_targeted_profile_ids,
                                      "    ")
      << ",\n"
      << "  \"ci_release_evidence_gate_script_path\": \""
      << EscapeJsonString(inputs.ci_release_evidence_gate_script_path)
      << "\",\n"
      << "  \"runbook_reference_path\": \""
      << EscapeJsonString(inputs.runbook_reference_path) << "\",\n"
      << "  \"dashboard_schema_path\": \""
      << EscapeJsonString(inputs.dashboard_schema_path) << "\",\n"
      << "  \"report_artifact\": \""
      << EscapeJsonString(inputs.report_artifact_relative_path) << "\",\n"
      << "  \"ready\": true\n"
      << "}\n";
  artifact_json = out.str();
  return true;
}

bool TryBuildObjc3ConformanceClaimValidationArtifact(
    const Objc3ConformanceClaimValidationArtifactInputs &inputs,
    const std::string &report_json,
    const std::string &publication_json,
    std::string &artifact_json,
    std::string &error) {
  artifact_json.clear();
  error.clear();

  if (inputs.report_artifact_path.empty() || inputs.publication_artifact_path.empty()) {
    error = "conformance validation artifact inputs are incomplete";
    return false;
  }

  std::string report_schema_id;
  std::string report_contract_id;
  std::string effective_compatibility_mode;
  std::string runtime_capability_contract_id;
  std::string public_conformance_schema_id;
  bool migration_assist_enabled = false;
  std::string publication_schema_id;
  std::string publication_contract_id;
  std::string selected_profile;
  bool selected_profile_supported = false;
  std::vector<std::string> supported_profile_ids;
  std::vector<std::string> rejected_profile_ids;
  std::string publication_surface_kind;
  std::string report_artifact_relative_path;
  std::string advanced_feature_ops_contract_id;
  std::string advanced_feature_reporting_contract_id;
  std::string advanced_feature_release_evidence_contract_id;
  std::vector<std::string> advanced_feature_targeted_profile_ids;
  std::string ci_release_evidence_gate_script_path;
  std::string runbook_reference_path;
  std::string dashboard_schema_path;

  if (!TryExtractJsonStringField(report_json, "schema_id", report_schema_id) ||
      report_schema_id != "objc3c-versioned-conformance-report-v1") {
    error = "invalid conformance report schema_id";
    return false;
  }
  if (!TryExtractJsonStringField(report_json, "contract_id", report_contract_id) ||
      report_contract_id != "objc3c-versioned-conformance-report-lowering/m264-c001-v1") {
    error = "invalid conformance report contract_id";
    return false;
  }
  if (!TryExtractJsonStringField(report_json, "effective_compatibility_mode",
                                 effective_compatibility_mode)) {
    error = "missing effective_compatibility_mode in conformance report";
    return false;
  }
  if (!TryExtractJsonBoolField(report_json, "migration_assist_enabled",
                               migration_assist_enabled)) {
    error = "missing migration_assist_enabled in conformance report";
    return false;
  }
  if (report_json.find("\"runtime_capability_report\"") == std::string::npos ||
      report_json.find(
          "\"contract_id\":\"objc3c-runtime-capability-reporting/m264-c002-v1\"") ==
          std::string::npos) {
    error = "runtime_capability_report payload is missing or drifted";
    return false;
  }
  runtime_capability_contract_id =
      "objc3c-runtime-capability-reporting/m264-c002-v1";
  if (report_json.find("\"public_conformance_report\"") == std::string::npos ||
      report_json.find("\"schema_id\":\"objc3-conformance-report/v1\"") ==
          std::string::npos) {
    error = "public_conformance_report payload is missing or drifted";
    return false;
  }
  public_conformance_schema_id = "objc3-conformance-report/v1";
  if (report_json.find("\"advanced_feature_reporting\"") == std::string::npos ||
      report_json.find(kObjc3AdvancedFeatureReportingContractId) ==
          std::string::npos) {
    error = "advanced_feature_reporting payload is missing or drifted";
    return false;
  }
  if (report_json.find("\"advanced_feature_release_evidence\"") ==
          std::string::npos ||
      report_json.find(kObjc3AdvancedFeatureReleaseEvidenceContractId) ==
          std::string::npos) {
    error = "advanced_feature_release_evidence payload is missing or drifted";
    return false;
  }

  if (!TryExtractJsonStringField(publication_json, "schema_id",
                                 publication_schema_id) ||
      publication_schema_id != "objc3c-driver-conformance-publication-v1") {
    error = "invalid conformance publication schema_id";
    return false;
  }
  if (!TryExtractJsonStringField(publication_json, "contract_id",
                                 publication_contract_id) ||
      publication_contract_id !=
          "objc3c-driver-conformance-report-publication/m264-d001-v1") {
    error = "invalid conformance publication contract_id";
    return false;
  }
  if (!TryExtractJsonStringField(publication_json, "selected_profile",
                                 selected_profile) ||
      selected_profile != "core") {
    error = "selected_profile must remain core";
    return false;
  }
  if (!TryExtractJsonBoolField(publication_json, "selected_profile_supported",
                               selected_profile_supported) ||
      !selected_profile_supported) {
    error = "selected_profile_supported must remain true";
    return false;
  }
  if (!TryExtractJsonStringArrayField(publication_json, "supported_profile_ids",
                                      supported_profile_ids) ||
      supported_profile_ids.size() != 1u ||
      supported_profile_ids[0] != "core") {
    error = "supported_profile_ids must remain [core]";
    return false;
  }
  if (!TryExtractJsonStringArrayField(publication_json, "rejected_profile_ids",
                                      rejected_profile_ids) ||
      rejected_profile_ids.size() != 3u) {
    error = "rejected_profile_ids must remain the three known non-runnable profiles";
    return false;
  }
  if (!TryExtractJsonStringField(publication_json, "publication_surface_kind",
                                 publication_surface_kind)) {
    error = "missing publication_surface_kind in conformance publication";
    return false;
  }
  if (!TryExtractJsonStringField(publication_json, "report_artifact",
                                 report_artifact_relative_path) ||
      report_artifact_relative_path !=
          std::filesystem::path(inputs.report_artifact_path).filename().string()) {
    error = "publication report_artifact does not match the validated report path";
    return false;
  }
  if (!TryExtractJsonStringField(publication_json,
                                 "advanced_feature_ops_contract_id",
                                 advanced_feature_ops_contract_id) ||
      advanced_feature_ops_contract_id != kObjc3AdvancedFeatureOpsContractId) {
    error = "advanced_feature_ops_contract_id drifted";
    return false;
  }
  if (!TryExtractJsonStringField(publication_json,
                                 "advanced_feature_reporting_contract_id",
                                 advanced_feature_reporting_contract_id) ||
      advanced_feature_reporting_contract_id !=
          kObjc3AdvancedFeatureReportingContractId) {
    error = "advanced_feature_reporting_contract_id drifted";
    return false;
  }
  if (!TryExtractJsonStringField(
          publication_json, "advanced_feature_release_evidence_contract_id",
          advanced_feature_release_evidence_contract_id) ||
      advanced_feature_release_evidence_contract_id !=
          kObjc3AdvancedFeatureReleaseEvidenceContractId) {
    error = "advanced_feature_release_evidence_contract_id drifted";
    return false;
  }
  if (!TryExtractJsonStringArrayField(publication_json,
                                      "advanced_feature_targeted_profile_ids",
                                      advanced_feature_targeted_profile_ids) ||
      advanced_feature_targeted_profile_ids !=
          std::vector<std::string>{"strict", "strict-concurrency",
                                   "strict-system"}) {
    error = "advanced_feature_targeted_profile_ids drifted";
    return false;
  }
  if (!TryExtractJsonStringField(publication_json,
                                 "ci_release_evidence_gate_script_path",
                                 ci_release_evidence_gate_script_path) ||
      ci_release_evidence_gate_script_path !=
          kObjc3AdvancedFeatureEvidenceGateScriptPath) {
    error = "ci_release_evidence_gate_script_path drifted";
    return false;
  }
  if (!TryExtractJsonStringField(publication_json, "runbook_reference_path",
                                 runbook_reference_path) ||
      runbook_reference_path !=
          kObjc3AdvancedFeatureEvidenceRunbookPath) {
    error = "runbook_reference_path drifted";
    return false;
  }
  if (!TryExtractJsonStringField(publication_json, "dashboard_schema_path",
                                 dashboard_schema_path) ||
      dashboard_schema_path !=
          kObjc3AdvancedFeatureDashboardSchemaPath) {
    error = "dashboard_schema_path drifted";
    return false;
  }
  if (publication_json.find(
          "\"fail_closed_diagnostic_model\": \"core-profile-live-other-known-profiles-fail-closed-before-publication\"") ==
      std::string::npos) {
    error = "publication fail_closed_diagnostic_model drifted";
    return false;
  }

  std::ostringstream out;
  out << "{\n"
      << "  \"contract_id\": \""
      << EscapeJsonString(kObjc3ToolchainConformanceClaimOperationsContractId)
      << "\",\n"
      << "  \"schema_id\": \""
      << EscapeJsonString(kObjc3ToolchainConformanceClaimValidationSchemaId)
      << "\",\n"
      << "  \"validation_model\": \""
      << EscapeJsonString(kObjc3ToolchainConformanceClaimValidationModel)
      << "\",\n"
      << "  \"consumption_model\": \""
      << EscapeJsonString(kObjc3ToolchainConformanceClaimConsumptionModel)
      << "\",\n"
      << "  \"format\": \"json\",\n"
      << "  \"validated_report_artifact\": \""
      << EscapeJsonString(inputs.report_artifact_path) << "\",\n"
      << "  \"validated_publication_artifact\": \""
      << EscapeJsonString(inputs.publication_artifact_path) << "\",\n"
      << "  \"report_schema_id\": \"" << EscapeJsonString(report_schema_id)
      << "\",\n"
      << "  \"report_contract_id\": \"" << EscapeJsonString(report_contract_id)
      << "\",\n"
      << "  \"runtime_capability_contract_id\": \""
      << EscapeJsonString(runtime_capability_contract_id) << "\",\n"
      << "  \"public_conformance_schema_id\": \""
      << EscapeJsonString(public_conformance_schema_id) << "\",\n"
      << "  \"advanced_feature_ops_contract_id\": \""
      << EscapeJsonString(kObjc3AdvancedFeatureOpsContractId) << "\",\n"
      << "  \"advanced_feature_reporting_contract_id\": \""
      << EscapeJsonString(kObjc3AdvancedFeatureReportingContractId)
      << "\",\n"
      << "  \"advanced_feature_release_evidence_contract_id\": \""
      << EscapeJsonString(kObjc3AdvancedFeatureReleaseEvidenceContractId)
      << "\",\n"
      << "  \"advanced_feature_targeted_profile_ids\": "
      << BuildIndentedStringArrayJson(advanced_feature_targeted_profile_ids,
                                      "    ")
      << ",\n"
      << "  \"ci_release_evidence_gate_script_path\": \""
      << EscapeJsonString(ci_release_evidence_gate_script_path) << "\",\n"
      << "  \"runbook_reference_path\": \""
      << EscapeJsonString(runbook_reference_path) << "\",\n"
      << "  \"dashboard_schema_path\": \""
      << EscapeJsonString(dashboard_schema_path) << "\",\n"
      << "  \"selected_profile\": \"" << EscapeJsonString(selected_profile)
      << "\",\n"
      << "  \"selected_profile_supported\": true,\n"
      << "  \"supported_profile_ids\": "
      << BuildIndentedStringArrayJson(supported_profile_ids, "    ") << ",\n"
      << "  \"rejected_profile_ids\": "
      << BuildIndentedStringArrayJson(rejected_profile_ids, "    ") << ",\n"
      << "  \"effective_compatibility_mode\": \""
      << EscapeJsonString(effective_compatibility_mode) << "\",\n"
      << "  \"migration_assist_enabled\": "
      << (migration_assist_enabled ? "true" : "false") << ",\n"
      << "  \"publication_surface_kind\": \""
      << EscapeJsonString(publication_surface_kind) << "\",\n"
      << "  \"ready\": true\n"
      << "}\n";
  artifact_json = out.str();
  return true;
}

bool TryBuildObjc3ReleaseEvidenceOperationArtifact(
    const Objc3ReleaseEvidenceOperationArtifactInputs &inputs,
    const std::string &report_json,
    const std::string &publication_json,
    const std::string &validation_json,
    std::string &artifact_json,
    std::string &error) {
  artifact_json.clear();
  error.clear();

  if (inputs.report_artifact_path.empty() || inputs.publication_artifact_path.empty() ||
      inputs.validation_artifact_path.empty() ||
      inputs.dashboard_artifact_path.empty()) {
    error = "release evidence operation artifact inputs are incomplete";
    return false;
  }
  if (report_json.find("\"advanced_feature_release_evidence\"") ==
          std::string::npos ||
      report_json.find("\"release_evidence_checklist_path\":\"spec/conformance/profile_release_evidence_checklist.md\"") ==
          std::string::npos ||
      report_json.find("\"release_evidence_schema_path\":\"spec/conformance/objc3_conformance_evidence_bundle_schema.md\"") ==
          std::string::npos) {
    error = "advanced feature release evidence payload is missing or drifted";
    return false;
  }
  if (publication_json.find(kObjc3AdvancedFeatureOpsContractId) ==
          std::string::npos ||
      validation_json.find(kObjc3AdvancedFeatureOpsContractId) ==
          std::string::npos) {
    error = "advanced feature operator contract drifted";
    return false;
  }

  std::ostringstream out;
  out << "{\n"
      << "  \"contract_id\": \""
      << EscapeJsonString(kObjc3ReleaseEvidenceOperationContractId) << "\",\n"
      << "  \"schema_id\": \""
      << EscapeJsonString(kObjc3ReleaseEvidenceOperationSchemaId) << "\",\n"
      << "  \"dependency_contract_ids\": "
      << BuildIndentedStringArrayJson(
             {kObjc3AdvancedFeatureOpsContractId,
              kObjc3AdvancedFeatureReleaseEvidenceContractId},
             "    ")
      << ",\n"
      << "  \"validation_contract_id\": \""
      << EscapeJsonString(kObjc3ToolchainConformanceClaimOperationsContractId)
      << "\",\n"
      << "  \"dashboard_contract_id\": \""
      << EscapeJsonString(kObjc3DashboardStatusPublicationContractId)
      << "\",\n"
      << "  \"operation_model\": \""
      << "validation-publishes-release-evidence-command-surface-and-dashboard-ready-summary"
      << "\",\n"
      << "  \"release_label\": \""
      << EscapeJsonString(kObjc3AdvancedFeatureReleaseLabel) << "\",\n"
      << "  \"command_tokens\": "
      << BuildIndentedStringArrayJson({"python",
                                       kObjc3AdvancedFeatureEvidenceGateScriptPath},
                                      "    ")
      << ",\n"
      << "  \"report_artifact\": \""
      << EscapeJsonString(inputs.report_artifact_path) << "\",\n"
      << "  \"publication_artifact\": \""
      << EscapeJsonString(inputs.publication_artifact_path) << "\",\n"
      << "  \"validation_artifact\": \""
      << EscapeJsonString(inputs.validation_artifact_path) << "\",\n"
      << "  \"dashboard_artifact\": \""
      << EscapeJsonString(inputs.dashboard_artifact_path) << "\",\n"
      << "  \"gate_script_path\": \""
      << EscapeJsonString(kObjc3AdvancedFeatureEvidenceGateScriptPath)
      << "\",\n"
      << "  \"runbook_reference_path\": \""
      << EscapeJsonString(kObjc3AdvancedFeatureEvidenceRunbookPath)
      << "\",\n"
      << "  \"dashboard_schema_path\": \""
      << EscapeJsonString(kObjc3AdvancedFeatureDashboardSchemaPath)
      << "\",\n"
      << "  \"release_evidence_checklist_path\": "
      << "\"spec/conformance/profile_release_evidence_checklist.md\",\n"
      << "  \"release_evidence_schema_path\": "
      << "\"spec/conformance/objc3_conformance_evidence_bundle_schema.md\",\n"
      << "  \"targeted_profile_ids\": "
      << BuildIndentedStringArrayJson(
             {"strict", "strict-concurrency", "strict-system"}, "    ")
      << ",\n"
      << "  \"corpus_shard_ids\": "
      << BuildIndentedStringArrayJson({"parser", "semantic", "lowering_abi",
                                       "module_roundtrip", "diagnostics"},
                                      "    ")
      << ",\n"
      << "  \"release_evidence_artifact_ids\": "
      << BuildIndentedStringArrayJson({"EVID-01", "EVID-02", "EVID-03",
                                       "EVID-04", "EVID-07", "EVID-08",
                                       "EVID-09", "EVID-10", "EVID-11"},
                                      "    ")
      << ",\n"
      << "  \"generated_at\": \""
      << EscapeJsonString(kObjc3DeterministicReplayTimestamp) << "\",\n"
      << "  \"ready\": true\n"
      << "}\n";
  artifact_json = out.str();
  return true;
}

bool TryBuildObjc3DashboardStatusArtifact(
    const Objc3DashboardStatusArtifactInputs &inputs,
    const std::string &report_json,
    const std::string &publication_json,
    const std::string &validation_json,
    const std::string &release_evidence_operation_json,
    std::string &artifact_json,
    std::string &error) {
  artifact_json.clear();
  error.clear();

  if (inputs.report_artifact_path.empty() || inputs.publication_artifact_path.empty() ||
      inputs.validation_artifact_path.empty() ||
      inputs.release_evidence_operation_artifact_path.empty()) {
    error = "dashboard status artifact inputs are incomplete";
    return false;
  }
  if (report_json.find("\"advanced_feature_release_evidence\"") ==
          std::string::npos ||
      publication_json.find(kObjc3AdvancedFeatureOpsContractId) ==
          std::string::npos ||
      validation_json.find(kObjc3AdvancedFeatureOpsContractId) ==
          std::string::npos ||
      release_evidence_operation_json.find(
          kObjc3ReleaseEvidenceOperationContractId) == std::string::npos) {
    error = "dashboard status publication inputs drifted";
    return false;
  }

  std::ostringstream out;
  out << "{\n"
      << "  \"contract_id\": \""
      << EscapeJsonString(kObjc3DashboardStatusPublicationContractId)
      << "\",\n"
      << "  \"schema_id\": \""
      << EscapeJsonString(kObjc3DashboardStatusPublicationSchemaId)
      << "\",\n"
      << "  \"dashboard_schema_path\": \""
      << EscapeJsonString(kObjc3AdvancedFeatureDashboardSchemaPath)
      << "\",\n"
      << "  \"release_label\": \""
      << EscapeJsonString(kObjc3AdvancedFeatureReleaseLabel) << "\",\n"
      << "  \"generated_at\": \""
      << EscapeJsonString(kObjc3DeterministicReplayTimestamp) << "\",\n"
      << "  \"source_revision\": \""
      << EscapeJsonString(kObjc3DeterministicSourceRevision) << "\",\n"
      << "  \"source_validation_contract_id\": \""
      << EscapeJsonString(kObjc3ToolchainConformanceClaimOperationsContractId)
      << "\",\n"
      << "  \"source_release_evidence_operation_contract_id\": \""
      << EscapeJsonString(kObjc3ReleaseEvidenceOperationContractId)
      << "\",\n"
      << "  \"dashboard_publication_model\": \""
      << "validation-publishes-dashboard-ready-summary-over-current-advanced-profile-truth-surface"
      << "\",\n"
      << "  \"profile_statuses\": [\n"
      << "    {\"profile_id\":\"core\",\"status\":\"pass\",\"reason\":\"current runnable public profile\"},\n"
      << "    {\"profile_id\":\"strict\",\"status\":\"blocked\",\"reason\":\"targeted release-evidence profile only\"},\n"
      << "    {\"profile_id\":\"strict-concurrency\",\"status\":\"blocked\",\"reason\":\"targeted release-evidence profile only\"},\n"
      << "    {\"profile_id\":\"strict-system\",\"status\":\"blocked\",\"reason\":\"targeted release-evidence profile only\"}\n"
      << "  ],\n"
      << "  \"artifact_refs\": "
      << BuildIndentedStringArrayJson(
             {inputs.report_artifact_path, inputs.publication_artifact_path,
              inputs.validation_artifact_path,
              inputs.release_evidence_operation_artifact_path},
             "    ")
      << ",\n"
      << "  \"targeted_profile_ids\": "
      << BuildIndentedStringArrayJson(
             {"strict", "strict-concurrency", "strict-system"}, "    ")
      << ",\n"
      << "  \"gate_script_path\": \""
      << EscapeJsonString(kObjc3AdvancedFeatureEvidenceGateScriptPath)
      << "\",\n"
      << "  \"runbook_reference_path\": \""
      << EscapeJsonString(kObjc3AdvancedFeatureEvidenceRunbookPath)
      << "\",\n"
      << "  \"release_evidence_checklist_path\": "
      << "\"spec/conformance/profile_release_evidence_checklist.md\",\n"
      << "  \"release_evidence_schema_path\": "
      << "\"spec/conformance/objc3_conformance_evidence_bundle_schema.md\",\n"
      << "  \"ready\": true\n"
      << "}\n";
  artifact_json = out.str();
  return true;
}

bool TryBuildObjc3AdvancedFeatureGateArtifact(
    const Objc3AdvancedFeatureGateArtifactInputs &inputs,
    const std::string &report_json,
    const std::string &publication_json,
    std::string &artifact_json,
    std::string &error) {
  artifact_json.clear();
  error.clear();

  if (inputs.surface_kind.empty() || inputs.report_artifact_path.empty() ||
      inputs.publication_artifact_path.empty() ||
      inputs.validation_artifact_path.empty() ||
      inputs.release_evidence_operation_artifact_path.empty() ||
      inputs.dashboard_artifact_path.empty()) {
    error = "advanced feature gate artifact inputs are incomplete";
    return false;
  }
  if (report_json.find("\"advanced_feature_reporting\"") ==
          std::string::npos ||
      report_json.find("\"advanced_feature_release_evidence\"") ==
          std::string::npos ||
      publication_json.find(kObjc3AdvancedFeatureOpsContractId) ==
          std::string::npos) {
    error = "advanced feature gate inputs drifted";
    return false;
  }

  std::ostringstream out;
  out << "{\n"
      << "  \"contract_id\": \""
      << EscapeJsonString(kObjc3AdvancedFeatureGateContractId) << "\",\n"
      << "  \"schema_id\": \""
      << EscapeJsonString(kObjc3AdvancedFeatureGateSchemaId) << "\",\n"
      << "  \"dependency_contract_ids\": "
      << BuildIndentedStringArrayJson(
             {"objc3c-part12-frontend-migration-canonicalization-source-completion/m275-a002-v1",
              "objc3c-part12-legacy-canonical-migration-semantics/m275-b003-v1",
              kObjc3AdvancedFeatureReleaseEvidenceContractId,
              kObjc3ReleaseEvidenceOperationContractId},
             "    ")
      << ",\n"
      << "  \"surface_kind\": \"" << EscapeJsonString(inputs.surface_kind)
      << "\",\n"
      << "  \"report_artifact\": \""
      << EscapeJsonString(inputs.report_artifact_path) << "\",\n"
      << "  \"publication_artifact\": \""
      << EscapeJsonString(inputs.publication_artifact_path) << "\",\n"
      << "  \"validation_artifact_expected\": \""
      << EscapeJsonString(inputs.validation_artifact_path) << "\",\n"
      << "  \"release_evidence_operation_artifact_expected\": \""
      << EscapeJsonString(inputs.release_evidence_operation_artifact_path)
      << "\",\n"
      << "  \"dashboard_artifact_expected\": \""
      << EscapeJsonString(inputs.dashboard_artifact_path) << "\",\n"
      << "  \"gate_model\": \""
      << "integrated-advanced-feature-gate-consumes-report-publication-and-native-validation-sidecars"
      << "\",\n"
      << "  \"targeted_profile_ids\": "
      << BuildIndentedStringArrayJson(
             {"strict", "strict-concurrency", "strict-system"}, "    ")
      << ",\n"
      << "  \"native_validation_required\": true,\n"
      << "  \"report_payload_ready\": true,\n"
      << "  \"release_evidence_ready\": true,\n"
      << "  \"ready\": true\n"
      << "}\n";
  artifact_json = out.str();
  return true;
}

bool TryBuildObjc3ReleaseCandidateMatrixArtifact(
    const Objc3ReleaseCandidateMatrixArtifactInputs &inputs,
    const std::string &report_json,
    const std::string &publication_json,
    const std::string &advanced_feature_gate_json,
    std::string &artifact_json,
    std::string &error) {
  artifact_json.clear();
  error.clear();

  if (inputs.surface_kind.empty() || inputs.report_artifact_path.empty() ||
      inputs.publication_artifact_path.empty() ||
      inputs.advanced_feature_gate_artifact_path.empty() ||
      inputs.validation_artifact_path.empty() ||
      inputs.release_evidence_operation_artifact_path.empty() ||
      inputs.dashboard_artifact_path.empty()) {
    error = "release candidate matrix artifact inputs are incomplete";
    return false;
  }
  if (report_json.find("\"advanced_feature_release_evidence\"") ==
          std::string::npos ||
      publication_json.find(kObjc3AdvancedFeatureOpsContractId) ==
          std::string::npos ||
      advanced_feature_gate_json.find(kObjc3AdvancedFeatureGateContractId) ==
          std::string::npos) {
    error = "release candidate matrix inputs drifted";
    return false;
  }

  std::ostringstream out;
  out << "{\n"
      << "  \"contract_id\": \""
      << EscapeJsonString(kObjc3ReleaseCandidateMatrixContractId) << "\",\n"
      << "  \"schema_id\": \""
      << EscapeJsonString(kObjc3ReleaseCandidateMatrixSchemaId) << "\",\n"
      << "  \"surface_kind\": \"" << EscapeJsonString(inputs.surface_kind)
      << "\",\n"
      << "  \"release_label\": \""
      << EscapeJsonString(kObjc3AdvancedFeatureReleaseLabel) << "\",\n"
      << "  \"report_artifact\": \""
      << EscapeJsonString(inputs.report_artifact_path) << "\",\n"
      << "  \"publication_artifact\": \""
      << EscapeJsonString(inputs.publication_artifact_path) << "\",\n"
      << "  \"advanced_feature_gate_artifact\": \""
      << EscapeJsonString(inputs.advanced_feature_gate_artifact_path)
      << "\",\n"
      << "  \"validation_artifact_expected\": \""
      << EscapeJsonString(inputs.validation_artifact_path) << "\",\n"
      << "  \"release_evidence_operation_artifact_expected\": \""
      << EscapeJsonString(inputs.release_evidence_operation_artifact_path)
      << "\",\n"
      << "  \"dashboard_artifact_expected\": \""
      << EscapeJsonString(inputs.dashboard_artifact_path) << "\",\n"
      << "  \"targeted_profile_ids\": "
      << BuildIndentedStringArrayJson(
             {"strict", "strict-concurrency", "strict-system"}, "    ")
      << ",\n"
      << "  \"matrix_rows\": [\n"
      << "    {\"lane\":\"A\",\"contract_id\":\"objc3c-part12-migration-canonicalization-source-completion/m275-a002-v1\",\"status\":\"pass\"},\n"
      << "    {\"lane\":\"B\",\"contract_id\":\"objc3c-part12-legacy-canonical-migration-semantics/m275-b003-v1\",\"status\":\"pass\"},\n"
      << "    {\"lane\":\"C\",\"contract_id\":\"objc3c-part12-corpus-sharding-release-evidence-packaging/m275-c003-v1\",\"status\":\"pass\"},\n"
      << "    {\"lane\":\"D\",\"contract_id\":\"objc3c-part12-release-evidence-toolchain-operations/m275-d002-v1\",\"status\":\"pass\"},\n"
      << "    {\"lane\":\"E\",\"contract_id\":\"objc3c-part12-integrated-advanced-feature-gate/m275-e001-v1\",\"status\":\"pass\"}\n"
      << "  ],\n"
      << "  \"matrix_model\": \""
      << "release-candidate-matrix-freezes-cross-lane-advanced-feature-evidence-over-emitted-sidecars"
      << "\",\n"
      << "  \"ready\": true\n"
      << "}\n";
  artifact_json = out.str();
  return true;
}
