#pragma once

#include <string>
#include <vector>

namespace objc3::clang_foundation {

inline constexpr unsigned kInterfaceSchemaMajor = 1;
inline constexpr unsigned kInterfaceSchemaMinor = 0;
inline constexpr const char *kInterfaceSchemaId = "objc3c.semantic-interface.v1";

// Identity fields that can change the meaning or link contract of an import.
// File paths and timestamps intentionally do not participate.
struct InterfaceModuleIdentity {
  std::string canonical_name;
  std::string semantic_fingerprint;
  std::string target_triple;
  std::string runtime_abi;
  std::string extension_abi;
  std::string language_revision;
  std::string language_profile;
  std::string package_identity;
};

struct InterfaceEffectIdentity {
  bool is_async = false;
  bool is_throwing = false;
  std::string throws_kind;
  std::string error_type_identity;
  std::string actor_isolation_identity;
  std::string executor_affinity_identity;
  std::string ownership_identity;
};

struct InterfaceDeclarationIdentity {
  std::string stable_id;       // Clang USR, normalized for file-scope C typedefs.
  std::string kind;
  std::string qualified_name;
  std::string canonical_type_identity;
  InterfaceEffectIdentity effects;
};

// Compare the complete declaration contract, including every modeled effect.
[[nodiscard]] bool SameInterfaceDeclarationIdentity(
    const InterfaceDeclarationIdentity &left,
    const InterfaceDeclarationIdentity &right);

struct InterfaceDependency {
  InterfaceModuleIdentity module;
  bool reexported = false;
  bool public_import = false;
};

// Canonical source and exported identities needed to parse a dependent
// interface through Clang's normal frontend. The path is a stable virtual
// include name; it is never a module identity or a filesystem authority.
struct InterfaceSourceArtifact {
  InterfaceModuleIdentity module;
  std::string virtual_include_path;
  std::string source_bytes;
  std::vector<InterfaceDeclarationIdentity> declarations;
  std::vector<InterfaceDependency> dependencies;
};


// `required_semantics` names versioned semantics whose absence changes meaning.
// Optional producer hints belong elsewhere and are ignored by importers.
struct SemanticInterfaceSidecar {
  unsigned schema_major = kInterfaceSchemaMajor;
  unsigned schema_minor = kInterfaceSchemaMinor;
  InterfaceModuleIdentity module;
  std::string canonical_source_sha256;
  std::vector<std::string> required_semantics;
  std::vector<InterfaceDeclarationIdentity> declarations;
  std::vector<InterfaceDependency> dependencies;
};

struct InterfaceContractDiagnostic {
  std::string code;
  std::string message;
  std::string subject;
};

struct InterfaceEmissionArtifact {
  bool success = false;
  std::string source_bytes;
  std::string sidecar_json;
  InterfaceModuleIdentity identity;
  std::vector<InterfaceContractDiagnostic> diagnostics;
};

struct InterfaceContractValidation {
  std::vector<InterfaceContractDiagnostic> diagnostics;
  [[nodiscard]] bool ok() const { return diagnostics.empty(); }
};

// Stable fingerprint input sorts semantically unordered collections and omits
// source formatting and its own fingerprint. The sidecar codec owns JSON.
[[nodiscard]] std::string BuildSemanticFingerprintInput(
    const SemanticInterfaceSidecar &sidecar);

[[nodiscard]] InterfaceContractValidation ValidateSemanticInterface(
    const SemanticInterfaceSidecar &sidecar,
    const InterfaceModuleIdentity &expected,
    const std::string &canonical_source_sha256,
    const std::vector<InterfaceDeclarationIdentity> &parsed_clang_declarations,
    const std::vector<std::string> &supported_required_semantics);

}  // namespace objc3::clang_foundation
