#include "clang_foundation/interface_contract.h"

#include <algorithm>
#include <cctype>
#include <sstream>
#include <tuple>

namespace objc3::clang_foundation {
namespace {
void Field(std::ostringstream &out, const std::string &key,
           const std::string &value) {
  out << key.size() << ':' << key << value.size() << ':' << value << '\n';
}
void Bool(std::ostringstream &out, const std::string &key, bool value) {
  Field(out, key, value ? "1" : "0");
}
void ModuleFields(std::ostringstream &out, const std::string &prefix,
                  const InterfaceModuleIdentity &m,
                  bool include_fingerprint = true) {
  Field(out, prefix + ".name", m.canonical_name);
  if (include_fingerprint)
    Field(out, prefix + ".fingerprint", m.semantic_fingerprint);
  Field(out, prefix + ".target", m.target_triple);
  Field(out, prefix + ".runtime", m.runtime_abi);
  Field(out, prefix + ".extension_abi", m.extension_abi);
  Field(out, prefix + ".language_revision", m.language_revision);
  Field(out, prefix + ".profile", m.language_profile);
  Field(out, prefix + ".package", m.package_identity);
}
std::string DeclarationKey(const InterfaceDeclarationIdentity &d) {
  std::ostringstream out;
  out << d.stable_id << '\n' << d.kind << '\n' << d.qualified_name << '\n'
      << d.canonical_type_identity << '\n' << d.effects.is_async << '\n'
      << d.effects.is_throwing << '\n' << d.effects.throws_kind << '\n'
      << d.effects.error_type_identity << '\n'
      << d.effects.actor_isolation_identity << '\n'
      << d.effects.executor_affinity_identity << '\n'
      << d.effects.ownership_identity;
  return out.str();
}
bool HasEffects(const InterfaceEffectIdentity &e) {
  return e.is_async || e.is_throwing || !e.throws_kind.empty() ||
         !e.error_type_identity.empty() || !e.actor_isolation_identity.empty() ||
         !e.executor_affinity_identity.empty() || !e.ownership_identity.empty();
}
bool IsSha256(const std::string &digest) {
  return digest.size() == 64 &&
         std::all_of(digest.begin(), digest.end(), [](unsigned char c) {
           return std::isxdigit(c) != 0 && !std::isupper(c);
         });
}
bool Empty(const InterfaceModuleIdentity &m) {
  return m.canonical_name.empty() || m.semantic_fingerprint.empty() ||
         m.target_triple.empty() || m.runtime_abi.empty() ||
         m.extension_abi.empty() || m.language_revision.empty() ||
         m.language_profile.empty() || m.package_identity.empty();
}
void Add(InterfaceContractValidation &r, std::string code,
         std::string message, std::string subject = {}) {
  r.diagnostics.push_back({std::move(code), std::move(message), std::move(subject)});
}
}  // namespace

bool SameInterfaceDeclarationIdentity(
    const InterfaceDeclarationIdentity &left,
    const InterfaceDeclarationIdentity &right) {
  return std::tie(left.stable_id, left.kind, left.qualified_name,
                  left.canonical_type_identity,
                  left.effects.is_async, left.effects.is_throwing,
                  left.effects.throws_kind, left.effects.error_type_identity,
                  left.effects.actor_isolation_identity,
                  left.effects.executor_affinity_identity,
                  left.effects.ownership_identity) ==
         std::tie(right.stable_id, right.kind, right.qualified_name,
                  right.canonical_type_identity,
                  right.effects.is_async, right.effects.is_throwing,
                  right.effects.throws_kind, right.effects.error_type_identity,
                  right.effects.actor_isolation_identity,
                  right.effects.executor_affinity_identity,
                  right.effects.ownership_identity);
}

std::string BuildSemanticFingerprintInput(const SemanticInterfaceSidecar &sidecar) {
  std::ostringstream out;
  out << kInterfaceSchemaId << '\n' << sidecar.schema_major << '.'
      << sidecar.schema_minor << '\n';
  // Exclude the fingerprint itself and source bytes to avoid self-reference
  // and rebuild invalidation for comments/formatting in the generated source.
  ModuleFields(out, "module", sidecar.module, false);
  Field(out, "ownership-projection", sidecar.ownership_projection);
  auto semantics = sidecar.required_semantics;
  std::sort(semantics.begin(), semantics.end());
  for (const auto &s : semantics) Field(out, "required", s);
  auto declarations = sidecar.declarations;
  std::sort(declarations.begin(), declarations.end(),
            [](const auto &a, const auto &b) { return a.stable_id < b.stable_id; });
  for (const auto &d : declarations) Field(out, "declaration", DeclarationKey(d));
  auto dependencies = sidecar.dependencies;
  std::sort(dependencies.begin(), dependencies.end(), [](const auto &a, const auto &b) {
    return std::tie(a.module.canonical_name, a.module.semantic_fingerprint) <
           std::tie(b.module.canonical_name, b.module.semantic_fingerprint);
  });
  for (std::size_t i = 0; i < dependencies.size(); ++i) {
    const auto prefix = "dependency." + std::to_string(i);
    ModuleFields(out, prefix, dependencies[i].module);
    Bool(out, prefix + ".public", dependencies[i].public_import);
    Bool(out, prefix + ".reexported", dependencies[i].reexported);
  }
  return out.str();
}

InterfaceContractValidation ValidateSemanticInterface(
    const SemanticInterfaceSidecar &sidecar,
    const InterfaceModuleIdentity &expected,
    const std::string &canonical_source_sha256,
    const std::vector<InterfaceDeclarationIdentity> &parsed_clang_declarations,
    const std::vector<std::string> &supported_required_semantics) {
  InterfaceContractValidation result;
  if (sidecar.schema_major != kInterfaceSchemaMajor ||
      sidecar.schema_minor > kInterfaceSchemaMinor)
    Add(result, "O3IFC001", "unsupported semantic interface schema version");
  if (Empty(sidecar.module) || Empty(expected))
    Add(result, "O3IFC002", "module identity is incomplete");
  if (sidecar.module.canonical_name != expected.canonical_name ||
      sidecar.module.semantic_fingerprint != expected.semantic_fingerprint ||
      sidecar.module.package_identity != expected.package_identity)
    Add(result, "O3IFC003", "module or package identity mismatch", expected.canonical_name);
  if (sidecar.module.target_triple != expected.target_triple ||
      sidecar.module.runtime_abi != expected.runtime_abi ||
      sidecar.module.extension_abi != expected.extension_abi ||
      sidecar.module.language_revision != expected.language_revision ||
      sidecar.module.language_profile != expected.language_profile)
    Add(result, "O3IFC004", "target, runtime ABI, extension ABI, language revision, or profile mismatch", expected.canonical_name);
  if (!IsSha256(sidecar.canonical_source_sha256) ||
      !IsSha256(canonical_source_sha256) ||
      sidecar.canonical_source_sha256 != canonical_source_sha256)
    Add(result, "O3IFC005", "generated canonical source digest mismatch", expected.canonical_name);
  const bool projection_semantic = std::find(sidecar.required_semantics.begin(),
                                             sidecar.required_semantics.end(),
                                             "objc3.ownership-projection.v1") !=
                                   sidecar.required_semantics.end();
  if (projection_semantic != !sidecar.ownership_projection.empty())
    Add(result, "O3IFC017",
        "ownership projection provenance and capability must agree");
  const bool has_availability =
      std::any_of(sidecar.declarations.begin(), sidecar.declarations.end(),
                  [](const auto &declaration) {
                    return declaration.canonical_type_identity.find(
                               ";api-availability=") != std::string::npos;
                  });
  const bool availability_semantic =
      std::find(
          sidecar.required_semantics.begin(), sidecar.required_semantics.end(),
          "objc3.api-availability.v1") != sidecar.required_semantics.end();
  if (has_availability != availability_semantic)
    Add(result, "O3IFC018",
        "native availability metadata and capability must agree");
  auto supported = supported_required_semantics;
  std::sort(supported.begin(), supported.end());
  auto required = sidecar.required_semantics;
  std::sort(required.begin(), required.end());
  if (std::adjacent_find(required.begin(), required.end()) != required.end())
    Add(result, "O3IFC006", "duplicate required semantic capability", expected.canonical_name);
  for (const auto &semantic : required)
    if (!std::binary_search(supported.begin(), supported.end(), semantic))
      Add(result, "O3IFC007", "unknown required semantic: " + semantic, expected.canonical_name);
  auto expected_decls = sidecar.declarations;
  auto actual_decls = parsed_clang_declarations;
  auto by_id = [](const auto &a, const auto &b) { return a.stable_id < b.stable_id; };
  std::sort(expected_decls.begin(), expected_decls.end(), by_id);
  std::sort(actual_decls.begin(), actual_decls.end(), by_id);
  for (std::size_t i = 1; i < expected_decls.size(); ++i)
    if (expected_decls[i - 1].stable_id == expected_decls[i].stable_id)
      Add(result, "O3IFC008", "duplicate sidecar declaration identity", expected_decls[i].stable_id);
  if (expected_decls.size() != actual_decls.size())
    Add(result, "O3IFC009", "Clang declaration set does not match sidecar identities", expected.canonical_name);
  const auto count = std::min(expected_decls.size(), actual_decls.size());
  for (std::size_t i = 0; i < count; ++i)
    if (DeclarationKey(expected_decls[i]) != DeclarationKey(actual_decls[i]))
      Add(result, "O3IFC010", "declaration type or effect identity mismatch", expected_decls[i].stable_id);
  if ((sidecar.module.language_profile.find(";language=c++;") !=
           std::string::npos ||
       sidecar.module.language_profile.find(";language=objective-c++;") !=
           std::string::npos) &&
      !std::binary_search(required.begin(), required.end(),
                          "objc3.cxx-data-interfaces.v1"))
    Add(result, "O3IFC016",
        "C++ declarations are missing their semantic interface capability",
        expected.canonical_name);
  const bool effect_semantic = std::binary_search(
      required.begin(), required.end(), "objc3.canonical-function-effects.v1");
  for (const auto &declaration : sidecar.declarations)
    if (HasEffects(declaration.effects) && !effect_semantic)
      Add(result, "O3IFC012",
          "effect-bearing declaration is missing the canonical function-effects semantic capability",
          declaration.stable_id);
  const bool return_ownership_semantic = std::binary_search(
      required.begin(), required.end(), "objc3.native-return-ownership.v1");
  const bool direct_semantic = std::binary_search(
      required.begin(), required.end(), "objc3.direct-dispatch.v1");
  for (const auto &declaration : sidecar.declarations)
    if (declaration.kind == "ObjCMethod" &&
        declaration.canonical_type_identity.find(";dispatch=direct") != std::string::npos &&
        !direct_semantic)
      Add(result, "O3IFC015", "direct method is missing its semantic capability",
          declaration.stable_id);
  for (const auto &declaration : sidecar.declarations)
    if (declaration.effects.ownership_identity.starts_with("native-return:") &&
        !return_ownership_semantic)
      Add(result, "O3IFC014", "managed return ownership is missing its semantic capability",
          declaration.stable_id);
  const bool carrier_semantic = std::binary_search(
      required.begin(), required.end(), "objc3.native-carrier-types.v1");
  for (const auto &declaration : sidecar.declarations)
    if ((declaration.canonical_type_identity.find("Optional(") !=
             std::string::npos ||
         declaration.canonical_type_identity.find("Result(") !=
             std::string::npos) &&
        !carrier_semantic)
      Add(result, "O3IFC013",
          "carrier declaration is missing the native carrier type semantic capability",
          declaration.stable_id);
  for (const auto &dep : sidecar.dependencies)
    if (dep.module.canonical_name.empty() || dep.module.semantic_fingerprint.empty() ||
        dep.module.target_triple != sidecar.module.target_triple ||
        dep.module.runtime_abi != sidecar.module.runtime_abi ||
        dep.module.extension_abi != sidecar.module.extension_abi ||
        dep.module.language_revision != sidecar.module.language_revision)
      Add(result, "O3IFC011", "dependency identity is incomplete or ABI-incompatible", dep.module.canonical_name);
  return result;
}

}  // namespace objc3::clang_foundation
