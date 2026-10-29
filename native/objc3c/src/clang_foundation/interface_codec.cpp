#include "clang_foundation/interface_codec.h"
#include "clang_foundation/runtime_interface_types.h"
#include "clang_foundation/module_names.h"

#include <algorithm>
#include <array>
#include <cstdint>
#include <set>
#include <string>
#include <system_error>
#include <tuple>
#include <utility>
#include <vector>

#include "llvm/Support/JSON.h"
#include "llvm/Support/FormatVariadic.h"
#include "llvm/Support/SHA256.h"
#include "llvm/Support/Error.h"
#include "llvm/Support/raw_ostream.h"

namespace objc3::clang_foundation {
namespace {
using llvm::json::Array;
using llvm::json::Object;
using llvm::json::Value;

constexpr std::size_t kMaxSidecarBytes = 32u * 1024u * 1024u;
constexpr std::size_t kMaxDeclarations = 100000u;
constexpr std::size_t kMaxDependencies = 8192u;
constexpr std::size_t kMaxRequiredSemantics = 512u;
constexpr std::size_t kMaxStringBytes = 65536u;

llvm::Error Invalid(llvm::StringRef message) {
  return llvm::createStringError(std::errc::invalid_argument, "%s",
                                 message.str().c_str());
}

bool IsSha256(llvm::StringRef value) {
  if (value.size() != 64) return false;
  return std::all_of(value.begin(), value.end(), [](unsigned char c) {
    return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f');
  });
}

llvm::Expected<std::string> RequiredString(const Object &object,
                                           llvm::StringRef key,
                                           llvm::StringRef path) {
  auto value = object.getString(key);
  if (!value) return Invalid(llvm::formatv("{0}/{1}: required string missing or has wrong type", path, key).str());
  if (value->empty() || value->size() > kMaxStringBytes)
    return Invalid(llvm::formatv("{0}/{1}: string is empty or exceeds limit", path, key).str());
  return value->str();
}

llvm::Expected<std::string> RequiredStringAllowEmpty(const Object &object,
                                                    llvm::StringRef key,
                                                    llvm::StringRef path) {
  auto value = object.getString(key);
  if (!value) return Invalid(llvm::formatv("{0}/{1}: required string missing or has wrong type", path, key).str());
  if (value->size() > kMaxStringBytes)
    return Invalid(llvm::formatv("{0}/{1}: string exceeds limit", path, key).str());
  return value->str();
}

llvm::Expected<bool> RequiredBool(const Object &object, llvm::StringRef key,
                                  llvm::StringRef path) {
  auto value = object.getBoolean(key);
  if (!value) return Invalid(llvm::formatv("{0}/{1}: required boolean missing or has wrong type", path, key).str());
  return *value;
}

llvm::Expected<std::uint64_t> RequiredUnsigned(const Object &object,
                                               llvm::StringRef key,
                                               llvm::StringRef path) {
  const Value *value = object.get(key);
  if (!value) return Invalid(llvm::formatv("{0}/{1}: required integer missing", path, key).str());
  auto number = value->getAsInteger();
  if (!number || *number < 0)
    return Invalid(llvm::formatv("{0}/{1}: expected nonnegative integer", path, key).str());
  return static_cast<std::uint64_t>(*number);
}

llvm::Expected<const Object *> RequiredObject(const Object &object,
                                              llvm::StringRef key,
                                              llvm::StringRef path) {
  auto *value = object.getObject(key);
  if (!value) return Invalid(llvm::formatv("{0}/{1}: required object missing or has wrong type", path, key).str());
  return value;
}

llvm::Expected<const Array *> RequiredArray(const Object &object,
                                            llvm::StringRef key,
                                            llvm::StringRef path,
                                            std::size_t maximum) {
  auto *value = object.getArray(key);
  if (!value) return Invalid(llvm::formatv("{0}/{1}: required array missing or has wrong type", path, key).str());
  if (value->size() > maximum)
    return Invalid(llvm::formatv("{0}/{1}: array exceeds item limit", path, key).str());
  return value;
}

llvm::Error CheckKeys(const Object &object,
                      std::initializer_list<llvm::StringRef> allowed,
                      llvm::StringRef path) {
  std::set<std::string> known;
  for (auto key : allowed) known.insert(key.str());
  for (const auto &entry : object)
    if (!known.count(entry.first.str()))
      return Invalid(llvm::formatv("{0}: unknown field '{1}'", path, entry.first).str());
  return llvm::Error::success();
}

Object EncodeModule(const InterfaceModuleIdentity &module) {
  return Object{{"canonical_name", module.canonical_name},
                {"semantic_fingerprint", module.semantic_fingerprint},
                {"target_triple", module.target_triple},
                {"runtime_abi", module.runtime_abi},
                {"extension_abi", module.extension_abi},
                {"language_revision", module.language_revision},
                {"language_profile", module.language_profile},
                {"package_identity", module.package_identity}};
}

llvm::Expected<InterfaceModuleIdentity> DecodeModule(const Object &object,
                                                     llvm::StringRef path) {
  if (auto error = CheckKeys(object,
      {"canonical_name", "semantic_fingerprint", "target_triple", "runtime_abi",
       "extension_abi", "language_revision", "language_profile", "package_identity"}, path))
    return std::move(error);
  InterfaceModuleIdentity result;
#define READ_MODULE_FIELD(field, key)                                      \
  do {                                                                      \
    auto value = RequiredString(object, key, path);                         \
    if (!value) return value.takeError();                                   \
    result.field = std::move(*value);                                       \
  } while (false)
  READ_MODULE_FIELD(canonical_name, "canonical_name");
  READ_MODULE_FIELD(semantic_fingerprint, "semantic_fingerprint");
  READ_MODULE_FIELD(target_triple, "target_triple");
  READ_MODULE_FIELD(runtime_abi, "runtime_abi");
  READ_MODULE_FIELD(extension_abi, "extension_abi");
  READ_MODULE_FIELD(language_revision, "language_revision");
  READ_MODULE_FIELD(language_profile, "language_profile");
  READ_MODULE_FIELD(package_identity, "package_identity");
#undef READ_MODULE_FIELD
  return result;
}

Object EncodeEffects(const InterfaceEffectIdentity &effects) {
  return Object{{"async", effects.is_async},
                {"async_abi_identity", effects.async_abi_identity},
                {"throws", effects.is_throwing},
                {"throws_kind", effects.throws_kind},
                {"error_type_identity", effects.error_type_identity},
                {"actor_isolation_identity", effects.actor_isolation_identity},
                {"executor_affinity_identity", effects.executor_affinity_identity},
                {"ownership_identity", effects.ownership_identity}};
}

llvm::Expected<InterfaceEffectIdentity> DecodeEffects(const Object &object,
                                                       llvm::StringRef path) {
  if (auto error = CheckKeys(object,
      {"async", "async_abi_identity", "throws", "throws_kind", "error_type_identity",
       "actor_isolation_identity", "executor_affinity_identity", "ownership_identity"}, path))
    return std::move(error);
  InterfaceEffectIdentity result;
  auto async = RequiredBool(object, "async", path);
  if (!async) return async.takeError();
  result.is_async = *async;
  auto throwing = RequiredBool(object, "throws", path);
  if (!throwing) return throwing.takeError();
  result.is_throwing = *throwing;
#define READ_EFFECT_FIELD(field, key)                                       \
  do {                                                                      \
    auto value = RequiredStringAllowEmpty(object, key, path);               \
    if (!value) return value.takeError();                                   \
    result.field = std::move(*value);                                       \
  } while (false)
  READ_EFFECT_FIELD(throws_kind, "throws_kind");
  READ_EFFECT_FIELD(error_type_identity, "error_type_identity");
  READ_EFFECT_FIELD(actor_isolation_identity, "actor_isolation_identity");
  READ_EFFECT_FIELD(executor_affinity_identity, "executor_affinity_identity");
  READ_EFFECT_FIELD(ownership_identity, "ownership_identity");
  READ_EFFECT_FIELD(async_abi_identity, "async_abi_identity");
#undef READ_EFFECT_FIELD
  return result;
}

Object EncodeDeclaration(const InterfaceDeclarationIdentity &declaration) {
  Object result{
      {"stable_id", declaration.stable_id},
      {"kind", declaration.kind},
      {"qualified_name", declaration.qualified_name},
      {"export_root", declaration.export_root},
      {"canonical_type_identity", declaration.canonical_type_identity},
      {"effects", EncodeEffects(declaration.effects)}};
  if (!declaration.native_symbol_name.empty())
    result["native_symbol_name"] = declaration.native_symbol_name;
  if (!declaration.spi_group.empty())
    result["spi_group"] = declaration.spi_group;
  return result;
}

llvm::Expected<InterfaceDeclarationIdentity> DecodeDeclaration(
    const Value &value, std::size_t index) {
  const auto path = llvm::formatv("/declarations/{0}", index).str();
  const auto *object = value.getAsObject();
  if (!object) return Invalid(llvm::formatv("{0}: expected object", path).str());
  if (auto error = CheckKeys(*object,
                             {"stable_id", "kind", "qualified_name", "export_root",
                              "canonical_type_identity", "effects", "spi_group",
                              "native_symbol_name"},
                             path))
    return std::move(error);
  InterfaceDeclarationIdentity result;
  if (object->get("native_symbol_name")) {
    auto name = RequiredString(*object, "native_symbol_name", path);
    if (!name)
      return name.takeError();
    result.native_symbol_name = std::move(*name);
  }
  if (object->get("spi_group")) {
    auto group = RequiredString(*object, "spi_group", path);
    if (!group)
      return group.takeError();
    if (!IsValidSPIGroup(*group))
      return Invalid("invalid SPI group");
    result.spi_group = std::move(*group);
  }
#define READ_DECL_FIELD(field, key)                                        \
  do {                                                                      \
    auto text = RequiredString(*object, key, path);                         \
    if (!text) return text.takeError();                                     \
    result.field = std::move(*text);                                        \
  } while (false)
  READ_DECL_FIELD(stable_id, "stable_id");
  READ_DECL_FIELD(kind, "kind");
  READ_DECL_FIELD(qualified_name, "qualified_name");
  READ_DECL_FIELD(export_root, "export_root");
  READ_DECL_FIELD(canonical_type_identity, "canonical_type_identity");
#undef READ_DECL_FIELD
  auto *effects = object->getObject("effects");
  if (!effects) return Invalid(llvm::formatv("{0}/effects: required object missing or has wrong type", path).str());
  auto parsed_effects = DecodeEffects(*effects, path + "/effects");
  if (!parsed_effects) return parsed_effects.takeError();
  result.effects = std::move(*parsed_effects);
  return result;
}

Object EncodeDependency(const InterfaceDependency &dependency) {
  return Object{{"module", EncodeModule(dependency.module)},
                {"reexported", dependency.reexported},
                {"public_import", dependency.public_import}};
}

llvm::Expected<InterfaceDependency> DecodeDependency(const Value &value,
                                                      std::size_t index) {
  const auto path = llvm::formatv("/dependencies/{0}", index).str();
  const auto *object = value.getAsObject();
  if (!object) return Invalid(llvm::formatv("{0}: expected object", path).str());
  if (auto error = CheckKeys(*object, {"module", "reexported", "public_import"}, path))
    return std::move(error);
  auto *module_object = object->getObject("module");
  if (!module_object) return Invalid(llvm::formatv("{0}/module: required object missing or has wrong type", path).str());
  auto module = DecodeModule(*module_object, path + "/module");
  if (!module) return module.takeError();
  auto reexported = RequiredBool(*object, "reexported", path);
  if (!reexported) return reexported.takeError();
  auto is_public = RequiredBool(*object, "public_import", path);
  if (!is_public) return is_public.takeError();
  return InterfaceDependency{std::move(*module), *reexported, *is_public};
}

std::set<std::string> Supported(llvm::ArrayRef<llvm::StringRef> values) {
  std::set<std::string> result;
  for (auto value : values) result.insert(value.str());
  return result;
}

llvm::Error ValidateForEncoding(
    const SemanticInterfaceSidecar &sidecar,
    const std::set<std::string> &supported) {
  auto valid_text = [](const std::string &value) {
    return value.size() <= kMaxStringBytes && llvm::json::isUTF8(value);
  };
  if (sidecar.schema_major != kInterfaceSchemaMajor ||
      sidecar.schema_minor != kInterfaceSchemaMinor)
    return Invalid("unsupported semantic interface schema version");
  if (!IsSha256(sidecar.canonical_source_sha256))
    return Invalid("source binding must be a lowercase SHA256 digest");
  if (!valid_text(sidecar.ownership_projection))
    return Invalid(
        "ownership projection exceeds string bound or is invalid UTF-8");
  const bool projected = std::find(sidecar.required_semantics.begin(),
                                   sidecar.required_semantics.end(),
                                   "objc3.ownership-projection.v1") !=
                         sidecar.required_semantics.end();
  if (projected != !sidecar.ownership_projection.empty())
    return Invalid("ownership projection provenance and capability must agree");
  const bool has_availability =
      std::any_of(sidecar.declarations.begin(), sidecar.declarations.end(),
                  [](const auto &declaration) {
                    return declaration.canonical_type_identity.find(
                               ";api-availability=") != std::string::npos;
                  });
  const bool availability_capability =
      std::find(
          sidecar.required_semantics.begin(), sidecar.required_semantics.end(),
          "objc3.api-availability.v1") != sidecar.required_semantics.end();
  if (has_availability != availability_capability)
    return Invalid("native availability metadata and capability must agree");
  const bool has_spi =
      std::any_of(sidecar.declarations.begin(), sidecar.declarations.end(),
                  [](const auto &d) { return !d.spi_group.empty(); });
  const bool spi_capability =
      std::find(sidecar.required_semantics.begin(),
                sidecar.required_semantics.end(),
                "objc3.api-partitions.v1") != sidecar.required_semantics.end();
  if (has_spi != spi_capability)
    return Invalid("SPI metadata and required capability must agree");
  for (const auto &d : sidecar.declarations)
    if (!d.spi_group.empty() && !IsValidSPIGroup(d.spi_group))
      return Invalid("invalid SPI group");
  const bool has_names =
      std::any_of(sidecar.declarations.begin(), sidecar.declarations.end(),
                  [](const auto &d) { return !d.native_symbol_name.empty(); });
  const bool has_lifecycle =
      std::any_of(sidecar.declarations.begin(), sidecar.declarations.end(),
                  HasCxxClassLifecycleIdentity);
  const bool lifecycle_capability =
      std::find(sidecar.required_semantics.begin(),
                sidecar.required_semantics.end(),
                kCxxClassLifecycleSemantic) != sidecar.required_semantics.end();
  if (has_lifecycle != lifecycle_capability)
    return Invalid(
        "C++ class lifecycle declarations and required capability must agree");
  const bool has_structure =
      std::any_of(sidecar.declarations.begin(), sidecar.declarations.end(),
                  HasCxxClassStructureIdentity);
  const bool structure_capability =
      std::find(sidecar.required_semantics.begin(),
                sidecar.required_semantics.end(),
                kCxxClassStructureSemantic) != sidecar.required_semantics.end();
  if (has_structure != structure_capability ||
      (has_structure && !has_lifecycle))
    return Invalid("C++ class structure declarations require matching "
                   "structure and lifecycle capabilities");
  const bool has_dispatch =
      std::any_of(sidecar.declarations.begin(), sidecar.declarations.end(),
                  HasCxxClassDispatchIdentity);
  const bool dispatch_capability =
      std::find(sidecar.required_semantics.begin(),
                sidecar.required_semantics.end(),
                kCxxClassDispatchSemantic) != sidecar.required_semantics.end();
  if (has_dispatch != dispatch_capability ||
      (has_dispatch && !has_lifecycle))
    return Invalid("C++ class dispatch declarations require matching dispatch "
                   "and lifecycle capabilities");
  const bool has_namespaces =
      std::any_of(sidecar.declarations.begin(), sidecar.declarations.end(),
                  HasCxxNamespaceIdentity);
  const bool namespace_capability =
      std::find(sidecar.required_semantics.begin(),
                sidecar.required_semantics.end(),
                kCxxNamespaceSemantic) != sidecar.required_semantics.end();
  if (has_namespaces != namespace_capability)
    return Invalid("C++ namespace identity and required capability must agree");
  const bool has_nested = std::any_of(sidecar.declarations.begin(),
      sidecar.declarations.end(), HasCxxNestedTypeIdentity);
  const bool nested_capability =
      std::find(sidecar.required_semantics.begin(), sidecar.required_semantics.end(),
                kCxxNestedTypeSemantic) != sidecar.required_semantics.end();
  if (has_nested != nested_capability)
    return Invalid("C++ nested type identity and required capability must agree");
  const bool has_using=std::any_of(sidecar.declarations.begin(),sidecar.declarations.end(),HasCxxUsingIdentity);
  const bool using_capability=std::find(sidecar.required_semantics.begin(),sidecar.required_semantics.end(),
                                     kCxxUsingSemantic)!=sidecar.required_semantics.end();
  if(has_using!=using_capability) return Invalid("C++ using identity and required capability must agree");
  const bool has_expressions = std::any_of(
      sidecar.declarations.begin(), sidecar.declarations.end(),
      HasCxxDeclarationExpressionIdentity);
  const bool expression_capability = std::find(
      sidecar.required_semantics.begin(), sidecar.required_semantics.end(),
      kCxxDeclarationExpressionSemantic) != sidecar.required_semantics.end();
  if (has_expressions != expression_capability)
    return Invalid("C++ declaration expression identity and required capability must agree");
  const bool has_definitions = std::any_of(
      sidecar.declarations.begin(), sidecar.declarations.end(), HasCxxDefinitionIdentity);
  const bool definition_capability = std::find(
      sidecar.required_semantics.begin(), sidecar.required_semantics.end(),
      kCxxDefinitionSemantic) != sidecar.required_semantics.end();
  if (has_definitions != definition_capability)
    return Invalid("C++ definition identity and required capability must agree");
  const bool has_comparisons = std::any_of(
      sidecar.declarations.begin(), sidecar.declarations.end(), HasCxxDefaultedComparisonIdentity);
  const bool comparison_capability = std::find(
      sidecar.required_semantics.begin(), sidecar.required_semantics.end(),
      kCxxDefaultedComparisonSemantic) != sidecar.required_semantics.end();
  if (has_comparisons != comparison_capability)
    return Invalid("C++ defaulted comparison identity and required capability must agree");
  const bool has_out_of_line_defaulting = std::any_of(
      sidecar.declarations.begin(), sidecar.declarations.end(), HasCxxOutOfLineDefaultingIdentity);
  const bool defaulting_capability = std::find(
      sidecar.required_semantics.begin(), sidecar.required_semantics.end(),
      kCxxOutOfLineDefaultingSemantic) != sidecar.required_semantics.end();
  if (has_out_of_line_defaulting != defaulting_capability)
    return Invalid("C++ out-of-line defaulting identity and required capability must agree");
  const bool has_immediate_defaulting = std::any_of(
      sidecar.declarations.begin(), sidecar.declarations.end(), HasCxxImmediateDefaultingIdentity);
  const bool immediate_capability = std::find(
      sidecar.required_semantics.begin(), sidecar.required_semantics.end(),
      kCxxImmediateDefaultingSemantic) != sidecar.required_semantics.end();
  if (has_immediate_defaulting != immediate_capability)
    return Invalid("C++ immediate defaulting identity and required capability must agree");
  const bool has_namespace_attributes = std::any_of(
      sidecar.declarations.begin(), sidecar.declarations.end(), HasCxxNamespaceAttributeIdentity);
  const bool namespace_attribute_capability = std::find(
      sidecar.required_semantics.begin(), sidecar.required_semantics.end(),
      kCxxNamespaceAttributeSemantic) != sidecar.required_semantics.end();
  if (has_namespace_attributes != namespace_attribute_capability)
    return Invalid("C++ namespace attribute identity and required capability must agree");
  const bool has_lambdas = std::any_of(
      sidecar.declarations.begin(), sidecar.declarations.end(), HasCxxLambdaExpressionIdentity);
  const bool lambda_capability = std::find(
      sidecar.required_semantics.begin(), sidecar.required_semantics.end(),
      kCxxLambdaExpressionSemantic) != sidecar.required_semantics.end();
  if (has_lambdas != lambda_capability)
    return Invalid("C++ lambda identity and required capability must agree");
  const bool has_thread_local = std::any_of(
      sidecar.declarations.begin(), sidecar.declarations.end(), HasCxxThreadLocalIdentity);
  const bool thread_local_capability = std::find(
      sidecar.required_semantics.begin(), sidecar.required_semantics.end(),
      kCxxThreadLocalSemantic) != sidecar.required_semantics.end();
  if (has_thread_local != thread_local_capability)
    return Invalid("C++ thread-local identity and required capability must agree");
  const bool has_friends = std::any_of(
      sidecar.declarations.begin(), sidecar.declarations.end(), HasCxxFriendIdentity);
  const bool friend_capability = std::find(
      sidecar.required_semantics.begin(), sidecar.required_semantics.end(),
      kCxxFriendSemantic) != sidecar.required_semantics.end();
  if (has_friends != friend_capability)
    return Invalid("C++ friend identity and required capability must agree");
  const bool has_blocks = std::any_of(
      sidecar.declarations.begin(), sidecar.declarations.end(), HasCxxBlockExpressionIdentity);
  const bool blocks_capability = std::find(
      sidecar.required_semantics.begin(), sidecar.required_semantics.end(),
      kCxxBlockExpressionSemantic) != sidecar.required_semantics.end();
  if (has_blocks != blocks_capability)
    return Invalid("C++ blocks expression identity and required capability must agree");
  const bool has_statements = std::any_of(
      sidecar.declarations.begin(), sidecar.declarations.end(), HasCxxStatementExpressionIdentity);
  const bool statements_capability = std::find(
      sidecar.required_semantics.begin(), sidecar.required_semantics.end(),
      kCxxStatementExpressionSemantic) != sidecar.required_semantics.end();
  if (has_statements != statements_capability)
    return Invalid("C++ statements expression identity and required capability must agree");
  const bool has_sdk_runtime = std::any_of(
      sidecar.declarations.begin(), sidecar.declarations.end(), HasSdkRuntimeDeclarationIdentity);
  const bool sdk_runtime_capability = std::find(
      sidecar.required_semantics.begin(), sidecar.required_semantics.end(),
      kSdkRuntimeDeclarationSemantic) != sidecar.required_semantics.end();
  if (has_sdk_runtime != sdk_runtime_capability)
    return Invalid("SDK runtime declaration identity and required capability must agree");
  if (has_sdk_runtime && std::none_of(
      sidecar.required_semantics.begin(),sidecar.required_semantics.end(),
      [](const auto &semantic) { return semantic.starts_with("objc3.sdk-runtime-frames.v4."); }))
    return Invalid("SDK runtime declarations require their pinned runtime header contract");
  const bool has_async_definitions = std::any_of(
      sidecar.declarations.begin(), sidecar.declarations.end(), HasCxxAsyncDefinitionIdentity);
  const bool async_definition_capability = std::find(
      sidecar.required_semantics.begin(), sidecar.required_semantics.end(),
      kCxxAsyncDefinitionSemantic) != sidecar.required_semantics.end();
  if (has_async_definitions != async_definition_capability)
    return Invalid("C++ async definition identity and required capability must agree");
  const bool nullable_writes = std::any_of(sidecar.declarations.begin(),sidecar.declarations.end(),HasNullableKeyPathWriteIdentity);
  const bool nullable_write_capability = std::find(sidecar.required_semantics.begin(),sidecar.required_semantics.end(),kNullableKeyPathWriteSemantic)!=sidecar.required_semantics.end();
  const bool nullable_write_paths = std::find(sidecar.required_semantics.begin(),sidecar.required_semantics.end(),"objc3.typed-keypaths.v1")!=sidecar.required_semantics.end();
  if (nullable_writes!=nullable_write_capability || (nullable_write_capability && !nullable_write_paths))
    return Invalid("nullable key-path write identity and required write/key-path capabilities must agree");
  const bool has_closure_factories=std::any_of(sidecar.declarations.begin(),
      sidecar.declarations.end(),HasCxxClosureFactoryIdentity);
  const bool closure_factory_capability=std::find(sidecar.required_semantics.begin(),
      sidecar.required_semantics.end(),kCxxClosureFactorySemantic)!=sidecar.required_semantics.end();
  const bool closure_factory_owners=std::all_of(sidecar.declarations.begin(),sidecar.declarations.end(),
      [](const auto &d){return !HasCxxClosureFactoryIdentity(d) || d.kind!="Namespace";});
  if (has_closure_factories!=closure_factory_capability || !closure_factory_owners)
    return Invalid("closure factory identities and required capability must agree");
  const bool has_effectful_lambdas=std::any_of(sidecar.declarations.begin(),
      sidecar.declarations.end(),HasCxxEffectfulLambdaIdentity);
  const bool effectful_lambda_capability=std::find(sidecar.required_semantics.begin(),
      sidecar.required_semantics.end(),kCxxEffectfulLambdaSemantic)!=sidecar.required_semantics.end();
  const bool effectful_lambda_owners=std::all_of(sidecar.declarations.begin(),sidecar.declarations.end(),
      [](const auto &d){return !HasCxxEffectfulLambdaIdentity(d) || d.kind!="Namespace";});
  if (has_effectful_lambdas!=effectful_lambda_capability || !effectful_lambda_owners)
    return Invalid("effectful lambda identities and required capability must agree");
  const bool has_address_template_arguments=std::any_of(sidecar.declarations.begin(),
      sidecar.declarations.end(),HasCxxAddressTemplateArgumentIdentity);
  const bool address_template_argument_capability=std::find(sidecar.required_semantics.begin(),
      sidecar.required_semantics.end(),kCxxAddressTemplateArgumentSemantic)!=sidecar.required_semantics.end();
  const bool address_template_argument_owners=std::all_of(sidecar.declarations.begin(),sidecar.declarations.end(),
      [](const auto &d){return !HasCxxAddressTemplateArgumentIdentity(d) || d.kind!="Namespace";});
  if (has_address_template_arguments!=address_template_argument_capability || !address_template_argument_owners)
    return Invalid("address template argument identities and required capability must agree");
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
    return Invalid("template callable projection identities and required capability must agree");
  const bool has_effectful_template_members=std::any_of(sidecar.declarations.begin(),
      sidecar.declarations.end(),HasCxxEffectfulTemplateMemberIdentity);
  const bool effectful_template_member_capability=std::find(sidecar.required_semantics.begin(),
      sidecar.required_semantics.end(),kCxxEffectfulTemplateMemberSemantic)!=sidecar.required_semantics.end();
  const bool effectful_template_member_owners=std::all_of(sidecar.declarations.begin(),sidecar.declarations.end(),
      [](const auto &d){return !HasCxxEffectfulTemplateMemberIdentity(d) || d.kind=="ClassTemplate" ||
          (d.kind=="CXXRecord" && HasCxxConcreteMemberTemplateIdentity(d));});
  if (has_effectful_template_members!=effectful_template_member_capability || !effectful_template_member_owners)
    return Invalid("effectful template-member identities and required capability must agree");
  const bool has_effectful_function_templates=std::any_of(sidecar.declarations.begin(),
      sidecar.declarations.end(),HasCxxEffectfulFunctionTemplateIdentity);
  const bool effectful_function_template_capability=std::find(sidecar.required_semantics.begin(),
      sidecar.required_semantics.end(),kCxxEffectfulFunctionTemplateSemantic)!=sidecar.required_semantics.end();
  const bool effectful_function_template_owners=std::all_of(sidecar.declarations.begin(),sidecar.declarations.end(),
      [](const auto &d){return !HasCxxEffectfulFunctionTemplateIdentity(d) || d.kind=="FunctionTemplate";});
  if (has_effectful_function_templates!=effectful_function_template_capability || !effectful_function_template_owners)
    return Invalid("effectful function-template identities and required capability must agree");
  const bool has_closed_parent_patterns=std::any_of(sidecar.declarations.begin(),
      sidecar.declarations.end(),HasCxxClosedParentPatternIdentity);
  const bool closed_parent_pattern_capability=std::find(sidecar.required_semantics.begin(),
      sidecar.required_semantics.end(),kCxxClosedParentPatternSemantic)!=sidecar.required_semantics.end();
  const bool closed_parent_pattern_owners=std::all_of(sidecar.declarations.begin(),sidecar.declarations.end(),
      [](const auto &d){return !HasCxxClosedParentPatternIdentity(d) ||
          d.kind=="ClassTemplate" || (d.kind=="CXXRecord" && HasCxxConcreteMemberTemplateIdentity(d));});
  if (has_closed_parent_patterns!=closed_parent_pattern_capability || !closed_parent_pattern_owners)
    return Invalid("closed-parent pattern identities and required capability must agree");
  const bool has_closed_parent_members=std::any_of(sidecar.declarations.begin(),
      sidecar.declarations.end(),HasCxxClosedParentMemberIdentity);
  const bool closed_parent_member_capability=std::find(sidecar.required_semantics.begin(),
      sidecar.required_semantics.end(),kCxxClosedParentMemberSemantic)!=sidecar.required_semantics.end();
  const bool closed_parent_member_owners=std::all_of(sidecar.declarations.begin(),sidecar.declarations.end(),
      [](const auto &d){return !HasCxxClosedParentMemberIdentity(d) ||
          d.kind=="ClassTemplate" || (d.kind=="CXXRecord" && HasCxxConcreteMemberTemplateIdentity(d));});
  if (has_closed_parent_members!=closed_parent_member_capability || !closed_parent_member_owners)
    return Invalid("closed-parent member identities and required capability must agree");
  const bool has_dependent_member_partials=std::any_of(sidecar.declarations.begin(),
      sidecar.declarations.end(),HasCxxDependentMemberPartialIdentity);
  const bool dependent_member_partial_capability=std::find(sidecar.required_semantics.begin(),
      sidecar.required_semantics.end(),kCxxDependentMemberPartialSemantic)!=sidecar.required_semantics.end();
  const bool dependent_member_partial_owners=std::all_of(sidecar.declarations.begin(),sidecar.declarations.end(),
      [](const auto &d){return !HasCxxDependentMemberPartialIdentity(d) ||
          d.kind=="ClassTemplate" || (d.kind=="CXXRecord" && HasCxxConcreteMemberTemplateIdentity(d));});
  if (has_dependent_member_partials!=dependent_member_partial_capability || !dependent_member_partial_owners)
    return Invalid("dependent member partial identities and required capability must agree");
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
    return Invalid("special-member specialization identity and required callable capability must agree");
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
    return Invalid("concrete member variable specialization identity and required primary capability must agree");
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
    return Invalid("concrete member function specialization identity and required primary capability must agree");
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
    return Invalid("concrete member class specialization identity and required primary capability must agree");
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
    return Invalid("detached nested template identity and required primary capability must agree");
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
    return Invalid("detached static data template identity and required primary capability must agree");
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
    return Invalid("detached callable template identity and required primary capability must agree");
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
    return Invalid("variable specialization storage identity and required primary capability must agree");
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
    return Invalid("partial variable specialization identity and required primary capability must agree");
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
    return Invalid("partial class specialization identity and required primary capability must agree");
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
    return Invalid("class specialization identity and required primary capability must agree");
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
    return Invalid("variable specialization identity and required primary capability must agree");
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
    return Invalid("function specialization identity and required primary capability must agree");
  const bool has_template_template_parameters=std::any_of(sidecar.declarations.begin(),
      sidecar.declarations.end(),HasCxxTemplateTemplateIdentity);
  const bool template_template_capability=std::find(sidecar.required_semantics.begin(),
      sidecar.required_semantics.end(),kCxxTemplateTemplateSemantic)!=sidecar.required_semantics.end();
  if (has_template_template_parameters!=template_template_capability)
    return Invalid("template-template parameter identity and required capability must agree");
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
    return Invalid("nominal alias identity and required alias/owner capability must agree");
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
    return Invalid("concrete member template identity and required record capability must agree");
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
    return Invalid("concrete nested template identity and required record/class/nested capabilities must agree");
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
    return Invalid("variadic template identity and required capability must agree");
  const bool has_nested_templates = std::any_of(
      sidecar.declarations.begin(),sidecar.declarations.end(),HasCxxNestedTemplateIdentity);
  const bool nested_template_capability = std::find(sidecar.required_semantics.begin(),
      sidecar.required_semantics.end(),kCxxNestedTemplateSemantic)!=sidecar.required_semantics.end();
  const bool nested_template_class = std::find(sidecar.required_semantics.begin(),
      sidecar.required_semantics.end(),kCxxClassTemplateSemantic)!=sidecar.required_semantics.end();
  if (has_nested_templates!=nested_template_capability ||
      (nested_template_capability && !nested_template_class))
    return Invalid("nested template identity and required nested/class capabilities must agree");
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
    return Invalid("member alias template identity and required member/class/alias capabilities must agree");
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
    return Invalid("member template identity and required member/class/data capabilities must agree");
  const bool has_template_deduction = std::any_of(
      sidecar.declarations.begin(),sidecar.declarations.end(),HasCxxTemplateDeductionIdentity);
  const bool template_deduction_capability = std::find(sidecar.required_semantics.begin(),
      sidecar.required_semantics.end(),kCxxTemplateDeductionSemantic)!=sidecar.required_semantics.end();
  const bool template_deduction_data = std::find(sidecar.required_semantics.begin(),
      sidecar.required_semantics.end(),"objc3.cxx-data-interfaces.v1")!=sidecar.required_semantics.end();
  if (has_template_deduction!=template_deduction_capability || (template_deduction_capability && !template_deduction_data))
    return Invalid("template deduction identity and required deduction/data capabilities must agree");
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
    return Invalid("template structure identity and required structure/data capabilities must agree");
  const bool has_class_templates = std::any_of(
      sidecar.declarations.begin(), sidecar.declarations.end(), HasCxxClassTemplateIdentity);
  const bool class_capability = std::find(sidecar.required_semantics.begin(),
      sidecar.required_semantics.end(), kCxxClassTemplateSemantic) != sidecar.required_semantics.end();
  const bool class_data_capability = std::find(sidecar.required_semantics.begin(),
      sidecar.required_semantics.end(), "objc3.cxx-data-interfaces.v1") != sidecar.required_semantics.end();
  if (has_class_templates != class_capability || (class_capability && !class_data_capability))
    return Invalid("class template identity and required class/data capabilities must agree");
  const bool has_variable_templates = std::any_of(
      sidecar.declarations.begin(), sidecar.declarations.end(), HasCxxVariableTemplateIdentity);
  const bool variable_capability = std::find(sidecar.required_semantics.begin(),
      sidecar.required_semantics.end(), kCxxVariableTemplateSemantic) != sidecar.required_semantics.end();
  const bool variable_data_capability = std::find(sidecar.required_semantics.begin(),
      sidecar.required_semantics.end(), "objc3.cxx-data-interfaces.v1") != sidecar.required_semantics.end();
  if (has_variable_templates != variable_capability || (variable_capability && !variable_data_capability))
    return Invalid("variable template identity and required variable/data capabilities must agree");
  const bool has_alias_templates = std::any_of(
      sidecar.declarations.begin(), sidecar.declarations.end(), HasCxxAliasTemplateIdentity);
  const bool alias_capability = std::find(sidecar.required_semantics.begin(),
      sidecar.required_semantics.end(), kCxxAliasTemplateSemantic) != sidecar.required_semantics.end();
  const bool alias_data_capability = std::find(sidecar.required_semantics.begin(),
      sidecar.required_semantics.end(), "objc3.cxx-data-interfaces.v1") != sidecar.required_semantics.end();
  if (has_alias_templates != alias_capability || (alias_capability && !alias_data_capability))
    return Invalid("alias template identity and required alias/data capabilities must agree");
  const bool has_function_templates = std::any_of(
      sidecar.declarations.begin(), sidecar.declarations.end(), HasCxxFunctionTemplateIdentity);
  const bool template_capability = std::find(sidecar.required_semantics.begin(),
      sidecar.required_semantics.end(), kCxxFunctionTemplateSemantic) != sidecar.required_semantics.end();
  const bool template_data_capability = std::find(sidecar.required_semantics.begin(),
      sidecar.required_semantics.end(), "objc3.cxx-data-interfaces.v1") != sidecar.required_semantics.end();
  if (has_function_templates != template_capability || (template_capability && !template_data_capability))
    return Invalid("function template identity and required template/data capabilities must agree");
  const bool has_concepts = std::any_of(
      sidecar.declarations.begin(), sidecar.declarations.end(), HasCxxConceptIdentity);
  const bool concept_capability = std::find(
      sidecar.required_semantics.begin(), sidecar.required_semantics.end(),
      kCxxConceptSemantic) != sidecar.required_semantics.end();
  const bool concept_data_capability = std::find(
      sidecar.required_semantics.begin(), sidecar.required_semantics.end(),
      "objc3.cxx-data-interfaces.v1") != sidecar.required_semantics.end();
  if (has_concepts != concept_capability || (concept_capability && !concept_data_capability))
    return Invalid("concept identity and required concept/data capabilities must agree");
  const bool has_union_values = std::any_of(
      sidecar.declarations.begin(), sidecar.declarations.end(), HasCxxUnionValueIdentity);
  const bool union_capability = std::find(
      sidecar.required_semantics.begin(), sidecar.required_semantics.end(),
      kCxxUnionValueSemantic) != sidecar.required_semantics.end();
  const bool union_data_capability = std::find(
      sidecar.required_semantics.begin(), sidecar.required_semantics.end(),
      "objc3.cxx-data-interfaces.v1") != sidecar.required_semantics.end();
  if (has_union_values != union_capability || (union_capability && !union_data_capability))
    return Invalid("C++ union identity and required data/union capabilities must agree");
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
    return Invalid("generic local lambda identity and required lambda capabilities must agree");
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
    return Invalid("C++ local type identity and required capability must agree");
  if (has_unnamed_local_types != unnamed_local_capability ||
      (unnamed_local_capability && !local_types_capability))
    return Invalid("C++ unnamed local type identity and required local-type capabilities must agree");
  const bool has_cxx_members = std::any_of(
      sidecar.declarations.begin(), sidecar.declarations.end(),
      [](const auto &d) { return IsCxxMemberDeclarationKind(d.kind); });
  const bool member_capability = std::find(
      sidecar.required_semantics.begin(), sidecar.required_semantics.end(),
      "objc3.cxx-member-interfaces.v1") != sidecar.required_semantics.end();
  if (has_cxx_members != member_capability)
    return Invalid("C++ member declarations and required capability must agree");
  const bool name_capability = std::find(sidecar.required_semantics.begin(),
                                         sidecar.required_semantics.end(),
                                         "objc3.module-owned-names.v1") !=
                               sidecar.required_semantics.end();
  if (has_names != name_capability)
    return Invalid("module-owned names and required capability must agree");
  for (const auto &d : sidecar.declarations)
    if (!HasValidModuleOwnedName(sidecar.module, d))
      return Invalid(
          "native symbol name differs from its module ownership contract");
  const auto &module = sidecar.module;
  if (module.canonical_name.empty() || module.semantic_fingerprint.empty() ||
      module.target_triple.empty() || module.runtime_abi.empty() ||
      module.extension_abi.empty() || module.language_revision.empty() ||
      module.language_profile.empty() || module.package_identity.empty())
    return Invalid("module identity fields must be nonempty");
  for (const auto *value : {&module.canonical_name, &module.semantic_fingerprint,
                            &module.target_triple, &module.runtime_abi,
                            &module.extension_abi, &module.language_revision,
                            &module.language_profile, &module.package_identity})
    if (!valid_text(*value))
      return Invalid("module identity field is invalid UTF-8 or exceeds string limit");
  if (sidecar.declarations.size() > kMaxDeclarations ||
      sidecar.dependencies.size() > kMaxDependencies ||
      sidecar.required_semantics.size() > kMaxRequiredSemantics)
    return Invalid("sidecar exceeds a collection bound");
  std::set<std::string> declaration_ids;
  for (const auto &declaration : sidecar.declarations) {
    if (declaration.stable_id.empty() || declaration.kind.empty() ||
        declaration.qualified_name.empty() || declaration.export_root.empty() ||
        declaration.canonical_type_identity.empty())
      return Invalid("declaration identity fields must be nonempty");
    if (!declaration_ids.insert(declaration.stable_id).second)
      return Invalid(llvm::formatv("duplicate declaration stable id '{0}'", declaration.stable_id).str());
    for (const auto *value : {&declaration.stable_id, &declaration.kind,
                              &declaration.qualified_name, &declaration.export_root,
                              &declaration.canonical_type_identity,
                              &declaration.effects.throws_kind,
                              &declaration.effects.error_type_identity,
                              &declaration.effects.actor_isolation_identity,
                              &declaration.effects.executor_affinity_identity,
                              &declaration.effects.ownership_identity,
                              &declaration.effects.async_abi_identity})
      if (!valid_text(*value))
        return Invalid("declaration field is invalid UTF-8 or exceeds string limit");
  }
  std::set<std::string> required;
  for (const auto &semantic : sidecar.required_semantics) {
    if (semantic.empty() || !valid_text(semantic))
      return Invalid("required semantic name is empty, invalid UTF-8, or exceeds limit");
    if (!required.insert(semantic).second)
      return Invalid(llvm::formatv("duplicate required semantic '{0}'", semantic).str());
    if (!supported.count(semantic))
      return Invalid(llvm::formatv("unknown required semantic '{0}'", semantic).str());
  }
  if (required.count("objc3.keypath-subscripts.v1") &&
      !required.count("objc3.typed-keypaths.v1"))
    return Invalid("key-path subscripts are missing their typed key-path capability");
  for (const auto &declaration : sidecar.declarations) {
    const auto &effects = declaration.effects;
    if (!IsValidAsyncABIIdentity(effects))
      return Invalid("invalid or missing physical async entry ABI identity");
    if (!effects.async_abi_identity.empty() && !required.count(kAsyncEntryABISemantic))
      return Invalid("async entry ABI is missing its semantic capability");
    if (effects.async_abi_identity.find("10:c-frame-v1") != std::string::npos &&
        !required.count(RuntimeInterfaceSemantic().str()))
      return Invalid("C frame entry ABI is missing its SDK header contract");
    if (effects.ownership_identity.find("native-contracts:v1:") != std::string::npos &&
        !required.count("objc3.native-ownership-contracts.v1"))
      return Invalid("native ownership contract is missing its semantic capability");
    if (declaration.canonical_type_identity.find("KeyPath<") != std::string::npos &&
        !required.count("objc3.typed-keypaths.v1"))
      return Invalid("typed key path is missing its semantic capability");
    if ((!effects.actor_isolation_identity.empty() ||
         !effects.executor_affinity_identity.empty()) &&
        !required.count("objc3.isolation-contracts.v1"))
      return Invalid("native isolation contract is missing its semantic capability");
  }
  std::set<std::pair<std::string, std::string>> dependency_ids;
  for (const auto &dependency : sidecar.dependencies) {
    const auto &id = dependency.module;
    if (id.canonical_name.empty() || id.semantic_fingerprint.empty() ||
        id.target_triple.empty() || id.runtime_abi.empty() ||
        id.extension_abi.empty() || id.language_revision.empty() ||
        id.language_profile.empty() || id.package_identity.empty())
      return Invalid("dependency identity fields must be nonempty");
    for (const auto *value : {&id.canonical_name, &id.semantic_fingerprint,
                              &id.target_triple, &id.runtime_abi,
                              &id.extension_abi, &id.language_revision,
                              &id.language_profile, &id.package_identity})
      if (!valid_text(*value))
        return Invalid("dependency identity is invalid UTF-8 or exceeds string limit");
    if (!dependency_ids.emplace(id.canonical_name, id.semantic_fingerprint).second)
      return Invalid(llvm::formatv("duplicate dependency identity '{0}'", id.canonical_name).str());
  }
  return llvm::Error::success();
}

}  // namespace

std::string ComputeInterfaceSourceSha256(llvm::StringRef source) {
  llvm::SHA256 hasher;
  hasher.update(source);
  const auto digest = hasher.final();
  static constexpr char hex[] = "0123456789abcdef";
  std::string result;
  result.reserve(64);
  for (std::uint8_t byte : digest) {
    result.push_back(hex[byte >> 4]);
    result.push_back(hex[byte & 0x0f]);
  }
  return result;
}

llvm::Expected<std::string> EncodeSemanticInterfaceSidecar(
    const SemanticInterfaceSidecar &sidecar,
    llvm::ArrayRef<llvm::StringRef> supported_required_semantics) {
  if (auto error = ValidateForEncoding(sidecar, Supported(supported_required_semantics)))
    return std::move(error);
  auto declarations = sidecar.declarations;
  std::sort(declarations.begin(), declarations.end(), [](const auto &a, const auto &b) {
    return a.stable_id < b.stable_id;
  });
  Array encoded_declarations;
  for (const auto &declaration : declarations)
    encoded_declarations.push_back(EncodeDeclaration(declaration));
  auto dependencies = sidecar.dependencies;
  std::sort(dependencies.begin(), dependencies.end(), [](const auto &a, const auto &b) {
    return std::tie(a.module.canonical_name, a.module.semantic_fingerprint) <
           std::tie(b.module.canonical_name, b.module.semantic_fingerprint);
  });
  Array encoded_dependencies;
  for (const auto &dependency : dependencies)
    encoded_dependencies.push_back(EncodeDependency(dependency));
  auto semantics = sidecar.required_semantics;
  std::sort(semantics.begin(), semantics.end());
  Array encoded_semantics;
  for (const auto &semantic : semantics) encoded_semantics.push_back(semantic);
  Object root{{"schema_id", kInterfaceSchemaId},
              {"schema_major", static_cast<std::int64_t>(sidecar.schema_major)},
              {"schema_minor", static_cast<std::int64_t>(sidecar.schema_minor)},
              {"module", EncodeModule(sidecar.module)},
              {"source_binding_sha256", sidecar.canonical_source_sha256},
              {"ownership_projection", sidecar.ownership_projection},
              {"required_semantics", std::move(encoded_semantics)},
              {"declarations", std::move(encoded_declarations)},
              {"dependencies", std::move(encoded_dependencies)}};
  std::string output;
  llvm::raw_string_ostream stream(output);
  stream << Value(std::move(root));
  stream.flush();
  if (output.size() > kMaxSidecarBytes)
    return Invalid("encoded sidecar exceeds byte limit");
  return output;
}

llvm::Expected<SemanticInterfaceSidecar> DecodeSemanticInterfaceSidecar(
    llvm::StringRef json_bytes, llvm::StringRef generated_source_bytes,
    llvm::ArrayRef<llvm::StringRef> supported_required_semantics) {
  if (json_bytes.size() > kMaxSidecarBytes)
    return Invalid("sidecar exceeds byte limit");
  if (!llvm::json::isUTF8(generated_source_bytes))
    return Invalid("paired .objc3i source is not valid UTF-8");
  auto parsed = llvm::json::parse(json_bytes);
  if (!parsed) return parsed.takeError();
  const auto *root = parsed->getAsObject();
  if (!root) return Invalid("sidecar root must be a JSON object");
  if (auto error =
          CheckKeys(*root,
                    {"schema_id", "schema_major", "schema_minor", "module",
                     "source_binding_sha256", "ownership_projection",
                     "required_semantics", "declarations", "dependencies"},
                    "/"))
    return std::move(error);

  auto schema = RequiredString(*root, "schema_id", "/");
  if (!schema) return schema.takeError();
  if (*schema != kInterfaceSchemaId) return Invalid("unsupported semantic interface schema id");
  auto major = RequiredUnsigned(*root, "schema_major", "/");
  if (!major) return major.takeError();
  auto minor = RequiredUnsigned(*root, "schema_minor", "/");
  if (!minor) return minor.takeError();
  if (*major != kInterfaceSchemaMajor || *minor != kInterfaceSchemaMinor)
    return Invalid("unsupported semantic interface schema version");

  SemanticInterfaceSidecar result;
  result.schema_major = static_cast<unsigned>(*major);
  result.schema_minor = static_cast<unsigned>(*minor);
  auto projection =
      RequiredStringAllowEmpty(*root, "ownership_projection", "/");
  if (!projection)
    return projection.takeError();
  result.ownership_projection = std::move(*projection);
  auto *module_object = root->getObject("module");
  if (!module_object) return Invalid("/module: required object missing or has wrong type");
  auto module = DecodeModule(*module_object, "/module");
  if (!module) return module.takeError();
  result.module = std::move(*module);
  auto source_hash = RequiredString(*root, "source_binding_sha256", "/");
  if (!source_hash) return source_hash.takeError();
  if (!IsSha256(*source_hash)) return Invalid("source binding is not a lowercase SHA256 digest");
  if (*source_hash != ComputeInterfaceSourceSha256(generated_source_bytes))
    return Invalid("source binding digest does not match paired .objc3i bytes");
  result.canonical_source_sha256 = std::move(*source_hash);

  auto required = RequiredArray(*root, "required_semantics", "/", kMaxRequiredSemantics);
  if (!required) return required.takeError();
  const auto supported = Supported(supported_required_semantics);
  std::set<std::string> seen_semantics;
  for (std::size_t i = 0; i < (*required)->size(); ++i) {
    const auto text = (*required)->operator[](i).getAsString();
    if (!text || text->empty() || text->size() > kMaxStringBytes)
      return Invalid(llvm::formatv("/required_semantics/{0}: expected nonempty bounded string", i).str());
    if (!seen_semantics.insert(text->str()).second)
      return Invalid(llvm::formatv("/required_semantics/{0}: duplicate semantic", i).str());
    if (!supported.count(text->str()))
      return Invalid(llvm::formatv("/required_semantics/{0}: unknown required semantic '{1}'", i, *text).str());
    result.required_semantics.push_back(text->str());
  }

  auto declarations = RequiredArray(*root, "declarations", "/", kMaxDeclarations);
  if (!declarations) return declarations.takeError();
  std::set<std::string> declaration_ids;
  for (std::size_t i = 0; i < (*declarations)->size(); ++i) {
    auto declaration = DecodeDeclaration((*declarations)->operator[](i), i);
    if (!declaration) return declaration.takeError();
    if (!declaration_ids.insert(declaration->stable_id).second)
      return Invalid(llvm::formatv("/declarations/{0}: duplicate stable id '{1}'", i, declaration->stable_id).str());
    result.declarations.push_back(std::move(*declaration));
  }

  auto dependencies = RequiredArray(*root, "dependencies", "/", kMaxDependencies);
  if (!dependencies) return dependencies.takeError();
  std::set<std::pair<std::string, std::string>> dependency_ids;
  for (std::size_t i = 0; i < (*dependencies)->size(); ++i) {
    auto dependency = DecodeDependency((*dependencies)->operator[](i), i);
    if (!dependency) return dependency.takeError();
    const auto key = std::make_pair(dependency->module.canonical_name,
                                    dependency->module.semantic_fingerprint);
    if (!dependency_ids.insert(key).second)
      return Invalid(llvm::formatv("/dependencies/{0}: duplicate module identity", i).str());
    result.dependencies.push_back(std::move(*dependency));
  }
  const bool projected = seen_semantics.count("objc3.ownership-projection.v1");
  if (projected != !result.ownership_projection.empty())
    return Invalid("ownership projection provenance and capability must agree");
  if (auto error = ValidateForEncoding(result, supported))
    return std::move(error);
  return result;
}

}  // namespace objc3::clang_foundation
