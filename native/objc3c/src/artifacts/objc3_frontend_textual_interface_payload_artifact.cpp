#include "artifacts/objc3_frontend_textual_interface_payload_artifact.h"

#include <set>
#include <string>
#include <utility>
#include <vector>

#include "artifacts/json/artifact_json_publication_contract.h"
#include "artifacts/json/artifact_json_writer.h"
#include "io/json/json_value.h"
#include "support/objc3_value_type_names.h"

namespace objc3::artifacts::frontend {
namespace {

using objc3::io::json::JsonValue;
using JsonArray = JsonValue::Array;
using JsonObject = JsonValue::Object;

JsonValue StringArray(std::vector<std::string> values) {
  JsonArray array;
  array.reserve(values.size());
  for (std::string &value : values) {
    array.push_back(JsonValue::String(std::move(value)));
  }
  return JsonValue::ArrayValue(std::move(array));
}

JsonValue SizeValue(std::size_t value) {
  return JsonValue::Number(static_cast<double>(value));
}

JsonValue SourceAnchor(const std::filesystem::path &input_path,
                       unsigned line,
                       unsigned column) {
  JsonObject anchor;
  anchor["path"] = JsonValue::String(input_path.generic_string());
  anchor["line"] = SizeValue(line);
  anchor["column"] = SizeValue(column);
  return JsonValue::ObjectValue(std::move(anchor));
}

std::string TypeName(ValueType type) {
  return objc3c::support::ValueTypeName(type);
}

JsonValue ValueOptionalContract() {
  JsonObject contract;
  contract["issue_ref"] = SizeValue(8234u);
  contract["canonical_spelling"] = JsonValue::String("Optional<T>");
  contract["source_status"] = JsonValue::String("reserved-rejected-before-sema");
  contract["lowercase_alias_accepted"] = JsonValue::Bool(false);
  contract["abi_layout_status"] = JsonValue::String("reserved-no-layout");
  contract["nil_to_scalar_coercion_allowed"] = JsonValue::Bool(false);
  contract["nullable_pointer_conversion_allowed"] = JsonValue::Bool(false);
  contract["throws_result_conversion_allowed"] = JsonValue::Bool(false);
  contract["interface_roundtrip_status"] =
      JsonValue::String("reserved-feature-marker-imported");
  return JsonValue::ObjectValue(std::move(contract));
}

JsonValue GenericParams(const std::vector<Objc3GenericParamDecl> &params,
                        const std::filesystem::path &input_path) {
  JsonArray array;
  array.reserve(params.size());
  for (const Objc3GenericParamDecl &param : params) {
    JsonObject object;
    object["name"] = JsonValue::String(param.name);
    object["variance"] = JsonValue::String(param.variance_spelling);
    object["has_constraint"] = JsonValue::Bool(param.has_constraint);
    object["constraint_type_name"] =
        JsonValue::String(param.constraint_type_name);
    object["constraint_generic_suffix"] =
        JsonValue::String(param.constraint_generic_suffix_text);
    object["source_anchor"] = SourceAnchor(input_path, param.line, param.column);
    array.push_back(JsonValue::ObjectValue(std::move(object)));
  }
  return JsonValue::ArrayValue(std::move(array));
}

JsonValue ParamTypeShape(const FuncParam &param,
                         const std::filesystem::path &input_path) {
  JsonObject object;
  object["name"] = JsonValue::String(param.name);
  object["type"] = JsonValue::String(TypeName(param.type));
  object["object_pointer_type_name"] =
      JsonValue::String(param.object_pointer_type_name);
  object["generic_suffix"] = JsonValue::String(param.generic_suffix_text);
  object["pointer_depth"] = SizeValue(param.pointer_declarator_depth);
  object["nullability_profile"] =
      JsonValue::String(param.nullability_flow_profile);
  object["ownership_qualifier"] =
      JsonValue::String(param.ownership_qualifier_spelling);
  object["ownership_symbol"] =
      JsonValue::String(param.ownership_qualifier_symbol);
  object["borrowed_pointer_profile"] =
      JsonValue::String(param.borrowed_pointer_profile);
  object["source_anchor"] = SourceAnchor(input_path, param.line, param.column);
  return JsonValue::ObjectValue(std::move(object));
}

JsonValue ParamTypeShapes(const std::vector<FuncParam> &params,
                          const std::filesystem::path &input_path) {
  JsonArray array;
  array.reserve(params.size());
  for (const FuncParam &param : params) {
    array.push_back(ParamTypeShape(param, input_path));
  }
  return JsonValue::ArrayValue(std::move(array));
}

JsonValue FunctionTypeSignature(const FunctionDecl &function,
                                const std::filesystem::path &input_path) {
  JsonObject signature;
  signature["result_type"] = JsonValue::String(TypeName(function.return_type));
  signature["result_object_pointer_type_name"] =
      JsonValue::String(function.return_object_pointer_type_name);
  signature["result_generic_suffix"] =
      JsonValue::String(function.return_generic_suffix_text);
  signature["result_pointer_depth"] =
      SizeValue(function.return_pointer_declarator_depth);
  signature["parameters"] = ParamTypeShapes(function.params, input_path);
  signature["value_optional_contract"] = ValueOptionalContract();
  return JsonValue::ObjectValue(std::move(signature));
}

JsonValue MethodTypeSignature(const Objc3MethodDecl &method,
                              const std::filesystem::path &input_path) {
  JsonObject signature;
  signature["selector"] = JsonValue::String(method.selector);
  signature["is_class_method"] = JsonValue::Bool(method.is_class_method);
  signature["result_type"] = JsonValue::String(TypeName(method.return_type));
  signature["result_object_pointer_type_name"] =
      JsonValue::String(method.return_object_pointer_type_name);
  signature["result_generic_suffix"] =
      JsonValue::String(method.return_generic_suffix_text);
  signature["result_pointer_depth"] =
      SizeValue(method.return_pointer_declarator_depth);
  signature["parameters"] = ParamTypeShapes(method.params, input_path);
  signature["value_optional_contract"] = ValueOptionalContract();
  return JsonValue::ObjectValue(std::move(signature));
}

JsonValue TypedThrowsContract(bool throws_declared) {
  JsonObject contract;
  contract["issue_ref"] = SizeValue(8233u);
  contract["canonical_syntax"] = JsonValue::String("throws(E)");
  contract["throws_kind"] =
      JsonValue::String(throws_declared ? "untyped" : "none");
  contract["declared_error_type"] =
      JsonValue::String(throws_declared ? "id<Error>" : "");
  contract["typed_payload_arity"] = SizeValue(0u);
  contract["typed_payload_status"] =
      JsonValue::String("reserved-rejected-before-sema");
  contract["silent_erasure_allowed"] = JsonValue::Bool(false);
  contract["multi_payload_supported"] = JsonValue::Bool(false);
  contract["abi_status"] = JsonValue::String("reserved-no-lowering");
  contract["interface_roundtrip_status"] =
      JsonValue::String("reserved-feature-marker-imported");
  return JsonValue::ObjectValue(std::move(contract));
}

JsonValue Effects(bool async_declared,
                  bool throws_declared,
                  bool nonisolated_declared,
                  bool executor_affinity_declared,
                  const std::string &executor_affinity_kind,
                  const std::string &executor_affinity_name,
                  const std::string &throws_profile,
                  const std::string &actor_profile) {
  JsonObject effects;
  effects["async"] = JsonValue::Bool(async_declared);
  effects["throws"] = JsonValue::Bool(throws_declared);
  effects["throws_kind"] =
      JsonValue::String(throws_declared ? "untyped" : "none");
  effects["declared_error_type"] =
      JsonValue::String(throws_declared ? "id<Error>" : "");
  effects["typed_throws"] = TypedThrowsContract(throws_declared);
  effects["throws_profile"] = JsonValue::String(throws_profile);
  effects["actor_nonisolated"] = JsonValue::Bool(nonisolated_declared);
  effects["actor_isolation_profile"] = JsonValue::String(actor_profile);
  effects["executor_affinity_declared"] =
      JsonValue::Bool(executor_affinity_declared);
  effects["executor_affinity_kind"] =
      JsonValue::String(executor_affinity_kind);
  effects["executor_affinity_name"] =
      JsonValue::String(executor_affinity_name);
  return JsonValue::ObjectValue(std::move(effects));
}

JsonValue FunctionEffects(const FunctionDecl &function) {
  return Effects(function.async_declared, function.throws_declared,
                 function.objc_nonisolated_declared,
                 function.executor_affinity_declared,
                 function.executor_affinity_kind,
                 function.executor_affinity_name,
                 function.throws_declaration_profile,
                 function.actor_isolation_sendability_profile);
}

JsonValue MethodEffects(const Objc3MethodDecl &method) {
  return Effects(method.async_declared, method.throws_declared,
                 method.objc_nonisolated_declared,
                 method.executor_affinity_declared,
                 method.executor_affinity_kind,
                 method.executor_affinity_name,
                 method.throws_declaration_profile,
                 method.actor_isolation_sendability_profile);
}

JsonValue Ownership(const std::string &qualifier,
                    const std::string &symbol,
                    const std::string &operation_profile,
                    const std::string &lifetime_profile,
                    bool weak_reference,
                    bool unowned_reference,
                    bool unowned_safe_reference) {
  JsonObject ownership;
  ownership["qualifier"] = JsonValue::String(qualifier);
  ownership["symbol"] = JsonValue::String(symbol);
  ownership["operation_profile"] = JsonValue::String(operation_profile);
  ownership["lifetime_profile"] = JsonValue::String(lifetime_profile);
  ownership["weak_reference"] = JsonValue::Bool(weak_reference);
  ownership["unowned_reference"] = JsonValue::Bool(unowned_reference);
  ownership["unowned_safe_reference"] = JsonValue::Bool(unowned_safe_reference);
  return JsonValue::ObjectValue(std::move(ownership));
}

JsonValue FunctionOwnership(const FunctionDecl &function) {
  return Ownership(function.return_ownership_qualifier_spelling,
                   function.return_ownership_qualifier_symbol,
                   function.return_ownership_operation_profile,
                   function.return_ownership_lifetime_profile,
                   function.return_ownership_is_weak_reference,
                   function.return_ownership_is_unowned_reference,
                   function.return_ownership_is_unowned_safe_reference);
}

JsonValue MethodOwnership(const Objc3MethodDecl &method) {
  return Ownership(method.return_ownership_qualifier_spelling,
                   method.return_ownership_qualifier_symbol,
                   method.return_ownership_operation_profile,
                   method.return_ownership_lifetime_profile,
                   method.return_ownership_is_weak_reference,
                   method.return_ownership_is_unowned_reference,
                   method.return_ownership_is_unowned_safe_reference);
}

JsonValue PropertyTypeSignature(const Objc3PropertyDecl &property,
                                const std::filesystem::path &input_path) {
  JsonObject signature;
  signature["type"] = JsonValue::String(TypeName(property.type));
  signature["object_pointer_type_name"] =
      JsonValue::String(property.object_pointer_type_name);
  signature["generic_suffix"] = JsonValue::String(property.generic_suffix_text);
  signature["pointer_depth"] = SizeValue(property.pointer_declarator_depth);
  signature["nullability_profile"] =
      JsonValue::String(property.nullability_flow_profile);
  signature["value_optional_contract"] = ValueOptionalContract();
  signature["source_anchor"] =
      SourceAnchor(input_path, property.line, property.column);
  return JsonValue::ObjectValue(std::move(signature));
}

JsonValue PropertyEffects(const Objc3PropertyDecl &property) {
  JsonObject effects;
  effects["readonly"] = JsonValue::Bool(property.is_readonly);
  effects["atomic"] = JsonValue::Bool(property.is_atomic);
  effects["class"] = JsonValue::Bool(property.is_class);
  effects["direct"] = JsonValue::Bool(property.is_direct);
  return JsonValue::ObjectValue(std::move(effects));
}

JsonValue PropertyOwnership(const Objc3PropertyDecl &property) {
  return Ownership(property.ownership_qualifier_spelling,
                   property.ownership_qualifier_symbol,
                   property.ownership_operation_profile,
                   property.ownership_lifetime_profile,
                   property.ownership_is_weak_reference,
                   property.ownership_is_unowned_reference,
                   property.ownership_is_unowned_safe_reference);
}

JsonValue RuntimeMetadata(std::string semantic_link_symbol,
                          std::string scope_owner_symbol,
                          std::vector<std::string> scope_path,
                          std::vector<std::string> lookup_symbols) {
  JsonObject metadata;
  metadata["semantic_link_symbol"] =
      JsonValue::String(std::move(semantic_link_symbol));
  metadata["scope_owner_symbol"] =
      JsonValue::String(std::move(scope_owner_symbol));
  metadata["scope_path"] = StringArray(std::move(scope_path));
  metadata["lookup_symbols"] = StringArray(std::move(lookup_symbols));
  return JsonValue::ObjectValue(std::move(metadata));
}

JsonValue Declaration(std::string declaration_id,
                      std::string kind,
                      std::string name,
                      JsonValue type_signature,
                      JsonValue effects,
                      JsonValue generics,
                      JsonValue ownership,
                      JsonValue runtime_metadata,
                      JsonValue source_anchor) {
  JsonObject declaration;
  declaration["declaration_id"] = JsonValue::String(std::move(declaration_id));
  declaration["kind"] = JsonValue::String(std::move(kind));
  declaration["name"] = JsonValue::String(std::move(name));
  declaration["visibility"] = JsonValue::String("public");
  declaration["type_signature"] = std::move(type_signature);
  declaration["effects"] = std::move(effects);
  declaration["generics"] = std::move(generics);
  declaration["ownership"] = std::move(ownership);
  declaration["runtime_metadata"] = std::move(runtime_metadata);
  declaration["source_anchor"] = std::move(source_anchor);
  return JsonValue::ObjectValue(std::move(declaration));
}

JsonArray BuildInterfaceMemberDeclarations(
    const std::string &module_name,
    const std::string &owner_kind,
    const std::string &owner_name,
    const std::vector<Objc3PropertyDecl> &properties,
    const std::vector<Objc3MethodDecl> &methods,
    const std::filesystem::path &input_path) {
  JsonArray declarations;
  declarations.reserve(properties.size() + methods.size());
  for (const Objc3PropertyDecl &property : properties) {
    declarations.push_back(Declaration(
        module_name + "::" + owner_kind + "::" + owner_name + "::property::" +
            property.name,
        "property", property.name, PropertyTypeSignature(property, input_path),
        PropertyEffects(property), JsonValue::ArrayValue({}),
        PropertyOwnership(property),
        RuntimeMetadata(property.property_synthesis_symbol,
                        property.scope_owner_symbol, {},
                        {property.ivar_binding_symbol,
                         property.executable_synthesized_binding_symbol,
                         property.executable_ivar_layout_symbol}),
        SourceAnchor(input_path, property.line, property.column)));
  }
  for (const Objc3MethodDecl &method : methods) {
    declarations.push_back(Declaration(
        module_name + "::" + owner_kind + "::" + owner_name + "::method::" +
            method.selector,
        "method", method.selector, MethodTypeSignature(method, input_path),
        MethodEffects(method), JsonValue::ArrayValue({}), MethodOwnership(method),
        RuntimeMetadata(method.method_lookup_symbol, method.scope_owner_symbol,
                        {}, {method.override_lookup_symbol,
                             method.conflict_lookup_symbol}),
        SourceAnchor(input_path, method.line, method.column)));
  }
  return declarations;
}

JsonValue BuildDeclarations(const Objc3Program &program,
                            const std::filesystem::path &input_path) {
  JsonArray declarations;
  declarations.reserve(program.globals.size() + program.protocols.size() +
                       program.interfaces.size() + program.functions.size());
  for (const GlobalDecl &global : program.globals) {
    JsonObject signature;
    signature["type"] = JsonValue::String("global");
    declarations.push_back(Declaration(
        program.module_name + "::global::" + global.name, "global",
        global.name, JsonValue::ObjectValue(std::move(signature)),
        JsonValue::ObjectValue({}), JsonValue::ArrayValue({}),
        JsonValue::ObjectValue({}),
        RuntimeMetadata(global.semantic_link_symbol, global.scope_owner_symbol,
                        global.scope_path_lexicographic, {}),
        SourceAnchor(input_path, global.line, global.column)));
  }
  for (const Objc3ProtocolDecl &protocol : program.protocols) {
    JsonObject signature;
    signature["inherited_protocols"] =
        StringArray(protocol.inherited_protocols_lexicographic);
    signature["members"] = JsonValue::ArrayValue(BuildInterfaceMemberDeclarations(
        program.module_name, "protocol", protocol.name, protocol.properties,
        protocol.methods, input_path));
    declarations.push_back(Declaration(
        program.module_name + "::protocol::" + protocol.name, "protocol",
        protocol.name, JsonValue::ObjectValue(std::move(signature)),
        JsonValue::ObjectValue({}), JsonValue::ArrayValue({}),
        JsonValue::ObjectValue({}),
        RuntimeMetadata(protocol.semantic_link_symbol, protocol.scope_owner_symbol,
                        protocol.scope_path_lexicographic,
                        protocol.method_lookup_symbols_lexicographic),
        SourceAnchor(input_path, protocol.line, protocol.column)));
  }
  for (const Objc3InterfaceDecl &interface_decl : program.interfaces) {
    JsonObject signature;
    signature["super_name"] = JsonValue::String(interface_decl.super_name);
    signature["category_name"] =
        JsonValue::String(interface_decl.category_name);
    signature["is_actor"] = JsonValue::Bool(interface_decl.is_actor);
    signature["adopted_protocols"] =
        StringArray(interface_decl.adopted_protocols_lexicographic);
    signature["members"] = JsonValue::ArrayValue(BuildInterfaceMemberDeclarations(
        program.module_name, "interface", interface_decl.name,
        interface_decl.properties, interface_decl.methods, input_path));
    declarations.push_back(Declaration(
        program.module_name + "::interface::" + interface_decl.name,
        interface_decl.is_actor ? "actor_interface" : "interface",
        interface_decl.name, JsonValue::ObjectValue(std::move(signature)),
        JsonValue::ObjectValue({}),
        GenericParams(interface_decl.generic_params, input_path),
        JsonValue::ObjectValue({}),
        RuntimeMetadata(interface_decl.semantic_link_symbol,
                        interface_decl.scope_owner_symbol,
                        interface_decl.scope_path_lexicographic,
                        interface_decl.method_lookup_symbols_lexicographic),
        SourceAnchor(input_path, interface_decl.line, interface_decl.column)));
  }
  for (const FunctionDecl &function : program.functions) {
    declarations.push_back(Declaration(
        program.module_name + "::function::" + function.name, "function",
        function.name, FunctionTypeSignature(function, input_path),
        FunctionEffects(function),
        GenericParams(function.generic_params, input_path),
        FunctionOwnership(function),
        RuntimeMetadata("", function.scope_owner_symbol,
                        function.scope_path_lexicographic, {}),
        SourceAnchor(input_path, function.line, function.column)));
  }
  return JsonValue::ArrayValue(std::move(declarations));
}

void AddAnnotatedImport(std::set<std::string> &imports,
                        const std::string &module_name) {
  if (!module_name.empty()) {
    imports.insert(module_name);
  }
}

JsonValue BuildImports(const Objc3Program &program,
                       const Objc3FrontendOptions &options) {
  std::set<std::string> imports;
  for (const std::string &path : options.imported_runtime_surface_paths) {
    imports.insert("runtime-surface:" + path);
  }
  for (const FunctionDecl &function : program.functions) {
    AddAnnotatedImport(imports, function.objc_import_module_name);
  }
  for (const Objc3InterfaceDecl &interface_decl : program.interfaces) {
    for (const Objc3MethodDecl &method : interface_decl.methods) {
      AddAnnotatedImport(imports, method.objc_import_module_name);
    }
  }
  for (const Objc3ProtocolDecl &protocol : program.protocols) {
    for (const Objc3MethodDecl &method : protocol.methods) {
      AddAnnotatedImport(imports, method.objc_import_module_name);
    }
  }

  JsonArray array;
  for (const std::string &import_name : imports) {
    JsonObject import;
    import["from_module"] = JsonValue::String(program.module_name);
    import["to_module"] = JsonValue::String(import_name);
    import["interface_payload_id"] =
        JsonValue::String(import_name + ":standalone-textual-interface:v1");
    import["lock_identity"] =
        JsonValue::String("package-lock:" + import_name + ":trust:v1");
    import["capability_requirements"] =
        StringArray({"modules.public-import-lookup"});
    array.push_back(JsonValue::ObjectValue(std::move(import)));
  }
  return JsonValue::ArrayValue(std::move(array));
}

JsonValue BuildReservedMetadata(const Objc3Program &program,
                                const std::filesystem::path &input_path) {
  JsonArray array;
  for (const Objc3ImplementationDecl &implementation :
       program.implementations) {
    JsonObject reserved;
    reserved["surface"] = JsonValue::String("implementation_declaration");
    reserved["name"] = JsonValue::String(implementation.name);
    reserved["status"] = JsonValue::String("reserved-not-exported");
    reserved["diagnostic"] = JsonValue::String("O3IFC8238");
    reserved["fail_closed"] = JsonValue::Bool(true);
    reserved["source_anchor"] =
        SourceAnchor(input_path, implementation.line, implementation.column);
    array.push_back(JsonValue::ObjectValue(std::move(reserved)));
  }
  return JsonValue::ArrayValue(std::move(array));
}

JsonValue BuildSourceCounts(const Objc3Program &program) {
  JsonObject counts;
  counts["globals"] = SizeValue(program.globals.size());
  counts["protocols"] = SizeValue(program.protocols.size());
  counts["interfaces"] = SizeValue(program.interfaces.size());
  counts["implementations_reserved"] = SizeValue(program.implementations.size());
  counts["functions"] = SizeValue(program.functions.size());
  return JsonValue::ObjectValue(std::move(counts));
}

JsonValue IssueRefs() {
  JsonArray issue_refs;
  issue_refs.push_back(SizeValue(8238u));
  issue_refs.push_back(SizeValue(8208u));
  return JsonValue::ArrayValue(std::move(issue_refs));
}

JsonValue SourceTruthPolicy() {
  JsonObject policy;
  policy["source_truth"] =
      JsonValue::String("native-artifact-schema-fixture-public-command");
  policy["local_temp_source_truth_allowed"] = JsonValue::Bool(false);
  policy["generated_output_source_truth_allowed"] = JsonValue::Bool(false);
  policy["fallback_success_allowed"] = JsonValue::Bool(false);
  policy["schema_registry_required"] = JsonValue::Bool(true);
  policy["public_command_evidence_required"] = JsonValue::Bool(true);
  policy["capability_truth_row"] =
      JsonValue::String("modules.standalone-textual-interface-payload");
  policy["umbrella_readiness_issue_ref"] = SizeValue(8208u);
  return JsonValue::ObjectValue(std::move(policy));
}

JsonValue NegativeCase(std::string case_id,
                       std::string target,
                       std::string expected_failure,
                       std::string diagnostic = std::string{}) {
  JsonObject negative;
  negative["case_id"] = JsonValue::String(std::move(case_id));
  negative["target"] = JsonValue::String(std::move(target));
  negative["expected_failure"] = JsonValue::String(std::move(expected_failure));
  negative["fail_closed"] = JsonValue::Bool(true);
  if (!diagnostic.empty()) {
    negative["diagnostic"] = JsonValue::String(std::move(diagnostic));
  }
  return JsonValue::ObjectValue(std::move(negative));
}

JsonValue NegativeCases() {
  JsonArray cases;
  cases.push_back(NegativeCase("stale-schema", "schema_version",
                               "schema validation failed before import"));
  cases.push_back(NegativeCase("unlocked-import", "imports[0].lock_identity",
                               "package lock/trust identity required"));
  cases.push_back(NegativeCase(
      "hidden-declaration", "declarations[0].kind",
      "implementation declarations remain reserved metadata", "O3IFC8238"));
  cases.push_back(NegativeCase("count-drift", "source_counts.interfaces",
                               "source_counts drift rejected"));
  cases.push_back(NegativeCase(
      "reserved-roundtrip", "interface_roundtrip.parse_status",
      "reserved importer status cannot satisfy support", "O3IFC8238"));
  return JsonValue::ArrayValue(std::move(cases));
}

JsonValue BuildTargetConstraints(const Objc3FrontendOptions &options) {
  JsonObject target;
  target["language_version"] = SizeValue(options.language_version);
  target["language_profile"] = JsonValue::String("canonical");
  target["arc_mode"] = JsonValue::String(
      options.arc_mode == Objc3FrontendArcMode::kEnabled ? "enabled"
                                                         : "disabled");
  target["lowering_contract"] =
      JsonValue::String("objc3-lowering-contract-active");
  return JsonValue::ObjectValue(std::move(target));
}

JsonValue BuildRoundtripRecord(const Objc3Program &program) {
  JsonObject roundtrip;
  roundtrip["payload_id"] = JsonValue::String(
      program.module_name + ":standalone-textual-interface:v1");
  roundtrip["parse_status"] = JsonValue::String("supported");
  roundtrip["semantic_equivalence_status"] =
      JsonValue::String("supported");
  roundtrip["drift_diagnostic"] = JsonValue::String("O3IFC8238");
  return JsonValue::ObjectValue(std::move(roundtrip));
}

}  // namespace

std::string BuildObjc3StandaloneTextualInterfacePayloadArtifact(
    const std::filesystem::path &input_path,
    const Objc3Program &program,
    const Objc3FrontendOptions &options,
    const Objc3FrontendPipelineResult &pipeline_result) {
  JsonObject payload;
  payload["payload_kind"] =
      JsonValue::String(kObjc3StandaloneTextualInterfacePayloadKind);
  payload["schema_version"] =
      JsonValue::String(kObjc3StandaloneTextualInterfacePayloadSchemaId);
  payload["producer_version"] = JsonValue::String("objc3c-native");
  payload["module_id"] = JsonValue::String(program.module_name);
  payload["package_id"] =
      JsonValue::String("unpackaged:" + program.module_name);
  payload["interface_payload_id"] = JsonValue::String(
      program.module_name + ":standalone-textual-interface:v1");
  payload["source_anchor"] = SourceAnchor(input_path, 1, 1);
  payload["target_constraints"] = BuildTargetConstraints(options);
  payload["imports"] = BuildImports(program, options);
  payload["declarations"] = BuildDeclarations(program, input_path);
  payload["reserved_metadata"] = BuildReservedMetadata(program, input_path);
  payload["interface_roundtrip"] = BuildRoundtripRecord(program);
  payload["source_counts"] = BuildSourceCounts(program);
  payload["issue_refs"] = IssueRefs();
  payload["source_truth_policy"] = SourceTruthPolicy();
  payload["negative_cases"] = NegativeCases();
  payload["capability_requirements"] =
      StringArray({"modules.public-import-lookup",
                   "modules.standalone-textual-interface-payload"});
  payload["public_commands"] =
      StringArray({"npm run objc3c -- compile-objc3c <input.objc3>",
                   "npm run objc3c -- validate-standalone-textual-interface-payload"});
  payload["diagnostic_count"] = SizeValue(pipeline_result.program.ast.diagnostics.size());

  objc3::artifacts::json::ArtifactJsonPublicationRequest request;
  request.schema_id = kObjc3StandaloneTextualInterfacePayloadSchemaId;
  request.payload = JsonValue::ObjectValue(std::move(payload));
  const objc3::artifacts::json::ArtifactJsonPublicationResult publication =
      objc3::artifacts::json::PublishRegisteredArtifactJson(request);
  return publication.ok ? publication.artifact_json : std::string{};
}

}  // namespace objc3::artifacts::frontend
