#include "clang_foundation/interface_artifact_io.h"

#include <array>
#include <chrono>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <functional>
#include <random>
#include <string>
#include <system_error>

#if defined(_WIN32)
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif

#include "clang_foundation/interface_codec.h"

namespace objc3::clang_foundation {
namespace {

namespace fs = std::filesystem;

bool HasReparseAncestor(const fs::path &directory) {
  const auto normalized = directory.lexically_normal();
#if defined(_WIN32)
  fs::path current = normalized.root_path();
  for (const auto &component : normalized.relative_path()) {
    current /= component;
    const DWORD attributes = GetFileAttributesW(current.c_str());
    if (attributes == INVALID_FILE_ATTRIBUTES ||
        (attributes & FILE_ATTRIBUTE_REPARSE_POINT) != 0)
      return true;
  }
  return false;
#else
  fs::path current;
  for (const auto &component : normalized) {
    current /= component;
    std::error_code error;
    const auto status = fs::symlink_status(current, error);
    if (error || fs::is_symlink(status)) return true;
  }
  return false;
#endif
}

void AddDiagnostic(InterfaceArtifactWriteResult &result, const char *code,
                   std::string message, std::string subject = {}) {
  result.diagnostics.push_back(
      {code, std::move(message), std::move(subject)});
}

bool ReadFile(const fs::path &path, std::string &bytes,
              std::error_code &error) {
  std::ifstream input(path, std::ios::binary);
  if (!input) {
    error = std::make_error_code(std::errc::io_error);
    return false;
  }
  bytes.assign(std::istreambuf_iterator<char>(input),
               std::istreambuf_iterator<char>());
  if (input.bad()) {
    error = std::make_error_code(std::errc::io_error);
    return false;
  }
  return true;
}

bool WriteFile(const fs::path &path, const std::string &bytes,
               std::error_code &error) {
  std::ofstream output(path, std::ios::binary | std::ios::out | std::ios::trunc);
  if (!output) {
    error = std::make_error_code(std::errc::io_error);
    return false;
  }
  output.write(bytes.data(), static_cast<std::streamsize>(bytes.size()));
  output.flush();
  if (!output) {
    error = std::make_error_code(std::errc::io_error);
    return false;
  }
  output.close();
  if (!output) {
    error = std::make_error_code(std::errc::io_error);
    return false;
  }
  return true;
}

bool InspectDestination(const fs::path &path, bool &exists,
                       std::error_code &error) {
  const auto status = fs::symlink_status(path, error);
  if (error == std::errc::no_such_file_or_directory) {
    error.clear();
    exists = false;
    return true;
  }
  if (error) return false;
  exists = fs::exists(status);
  if (!exists) return true;
#if defined(_WIN32)
  const DWORD attributes = GetFileAttributesW(path.c_str());
  if (attributes == INVALID_FILE_ATTRIBUTES ||
      (attributes & FILE_ATTRIBUTE_REPARSE_POINT) != 0) {
    error = std::make_error_code(std::errc::operation_not_permitted);
    return false;
  }
#endif
  if (fs::is_symlink(status)) {
    error = std::make_error_code(std::errc::operation_not_permitted);
    return false;
  }
  if (!fs::is_regular_file(status)) {
    error = std::make_error_code(std::errc::invalid_argument);
    return false;
  }
  return true;
}

bool SameModuleIdentity(const InterfaceModuleIdentity &a,
                        const InterfaceModuleIdentity &b) {
  return a.canonical_name == b.canonical_name &&
         a.semantic_fingerprint == b.semantic_fingerprint &&
         a.target_triple == b.target_triple && a.runtime_abi == b.runtime_abi &&
         a.extension_abi == b.extension_abi &&
         a.language_revision == b.language_revision &&
         a.language_profile == b.language_profile &&
         a.package_identity == b.package_identity;
}

bool ValidateArtifact(const InterfaceEmissionArtifact &artifact,
                      InterfaceArtifactWriteResult &result) {
  if (!artifact.success) {
    AddDiagnostic(result, "O3IFIO100",
                  "cannot publish a failed interface emission artifact");
    return false;
  }
  if (artifact.source_bytes.empty() || artifact.sidecar_json.empty()) {
    AddDiagnostic(result, "O3IFIO101",
                  "interface source and sidecar bytes must both be present");
    return false;
  }

  auto decoded = DecodeSemanticInterfaceSidecar(
      artifact.sidecar_json, artifact.source_bytes, {});
  if (!decoded) {
    AddDiagnostic(result, "O3IFIO102",
                  "interface artifact sidecar is malformed or is not bound to its source: " +
                      llvm::toString(decoded.takeError()));
    return false;
  }
  if (!SameModuleIdentity(artifact.identity, decoded->module)) {
    AddDiagnostic(result, "O3IFIO103",
                  "interface artifact identity does not match its sidecar",
                  artifact.identity.canonical_name);
    return false;
  }
  const auto fingerprint = ComputeInterfaceSourceSha256(
      BuildSemanticFingerprintInput(*decoded));
  if (fingerprint != decoded->module.semantic_fingerprint) {
    AddDiagnostic(result, "O3IFIO104",
                  "interface sidecar semantic fingerprint is inconsistent",
                  decoded->module.canonical_name);
    return false;
  }
  return true;
}

fs::path MakeTransactionDirectory(const fs::path &parent,
                                  std::error_code &error) {
  std::random_device random;
  for (unsigned attempt = 0; attempt != 32; ++attempt) {
    const auto nonce = (static_cast<std::uint64_t>(random()) << 32) ^ random();
    const auto ticks = static_cast<std::uint64_t>(
        std::chrono::steady_clock::now().time_since_epoch().count());
    const auto name = ".objc3i-txn-" + std::to_string(ticks) + "-" +
                      std::to_string(nonce) + "-" + std::to_string(attempt);
    auto path = parent / name;
    if (fs::create_directory(path, error)) return path;
    if (error == std::errc::file_exists) {
      error.clear();
      continue;
    }
    return {};
  }
  error = std::make_error_code(std::errc::file_exists);
  return {};
}

bool RemoveKnownFile(const fs::path &path, std::error_code &error) {
  const auto status = fs::symlink_status(path, error);
  if (error == std::errc::no_such_file_or_directory) {
    error.clear();
    return true;
  }
  if (error) return false;
  if (fs::is_symlink(status) || !fs::is_regular_file(status)) {
    error = std::make_error_code(std::errc::operation_not_permitted);
    return false;
  }
  fs::remove(path, error);
  return !error;
}

}  // namespace

static InterfaceArtifactWriteResult WriteSemanticInterfaceArtifactImpl(
    const InterfaceEmissionArtifact &artifact,
    const std::string &explicit_source_path,
    int fail_before_pair_publish_index,
    const std::function<bool(std::string &)> &before_pair_publish = {});
static InterfaceArtifactWriteResult WriteSemanticInterfaceArtifactWithObjectImpl(
    const InterfaceEmissionArtifact &artifact,
    const std::string &explicit_source_path,
    const std::string &staged_object_path,
    const std::string &final_object_path,
    int fail_before_pair_publish_index);

InterfaceArtifactWriteResult WriteSemanticInterfaceArtifact(
    const InterfaceEmissionArtifact &artifact,
    const std::string &explicit_source_path) {
  return WriteSemanticInterfaceArtifactImpl(artifact, explicit_source_path, -1,
                                            {});
}

static InterfaceArtifactWriteResult WriteSemanticInterfaceArtifactImpl(
    const InterfaceEmissionArtifact &artifact,
    const std::string &explicit_source_path,
    int fail_before_pair_publish_index,
    const std::function<bool(std::string &)> &before_pair_publish) {
  InterfaceArtifactWriteResult result;
  if (!ValidateArtifact(artifact, result)) return result;
  if (explicit_source_path.empty()) {
    AddDiagnostic(result, "O3IFIO105",
                  "an explicit .objc3i source path is required");
    return result;
  }

  std::error_code error;
  fs::path requested_path = fs::absolute(fs::path(explicit_source_path), error);
  if (error) {
    AddDiagnostic(result, "O3IFIO106",
                  "cannot resolve the explicit interface output path: " +
                      error.message(),
                  explicit_source_path);
    return result;
  }
  if (requested_path.extension() != ".objc3i") {
    AddDiagnostic(result, "O3IFIO107",
                  "interface source output path must end in .objc3i",
                  requested_path.string());
    return result;
  }
  if (HasReparseAncestor(requested_path.parent_path())) {
    AddDiagnostic(result, "O3IFIO108",
                  "interface output path contains an inaccessible or reparse-point ancestor",
                  requested_path.parent_path().string());
    return result;
  }
  fs::path parent = fs::canonical(requested_path.parent_path(), error);
  if (error || !fs::is_directory(parent, error) || error) {
    AddDiagnostic(result, "O3IFIO108",
                  "interface output directory must already exist and be accessible",
                  requested_path.parent_path().string());
    return result;
  }
  const fs::path source_path = parent / requested_path.filename();
  const fs::path sidecar_path(source_path.string() + ".json");
  result.source_path = source_path.string();
  result.sidecar_path = sidecar_path.string();

  bool source_existed = false;
  bool sidecar_existed = false;
  if (!InspectDestination(source_path, source_existed, error) ||
      !InspectDestination(sidecar_path, sidecar_existed, error)) {
    AddDiagnostic(result, "O3IFIO109",
                  "existing interface outputs must be regular non-symlink files: " +
                      error.message(),
                  error ? source_path.string() : sidecar_path.string());
    return result;
  }

  const fs::path transaction = MakeTransactionDirectory(parent, error);
  if (error || transaction.empty()) {
    AddDiagnostic(result, "O3IFIO110",
                  "cannot create a private sibling staging directory: " +
                      error.message(),
                  parent.string());
    return result;
  }
  const fs::path staged_source = transaction / "source.stage";
  const fs::path staged_sidecar = transaction / "sidecar.stage";
  const fs::path backup_source = transaction / "source.previous";
  const fs::path backup_sidecar = transaction / "sidecar.previous";

  auto keep_transaction = [&]() {
    // An incomplete rollback may leave the previous pair here for recovery.
    // The path is returned in the diagnostic and is never recursively removed.
    return transaction.string();
  };
  auto cleanup_transaction = [&]() {
    std::error_code cleanup_error;
    const std::array<fs::path, 4> known_files = {
        staged_source, staged_sidecar, backup_source, backup_sidecar};
    for (const auto &path : known_files) {
      if (!RemoveKnownFile(path, cleanup_error)) return false;
    }
    fs::remove(transaction, cleanup_error);
    return !cleanup_error;
  };

  if (!WriteFile(staged_source, artifact.source_bytes, error) ||
      !WriteFile(staged_sidecar, artifact.sidecar_json, error)) {
    AddDiagnostic(result, "O3IFIO111",
                  "failed to stage interface pair: " + error.message(),
                  keep_transaction());
    cleanup_transaction();
    return result;
  }
  std::string staged_source_bytes;
  std::string staged_sidecar_bytes;
  if (!ReadFile(staged_source, staged_source_bytes, error) ||
      !ReadFile(staged_sidecar, staged_sidecar_bytes, error) ||
      staged_source_bytes != artifact.source_bytes ||
      staged_sidecar_bytes != artifact.sidecar_json) {
    AddDiagnostic(result, "O3IFIO112",
                  "staged interface pair did not verify byte-for-byte: " +
                      error.message(),
                  keep_transaction());
    cleanup_transaction();
    return result;
  }
  auto staged_decoded = DecodeSemanticInterfaceSidecar(
      staged_sidecar_bytes, staged_source_bytes, {});
  if (!staged_decoded) {
    AddDiagnostic(result, "O3IFIO113",
                  "staged interface pair failed source-binding validation: " +
                      llvm::toString(staged_decoded.takeError()),
                  keep_transaction());
    cleanup_transaction();
    return result;
  }

  bool source_backed_up = false;
  bool sidecar_backed_up = false;
  bool source_published = false;
  bool sidecar_published = false;
  auto rollback = [&]() {
    bool restored = true;
    std::error_code rollback_error;
    auto remove_published = [&](const fs::path &path,
                                const std::string &expected) {
      std::string current;
      if (!ReadFile(path, current, rollback_error) || current != expected) {
        restored = false;
        return;
      }
      if (!RemoveKnownFile(path, rollback_error)) restored = false;
    };
    if (sidecar_published)
      remove_published(sidecar_path, artifact.sidecar_json);
    if (source_published) remove_published(source_path, artifact.source_bytes);
    auto restore = [&](const fs::path &backup, const fs::path &destination,
                       bool backed_up) {
      if (!backed_up) return;
      std::error_code status_error;
      const auto status = fs::symlink_status(destination, status_error);
      if (!status_error && fs::exists(status)) {
        restored = false;
        return;
      }
      if (status_error != std::errc::no_such_file_or_directory && status_error) {
        restored = false;
        return;
      }
      status_error.clear();
      fs::rename(backup, destination, status_error);
      if (status_error) restored = false;
    };
    restore(backup_sidecar, sidecar_path, sidecar_backed_up);
    restore(backup_source, source_path, source_backed_up);
    return restored;
  };

  auto fail_publish = [&](const char *code, const fs::path &failed_path) {
    const bool restored = rollback();
    AddDiagnostic(result, code,
                  std::string("publishing the interface pair failed: ") +
                      error.message() +
                      (restored ? "; the previous pair was restored"
                                : "; rollback was incomplete; recoverable files are in the transaction directory"),
                  restored ? failed_path.string() : keep_transaction());
    if (restored) cleanup_transaction();
    return result;
  };

  if (source_existed) {
    fs::rename(source_path, backup_source, error);
    if (error) return fail_publish("O3IFIO114", source_path);
    source_backed_up = true;
  }
  if (sidecar_existed) {
    fs::rename(sidecar_path, backup_sidecar, error);
    if (error) return fail_publish("O3IFIO115", sidecar_path);
    sidecar_backed_up = true;
  }
  std::string callback_failure;
  if (before_pair_publish && !before_pair_publish(callback_failure)) {
    error = std::make_error_code(std::errc::io_error);
    auto failed = fail_publish("O3IFIO115", source_path);
    if (!callback_failure.empty() && !failed.diagnostics.empty())
      failed.diagnostics.back().message += ": " + callback_failure;
    return failed;
  }
  if (fail_before_pair_publish_index == 0) {
    error = std::make_error_code(std::errc::io_error);
    return fail_publish("O3IFIO116", source_path);
  }
  fs::rename(staged_source, source_path, error);
  if (error) return fail_publish("O3IFIO116", source_path);
  source_published = true;
  if (fail_before_pair_publish_index == 1) {
    error = std::make_error_code(std::errc::io_error);
    return fail_publish("O3IFIO117", sidecar_path);
  }
  fs::rename(staged_sidecar, sidecar_path, error);
  if (error) return fail_publish("O3IFIO117", sidecar_path);
  sidecar_published = true;

  // Confirm the visible pair remains byte-identical and bound before treating
  // the two individually atomic renames as one successful publication.
  std::string visible_source;
  std::string visible_sidecar;
  if (!ReadFile(source_path, visible_source, error) ||
      !ReadFile(sidecar_path, visible_sidecar, error) ||
      visible_source != artifact.source_bytes ||
      visible_sidecar != artifact.sidecar_json) {
    fail_publish("O3IFIO118", source_path);
    return result;
  }
  auto visible_decoded = DecodeSemanticInterfaceSidecar(
      visible_sidecar, visible_source, {});
  if (!visible_decoded) {
    error = std::make_error_code(std::errc::invalid_argument);
    fail_publish("O3IFIO119", sidecar_path);
    return result;
  }

  result.success = true;
  cleanup_transaction();
  return result;
}

InterfaceArtifactWriteResult WriteSemanticInterfaceArtifactWithObject(
    const InterfaceEmissionArtifact &artifact,
    const std::string &explicit_source_path,
    const std::string &staged_object_path,
    const std::string &final_object_path) {
  return WriteSemanticInterfaceArtifactWithObjectImpl(
      artifact, explicit_source_path, staged_object_path, final_object_path,
      -1);
}

static InterfaceArtifactWriteResult
WriteSemanticInterfaceArtifactWithObjectImpl(
    const InterfaceEmissionArtifact &artifact,
    const std::string &explicit_source_path,
    const std::string &staged_object_path,
    const std::string &final_object_path,
    int fail_before_pair_publish_index) {
  InterfaceArtifactWriteResult result;
  if (!ValidateArtifact(artifact, result)) return result;
  if (explicit_source_path.empty() || staged_object_path.empty() ||
      final_object_path.empty()) {
    AddDiagnostic(result, "O3IFIO120",
                  "interface source, staged object, and final object paths are required");
    return result;
  }

  std::error_code error;
  const fs::path requested_source =
      fs::absolute(fs::path(explicit_source_path), error).lexically_normal();
  if (error || requested_source.extension() != ".objc3i") {
    AddDiagnostic(result, "O3IFIO121",
                  "explicit interface path must resolve to a .objc3i source",
                  explicit_source_path);
    return result;
  }
  if (HasReparseAncestor(requested_source.parent_path())) {
    AddDiagnostic(result, "O3IFIO122",
                  "interface output path contains an inaccessible or reparse-point ancestor",
                  requested_source.parent_path().string());
    return result;
  }
  const fs::path interface_parent =
      fs::canonical(requested_source.parent_path(), error);
  if (error || !fs::is_directory(interface_parent, error) || error) {
    AddDiagnostic(result, "O3IFIO123",
                  "interface output directory must already exist and be accessible",
                  requested_source.parent_path().string());
    return result;
  }
  const fs::path source_destination = interface_parent / requested_source.filename();
  const fs::path sidecar_destination(source_destination.string() + ".json");
  result.source_path = source_destination.string();
  result.sidecar_path = sidecar_destination.string();

  const fs::path requested_object =
      fs::absolute(fs::path(final_object_path), error).lexically_normal();
  if (error) {
    AddDiagnostic(result, "O3IFIO124",
                  "cannot resolve final object output path: " + error.message(),
                  final_object_path);
    return result;
  }
  const fs::path requested_stage =
      fs::absolute(fs::path(staged_object_path), error).lexically_normal();
  if (error) {
    AddDiagnostic(result, "O3IFIO125",
                  "cannot resolve staged object path: " + error.message(),
                  staged_object_path);
    return result;
  }
  if (HasReparseAncestor(requested_object.parent_path()) ||
      HasReparseAncestor(requested_stage.parent_path())) {
    AddDiagnostic(result, "O3IFIO126",
                  "object output paths contain an inaccessible or reparse-point ancestor",
                  requested_object.parent_path().string());
    return result;
  }
  const fs::path object_parent = fs::canonical(requested_object.parent_path(), error);
  if (error || !fs::is_directory(object_parent, error) || error) {
    AddDiagnostic(result, "O3IFIO127",
                  "final object directory must already exist and be accessible",
                  requested_object.parent_path().string());
    return result;
  }
  const fs::path staged_parent = fs::canonical(requested_stage.parent_path(), error);
  if (error || staged_parent != object_parent) {
    AddDiagnostic(result, "O3IFIO128",
                  "compiler-staged object must be a sibling of the final object",
                  requested_stage.string());
    return result;
  }
  const fs::path object_destination = object_parent / requested_object.filename();
  const fs::path staged_object = object_parent / requested_stage.filename();
  if (object_destination == staged_object ||
      object_destination == source_destination ||
      object_destination == sidecar_destination ||
      staged_object == source_destination || staged_object == sidecar_destination) {
    AddDiagnostic(result, "O3IFIO129",
                  "object and interface outputs must use distinct file paths",
                  object_destination.string());
    return result;
  }
  bool staged_exists = false;
  bool object_existed = false;
  bool source_existed = false;
  bool sidecar_existed = false;
  if (!InspectDestination(staged_object, staged_exists, error) ||
      !staged_exists) {
    AddDiagnostic(result, "O3IFIO130",
                  "compiler-staged object must exist as a regular non-symlink file",
                  staged_object.string());
    return result;
  }
  if (!InspectDestination(object_destination, object_existed, error) ||
      !InspectDestination(source_destination, source_existed, error) ||
      !InspectDestination(sidecar_destination, sidecar_existed, error)) {
    AddDiagnostic(result, "O3IFIO131",
                  "existing object/interface outputs must be regular non-symlink files: " +
                      error.message(),
                  error ? object_destination.string() : sidecar_destination.string());
    return result;
  }
  (void)source_existed;
  (void)sidecar_existed;

  const fs::path transaction = MakeTransactionDirectory(object_parent, error);
  if (error || transaction.empty()) {
    AddDiagnostic(result, "O3IFIO132",
                  "cannot create a private object backup directory: " +
                      error.message(),
                  object_parent.string());
    return result;
  }
  const fs::path backup_object = transaction / "object.previous";
  bool object_backed_up = false;
  bool object_published = false;

  auto restore_object = [&]() {
    bool restored = true;
    std::error_code rollback_error;
    if (object_published) {
      bool destination_exists = false;
      if (!InspectDestination(object_destination, destination_exists,
                              rollback_error) ||
          !destination_exists) {
        restored = false;
      } else {
        fs::rename(object_destination, staged_object, rollback_error);
        if (rollback_error) restored = false;
      }
    }
    if (object_backed_up) {
      bool destination_exists = false;
      rollback_error.clear();
      if (!InspectDestination(object_destination, destination_exists,
                              rollback_error) ||
          destination_exists) {
        restored = false;
      } else {
        fs::rename(backup_object, object_destination, rollback_error);
        if (rollback_error) restored = false;
      }
    }
    return restored;
  };

  auto report_object_failure = [&](const char *code, std::string message) {
    const bool restored = restore_object();
    AddDiagnostic(result, code,
                  std::move(message) +
                      (restored ? "; object output rolled back and prior output restored when present"
                                : "; object rollback incomplete; backup is retained for recovery"),
                  restored ? object_destination.string() : transaction.string());
    if (restored) {
      std::error_code cleanup_error;
      if (object_backed_up) RemoveKnownFile(backup_object, cleanup_error);
      fs::remove(transaction, cleanup_error);
    }
  };

  auto publish_staged_object = [&](std::string &failure) {
    std::error_code object_error;
    if (object_existed) {
      fs::rename(object_destination, backup_object, object_error);
      if (object_error) {
        failure = "failed to preserve the previous object output: " +
                  object_error.message();
        return false;
      }
      object_backed_up = true;
    }
    fs::rename(staged_object, object_destination, object_error);
    if (object_error) {
      failure = "failed to publish the staged object: " +
                object_error.message();
      return false;
    }
    object_published = true;
    return true;
  };
  auto interface_result = WriteSemanticInterfaceArtifactImpl(
      artifact, explicit_source_path, fail_before_pair_publish_index,
      publish_staged_object);
  if (!interface_result.success) {
    result.diagnostics = std::move(interface_result.diagnostics);
    result.source_path = std::move(interface_result.source_path);
    result.sidecar_path = std::move(interface_result.sidecar_path);
    report_object_failure(
        "O3IFIO135",
        "interface pair transaction failed; the object staging step was rolled back");
    return result;
  }

  result.success = true;
  result.source_path = std::move(interface_result.source_path);
  result.sidecar_path = std::move(interface_result.sidecar_path);
  if (object_backed_up) RemoveKnownFile(backup_object, error);
  fs::remove(transaction, error);
  return result;
}

namespace testing {

InterfaceArtifactWriteResult
WriteSemanticInterfaceArtifactWithObjectFailingBeforePairPublish(
    const InterfaceEmissionArtifact &artifact,
    const std::string &explicit_source_path,
    const std::string &staged_object_path,
    const std::string &final_object_path,
    unsigned pair_publish_index) {
  return WriteSemanticInterfaceArtifactWithObjectImpl(
      artifact, explicit_source_path, staged_object_path, final_object_path,
      static_cast<int>(pair_publish_index));
}

}  // namespace testing

}  // namespace objc3::clang_foundation
