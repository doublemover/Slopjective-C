#pragma once

#include <string>
#include <vector>

namespace objc3::clang_foundation {

inline constexpr unsigned kInterfaceSchemaMajor = 2;
inline constexpr unsigned kInterfaceSchemaMinor = 2;
inline constexpr const char *kInterfaceSchemaId =
    "objc3c.semantic-interface.v2";

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
  // Position-tagged physical entry ABI, including nested callable types.
  // Logical async effects alone cannot distinguish a C frame from task<T>.
  std::string async_abi_identity;
};

inline constexpr const char *kAsyncEntryABISemantic =
    "objc3.async-entry-abi.v1";

[[nodiscard]] bool
IsValidAsyncABIIdentity(const InterfaceEffectIdentity &effects);

struct InterfaceDeclarationIdentity {
  std::string stable_id; // Clang USR, normalized for file-scope C typedefs.
  std::string kind;
  std::string qualified_name;
  std::string canonical_type_identity;
  InterfaceEffectIdentity effects;
  // Empty for public API. A named SPI group is a required semantic contract.
  std::string spi_group;
  std::string native_symbol_name;
  // Native declaration owning this row's export policy. Class members and
  // friend functions bind to their class; namespace descendants retain scope.
  std::string export_root;
};

inline bool IsCxxMemberDeclarationKind(const std::string &kind) {
  return kind == "CXXMethod" || kind == "CXXConstructor" ||
         kind == "CXXDestructor" || kind == "CXXConversion";
}
inline constexpr const char *kCxxClassLifecycleSemantic =
    "objc3.cxx-class-lifecycles.v1";
inline bool
HasCxxClassLifecycleIdentity(const InterfaceDeclarationIdentity &d) {
  return d.kind == "CXXConstructor" || d.kind == "CXXDestructor" ||
         d.kind == "CXXConversion" ||
         d.canonical_type_identity.find(";cxx-class-lifecycle:v1;") !=
             std::string::npos;
}

inline constexpr const char *kCxxClassStructureSemantic =
    "objc3.cxx-class-structure.v1";
inline bool
HasCxxClassStructureIdentity(const InterfaceDeclarationIdentity &d) {
  return d.canonical_type_identity.find(";cxx-class-structure:v1;") !=
         std::string::npos;
}

inline constexpr const char *kCxxClassDispatchSemantic =
    "objc3.cxx-class-dispatch.v1";
inline bool
HasCxxClassDispatchIdentity(const InterfaceDeclarationIdentity &d) {
  return d.canonical_type_identity.find(";cxx-class-dispatch:v1;") !=
         std::string::npos;
}

inline constexpr const char *kCxxNamespaceSemantic =
    "objc3.cxx-namespaces.v1";
inline bool HasCxxNamespaceIdentity(const InterfaceDeclarationIdentity &d) {
  return d.canonical_type_identity.find(";cxx-namespaces:v1;") !=
         std::string::npos;
}

inline constexpr const char *kCxxNestedTypeSemantic =
    "objc3.cxx-nested-types.v1";
inline bool HasCxxNestedTypeIdentity(const InterfaceDeclarationIdentity &d) {
  return d.canonical_type_identity.find(";cxx-nested-types:v1;") !=
         std::string::npos;
}

inline constexpr const char *kCxxUsingSemantic="objc3.cxx-using.v1";
inline bool HasCxxUsingIdentity(const InterfaceDeclarationIdentity &d) {
  return d.canonical_type_identity.find(";cxx-using:v1;")!=std::string::npos;
}
inline constexpr const char *kCxxDeclarationExpressionSemantic =
    "objc3.cxx-declaration-expressions.v1";
inline bool HasCxxDeclarationExpressionIdentity(const InterfaceDeclarationIdentity &d) {
  return d.canonical_type_identity.find(";cxx-declaration-expressions:v1;") !=
         std::string::npos;
}
inline constexpr const char *kCxxDefinitionSemantic = "objc3.cxx-definitions.v1";
inline bool HasCxxDefinitionIdentity(const InterfaceDeclarationIdentity &d) {
  return d.canonical_type_identity.find(";cxx-definitions:v1;") != std::string::npos;
}
inline constexpr const char *kCxxDefaultedComparisonSemantic =
    "objc3.cxx-defaulted-comparisons.v1";
inline bool HasCxxDefaultedComparisonIdentity(const InterfaceDeclarationIdentity &d) {
  return d.canonical_type_identity.find(";cxx-defaulted-comparisons:v1;") != std::string::npos;
}
inline constexpr const char *kCxxOutOfLineDefaultingSemantic =
    "objc3.cxx-out-of-line-defaulting.v1";
