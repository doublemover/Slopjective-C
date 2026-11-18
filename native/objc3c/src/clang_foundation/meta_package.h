#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace objc3::clang_foundation {

enum class MetaEntryKind { Derive, Macro, PropertyBehavior };
enum class MetaMemberKind { Integer, Floating, Object, Block, Pointer, Record };

struct MetaInstruction {
  enum class Operation { Text, Properties, Ivars, Conditional, Error };
  Operation operation = Operation::Text;
  std::string text;
  std::string separator;
  std::string predicate;
  std::optional<MetaMemberKind> member_kind;
  std::optional<bool> writable;
  std::vector<MetaInstruction> body;
  std::vector<MetaInstruction> alternative;
};

struct MetaEntrypoint {
  std::string name;
  MetaEntryKind kind = MetaEntryKind::Derive;
  std::vector<std::string> required_capabilities;
  std::vector<std::string> protocols;
  std::vector<MetaInstruction> ivars;
  std::vector<MetaInstruction> declarations;
  std::vector<MetaInstruction> implementations;
  // Property behaviors must state their dispatch and executor requirements.
  std::string dispatch;
  std::string executor;
};

bool IsMetaExecutorContract(std::string_view value);

struct MetaPackage {
  std::string id;
  std::string version;
  std::string fingerprint;
  std::string manifest_path;
  std::vector<std::string> capabilities;
  std::vector<MetaEntrypoint> entrypoints;
};

struct MetaInputDigest {
  std::string path;
  std::string sha256;
};

struct MetaPackageSet {
  std::vector<MetaPackage> packages;
  std::vector<MetaInputDigest> inputs;
  std::string fingerprint;

  static std::optional<MetaPackageSet> Load(
      const std::vector<std::string> &explicit_packages,
      const std::string &lock_path, std::string &error);
  const MetaEntrypoint *Resolve(std::string_view name, MetaEntryKind kind,
                               const MetaPackage **owner,
                               std::string &error) const;
  bool HasPortableDerives() const;
  bool VerifyInputs(std::string &error) const;
};

// Canonical semantic JSON, excluding the declared fingerprint. Paths and JSON
// whitespace never join package identity; template source bytes do.
std::optional<std::string> ComputeMetaPackageFingerprintFromFile(
    const std::string &path,std::string &error);
std::optional<std::string> ComputeMetaPackageFingerprint(
    std::string_view document, std::string &error);

}  // namespace objc3::clang_foundation
