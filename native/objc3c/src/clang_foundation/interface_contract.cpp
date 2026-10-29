#include "clang_foundation/interface_contract.h"
#include "clang_foundation/module_names.h"

#include <algorithm>
#include <cctype>
#include <set>
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
  out << d.stable_id << '\n'
      << d.kind << '\n'
      << d.qualified_name << '\n'
      << d.canonical_type_identity << '\n'
      << d.effects.is_async << '\n'
      << d.effects.is_throwing << '\n'
      << d.effects.throws_kind << '\n'
      << d.effects.error_type_identity << '\n'
      << d.effects.actor_isolation_identity << '\n'
      << d.effects.executor_affinity_identity << '\n'
      << d.effects.ownership_identity;
  Field(out, "export-root", d.export_root);
  Field(out, "async-entry-abi", d.effects.async_abi_identity);
  if (!d.spi_group.empty())
    Field(out, "spi-group", d.spi_group);
  if (!d.native_symbol_name.empty())
    Field(out, "native-symbol-name", d.native_symbol_name);
  return out.str();
}
bool HasEffects(const InterfaceEffectIdentity &e) {
  return e.is_async || e.is_throwing || !e.throws_kind.empty() ||
         !e.error_type_identity.empty() ||
         !e.actor_isolation_identity.empty() ||
         !e.executor_affinity_identity.empty() ||
         !e.ownership_identity.empty() || !e.async_abi_identity.empty();
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
void Add(InterfaceContractValidation &r, std::string code, std::string message,
         std::string subject = {}) {
  r.diagnostics.push_back(
      {std::move(code), std::move(message), std::move(subject)});
}
} // namespace

bool IsValidAsyncABIIdentity(const InterfaceEffectIdentity &effects) {
  const auto &text = effects.async_abi_identity;
  if (text.empty())
    return !effects.is_async;
  if (!text.starts_with("v1:"))
    return false;
  std::size_t cursor = 3;
  auto field = [&](std::string &value) {
    const auto start = cursor;
    std::size_t size = 0;
    while (cursor < text.size() && text[cursor] >= '0' && text[cursor] <= '9') {
      const auto digit = static_cast<std::size_t>(text[cursor++] - '0');
      // Bound arithmetic by the whole input before multiplying.
      if (size > text.size() / 10 || size * 10 + digit > text.size())
        return false;
      size = size * 10 + digit;
    }
    if (cursor == start || cursor == text.size() || text[cursor++] != ':' ||
        (cursor - start > 2 && text[start] == '0') ||
        size > text.size() - cursor)
      return false;
    value = text.substr(cursor, size);
    cursor += size;
    return true;
  };
  std::set<std::string> positions;
  while (cursor < text.size()) {
    std::string position, abi;
    if (!field(position) || !field(abi) ||
        !(position == "type" || position.starts_with("type.")) ||
        !positions.insert(position).second ||
        (abi != "c-frame-v1" && abi != "cxx-task-v1"))
      return false;
  }
  return !positions.empty();
}

bool IsValidSPIGroup(const std::string &group) {
  auto letter = [](unsigned char c) {
    return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c == '_';
  };
  return !group.empty() && group.size() <= 64 && letter(group.front()) &&
         std::all_of(group.begin(), group.end(), [&](unsigned char c) {
           return letter(c) || (c >= '0' && c <= '9');
         });
}
bool IsGrantedSPI(const InterfaceDeclarationIdentity &declaration,
                  const std::vector<std::string> &groups) {
  return declaration.spi_group.empty() ||
         std::find(groups.begin(), groups.end(), declaration.spi_group) !=
             groups.end();
}

bool SameInterfaceDeclarationIdentity(
    const InterfaceDeclarationIdentity &left,
    const InterfaceDeclarationIdentity &right) {
  return std::tie(left.stable_id, left.kind, left.qualified_name, left.export_root,
                  left.canonical_type_identity, left.spi_group,
                  left.native_symbol_name, left.effects.is_async,
                  left.effects.is_throwing, left.effects.throws_kind,
                  left.effects.error_type_identity,
                  left.effects.actor_isolation_identity,
                  left.effects.executor_affinity_identity,
                  left.effects.ownership_identity,
                  left.effects.async_abi_identity) ==
         std::tie(right.stable_id, right.kind, right.qualified_name, right.export_root,
                  right.canonical_type_identity, right.spi_group,
                  right.native_symbol_name, right.effects.is_async,
                  right.effects.is_throwing, right.effects.throws_kind,
                  right.effects.error_type_identity,
                  right.effects.actor_isolation_identity,
                  right.effects.executor_affinity_identity,
                  right.effects.ownership_identity,
                  right.effects.async_abi_identity);
}