inline bool HasCxxOutOfLineDefaultingIdentity(const InterfaceDeclarationIdentity &d) {
  return d.canonical_type_identity.find(";cxx-out-of-line-defaulting:v1;") != std::string::npos;
}
inline constexpr const char *kCxxImmediateDefaultingSemantic =
    "objc3.cxx-immediate-defaulting.v1";
inline bool HasCxxImmediateDefaultingIdentity(const InterfaceDeclarationIdentity &d) {
  return d.canonical_type_identity.find(";cxx-immediate-defaulting:v1;") != std::string::npos;
}
inline constexpr const char *kCxxNamespaceAttributeSemantic =
    "objc3.cxx-namespace-attributes.v1";
inline bool HasCxxNamespaceAttributeIdentity(const InterfaceDeclarationIdentity &d) {
  return d.canonical_type_identity.find(";cxx-namespace-attributes:v1;") != std::string::npos;
}
inline constexpr const char *kCxxLambdaExpressionSemantic = "objc3.cxx-lambda-expressions.v1";
inline bool HasCxxLambdaExpressionIdentity(const InterfaceDeclarationIdentity &d) {
  return d.canonical_type_identity.find(";cxx-lambda-expressions:v1;") != std::string::npos;
}
inline constexpr const char *kCxxThreadLocalSemantic = "objc3.cxx-thread-local.v1";
inline bool HasCxxThreadLocalIdentity(const InterfaceDeclarationIdentity &d) {
  return d.canonical_type_identity.find(";cxx-thread-local:v1;") != std::string::npos;
}
inline constexpr const char *kCxxFriendSemantic = "objc3.cxx-friends.v1";
inline bool HasCxxFriendIdentity(const InterfaceDeclarationIdentity &d) {
  return d.canonical_type_identity.find(";cxx-friends:v1;") != std::string::npos;
}
inline constexpr const char *kCxxBlockExpressionSemantic = "objc3.cxx-block-expressions.v1";
inline bool HasCxxBlockExpressionIdentity(const InterfaceDeclarationIdentity &d) {
  return d.canonical_type_identity.find(";cxx-block-expressions:v1;") != std::string::npos;
}
inline constexpr const char *kCxxStatementExpressionSemantic = "objc3.cxx-statement-expressions.v1";
inline bool HasCxxStatementExpressionIdentity(const InterfaceDeclarationIdentity &d) {
  return d.canonical_type_identity.find(";cxx-statement-expressions:v1;") != std::string::npos;
}
inline constexpr const char *kCxxAsyncDefinitionSemantic = "objc3.cxx-async-definitions.v1";
inline bool HasCxxAsyncDefinitionIdentity(const InterfaceDeclarationIdentity &d) {
  return d.canonical_type_identity.find(";cxx-async-definitions:v1;") != std::string::npos;
}
inline constexpr const char *kCxxLocalTypeSemantic = "objc3.cxx-local-types.v1";
inline bool HasCxxLocalTypeIdentity(const InterfaceDeclarationIdentity &d) {
  return d.canonical_type_identity.find(";cxx-local-types:v1;") != std::string::npos;
}
inline bool IsCxxOverloadDeclaration(const InterfaceDeclarationIdentity &d) {
  return d.kind=="Function" || IsCxxMemberDeclarationKind(d.kind) ||
         (d.kind=="Using" && d.canonical_type_identity.find(";functions=1;")!=std::string::npos);
}

// Compare the complete declaration contract, including every modeled effect.
[[nodiscard]] bool
SameInterfaceDeclarationIdentity(const InterfaceDeclarationIdentity &left,
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
  // Canonical producer sidecar for a verified opposite-ownership C ABI view.
  // Its source binding names these exact canonical declaration bytes.
  std::string ownership_projection;
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

[[nodiscard]] bool IsValidSPIGroup(const std::string &group);
[[nodiscard]] bool IsGrantedSPI(const InterfaceDeclarationIdentity &declaration,
                                const std::vector<std::string> &groups);

// Stable fingerprint input sorts semantically unordered collections and omits
// source formatting and its own fingerprint. The sidecar codec owns JSON.
[[nodiscard]] std::string
BuildSemanticFingerprintInput(const SemanticInterfaceSidecar &sidecar);

[[nodiscard]] InterfaceContractValidation ValidateSemanticInterface(
    const SemanticInterfaceSidecar &sidecar,
    const InterfaceModuleIdentity &expected,
    const std::string &canonical_source_sha256,
    const std::vector<InterfaceDeclarationIdentity> &parsed_clang_declarations,
    const std::vector<std::string> &supported_required_semantics);

} // namespace objc3::clang_foundation