std::string
BuildSemanticFingerprintInput(const SemanticInterfaceSidecar &sidecar) {
  std::ostringstream out;
  out << kInterfaceSchemaId << '\n'
      << sidecar.schema_major << '.' << sidecar.schema_minor << '\n';
  // Exclude the fingerprint itself and source bytes to avoid self-reference
  // and rebuild invalidation for comments/formatting in the generated source.
  ModuleFields(out, "module", sidecar.module, false);
  Field(out, "ownership-projection", sidecar.ownership_projection);
  auto semantics = sidecar.required_semantics;
  std::sort(semantics.begin(), semantics.end());
  for (const auto &s : semantics)
    Field(out, "required", s);
  auto declarations = sidecar.declarations;
  std::sort(
      declarations.begin(), declarations.end(),
      [](const auto &a, const auto &b) { return a.stable_id < b.stable_id; });
  for (const auto &d : declarations)
    Field(out, "declaration", DeclarationKey(d));
  auto dependencies = sidecar.dependencies;
  std::sort(dependencies.begin(), dependencies.end(),
            [](const auto &a, const auto &b) {
              return std::tie(a.module.canonical_name,
                              a.module.semantic_fingerprint) <
                     std::tie(b.module.canonical_name,
                              b.module.semantic_fingerprint);
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
      sidecar.schema_minor != kInterfaceSchemaMinor)
    Add(result, "O3IFC001", "unsupported semantic interface schema version");
  if (Empty(sidecar.module) || Empty(expected))
    Add(result, "O3IFC002", "module identity is incomplete");
  if (sidecar.module.canonical_name != expected.canonical_name ||
      sidecar.module.semantic_fingerprint != expected.semantic_fingerprint ||
      sidecar.module.package_identity != expected.package_identity)
    Add(result, "O3IFC003", "module or package identity mismatch",
        expected.canonical_name);
  if (sidecar.module.target_triple != expected.target_triple ||
      sidecar.module.runtime_abi != expected.runtime_abi ||
      sidecar.module.extension_abi != expected.extension_abi ||
      sidecar.module.language_revision != expected.language_revision ||
      sidecar.module.language_profile != expected.language_profile)
    Add(result, "O3IFC004",
        "target, runtime ABI, extension ABI, language revision, or profile "
        "mismatch",
        expected.canonical_name);
  if (!IsSha256(sidecar.canonical_source_sha256) ||
      !IsSha256(canonical_source_sha256) ||
      sidecar.canonical_source_sha256 != canonical_source_sha256)
    Add(result, "O3IFC005", "generated canonical source digest mismatch",
        expected.canonical_name);
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
    Add(result, "O3IFC006", "duplicate required semantic capability",
        expected.canonical_name);
  for (const auto &semantic : required)
    if (!std::binary_search(supported.begin(), supported.end(), semantic))
      Add(result, "O3IFC007", "unknown required semantic: " + semantic,
          expected.canonical_name);
  auto expected_decls = sidecar.declarations;
  auto actual_decls = parsed_clang_declarations;
  auto by_id = [](const auto &a, const auto &b) {
    return a.stable_id < b.stable_id;
  };
  std::sort(expected_decls.begin(), expected_decls.end(), by_id);
  std::sort(actual_decls.begin(), actual_decls.end(), by_id);
  for (std::size_t i = 1; i < expected_decls.size(); ++i)
    if (expected_decls[i - 1].stable_id == expected_decls[i].stable_id)
      Add(result, "O3IFC008", "duplicate sidecar declaration identity",
          expected_decls[i].stable_id);
  if (expected_decls.size() != actual_decls.size())
    Add(result, "O3IFC009",
        "Clang declaration set does not match sidecar identities",
        expected.canonical_name);
  const auto count = std::min(expected_decls.size(), actual_decls.size());
  for (std::size_t i = 0; i < count; ++i)
    if (DeclarationKey(expected_decls[i]) != DeclarationKey(actual_decls[i]))
      Add(result, "O3IFC010", "declaration type or effect identity mismatch",
          expected_decls[i].stable_id);
  if ((sidecar.module.language_profile.find(";language=c++;") !=
           std::string::npos ||
       sidecar.module.language_profile.find(";language=objective-c++;") !=
           std::string::npos) &&
      !std::binary_search(required.begin(), required.end(),
                          "objc3.cxx-data-interfaces.v1"))
    Add(result, "O3IFC016",
        "C++ declarations are missing their semantic interface capability",
        expected.canonical_name);
  const bool has_lifecycle =
      std::any_of(sidecar.declarations.begin(), sidecar.declarations.end(),
                  HasCxxClassLifecycleIdentity);
  const bool lifecycle_capability =
      std::find(sidecar.required_semantics.begin(),
                sidecar.required_semantics.end(),
                kCxxClassLifecycleSemantic) != sidecar.required_semantics.end();
  if (has_lifecycle != lifecycle_capability)
    Add(result, "O3IFC026",
        "C++ class lifecycle declarations and required capability must agree",
        expected.canonical_name);
  const bool has_structure =
      std::any_of(sidecar.declarations.begin(), sidecar.declarations.end(),
                  HasCxxClassStructureIdentity);
  const bool structure_capability =
      std::find(sidecar.required_semantics.begin(),
                sidecar.required_semantics.end(),
                kCxxClassStructureSemantic) != sidecar.required_semantics.end();
  if (has_structure != structure_capability ||
      (has_structure && !has_lifecycle))
    Add(result, "O3IFC027",
        "C++ class structure declarations require matching structure and "
        "lifecycle capabilities",
        expected.canonical_name);
  const bool has_dispatch =
      std::any_of(sidecar.declarations.begin(), sidecar.declarations.end(),
                  HasCxxClassDispatchIdentity);
  const bool dispatch_capability =
      std::find(sidecar.required_semantics.begin(),
                sidecar.required_semantics.end(),
                kCxxClassDispatchSemantic) != sidecar.required_semantics.end();
  if (has_dispatch != dispatch_capability ||
      (has_dispatch && !has_lifecycle))
    Add(result, "O3IFC028",
        "C++ class dispatch declarations require matching dispatch and "
        "lifecycle capabilities", expected.canonical_name);
  const bool has_namespaces =
      std::any_of(sidecar.declarations.begin(), sidecar.declarations.end(),
                  HasCxxNamespaceIdentity);
  const bool namespace_capability =
      std::find(sidecar.required_semantics.begin(),
                sidecar.required_semantics.end(),
                kCxxNamespaceSemantic) != sidecar.required_semantics.end();
  if (has_namespaces != namespace_capability)
    Add(result, "O3IFC029", "C++ namespace identity and required capability must agree",
        expected.canonical_name);
  const bool has_nested = std::any_of(sidecar.declarations.begin(),
      sidecar.declarations.end(), HasCxxNestedTypeIdentity);
  const bool nested_capability =
      std::find(sidecar.required_semantics.begin(), sidecar.required_semantics.end(),
                kCxxNestedTypeSemantic) != sidecar.required_semantics.end();
  if (has_nested != nested_capability)
    Add(result, "O3IFC030", "C++ nested type identity and required capability must agree",
        expected.canonical_name);
  const bool has_using=std::any_of(sidecar.declarations.begin(),sidecar.declarations.end(),HasCxxUsingIdentity);
  const bool using_capability=std::find(sidecar.required_semantics.begin(),sidecar.required_semantics.end(),
                                     kCxxUsingSemantic)!=sidecar.required_semantics.end();
  if(has_using!=using_capability) Add(result,"O3IFC031","C++ using identity and required capability must agree",expected.canonical_name);
  const bool has_expressions = std::any_of(
      sidecar.declarations.begin(), sidecar.declarations.end(),
      HasCxxDeclarationExpressionIdentity);
  const bool expression_capability = std::find(
      sidecar.required_semantics.begin(), sidecar.required_semantics.end(),
      kCxxDeclarationExpressionSemantic) != sidecar.required_semantics.end();
  if (has_expressions != expression_capability)
    Add(result, "O3IFC032", "C++ declaration expression identity and required capability must agree", expected.canonical_name);
  const bool has_definitions = std::any_of(
      sidecar.declarations.begin(), sidecar.declarations.end(), HasCxxDefinitionIdentity);
  const bool definition_capability = std::find(
      sidecar.required_semantics.begin(), sidecar.required_semantics.end(),
      kCxxDefinitionSemantic) != sidecar.required_semantics.end();
  if (has_definitions != definition_capability)
    Add(result, "O3IFC033", "C++ definition identity and required capability must agree", expected.canonical_name);
  const bool has_comparisons = std::any_of(
      sidecar.declarations.begin(), sidecar.declarations.end(), HasCxxDefaultedComparisonIdentity);
  const bool comparison_capability = std::find(
      sidecar.required_semantics.begin(), sidecar.required_semantics.end(),
      kCxxDefaultedComparisonSemantic) != sidecar.required_semantics.end();
  if (has_comparisons != comparison_capability)
    Add(result, "O3IFC034", "C++ defaulted comparison identity and required capability must agree", expected.canonical_name);
  const bool has_out_of_line_defaulting = std::any_of(
      sidecar.declarations.begin(), sidecar.declarations.end(), HasCxxOutOfLineDefaultingIdentity);
  const bool defaulting_capability = std::find(
      sidecar.required_semantics.begin(), sidecar.required_semantics.end(),
      kCxxOutOfLineDefaultingSemantic) != sidecar.required_semantics.end();
  if (has_out_of_line_defaulting != defaulting_capability)
    Add(result, "O3IFC035", "C++ out-of-line defaulting identity and required capability must agree", expected.canonical_name);
  const bool has_immediate_defaulting = std::any_of(
      sidecar.declarations.begin(), sidecar.declarations.end(), HasCxxImmediateDefaultingIdentity);
  const bool immediate_capability = std::find(
      sidecar.required_semantics.begin(), sidecar.required_semantics.end(),
      kCxxImmediateDefaultingSemantic) != sidecar.required_semantics.end();
  if (has_immediate_defaulting != immediate_capability)
    Add(result, "O3IFC036", "C++ immediate defaulting identity and required capability must agree", expected.canonical_name);
  const bool has_namespace_attributes = std::any_of(
      sidecar.declarations.begin(), sidecar.declarations.end(), HasCxxNamespaceAttributeIdentity);
  const bool namespace_attribute_capability = std::find(
      sidecar.required_semantics.begin(), sidecar.required_semantics.end(),
      kCxxNamespaceAttributeSemantic) != sidecar.required_semantics.end();
  if (has_namespace_attributes != namespace_attribute_capability)
    Add(result, "O3IFC037", "C++ namespace attribute identity and required capability must agree", expected.canonical_name);
  const bool has_lambdas = std::any_of(
      sidecar.declarations.begin(), sidecar.declarations.end(), HasCxxLambdaExpressionIdentity);
  const bool lambda_capability = std::find(
      sidecar.required_semantics.begin(), sidecar.required_semantics.end(),
      kCxxLambdaExpressionSemantic) != sidecar.required_semantics.end();
  if (has_lambdas != lambda_capability)
    Add(result, "O3IFC038", "C++ lambda identity and required capability must agree", expected.canonical_name);
  const bool has_thread_local = std::any_of(
      sidecar.declarations.begin(), sidecar.declarations.end(), HasCxxThreadLocalIdentity);
  const bool thread_local_capability = std::find(
      sidecar.required_semantics.begin(), sidecar.required_semantics.end(),
      kCxxThreadLocalSemantic) != sidecar.required_semantics.end();
  if (has_thread_local != thread_local_capability)
    Add(result, "O3IFC039", "C++ thread-local identity and required capability must agree", expected.canonical_name);
  const bool has_friends = std::any_of(
      sidecar.declarations.begin(), sidecar.declarations.end(), HasCxxFriendIdentity);
  const bool friend_capability = std::find(
      sidecar.required_semantics.begin(), sidecar.required_semantics.end(),
      kCxxFriendSemantic) != sidecar.required_semantics.end();
  if (has_friends != friend_capability)
    Add(result, "O3IFC040", "C++ friend identity and required capability must agree", expected.canonical_name);
  const bool has_blocks = std::any_of(
      sidecar.declarations.begin(), sidecar.declarations.end(), HasCxxBlockExpressionIdentity);
  const bool blocks_capability = std::find(
      sidecar.required_semantics.begin(), sidecar.required_semantics.end(),
      kCxxBlockExpressionSemantic) != sidecar.required_semantics.end();
  if (has_blocks != blocks_capability)
    Add(result, "O3IFC041", "C++ blocks expression identity and required capability must agree", expected.canonical_name);
  const bool has_statements = std::any_of(
      sidecar.declarations.begin(), sidecar.declarations.end(), HasCxxStatementExpressionIdentity);
  const bool statements_capability = std::find(
      sidecar.required_semantics.begin(), sidecar.required_semantics.end(),
      kCxxStatementExpressionSemantic) != sidecar.required_semantics.end();
  if (has_statements != statements_capability)
    Add(result, "O3IFC042", "C++ statements expression identity and required capability must agree", expected.canonical_name);
  const bool has_sdk_runtime = std::any_of(
      sidecar.declarations.begin(), sidecar.declarations.end(), HasSdkRuntimeDeclarationIdentity);
  const bool sdk_runtime_capability = std::find(
      sidecar.required_semantics.begin(), sidecar.required_semantics.end(),
      kSdkRuntimeDeclarationSemantic) != sidecar.required_semantics.end();
  if (has_sdk_runtime != sdk_runtime_capability)
    Add(result, "O3IFC045", "SDK runtime declaration identity and required capability must agree", expected.canonical_name);
  if (has_sdk_runtime && std::none_of(
      sidecar.required_semantics.begin(),sidecar.required_semantics.end(),
      [](const auto &semantic) { return semantic.starts_with("objc3.sdk-runtime-frames.v4."); }))
    Add(result,"O3IFC045","SDK runtime declarations require their pinned runtime header contract",expected.canonical_name);
  const bool has_async_definitions = std::any_of(
      sidecar.declarations.begin(), sidecar.declarations.end(), HasCxxAsyncDefinitionIdentity);
  const bool async_definition_capability = std::find(
      sidecar.required_semantics.begin(), sidecar.required_semantics.end(),
      kCxxAsyncDefinitionSemantic) != sidecar.required_semantics.end();
  if (has_async_definitions != async_definition_capability)
    Add(result, "O3IFC044", "C++ async definition identity and required capability must agree", expected.canonical_name);
  const bool nullable_writes = std::any_of(sidecar.declarations.begin(),sidecar.declarations.end(),HasNullableKeyPathWriteIdentity);
  const bool nullable_write_capability = std::find(sidecar.required_semantics.begin(),sidecar.required_semantics.end(),kNullableKeyPathWriteSemantic)!=sidecar.required_semantics.end();
  const bool nullable_write_paths = std::find(sidecar.required_semantics.begin(),sidecar.required_semantics.end(),"objc3.typed-keypaths.v1")!=sidecar.required_semantics.end();
  if (nullable_writes!=nullable_write_capability || (nullable_write_capability && !nullable_write_paths))
    Add(result,"O3IFC057","nullable key-path write identity and required write/key-path capabilities must agree",expected.canonical_name);
  const bool has_generic_factory_projections=std::any_of(sidecar.declarations.begin(),
      sidecar.declarations.end(),HasCxxGenericFactoryProjectionIdentity);
  const bool generic_factory_projection_capability=std::find(sidecar.required_semantics.begin(),
      sidecar.required_semantics.end(),kCxxGenericFactoryProjectionSemantic)!=sidecar.required_semantics.end();
  const bool generic_factory_projection_owners=std::all_of(sidecar.declarations.begin(),sidecar.declarations.end(),
      [](const auto &d){return !HasCxxGenericFactoryProjectionIdentity(d) || d.kind!="Namespace";});
  if (has_generic_factory_projections!=generic_factory_projection_capability || !generic_factory_projection_owners)
    Add(result,"O3IFC091","generic factory projection identities and required capability must agree",expected.canonical_name);
  const bool has_factory_template_arguments=std::any_of(sidecar.declarations.begin(),
      sidecar.declarations.end(),HasCxxFactoryTemplateArgumentIdentity);
  const bool factory_template_argument_capability=std::find(sidecar.required_semantics.begin(),
      sidecar.required_semantics.end(),kCxxFactoryTemplateArgumentSemantic)!=sidecar.required_semantics.end();
  const bool factory_template_argument_owners=std::all_of(sidecar.declarations.begin(),sidecar.declarations.end(),
      [](const auto &d){return !HasCxxFactoryTemplateArgumentIdentity(d) || d.kind!="Namespace";});
  if (has_factory_template_arguments!=factory_template_argument_capability || !factory_template_argument_owners)
    Add(result,"O3IFC090","factory template argument identities and required capability must agree",expected.canonical_name);
  const bool has_factory_type_projections=std::any_of(sidecar.declarations.begin(),
      sidecar.declarations.end(),HasCxxFactoryTypeProjectionIdentity);
  const bool factory_type_projection_capability=std::find(sidecar.required_semantics.begin(),
      sidecar.required_semantics.end(),kCxxFactoryTypeProjectionSemantic)!=sidecar.required_semantics.end();
  const bool factory_type_projection_owners=std::all_of(sidecar.declarations.begin(),sidecar.declarations.end(),
      [](const auto &d){return !HasCxxFactoryTypeProjectionIdentity(d) || d.kind!="Namespace";});
  if (has_factory_type_projections!=factory_type_projection_capability || !factory_type_projection_owners)
    Add(result,"O3IFC089","factory type projection identities and required capability must agree",expected.canonical_name);
  const bool has_local_factory_results=std::any_of(sidecar.declarations.begin(),
      sidecar.declarations.end(),HasCxxLocalFactoryResultIdentity);
  const bool local_factory_result_capability=std::find(sidecar.required_semantics.begin(),
      sidecar.required_semantics.end(),kCxxLocalFactoryResultSemantic)!=sidecar.required_semantics.end();
  const bool local_factory_result_owners=std::all_of(sidecar.declarations.begin(),sidecar.declarations.end(),
      [](const auto &d){return !HasCxxLocalFactoryResultIdentity(d) || d.kind!="Namespace";});
  if (has_local_factory_results!=local_factory_result_capability || !local_factory_result_owners)
    Add(result,"O3IFC088","local factory result identities and required capability must agree",expected.canonical_name);
  const bool has_closure_factories=std::any_of(sidecar.declarations.begin(),
      sidecar.declarations.end(),HasCxxClosureFactoryIdentity);
  const bool closure_factory_capability=std::find(sidecar.required_semantics.begin(),
      sidecar.required_semantics.end(),kCxxClosureFactorySemantic)!=sidecar.required_semantics.end();
  const bool closure_factory_owners=std::all_of(sidecar.declarations.begin(),sidecar.declarations.end(),
      [](const auto &d){return !HasCxxClosureFactoryIdentity(d) || d.kind!="Namespace";});
  if (has_closure_factories!=closure_factory_capability || !closure_factory_owners)
    Add(result,"O3IFC087","closure factory identities and required capability must agree",expected.canonical_name);
  const bool has_effectful_lambdas=std::any_of(sidecar.declarations.begin(),
      sidecar.declarations.end(),HasCxxEffectfulLambdaIdentity);
  const bool effectful_lambda_capability=std::find(sidecar.required_semantics.begin(),
      sidecar.required_semantics.end(),kCxxEffectfulLambdaSemantic)!=sidecar.required_semantics.end();
  const bool effectful_lambda_owners=std::all_of(sidecar.declarations.begin(),sidecar.declarations.end(),
      [](const auto &d){return !HasCxxEffectfulLambdaIdentity(d) || d.kind!="Namespace";});
  if (has_effectful_lambdas!=effectful_lambda_capability || !effectful_lambda_owners)
    Add(result,"O3IFC086","effectful lambda identities and required capability must agree",expected.canonical_name);
  const bool has_address_template_arguments=std::any_of(sidecar.declarations.begin(),
      sidecar.declarations.end(),HasCxxAddressTemplateArgumentIdentity);
  const bool address_template_argument_capability=std::find(sidecar.required_semantics.begin(),
      sidecar.required_semantics.end(),kCxxAddressTemplateArgumentSemantic)!=sidecar.required_semantics.end();
  const bool address_template_argument_owners=std::all_of(sidecar.declarations.begin(),sidecar.declarations.end(),
      [](const auto &d){return !HasCxxAddressTemplateArgumentIdentity(d) || d.kind!="Namespace";});
  if (has_address_template_arguments!=address_template_argument_capability || !address_template_argument_owners)
    Add(result,"O3IFC085","address template argument identities and required capability must agree",expected.canonical_name);
  const bool has_template_callable_projections=std::any_of(sidecar.declarations.begin(),
      sidecar.declarations.end(),HasCxxTemplateCallableProjectionIdentity);
  const bool template_callable_projection_capability=std::find(sidecar.required_semantics.begin(),
      sidecar.required_semantics.end(),kCxxTemplateCallableProjectionSemantic)!=sidecar.required_semantics.end();
  const bool template_callable_projection_owners=std::all_of(sidecar.declarations.begin(),sidecar.declarations.end(),
      [](const auto &d){return !HasCxxTemplateCallableProjectionIdentity(d) ||
          d.kind=="ClassTemplate" || d.kind=="FunctionTemplate" || d.kind=="TypeAliasTemplate" ||
          d.kind=="VarTemplate" || (d.kind=="CXXRecord" && HasCxxConcreteMemberTemplateIdentity(d)) ||
          (d.kind=="Function" && HasCxxFunctionSpecializationIdentity(d)) ||
          (d.kind=="VarTemplateSpecialization" && HasCxxVariableSpecializationIdentity(d));});
  if (has_template_callable_projections!=template_callable_projection_capability || !template_callable_projection_owners)
    Add(result,"O3IFC084","template callable projection identities and required capability must agree",expected.canonical_name);
  const bool has_effectful_template_members=std::any_of(sidecar.declarations.begin(),
      sidecar.declarations.end(),HasCxxEffectfulTemplateMemberIdentity);
  const bool effectful_template_member_capability=std::find(sidecar.required_semantics.begin(),
      sidecar.required_semantics.end(),kCxxEffectfulTemplateMemberSemantic)!=sidecar.required_semantics.end();
  const bool effectful_template_member_owners=std::all_of(sidecar.declarations.begin(),sidecar.declarations.end(),
      [](const auto &d){return !HasCxxEffectfulTemplateMemberIdentity(d) || d.kind=="ClassTemplate" ||
          (d.kind=="CXXRecord" && HasCxxConcreteMemberTemplateIdentity(d));});
  if (has_effectful_template_members!=effectful_template_member_capability || !effectful_template_member_owners)
    Add(result,"O3IFC083","effectful template-member identities and required capability must agree",expected.canonical_name);
  const bool has_effectful_function_templates=std::any_of(sidecar.declarations.begin(),
      sidecar.declarations.end(),HasCxxEffectfulFunctionTemplateIdentity);
  const bool effectful_function_template_capability=std::find(sidecar.required_semantics.begin(),
      sidecar.required_semantics.end(),kCxxEffectfulFunctionTemplateSemantic)!=sidecar.required_semantics.end();
  const bool effectful_function_template_owners=std::all_of(sidecar.declarations.begin(),sidecar.declarations.end(),
      [](const auto &d){return !HasCxxEffectfulFunctionTemplateIdentity(d) || d.kind=="FunctionTemplate";});
  if (has_effectful_function_templates!=effectful_function_template_capability || !effectful_function_template_owners)
    Add(result,"O3IFC082","effectful function-template identities and required capability must agree",expected.canonical_name);
  const bool has_closed_parent_patterns=std::any_of(sidecar.declarations.begin(),
      sidecar.declarations.end(),HasCxxClosedParentPatternIdentity);
  const bool closed_parent_pattern_capability=std::find(sidecar.required_semantics.begin(),
      sidecar.required_semantics.end(),kCxxClosedParentPatternSemantic)!=sidecar.required_semantics.end();
  const bool closed_parent_pattern_owners=std::all_of(sidecar.declarations.begin(),sidecar.declarations.end(),
      [](const auto &d){return !HasCxxClosedParentPatternIdentity(d) ||
          d.kind=="ClassTemplate" || (d.kind=="CXXRecord" && HasCxxConcreteMemberTemplateIdentity(d));});
  if (has_closed_parent_patterns!=closed_parent_pattern_capability || !closed_parent_pattern_owners)
    Add(result,"O3IFC081","closed-parent pattern identities and required capability must agree",expected.canonical_name);
  const bool has_closed_parent_members=std::any_of(sidecar.declarations.begin(),
      sidecar.declarations.end(),HasCxxClosedParentMemberIdentity);
  const bool closed_parent_member_capability=std::find(sidecar.required_semantics.begin(),
      sidecar.required_semantics.end(),kCxxClosedParentMemberSemantic)!=sidecar.required_semantics.end();
  const bool closed_parent_member_owners=std::all_of(sidecar.declarations.begin(),sidecar.declarations.end(),
      [](const auto &d){return !HasCxxClosedParentMemberIdentity(d) ||
          d.kind=="ClassTemplate" || (d.kind=="CXXRecord" && HasCxxConcreteMemberTemplateIdentity(d));});
  if (has_closed_parent_members!=closed_parent_member_capability || !closed_parent_member_owners)
    Add(result,"O3IFC080","closed-parent member identities and required capability must agree",expected.canonical_name);
  const bool has_dependent_member_partials=std::any_of(sidecar.declarations.begin(),
      sidecar.declarations.end(),HasCxxDependentMemberPartialIdentity);
  const bool dependent_member_partial_capability=std::find(sidecar.required_semantics.begin(),
      sidecar.required_semantics.end(),kCxxDependentMemberPartialSemantic)!=sidecar.required_semantics.end();
  const bool dependent_member_partial_owners=std::all_of(sidecar.declarations.begin(),sidecar.declarations.end(),
      [](const auto &d){return !HasCxxDependentMemberPartialIdentity(d) ||
          d.kind=="ClassTemplate" || (d.kind=="CXXRecord" && HasCxxConcreteMemberTemplateIdentity(d));});
  if (has_dependent_member_partials!=dependent_member_partial_capability || !dependent_member_partial_owners)
    Add(result,"O3IFC079","dependent member partial identities and required capability must agree",expected.canonical_name);
  const bool has_concrete_special_members=std::any_of(sidecar.declarations.begin(),
      sidecar.declarations.end(),HasCxxConcreteSpecialMemberSpecializationIdentity);
  const bool concrete_special_member_capability=std::find(sidecar.required_semantics.begin(),
      sidecar.required_semantics.end(),kCxxConcreteSpecialMemberSpecializationSemantic)!=sidecar.required_semantics.end();
  const bool concrete_special_member_base=std::find(sidecar.required_semantics.begin(),
      sidecar.required_semantics.end(),kCxxConcreteMemberFunctionSpecializationSemantic)!=sidecar.required_semantics.end();
  const bool concrete_special_member_owners=std::all_of(sidecar.declarations.begin(),sidecar.declarations.end(),
      [](const auto &d){return !HasCxxConcreteSpecialMemberSpecializationIdentity(d) ||
          (d.kind=="CXXRecord" && HasCxxConcreteMemberTemplateIdentity(d) && HasCxxConcreteMemberFunctionSpecializationIdentity(d));});
  if (has_concrete_special_members!=concrete_special_member_capability || !concrete_special_member_owners ||
      (concrete_special_member_capability && !concrete_special_member_base))
    Add(result,"O3IFC078","special-member specialization identity and required callable capability must agree",expected.canonical_name);
  const bool has_concrete_member_variable_specializations=std::any_of(sidecar.declarations.begin(),
      sidecar.declarations.end(),HasCxxConcreteMemberVariableSpecializationIdentity);
  const bool concrete_member_variable_specialization_capability=std::find(sidecar.required_semantics.begin(),
      sidecar.required_semantics.end(),kCxxConcreteMemberVariableSpecializationSemantic)!=sidecar.required_semantics.end();
  const bool concrete_member_variable_specialization_primary=std::find(sidecar.required_semantics.begin(),
      sidecar.required_semantics.end(),kCxxClassTemplateSemantic)!=sidecar.required_semantics.end();
  const bool concrete_member_variable_specialization_owners=std::all_of(sidecar.declarations.begin(),sidecar.declarations.end(),
      [](const auto &d){return !HasCxxConcreteMemberVariableSpecializationIdentity(d) ||
          (d.kind=="CXXRecord" && HasCxxConcreteMemberTemplateIdentity(d));});
  if (has_concrete_member_variable_specializations!=concrete_member_variable_specialization_capability || !concrete_member_variable_specialization_owners ||
      (concrete_member_variable_specialization_capability && !concrete_member_variable_specialization_primary &&
       std::find(sidecar.required_semantics.begin(),sidecar.required_semantics.end(),kCxxConcreteMemberTemplateSemantic)==sidecar.required_semantics.end()))
    Add(result,"O3IFC077","concrete member variable specialization identity and required primary capability must agree",expected.canonical_name);
  const bool has_concrete_member_function_specializations=std::any_of(sidecar.declarations.begin(),
      sidecar.declarations.end(),HasCxxConcreteMemberFunctionSpecializationIdentity);
  const bool concrete_member_function_specialization_capability=std::find(sidecar.required_semantics.begin(),
      sidecar.required_semantics.end(),kCxxConcreteMemberFunctionSpecializationSemantic)!=sidecar.required_semantics.end();
  const bool concrete_member_function_specialization_primary=std::find(sidecar.required_semantics.begin(),
      sidecar.required_semantics.end(),kCxxClassTemplateSemantic)!=sidecar.required_semantics.end();
  const bool concrete_member_function_specialization_owners=std::all_of(sidecar.declarations.begin(),sidecar.declarations.end(),
      [](const auto &d){return !HasCxxConcreteMemberFunctionSpecializationIdentity(d) ||
          (d.kind=="CXXRecord" && HasCxxConcreteMemberTemplateIdentity(d));});
  if (has_concrete_member_function_specializations!=concrete_member_function_specialization_capability || !concrete_member_function_specialization_owners ||
      (concrete_member_function_specialization_capability && !concrete_member_function_specialization_primary &&
       std::find(sidecar.required_semantics.begin(),sidecar.required_semantics.end(),kCxxConcreteMemberTemplateSemantic)==sidecar.required_semantics.end()))
    Add(result,"O3IFC076","concrete member function specialization identity and required primary capability must agree",expected.canonical_name);
  const bool has_concrete_member_class_specializations=std::any_of(sidecar.declarations.begin(),
      sidecar.declarations.end(),HasCxxConcreteMemberClassSpecializationIdentity);
  const bool concrete_member_class_specialization_capability=std::find(sidecar.required_semantics.begin(),
      sidecar.required_semantics.end(),kCxxConcreteMemberClassSpecializationSemantic)!=sidecar.required_semantics.end();
  const bool concrete_member_class_specialization_primary=std::find(sidecar.required_semantics.begin(),
      sidecar.required_semantics.end(),kCxxClassTemplateSemantic)!=sidecar.required_semantics.end();
  const bool concrete_member_class_specialization_owners=std::all_of(sidecar.declarations.begin(),sidecar.declarations.end(),
      [](const auto &d){return !HasCxxConcreteMemberClassSpecializationIdentity(d) ||
          (d.kind=="CXXRecord" && HasCxxConcreteNestedTemplateIdentity(d));});
  if (has_concrete_member_class_specializations!=concrete_member_class_specialization_capability || !concrete_member_class_specialization_owners ||
      (concrete_member_class_specialization_capability && !concrete_member_class_specialization_primary &&
       std::find(sidecar.required_semantics.begin(),sidecar.required_semantics.end(),kCxxConcreteNestedTemplateSemantic)==sidecar.required_semantics.end()))
    Add(result,"O3IFC075","concrete member class specialization identity and required primary capability must agree",expected.canonical_name);
  const bool has_detached_nested_templates=std::any_of(sidecar.declarations.begin(),
      sidecar.declarations.end(),HasCxxDetachedNestedTemplateIdentity);
  const bool detached_nested_template_capability=std::find(sidecar.required_semantics.begin(),
      sidecar.required_semantics.end(),kCxxDetachedNestedTemplateSemantic)!=sidecar.required_semantics.end();
  const bool detached_nested_template_primary=std::find(sidecar.required_semantics.begin(),
      sidecar.required_semantics.end(),kCxxClassTemplateSemantic)!=sidecar.required_semantics.end();
  const bool detached_nested_template_owners=std::all_of(sidecar.declarations.begin(),sidecar.declarations.end(),
      [](const auto &d){return !HasCxxDetachedNestedTemplateIdentity(d) ||
          (d.kind=="ClassTemplate" || (d.kind=="CXXRecord" && HasCxxConcreteNestedTemplateIdentity(d)));});
  if (has_detached_nested_templates!=detached_nested_template_capability || !detached_nested_template_owners ||
      (detached_nested_template_capability && !detached_nested_template_primary &&
       std::find(sidecar.required_semantics.begin(),sidecar.required_semantics.end(),kCxxConcreteNestedTemplateSemantic)==sidecar.required_semantics.end()))
    Add(result,"O3IFC074","detached nested template identity and required primary capability must agree",expected.canonical_name);
  const bool has_detached_static_data_templates=std::any_of(sidecar.declarations.begin(),
      sidecar.declarations.end(),HasCxxDetachedStaticDataTemplateIdentity);
  const bool detached_static_data_template_capability=std::find(sidecar.required_semantics.begin(),
      sidecar.required_semantics.end(),kCxxDetachedStaticDataTemplateSemantic)!=sidecar.required_semantics.end();
  const bool detached_static_data_template_primary=std::find(sidecar.required_semantics.begin(),
      sidecar.required_semantics.end(),kCxxClassTemplateSemantic)!=sidecar.required_semantics.end();
  const bool detached_static_data_template_owners=std::all_of(sidecar.declarations.begin(),sidecar.declarations.end(),
      [](const auto &d){return !HasCxxDetachedStaticDataTemplateIdentity(d) ||
          (d.kind=="ClassTemplate" || (d.kind=="CXXRecord" && HasCxxConcreteMemberTemplateIdentity(d)));});
  if (has_detached_static_data_templates!=detached_static_data_template_capability || !detached_static_data_template_owners ||
      (detached_static_data_template_capability && !detached_static_data_template_primary &&
       std::find(sidecar.required_semantics.begin(),sidecar.required_semantics.end(),kCxxConcreteMemberTemplateSemantic)==sidecar.required_semantics.end()))
    Add(result,"O3IFC073","detached static data template identity and required primary capability must agree",expected.canonical_name);
  const bool has_detached_callable_templates=std::any_of(sidecar.declarations.begin(),
      sidecar.declarations.end(),HasCxxDetachedCallableTemplateIdentity);
  const bool detached_callable_template_capability=std::find(sidecar.required_semantics.begin(),
      sidecar.required_semantics.end(),kCxxDetachedCallableTemplateSemantic)!=sidecar.required_semantics.end();
  const bool detached_callable_template_primary=std::find(sidecar.required_semantics.begin(),
      sidecar.required_semantics.end(),kCxxClassTemplateSemantic)!=sidecar.required_semantics.end();
  const bool detached_callable_template_owners=std::all_of(sidecar.declarations.begin(),sidecar.declarations.end(),
      [](const auto &d){return !HasCxxDetachedCallableTemplateIdentity(d) ||
          (d.kind=="ClassTemplate" || (d.kind=="CXXRecord" && HasCxxConcreteMemberTemplateIdentity(d)));});
  if (has_detached_callable_templates!=detached_callable_template_capability || !detached_callable_template_owners ||
      (detached_callable_template_capability && !detached_callable_template_primary &&
       std::find(sidecar.required_semantics.begin(),sidecar.required_semantics.end(),kCxxConcreteMemberTemplateSemantic)==sidecar.required_semantics.end()))
    Add(result,"O3IFC072","detached callable template identity and required primary capability must agree",expected.canonical_name);
  const bool has_variable_specialization_storages=std::any_of(sidecar.declarations.begin(),
      sidecar.declarations.end(),HasCxxVariableSpecializationStorageIdentity);
  const bool variable_specialization_storage_capability=std::find(sidecar.required_semantics.begin(),
      sidecar.required_semantics.end(),kCxxVariableSpecializationStorageSemantic)!=sidecar.required_semantics.end();
  const bool variable_specialization_storage_primary=std::find(sidecar.required_semantics.begin(),
      sidecar.required_semantics.end(),kCxxVariableSpecializationSemantic)!=sidecar.required_semantics.end();
  const bool variable_specialization_storage_owners=std::all_of(sidecar.declarations.begin(),sidecar.declarations.end(),
      [](const auto &d){return !HasCxxVariableSpecializationStorageIdentity(d) ||
          (d.kind=="VarTemplateSpecialization" && HasCxxVariableSpecializationIdentity(d));});
  if (has_variable_specialization_storages!=variable_specialization_storage_capability || !variable_specialization_storage_owners ||
      (variable_specialization_storage_capability && !variable_specialization_storage_primary))
    Add(result,"O3IFC071","variable specialization storage identity and required primary capability must agree",expected.canonical_name);
  const bool has_variable_partial_specializations=std::any_of(sidecar.declarations.begin(),
      sidecar.declarations.end(),HasCxxVariablePartialSpecializationIdentity);
  const bool variable_partial_specialization_capability=std::find(sidecar.required_semantics.begin(),
      sidecar.required_semantics.end(),kCxxVariablePartialSpecializationSemantic)!=sidecar.required_semantics.end();
  const bool variable_partial_specialization_primary=std::find(sidecar.required_semantics.begin(),
      sidecar.required_semantics.end(),kCxxVariableTemplateSemantic)!=sidecar.required_semantics.end();
  const bool variable_partial_specialization_owners=std::all_of(sidecar.declarations.begin(),sidecar.declarations.end(),
      [](const auto &d){return !HasCxxVariablePartialSpecializationIdentity(d) ||
          (d.kind=="VarTemplate" && HasCxxVariableTemplateIdentity(d));});
  if (has_variable_partial_specializations!=variable_partial_specialization_capability || !variable_partial_specialization_owners ||
      (variable_partial_specialization_capability && !variable_partial_specialization_primary))
    Add(result,"O3IFC070","partial variable specialization identity and required primary capability must agree",expected.canonical_name);
  const bool has_class_partial_specializations=std::any_of(sidecar.declarations.begin(),
      sidecar.declarations.end(),HasCxxClassPartialSpecializationIdentity);
  const bool class_partial_specialization_capability=std::find(sidecar.required_semantics.begin(),
      sidecar.required_semantics.end(),kCxxClassPartialSpecializationSemantic)!=sidecar.required_semantics.end();
  const bool class_partial_specialization_primary=std::find(sidecar.required_semantics.begin(),
      sidecar.required_semantics.end(),kCxxClassTemplateSemantic)!=sidecar.required_semantics.end();
  const bool class_partial_specialization_owners=std::all_of(sidecar.declarations.begin(),sidecar.declarations.end(),
      [](const auto &d){return !HasCxxClassPartialSpecializationIdentity(d) ||
          (d.kind=="ClassTemplate" && HasCxxClassTemplateIdentity(d) && HasCxxClassSpecializationIdentity(d));});
  const bool class_partial_base=std::find(sidecar.required_semantics.begin(),sidecar.required_semantics.end(),
      kCxxClassSpecializationSemantic)!=sidecar.required_semantics.end();
  if (has_class_partial_specializations!=class_partial_specialization_capability || !class_partial_specialization_owners ||
      (class_partial_specialization_capability && (!class_partial_specialization_primary || !class_partial_base)))
    Add(result,"O3IFC069","partial class specialization identity and required primary capability must agree",expected.canonical_name);
  const bool has_class_specializations=std::any_of(sidecar.declarations.begin(),
      sidecar.declarations.end(),HasCxxClassSpecializationIdentity);
  const bool class_specialization_capability=std::find(sidecar.required_semantics.begin(),
      sidecar.required_semantics.end(),kCxxClassSpecializationSemantic)!=sidecar.required_semantics.end();
  const bool class_specialization_primary=std::find(sidecar.required_semantics.begin(),
      sidecar.required_semantics.end(),kCxxClassTemplateSemantic)!=sidecar.required_semantics.end();
  const bool class_specialization_owners=std::all_of(sidecar.declarations.begin(),sidecar.declarations.end(),
      [](const auto &d){return !HasCxxClassSpecializationIdentity(d) ||
          (d.kind=="ClassTemplate" && HasCxxClassTemplateIdentity(d));});
  if (has_class_specializations!=class_specialization_capability || !class_specialization_owners ||
      (class_specialization_capability && !class_specialization_primary))
    Add(result,"O3IFC068","class specialization identity and required primary capability must agree",expected.canonical_name);
  const bool has_variable_specializations=std::any_of(sidecar.declarations.begin(),
      sidecar.declarations.end(),HasCxxVariableSpecializationIdentity);
  const bool variable_specialization_capability=std::find(sidecar.required_semantics.begin(),
      sidecar.required_semantics.end(),kCxxVariableSpecializationSemantic)!=sidecar.required_semantics.end();
  const bool variable_specialization_primary=std::find(sidecar.required_semantics.begin(),
      sidecar.required_semantics.end(),kCxxVariableTemplateSemantic)!=sidecar.required_semantics.end();
  const bool variable_specialization_owners=std::all_of(sidecar.declarations.begin(),sidecar.declarations.end(),
      [](const auto &d){return !HasCxxVariableSpecializationIdentity(d) ||
          (d.kind=="VarTemplateSpecialization" && HasCxxVariableTemplateIdentity(d) &&
           d.stable_id.starts_with("c:@VariableTemplateSpecialization@"));});
  if (has_variable_specializations!=variable_specialization_capability || !variable_specialization_owners ||
      (variable_specialization_capability && !variable_specialization_primary))
    Add(result,"O3IFC067","variable specialization identity and required primary capability must agree",expected.canonical_name);
  const bool has_function_specializations=std::any_of(sidecar.declarations.begin(),
      sidecar.declarations.end(),HasCxxFunctionSpecializationIdentity);
  const bool function_specialization_capability=std::find(sidecar.required_semantics.begin(),
      sidecar.required_semantics.end(),kCxxFunctionSpecializationSemantic)!=sidecar.required_semantics.end();
  const bool function_specialization_primary=std::find(sidecar.required_semantics.begin(),
      sidecar.required_semantics.end(),kCxxFunctionTemplateSemantic)!=sidecar.required_semantics.end();
  const bool function_specialization_owners=std::all_of(sidecar.declarations.begin(),sidecar.declarations.end(),
      [](const auto &d){return !HasCxxFunctionSpecializationIdentity(d) ||
          (d.kind=="Function" && HasCxxFunctionTemplateIdentity(d) &&
           d.stable_id.starts_with("c:@FunctionTemplateSpecialization@"));});
  if (has_function_specializations!=function_specialization_capability || !function_specialization_owners ||
      (function_specialization_capability && !function_specialization_primary))
    Add(result,"O3IFC066","function specialization identity and required primary capability must agree",expected.canonical_name);
  const bool has_template_template_parameters=std::any_of(sidecar.declarations.begin(),
      sidecar.declarations.end(),HasCxxTemplateTemplateIdentity);
  const bool template_template_capability=std::find(sidecar.required_semantics.begin(),
      sidecar.required_semantics.end(),kCxxTemplateTemplateSemantic)!=sidecar.required_semantics.end();
  if (has_template_template_parameters!=template_template_capability)
    Add(result,"O3IFC064","template-template parameter identity and required capability must agree",expected.canonical_name);
  const bool has_nominal_alias_templates=std::any_of(sidecar.declarations.begin(),
      sidecar.declarations.end(),HasCxxNominalAliasTemplateIdentity);
  const bool nominal_alias_capability=std::find(sidecar.required_semantics.begin(),
      sidecar.required_semantics.end(),kCxxNominalAliasTemplateSemantic)!=sidecar.required_semantics.end();
  const bool nominal_alias_dependency=std::find(sidecar.required_semantics.begin(),
      sidecar.required_semantics.end(),kCxxAliasTemplateSemantic)!=sidecar.required_semantics.end();
  const bool nominal_alias_owners=std::all_of(sidecar.declarations.begin(),sidecar.declarations.end(),
      [](const auto &d){return !HasCxxNominalAliasTemplateIdentity(d) ||
          (d.kind=="TypeAliasTemplate" && !d.native_symbol_name.empty() &&
           d.canonical_type_identity.find(";owner="+d.native_symbol_name+";")!=std::string::npos);});
  if (has_nominal_alias_templates!=nominal_alias_capability || !nominal_alias_owners ||
      (nominal_alias_capability && !nominal_alias_dependency))
    Add(result,"O3IFC065","nominal alias identity and required alias/owner capability must agree",expected.canonical_name);
  const bool has_concrete_member_templates = std::any_of(sidecar.declarations.begin(),
      sidecar.declarations.end(),HasCxxConcreteMemberTemplateIdentity);
  const bool concrete_member_capability = std::find(sidecar.required_semantics.begin(),
      sidecar.required_semantics.end(),kCxxConcreteMemberTemplateSemantic)!=sidecar.required_semantics.end();
  const bool concrete_member_owners = std::all_of(sidecar.declarations.begin(),sidecar.declarations.end(),
      [](const auto &d) {return !HasCxxConcreteMemberTemplateIdentity(d) ||
          (d.kind=="CXXRecord" && (HasCxxMemberTemplateIdentity(d) ||
           HasCxxMemberAliasTemplateIdentity(d) || HasCxxVariableTemplateIdentity(d) ||
           HasCxxClassTemplateIdentity(d)));});
  if (has_concrete_member_templates!=concrete_member_capability || !concrete_member_owners)
    Add(result,"O3IFC062","concrete member template identity and required record capability must agree",expected.canonical_name);
  const bool has_concrete_nested_templates = std::any_of(sidecar.declarations.begin(),
      sidecar.declarations.end(),HasCxxConcreteNestedTemplateIdentity);
  const bool concrete_nested_capability = std::find(sidecar.required_semantics.begin(),
      sidecar.required_semantics.end(),kCxxConcreteNestedTemplateSemantic)!=sidecar.required_semantics.end();
  const bool concrete_nested_class = std::find(sidecar.required_semantics.begin(),
      sidecar.required_semantics.end(),kCxxClassTemplateSemantic)!=sidecar.required_semantics.end();
  const bool concrete_nested_patterns = std::find(sidecar.required_semantics.begin(),
      sidecar.required_semantics.end(),kCxxNestedTemplateSemantic)!=sidecar.required_semantics.end();
  const bool concrete_nested_owners = std::all_of(sidecar.declarations.begin(),sidecar.declarations.end(),
      [](const auto &d) {return !HasCxxConcreteNestedTemplateIdentity(d) ||
          (d.kind=="CXXRecord" && HasCxxConcreteMemberTemplateIdentity(d));});
  if (has_concrete_nested_templates!=concrete_nested_capability || !concrete_nested_owners ||
      (concrete_nested_capability && (!concrete_member_capability || !concrete_nested_class || !concrete_nested_patterns)))
    Add(result,"O3IFC063","concrete nested template identity and required record/class/nested capabilities must agree",expected.canonical_name);
  const bool has_variadic_templates = std::any_of(
      sidecar.declarations.begin(),sidecar.declarations.end(),HasCxxVariadicTemplateIdentity);
  const bool variadic_template_capability = std::find(sidecar.required_semantics.begin(),
      sidecar.required_semantics.end(),kCxxVariadicTemplateSemantic)!=sidecar.required_semantics.end();
  const bool variadic_template_owner = std::all_of(sidecar.declarations.begin(),sidecar.declarations.end(),
      [](const auto &d) { return !HasCxxVariadicTemplateIdentity(d) ||
          (d.kind=="FunctionTemplate" || d.kind=="ClassTemplate" ||
           (d.kind=="CXXRecord" && HasCxxConcreteMemberTemplateIdentity(d))); });
  if (has_variadic_templates!=variadic_template_capability ||
      (variadic_template_capability && !variadic_template_owner))
    Add(result,"O3IFC061","variadic template identity and required capability must agree",expected.canonical_name);
  const bool has_nested_templates = std::any_of(
      sidecar.declarations.begin(),sidecar.declarations.end(),HasCxxNestedTemplateIdentity);
  const bool nested_template_capability = std::find(sidecar.required_semantics.begin(),
      sidecar.required_semantics.end(),kCxxNestedTemplateSemantic)!=sidecar.required_semantics.end();
  const bool nested_template_class = std::find(sidecar.required_semantics.begin(),
      sidecar.required_semantics.end(),kCxxClassTemplateSemantic)!=sidecar.required_semantics.end();
  if (has_nested_templates!=nested_template_capability ||
      (nested_template_capability && !nested_template_class))
    Add(result,"O3IFC060","nested template identity and required nested/class capabilities must agree",expected.canonical_name);
  const bool has_member_alias_templates = std::any_of(
      sidecar.declarations.begin(),sidecar.declarations.end(),HasCxxMemberAliasTemplateIdentity);
  const bool member_alias_capability = std::find(sidecar.required_semantics.begin(),
      sidecar.required_semantics.end(),kCxxMemberAliasTemplateSemantic)!=sidecar.required_semantics.end();
  const bool member_alias_class = std::find(sidecar.required_semantics.begin(),
      sidecar.required_semantics.end(),kCxxClassTemplateSemantic)!=sidecar.required_semantics.end();
  const bool member_alias_alias = std::find(sidecar.required_semantics.begin(),
      sidecar.required_semantics.end(),kCxxAliasTemplateSemantic)!=sidecar.required_semantics.end();
  if (has_member_alias_templates!=member_alias_capability ||
      (member_alias_capability && ((!member_alias_class && !concrete_member_capability) || !member_alias_alias)))
    Add(result,"O3IFC059","member alias template identity and required member/class/alias capabilities must agree",expected.canonical_name);
  const bool has_member_templates = std::any_of(
      sidecar.declarations.begin(),sidecar.declarations.end(),HasCxxMemberTemplateIdentity);
  const bool member_template_capability = std::find(sidecar.required_semantics.begin(),
      sidecar.required_semantics.end(),kCxxMemberTemplateSemantic)!=sidecar.required_semantics.end();
  const bool member_template_class = std::find(sidecar.required_semantics.begin(),
      sidecar.required_semantics.end(),kCxxClassTemplateSemantic)!=sidecar.required_semantics.end();
  const bool member_template_data = std::find(sidecar.required_semantics.begin(),
      sidecar.required_semantics.end(),"objc3.cxx-data-interfaces.v1")!=sidecar.required_semantics.end();
  if (has_member_templates!=member_template_capability ||
      (member_template_capability && ((!member_template_class && !concrete_member_capability) || !member_template_data)))
    Add(result,"O3IFC058","member template identity and required member/class/data capabilities must agree",expected.canonical_name);
  const bool has_template_deduction = std::any_of(
      sidecar.declarations.begin(),sidecar.declarations.end(),HasCxxTemplateDeductionIdentity);
  const bool template_deduction_capability = std::find(sidecar.required_semantics.begin(),
      sidecar.required_semantics.end(),kCxxTemplateDeductionSemantic)!=sidecar.required_semantics.end();
  const bool template_deduction_data = std::find(sidecar.required_semantics.begin(),
      sidecar.required_semantics.end(),"objc3.cxx-data-interfaces.v1")!=sidecar.required_semantics.end();
  if (has_template_deduction!=template_deduction_capability || (template_deduction_capability && !template_deduction_data))
    Add(result,"O3IFC056","template deduction identity and required deduction/data capabilities must agree",expected.canonical_name);
  const bool has_template_structure = std::any_of(
      sidecar.declarations.begin(), sidecar.declarations.end(), HasCxxTemplateStructureIdentity);
  const bool template_structure_capability = std::find(sidecar.required_semantics.begin(),
      sidecar.required_semantics.end(), kCxxTemplateStructureSemantic) != sidecar.required_semantics.end();
  const bool template_structure_data = std::find(sidecar.required_semantics.begin(),
      sidecar.required_semantics.end(), "objc3.cxx-data-interfaces.v1") != sidecar.required_semantics.end();
  const bool has_template_data = std::any_of(sidecar.declarations.begin(),sidecar.declarations.end(),
      [](const auto &declaration) {return HasCxxClassTemplateIdentity(declaration) || HasCxxVariableTemplateIdentity(declaration);});
  if (has_template_structure != template_structure_capability ||
      has_template_data != template_structure_capability || (template_structure_capability && !template_structure_data))
    Add(result,"O3IFC055","template structure identity and required structure/data capabilities must agree",expected.canonical_name);
  const bool has_class_templates = std::any_of(
      sidecar.declarations.begin(), sidecar.declarations.end(), HasCxxClassTemplateIdentity);
  const bool class_capability = std::find(sidecar.required_semantics.begin(),
      sidecar.required_semantics.end(), kCxxClassTemplateSemantic) != sidecar.required_semantics.end();
  const bool class_data_capability = std::find(sidecar.required_semantics.begin(),
      sidecar.required_semantics.end(), "objc3.cxx-data-interfaces.v1") != sidecar.required_semantics.end();
  if (has_class_templates != class_capability || (class_capability && !class_data_capability))
    Add(result,"O3IFC054","class template identity and required class/data capabilities must agree",expected.canonical_name);
  const bool has_variable_templates = std::any_of(
      sidecar.declarations.begin(), sidecar.declarations.end(), HasCxxVariableTemplateIdentity);
  const bool variable_capability = std::find(sidecar.required_semantics.begin(),
      sidecar.required_semantics.end(), kCxxVariableTemplateSemantic) != sidecar.required_semantics.end();
  const bool variable_data_capability = std::find(sidecar.required_semantics.begin(),
      sidecar.required_semantics.end(), "objc3.cxx-data-interfaces.v1") != sidecar.required_semantics.end();
  if (has_variable_templates != variable_capability || (variable_capability && !variable_data_capability))
    Add(result,"O3IFC053","variable template identity and required variable/data capabilities must agree",expected.canonical_name);
  const bool has_alias_templates = std::any_of(
      sidecar.declarations.begin(), sidecar.declarations.end(), HasCxxAliasTemplateIdentity);
  const bool alias_capability = std::find(sidecar.required_semantics.begin(),
      sidecar.required_semantics.end(), kCxxAliasTemplateSemantic) != sidecar.required_semantics.end();
  const bool alias_data_capability = std::find(sidecar.required_semantics.begin(),
      sidecar.required_semantics.end(), "objc3.cxx-data-interfaces.v1") != sidecar.required_semantics.end();
  if (has_alias_templates != alias_capability || (alias_capability && !alias_data_capability))
    Add(result,"O3IFC052","alias template identity and required alias/data capabilities must agree",expected.canonical_name);
  const bool has_function_templates = std::any_of(
      sidecar.declarations.begin(), sidecar.declarations.end(), HasCxxFunctionTemplateIdentity);
  const bool template_capability = std::find(sidecar.required_semantics.begin(),
      sidecar.required_semantics.end(), kCxxFunctionTemplateSemantic) != sidecar.required_semantics.end();
  const bool template_data_capability = std::find(sidecar.required_semantics.begin(),
      sidecar.required_semantics.end(), "objc3.cxx-data-interfaces.v1") != sidecar.required_semantics.end();
  if (has_function_templates != template_capability || (template_capability && !template_data_capability))
    Add(result,"O3IFC051","function template identity and required template/data capabilities must agree",expected.canonical_name);
  const bool has_concepts = std::any_of(
      sidecar.declarations.begin(), sidecar.declarations.end(), HasCxxConceptIdentity);
  const bool concept_capability = std::find(
      sidecar.required_semantics.begin(), sidecar.required_semantics.end(),
      kCxxConceptSemantic) != sidecar.required_semantics.end();
  const bool concept_data_capability = std::find(
      sidecar.required_semantics.begin(), sidecar.required_semantics.end(),
      "objc3.cxx-data-interfaces.v1") != sidecar.required_semantics.end();
  if (has_concepts != concept_capability || (concept_capability && !concept_data_capability))
    Add(result,"O3IFC050","concept identity and required concept/data capabilities must agree",expected.canonical_name);
  const bool has_union_values = std::any_of(
      sidecar.declarations.begin(), sidecar.declarations.end(), HasCxxUnionValueIdentity);
  const bool union_capability = std::find(
      sidecar.required_semantics.begin(), sidecar.required_semantics.end(),
      kCxxUnionValueSemantic) != sidecar.required_semantics.end();
  const bool union_data_capability = std::find(
      sidecar.required_semantics.begin(), sidecar.required_semantics.end(),
      "objc3.cxx-data-interfaces.v1") != sidecar.required_semantics.end();
  if (has_union_values != union_capability || (union_capability && !union_data_capability))
    Add(result,"O3IFC049","C++ union identity and required data/union capabilities must agree",expected.canonical_name);
  const bool has_generic_local_lambdas = std::any_of(
      sidecar.declarations.begin(),sidecar.declarations.end(),HasCxxGenericLocalLambdaIdentity);
  const bool generic_local_capability = std::find(
      sidecar.required_semantics.begin(),sidecar.required_semantics.end(),
      kCxxGenericLocalLambdaSemantic) != sidecar.required_semantics.end();
  const bool generic_lambda_base = std::find(
      sidecar.required_semantics.begin(),sidecar.required_semantics.end(),
      kCxxLambdaExpressionSemantic) != sidecar.required_semantics.end();
  if (has_generic_local_lambdas != generic_local_capability ||
      (generic_local_capability && !generic_lambda_base))
    Add(result,"O3IFC048","generic local lambda identity and required lambda capabilities must agree",expected.canonical_name);
  const bool has_unnamed_local_types = std::any_of(
      sidecar.declarations.begin(),sidecar.declarations.end(),HasCxxUnnamedLocalTypeIdentity);
  const bool unnamed_local_capability = std::find(
      sidecar.required_semantics.begin(),sidecar.required_semantics.end(),
      kCxxUnnamedLocalTypeSemantic) != sidecar.required_semantics.end();
  const bool has_local_types = std::any_of(
      sidecar.declarations.begin(), sidecar.declarations.end(), HasCxxLocalTypeIdentity);
  const bool local_types_capability = std::find(
      sidecar.required_semantics.begin(), sidecar.required_semantics.end(),
      kCxxLocalTypeSemantic) != sidecar.required_semantics.end();
  if (has_local_types != local_types_capability)
    Add(result, "O3IFC043", "C++ local type identity and required capability must agree", expected.canonical_name);
  if (has_unnamed_local_types != unnamed_local_capability ||
      (unnamed_local_capability && !local_types_capability))
    Add(result,"O3IFC047","C++ unnamed local type identity and required local-type capabilities must agree",expected.canonical_name);
  const bool has_cxx_members = std::any_of(
      sidecar.declarations.begin(), sidecar.declarations.end(),
      [](const auto &d) { return IsCxxMemberDeclarationKind(d.kind); });
  if (has_cxx_members != std::binary_search(required.begin(), required.end(),
                                            "objc3.cxx-member-interfaces.v1"))
    Add(result, "O3IFC021",
        "C++ member declarations and required capability must agree",
        expected.canonical_name);
  const bool has_spi =
      std::any_of(sidecar.declarations.begin(), sidecar.declarations.end(),
                  [](const auto &d) { return !d.spi_group.empty(); });
  if (has_spi != std::binary_search(required.begin(), required.end(),
                                    "objc3.api-partitions.v1"))
    Add(result, "O3IFC019", "SPI metadata and required capability must agree",
        expected.canonical_name);
  for (const auto &d : sidecar.declarations)
    if (!d.spi_group.empty() && !IsValidSPIGroup(d.spi_group))
      Add(result, "O3IFC019", "invalid SPI group", d.stable_id);
  const bool has_names =
      std::any_of(sidecar.declarations.begin(), sidecar.declarations.end(),
                  [](const auto &d) { return !d.native_symbol_name.empty(); });
  if (has_names != std::binary_search(required.begin(), required.end(),
                                      "objc3.module-owned-names.v1"))
    Add(result, "O3IFC020",
        "module-owned names and required capability must agree",
        expected.canonical_name);
  for (const auto &d : sidecar.declarations)
    if (!HasValidModuleOwnedName(sidecar.module, d))
      Add(result, "O3IFC020",
          "native symbol name differs from its module ownership contract",
          d.stable_id);
  if (std::binary_search(required.begin(), required.end(), "objc3.keypath-subscripts.v1") &&
      !std::binary_search(required.begin(), required.end(), "objc3.typed-keypaths.v1"))
    Add(result, "O3IFC046", "key-path subscripts are missing their typed key-path capability");
  const bool effect_semantic = std::binary_search(
      required.begin(), required.end(), "objc3.canonical-function-effects.v1");
  for (const auto &declaration : sidecar.declarations)
    if (HasEffects(declaration.effects) && !effect_semantic)
      Add(result, "O3IFC012",
          "effect-bearing declaration is missing the canonical "
          "function-effects semantic capability",
          declaration.stable_id);
  const bool return_ownership_semantic = std::binary_search(
      required.begin(), required.end(), "objc3.native-return-ownership.v1");
  const bool direct_semantic = std::binary_search(
      required.begin(), required.end(), "objc3.direct-dispatch.v1");
  for (const auto &declaration : sidecar.declarations)
    if (declaration.kind == "ObjCMethod" &&
        declaration.canonical_type_identity.find(";dispatch=direct") !=
            std::string::npos &&
        !direct_semantic)
      Add(result, "O3IFC015",
          "direct method is missing its semantic capability",
          declaration.stable_id);
  for (const auto &declaration : sidecar.declarations)
    if (declaration.effects.ownership_identity.starts_with("native-return:") &&
        !return_ownership_semantic)
      Add(result, "O3IFC014",
          "managed return ownership is missing its semantic capability",
          declaration.stable_id);
  for (const auto &declaration : sidecar.declarations) {
    const auto &effects = declaration.effects;
    if (!IsValidAsyncABIIdentity(effects))
      Add(result, "O3IFC025",
          "invalid or missing physical async entry ABI identity",
          declaration.stable_id);
    if (!effects.async_abi_identity.empty() &&
        !std::binary_search(required.begin(), required.end(),
                            kAsyncEntryABISemantic))
      Add(result, "O3IFC025",
          "async entry ABI is missing its semantic capability",
          declaration.stable_id);
    if (effects.async_abi_identity.find("10:c-frame-v1") != std::string::npos &&
        std::none_of(
            required.begin(), required.end(), [](const auto &semantic) {
              return semantic.starts_with("objc3.sdk-runtime-frames.v4.");
            }))
      Add(result, "O3IFC025",
          "C frame entry ABI is missing its SDK header contract",
          declaration.stable_id);
    if (effects.ownership_identity.find("native-contracts:v1:") !=
            std::string::npos &&
        !std::binary_search(required.begin(), required.end(),
                            "objc3.native-ownership-contracts.v1"))
      Add(result, "O3IFC022",
          "native ownership contract is missing its semantic capability",
          declaration.stable_id);
    if (declaration.canonical_type_identity.find("KeyPath<") !=
            std::string::npos &&
        !std::binary_search(required.begin(), required.end(),
                            "objc3.typed-keypaths.v1"))
      Add(result, "O3IFC024",
          "typed key path is missing its semantic capability",
          declaration.stable_id);
    if ((!effects.actor_isolation_identity.empty() ||
         !effects.executor_affinity_identity.empty()) &&
        !std::binary_search(required.begin(), required.end(),
                            "objc3.isolation-contracts.v1"))
      Add(result, "O3IFC023",
          "native isolation contract is missing its semantic capability",
          declaration.stable_id);
  }
  const bool carrier_semantic = std::binary_search(
      required.begin(), required.end(), "objc3.native-carrier-types.v1");
  for (const auto &declaration : sidecar.declarations)
    if ((declaration.canonical_type_identity.find("Optional(") !=
             std::string::npos ||
         declaration.canonical_type_identity.find("Result(") !=
             std::string::npos) &&
        !carrier_semantic)
      Add(result, "O3IFC013",
          "carrier declaration is missing the native carrier type semantic "
          "capability",
          declaration.stable_id);
  for (const auto &dep : sidecar.dependencies)
    if (dep.module.canonical_name.empty() ||
        dep.module.semantic_fingerprint.empty() ||
        dep.module.target_triple != sidecar.module.target_triple ||
        dep.module.runtime_abi != sidecar.module.runtime_abi ||
        dep.module.extension_abi != sidecar.module.extension_abi ||
        dep.module.language_revision != sidecar.module.language_revision)
      Add(result, "O3IFC011",
          "dependency identity is incomplete or ABI-incompatible",
          dep.module.canonical_name);
  return result;
}

} // namespace objc3::clang_foundation
