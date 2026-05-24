#include "artifacts/objc3_frontend_textual_interface_payload_import.h"

#include <algorithm>
#include <iterator>
#include <set>
#include <sstream>
#include <string>
#include <utility>

#include "artifacts/objc3_frontend_textual_interface_payload_artifact.h"
#include "ast/objc3_ast_value_optional_type.h"
#include "io/json/json_parser.h"
#include "io/json/json_value.h"

namespace objc3::artifacts::frontend {
namespace {

using objc3::io::json::JsonParseResult;
using objc3::io::json::JsonValue;
using JsonArray = JsonValue::Array;
using JsonObject = JsonValue::Object;

void AddDiagnostic(Objc3StandaloneTextualInterfaceImportResult &result,
                   std::string message,
                   std::string json_path) {
  result.diagnostics.push_back(
      {kObjc3StandaloneTextualInterfaceImportDiagnostic, std::move(message),
       std::move(json_path)});
}

const JsonValue *FindMember(const JsonObject &object,
                            const std::string &name) {
  const auto iter = object.find(name);
  return iter == object.end() ? nullptr : &iter->second;
}

std::string StringMember(const JsonObject &object,
                         const std::string &name,
                         Objc3StandaloneTextualInterfaceImportResult &result,
                         const std::string &json_path) {
  const JsonValue *value = FindMember(object, name);
  if (value == nullptr || !value->IsString()) {
    AddDiagnostic(result, "required string member missing: " + name, json_path);
    return {};
  }
  const std::string &text = value->AsString();
  if (text.empty()) {
    AddDiagnostic(result, "required string member is empty: " + name,
                  json_path + "/" + name);
  }
  return text;
}

std::string StringMemberAllowEmpty(
    const JsonObject &object,
    const std::string &name,
    Objc3StandaloneTextualInterfaceImportResult &result,
    const std::string &json_path) {
  const JsonValue *value = FindMember(object, name);
  if (value == nullptr || !value->IsString()) {
    AddDiagnostic(result, "required string member missing: " + name,
                  json_path);
    return {};
  }
  return value->AsString();
}

const JsonArray &ArrayMember(
    const JsonObject &object,
    const std::string &name,
    Objc3StandaloneTextualInterfaceImportResult &result,
    const std::string &json_path) {
  static const JsonArray empty;
  const JsonValue *value = FindMember(object, name);
  if (value == nullptr || !value->IsArray()) {
    AddDiagnostic(result, "required array member missing: " + name, json_path);
    return empty;
  }
  return value->AsArray();
}

const JsonObject &ObjectMember(
    const JsonObject &object,
    const std::string &name,
    Objc3StandaloneTextualInterfaceImportResult &result,
    const std::string &json_path) {
  static const JsonObject empty;
  const JsonValue *value = FindMember(object, name);
  if (value == nullptr || !value->IsObject()) {
    AddDiagnostic(result, "required object member missing: " + name, json_path);
    return empty;
  }
  return value->AsObject();
}

double NumberMember(const JsonObject &object,
                    const std::string &name,
                    Objc3StandaloneTextualInterfaceImportResult &result,
                    const std::string &json_path) {
  const JsonValue *value = FindMember(object, name);
  if (value == nullptr || !value->IsNumber()) {
    AddDiagnostic(result, "required numeric member missing: " + name,
                  json_path);
    return 0.0;
  }
  return value->AsNumber();
}

bool BoolMember(const JsonObject &object,
                const std::string &name,
                Objc3StandaloneTextualInterfaceImportResult &result,
                const std::string &json_path) {
  const JsonValue *value = FindMember(object, name);
  if (value == nullptr || !value->IsBool()) {
    AddDiagnostic(result, "required bool member missing: " + name, json_path);
    return false;
  }
  return value->AsBool();
}

void ExpectStringMember(
    const JsonObject &object,
    const std::string &name,
    const std::string &expected,
    Objc3StandaloneTextualInterfaceImportResult &result,
    const std::string &json_path) {
  if (StringMember(object, name, result, json_path) != expected) {
    AddDiagnostic(result,
                  "language evolution reserved contract drift for " + name,
                  json_path + "/" + name);
  }
}

void ExpectNumberMember(
    const JsonObject &object,
    const std::string &name,
    double expected,
    Objc3StandaloneTextualInterfaceImportResult &result,
    const std::string &json_path) {
  if (NumberMember(object, name, result, json_path) != expected) {
    AddDiagnostic(result,
                  "language evolution reserved contract drift for " + name,
                  json_path + "/" + name);
  }
}

void ExpectBoolMember(
    const JsonObject &object,
    const std::string &name,
    bool expected,
    Objc3StandaloneTextualInterfaceImportResult &result,
    const std::string &json_path) {
  if (BoolMember(object, name, result, json_path) != expected) {
    AddDiagnostic(result,
                  "language evolution reserved contract drift for " + name,
                  json_path + "/" + name);
  }
}

bool IsDurableSourcePath(const std::string &path) {
  static constexpr const char *kForbiddenPrefixes[] = {
      "tmp/",  "tmp\\",  "temp/", "temp\\", "generated/",
      "generated\\", "build/", "build\\", "dist/", "dist\\"};
  if (path.empty() || path[0] == '/' || path[0] == '\\' ||
      (path.size() > 1 && path[1] == ':') ||
      path.rfind("../", 0) == 0 || path.rfind("..\\", 0) == 0 ||
      path.find("/../") != std::string::npos ||
      path.find("\\..\\") != std::string::npos ||
      (path.size() >= 3 &&
       (path.compare(path.size() - 3, 3, "/..") == 0 ||
        path.compare(path.size() - 3, 3, "\\..") == 0))) {
    return false;
  }
  return std::none_of(std::begin(kForbiddenPrefixes),
                      std::end(kForbiddenPrefixes),
                      [&](const char *prefix) {
                        return path.rfind(prefix, 0) == 0;
                      });
}

void ValidateSourceAnchor(
    const JsonObject &object,
    Objc3StandaloneTextualInterfaceImportResult &result,
    const std::string &json_path) {
  const JsonObject &anchor = ObjectMember(object, "source_anchor", result,
                                          json_path);
  const std::string path =
      StringMember(anchor, "path", result, json_path + "/source_anchor");
  if (!IsDurableSourcePath(path)) {
    AddDiagnostic(result, "source anchor must point at durable checked-in source",
                  json_path + "/source_anchor/path");
  }
  (void)NumberMember(anchor, "line", result, json_path + "/source_anchor");
  (void)NumberMember(anchor, "column", result, json_path + "/source_anchor");
}

void ValidateStringArray(
    const JsonObject &object,
    const std::string &name,
    Objc3StandaloneTextualInterfaceImportResult &result,
    const std::string &json_path) {
  const JsonArray &array = ArrayMember(object, name, result, json_path);
  std::set<std::string> seen;
  for (std::size_t i = 0; i < array.size(); ++i) {
    if (!array[i].IsString() || array[i].AsString().empty()) {
      AddDiagnostic(result, "array member must contain non-empty strings: " +
                                name,
                    json_path + "/" + name);
      continue;
    }
    if (!seen.insert(array[i].AsString()).second) {
      AddDiagnostic(result, "array member contains duplicate string: " + name,
                    json_path + "/" + name);
    }
  }
}

bool ArrayContainsString(const JsonObject &object,
                         const std::string &name,
                         const std::string &expected) {
  const JsonValue *value = FindMember(object, name);
  if (value == nullptr || !value->IsArray()) {
    return false;
  }
  const JsonArray &array = value->AsArray();
  return std::any_of(array.begin(), array.end(), [&](const JsonValue &item) {
    return item.IsString() && item.AsString() == expected;
  });
}

bool ArrayContainsNumber(const JsonObject &object,
                         const std::string &name,
                         double expected) {
  const JsonValue *value = FindMember(object, name);
  if (value == nullptr || !value->IsArray()) {
    return false;
  }
  const JsonArray &array = value->AsArray();
  return std::any_of(array.begin(), array.end(), [&](const JsonValue &item) {
    return item.IsNumber() && item.AsNumber() == expected;
  });
}

std::size_t CountTopLevelKind(const JsonArray &declarations,
                              const std::string &kind) {
  std::size_t count = 0;
  for (const JsonValue &value : declarations) {
    if (!value.IsObject()) {
      continue;
    }
    const JsonValue *kind_value = FindMember(value.AsObject(), "kind");
    if (kind_value != nullptr && kind_value->IsString() &&
        kind_value->AsString() == kind) {
      ++count;
    }
  }
  return count;
}

void ValidateValueOptionalContract(
    const JsonObject &type_signature,
    Objc3StandaloneTextualInterfaceImportResult &result,
    const std::string &json_path) {
  const JsonObject &contract = ObjectMember(
      type_signature, "value_optional_contract", result, json_path);
  ExpectNumberMember(contract, "issue_ref", 8234.0, result,
                     json_path + "/value_optional_contract");
  ExpectStringMember(contract, "canonical_spelling", "Optional<T>", result,
                     json_path + "/value_optional_contract");
  ExpectStringMember(contract, "source_status",
                     kObjc3ValueOptionalSourceStatus,
                     result, json_path + "/value_optional_contract");
  ExpectBoolMember(contract, "semantic_value_model_supported", true, result,
                   json_path + "/value_optional_contract");
  ExpectBoolMember(contract, "explicit_present_absent_construction_modeled",
                   true, result, json_path + "/value_optional_contract");
  ExpectBoolMember(contract, "binding_narrowing_supported", true, result,
                   json_path + "/value_optional_contract");
  ExpectBoolMember(contract, "unwrap_requires_presence_check", true, result,
                   json_path + "/value_optional_contract");
  ExpectBoolMember(contract, "lowercase_alias_accepted", false, result,
                   json_path + "/value_optional_contract");
  ExpectStringMember(contract, "abi_layout_status",
                     kObjc3ValueOptionalAbiLayoutStatus, result,
                     json_path + "/value_optional_contract");
  ExpectStringMember(contract, "abi_layout_id",
                     kObjc3ValueOptionalAbiLayoutId, result,
                     json_path + "/value_optional_contract");
  ExpectStringMember(contract, "presence_field",
                     kObjc3ValueOptionalPresenceField, result,
                     json_path + "/value_optional_contract");
  ExpectStringMember(contract, "payload_storage_field",
                     kObjc3ValueOptionalPayloadStorageField, result,
                     json_path + "/value_optional_contract");
  ExpectStringMember(contract, "payload_cleanup_contract",
                     kObjc3ValueOptionalPayloadCleanupContract, result,
                     json_path + "/value_optional_contract");
  ExpectStringMember(contract, "absence_state",
                     kObjc3ValueOptionalAbsenceState, result,
                     json_path + "/value_optional_contract");
  ExpectStringMember(contract, "presence_state",
                     kObjc3ValueOptionalPresenceState, result,
                     json_path + "/value_optional_contract");
  ExpectBoolMember(contract, "stable_abi_layout_contract_supported", true,
                   result, json_path + "/value_optional_contract");
  ExpectBoolMember(contract, "interface_roundtrip_supported", true, result,
                   json_path + "/value_optional_contract");
  ExpectBoolMember(contract, "runtime_execution_supported", true, result,
                   json_path + "/value_optional_contract");
  ExpectBoolMember(contract, "lowering_supported", true, result,
                   json_path + "/value_optional_contract");
  ExpectStringMember(contract, "runtime_abi_payload_scope",
                     kObjc3ValueOptionalRuntimeAbiPayloadScope, result,
                     json_path + "/value_optional_contract");
  const JsonArray &supported_payloads = ArrayMember(
      contract, "supported_runtime_payload_forms", result,
      json_path + "/value_optional_contract");
  if (supported_payloads.size() != 3 ||
      !supported_payloads[0].IsString() ||
      supported_payloads[0].AsString() != "i32" ||
      !supported_payloads[1].IsString() ||
      supported_payloads[1].AsString() != "bool" ||
      !supported_payloads[2].IsString() ||
      supported_payloads[2].AsString() != "id") {
    AddDiagnostic(result,
                  "value optional contract must publish only supported i32/bool/id payload ABI",
                  json_path +
                      "/value_optional_contract/supported_runtime_payload_forms");
  }
  const JsonArray &supported_helper_payloads = ArrayMember(
      contract, "supported_runtime_helper_payload_forms", result,
      json_path + "/value_optional_contract");
  if (supported_helper_payloads.size() != 4 ||
      !supported_helper_payloads[0].IsString() ||
      supported_helper_payloads[0].AsString() != "i32" ||
      !supported_helper_payloads[1].IsString() ||
      supported_helper_payloads[1].AsString() != "bool" ||
      !supported_helper_payloads[2].IsString() ||
      supported_helper_payloads[2].AsString() != "id" ||
      !supported_helper_payloads[3].IsString() ||
      supported_helper_payloads[3].AsString() != "i64") {
    AddDiagnostic(
        result,
        "value optional helper contract must publish i32/bool/id plus full-width i64 helper payload ABI",
        json_path +
            "/value_optional_contract/supported_runtime_helper_payload_forms");
  }
  ExpectBoolMember(contract, "full_width_i64_runtime_helper_supported", true,
                   result, json_path + "/value_optional_contract");
  ExpectBoolMember(contract, "full_width_i64_language_call_abi_supported",
                   false, result, json_path + "/value_optional_contract");
  ExpectBoolMember(contract, "ir_payload_emission_supported", true, result,
                   json_path + "/value_optional_contract");
  ExpectBoolMember(contract, "call_abi_lowering_supported", true, result,
                   json_path + "/value_optional_contract");
  ExpectBoolMember(contract, "broad_public_runtime_support_claim_allowed",
                   false, result, json_path + "/value_optional_contract");
  ExpectBoolMember(contract, "nested_value_optional_runtime_supported", false,
                   result, json_path + "/value_optional_contract");
  ExpectBoolMember(contract, "generic_payload_runtime_supported", false,
                   result, json_path + "/value_optional_contract");
  ExpectBoolMember(contract, "property_storage_supported", false, result,
                   json_path + "/value_optional_contract");
  ExpectBoolMember(contract, "ivar_storage_supported", false, result,
                   json_path + "/value_optional_contract");
  ExpectBoolMember(contract, "nil_to_scalar_coercion_allowed", false, result,
                   json_path + "/value_optional_contract");
  ExpectBoolMember(contract, "implicit_nil_absence_allowed", false, result,
                   json_path + "/value_optional_contract");
  ExpectBoolMember(contract, "unchecked_unwrap_allowed", false, result,
                   json_path + "/value_optional_contract");
  ExpectBoolMember(contract, "nullable_pointer_conversion_allowed", false,
                   result, json_path + "/value_optional_contract");
  ExpectBoolMember(contract, "throws_result_conversion_allowed", false, result,
                   json_path + "/value_optional_contract");
  ExpectStringMember(contract, "interface_roundtrip_status",
                     kObjc3ValueOptionalInterfaceRoundtripStatus, result,
                     json_path + "/value_optional_contract");
  const char *required_records[] = {"optional_type", "optional_value",
                                    "optional_flow", "optional_rejection"};
  for (const char *record : required_records) {
    (void)ObjectMember(contract, record, result,
                       json_path + "/value_optional_contract");
  }
}

void ValidateTypedThrowsContract(
    const JsonObject &effects,
    Objc3StandaloneTextualInterfaceImportResult &result,
    const std::string &json_path) {
  const JsonObject &contract =
      ObjectMember(effects, "typed_throws", result, json_path);
  ExpectNumberMember(contract, "issue_ref", 8233.0, result,
                     json_path + "/typed_throws");
  ExpectStringMember(contract, "canonical_syntax", "throws(E)", result,
                     json_path + "/typed_throws");
  const std::string throws_kind =
      StringMember(effects, "throws_kind", result, json_path);
  const std::string declared_error_type =
      StringMemberAllowEmpty(effects, "declared_error_type", result,
                             json_path);
  ExpectStringMember(contract, "throws_kind", throws_kind, result,
                     json_path + "/typed_throws");
  if (StringMemberAllowEmpty(contract, "declared_error_type", result,
                             json_path + "/typed_throws") !=
      declared_error_type) {
    AddDiagnostic(result,
                  "language evolution typed throws contract drift for declared_error_type",
                  json_path + "/typed_throws/declared_error_type");
  }
  const bool throws_declared = BoolMember(effects, "throws", result, json_path);
  const std::string expected_effect_signature_key =
      throws_kind == "none" ? "throws:none"
                            : "throws:" + throws_kind + ":" +
                                  declared_error_type;
  ExpectStringMember(contract, "effect_signature_key",
                     expected_effect_signature_key, result,
                     json_path + "/typed_throws");
  ExpectStringMember(
      contract,
      "callable_compatibility_policy",
      throws_kind == "typed"
          ? "typed-throws-exact-payload-match-error-out-abi"
          : (throws_kind == "untyped" ? "untyped-throws-id-error-carrier"
                                      : "nonthrowing-only"),
      result, json_path + "/typed_throws");
  ExpectStringMember(contract, "semantic_identity_status",
                     "exact-effect-signature-preserved", result,
                     json_path + "/typed_throws");
  if (throws_kind != "none" && throws_kind != "untyped" &&
      throws_kind != "typed") {
    AddDiagnostic(result,
                  "typed throws metadata must be none, untyped, or typed",
                  json_path + "/throws_kind");
  }
  if (throws_kind == "none") {
    if (throws_declared) {
      AddDiagnostic(result,
                    "non-throwing declarations must not set throws=true",
                    json_path + "/throws");
    }
    if (!declared_error_type.empty()) {
      AddDiagnostic(result,
                    "non-throwing declarations must not import an error type",
                    json_path + "/declared_error_type");
    }
  }
  if (throws_kind == "untyped" && !throws_declared) {
    AddDiagnostic(result,
                  "untyped throws declarations must set throws=true",
                  json_path + "/throws");
  }
  if (throws_kind == "untyped" && declared_error_type != "id<Error>") {
    AddDiagnostic(result,
                  "untyped throws declarations must import id<Error> only",
                  json_path + "/declared_error_type");
  }
  if (throws_kind == "typed") {
    if (!throws_declared) {
      AddDiagnostic(result,
                    "typed throws declarations must set throws=true",
                    json_path + "/throws");
    }
    if (declared_error_type.empty()) {
      AddDiagnostic(result,
                    "typed throws declarations must preserve an error payload",
                    json_path + "/declared_error_type");
    }
    ExpectNumberMember(contract, "typed_payload_arity", 1.0, result,
                       json_path + "/typed_throws");
    ExpectStringMember(contract, "typed_payload_status",
                       "source-preserved-error-out-abi-lowered", result,
                       json_path + "/typed_throws");
    ExpectStringMember(contract, "abi_status", "typed-error-out-abi",
                       result, json_path + "/typed_throws");
    ExpectStringMember(contract, "interface_roundtrip_status",
                       "typed-payload-preserved", result,
                       json_path + "/typed_throws");
  } else {
    ExpectNumberMember(contract, "typed_payload_arity", 0.0, result,
                       json_path + "/typed_throws");
    ExpectStringMember(contract, "typed_payload_status", "not-declared",
                       result, json_path + "/typed_throws");
    ExpectStringMember(contract, "abi_status",
                       throws_kind == "untyped" ? "untyped-error-out-abi"
                                                : "none",
                       result, json_path + "/typed_throws");
    ExpectStringMember(contract, "interface_roundtrip_status",
                       "not-applicable", result,
                       json_path + "/typed_throws");
  }
  (void)StringMemberAllowEmpty(contract, "typed_payload_generic_suffix", result,
                               json_path + "/typed_throws");
  ExpectBoolMember(contract, "typed_payload_generic_suffix_terminated", true,
                   result, json_path + "/typed_throws");
  (void)NumberMember(contract, "typed_payload_pointer_depth", result,
                     json_path + "/typed_throws");
  ExpectBoolMember(contract, "typed_payload_lowering_ready",
                   throws_kind == "typed", result,
                   json_path + "/typed_throws");
  ExpectBoolMember(contract, "runtime_execution_claimed",
                   throws_kind == "typed", result,
                   json_path + "/typed_throws");
  ExpectBoolMember(contract, "silent_erasure_allowed", false, result,
                   json_path + "/typed_throws");
  ExpectBoolMember(contract, "multi_payload_supported", false, result,
                   json_path + "/typed_throws");
}

void ValidateLanguageEvolutionReservedContracts(
    const std::string &kind,
    const JsonObject &type_signature,
    const JsonObject &effects,
    Objc3StandaloneTextualInterfaceImportResult &result,
    const std::string &json_path) {
  if (kind == "function" || kind == "method" || kind == "property") {
    ValidateValueOptionalContract(type_signature, result,
                                  json_path + "/type_signature");
  }
  if (kind == "function" || kind == "method") {
    ValidateTypedThrowsContract(effects, result, json_path + "/effects");
  }
}

void ValidateImport(
    const JsonValue &value,
    Objc3StandaloneTextualInterfaceImportResult &result,
    const std::string &json_path,
    const std::string &module_id) {
  if (!value.IsObject()) {
    AddDiagnostic(result, "interface import record must be an object",
                  json_path);
    return;
  }
  const JsonObject &import = value.AsObject();
  if (StringMember(import, "from_module", result, json_path) != module_id) {
    AddDiagnostic(result, "interface import from_module must match module_id",
                  json_path + "/from_module");
  }
  (void)StringMember(import, "to_module", result, json_path);
  (void)StringMember(import, "interface_payload_id", result, json_path);
  const std::string lock_identity =
      StringMember(import, "lock_identity", result, json_path);
  if (lock_identity.rfind("package-lock:", 0) != 0 ||
      lock_identity.find(":trust:") == std::string::npos) {
    AddDiagnostic(result,
                  "interface import must carry package lock/trust identity",
                  json_path + "/lock_identity");
  }
  ValidateStringArray(import, "capability_requirements", result, json_path);
  if (!ArrayContainsString(import, "capability_requirements",
                           "modules.public-import-lookup")) {
    AddDiagnostic(result,
                  "interface import missing public module lookup capability",
                  json_path + "/capability_requirements");
  }
}

void ValidateDeclaration(
    const JsonValue &value,
    Objc3StandaloneTextualInterfaceImportResult &result,
    const std::string &json_path,
    std::set<std::string> &declaration_ids) {
  if (!value.IsObject()) {
    AddDiagnostic(result, "interface declaration record must be an object",
                  json_path);
    return;
  }
  const JsonObject &declaration = value.AsObject();
  const std::string declaration_id =
      StringMember(declaration, "declaration_id", result, json_path);
  if (!declaration_ids.insert(declaration_id).second) {
    AddDiagnostic(result, "duplicate declaration_id in interface payload",
                  json_path + "/declaration_id");
  }
  const std::string kind = StringMember(declaration, "kind", result, json_path);
  if (kind == "implementation" || kind == "private_implementation") {
    AddDiagnostic(result,
                  "implementation declarations must remain reserved metadata",
                  json_path + "/kind");
  }
  const std::string visibility =
      StringMember(declaration, "visibility", result, json_path);
  if (visibility != "public" && visibility != "package" &&
      visibility != "internal") {
    AddDiagnostic(result, "unsupported declaration visibility in payload",
                  json_path + "/visibility");
  }
  (void)StringMember(declaration, "name", result, json_path);
  const JsonObject &type_signature =
      ObjectMember(declaration, "type_signature", result, json_path);
  const JsonObject &effects =
      ObjectMember(declaration, "effects", result, json_path);
  (void)ArrayMember(declaration, "generics", result, json_path);
  (void)ObjectMember(declaration, "ownership", result, json_path);
  (void)ObjectMember(declaration, "runtime_metadata", result, json_path);
  ValidateLanguageEvolutionReservedContracts(kind, type_signature, effects,
                                             result, json_path);
  const JsonValue *members = FindMember(type_signature, "members");
  if (members != nullptr) {
    if (!members->IsArray()) {
      AddDiagnostic(result, "interface members must be declaration records",
                    json_path + "/type_signature/members");
    } else {
      const JsonArray &member_array = members->AsArray();
      for (std::size_t i = 0; i < member_array.size(); ++i) {
        ValidateDeclaration(member_array[i], result,
                            json_path + "/type_signature/members/" +
                                std::to_string(i),
                            declaration_ids);
      }
    }
  }
  ValidateSourceAnchor(declaration, result, json_path);
}

void ValidateReservedMetadata(
    const JsonValue &value,
    Objc3StandaloneTextualInterfaceImportResult &result,
    const std::string &json_path) {
  if (!value.IsObject()) {
    AddDiagnostic(result, "reserved metadata record must be an object",
                  json_path);
    return;
  }
  const JsonObject &metadata = value.AsObject();
  (void)StringMember(metadata, "surface", result, json_path);
  (void)StringMember(metadata, "name", result, json_path);
  (void)StringMember(metadata, "status", result, json_path);
  const std::string diagnostic =
      StringMember(metadata, "diagnostic", result, json_path);
  if (diagnostic.rfind("O3", 0) != 0) {
    AddDiagnostic(result, "reserved metadata diagnostic must be stable O3 code",
                  json_path + "/diagnostic");
  }
  const JsonValue *fail_closed = FindMember(metadata, "fail_closed");
  if (fail_closed == nullptr || !fail_closed->IsBool() ||
      !fail_closed->AsBool()) {
    AddDiagnostic(result, "reserved metadata must fail closed",
                  json_path + "/fail_closed");
  }
  ValidateSourceAnchor(metadata, result, json_path);
}

void ValidateSourceCounts(
    const JsonObject &source_counts,
    const JsonArray &declarations,
    const JsonArray &reserved_metadata,
    Objc3StandaloneTextualInterfaceImportResult &result) {
  const std::size_t globals = CountTopLevelKind(declarations, "global");
  const std::size_t protocols = CountTopLevelKind(declarations, "protocol");
  const std::size_t interfaces = CountTopLevelKind(declarations, "interface") +
                                 CountTopLevelKind(declarations,
                                                   "actor_interface");
  const std::size_t functions = CountTopLevelKind(declarations, "function");
  const std::size_t implementations_reserved = reserved_metadata.size();

  const struct {
    const char *name;
    std::size_t expected;
  } count_checks[] = {{"globals", globals},
                      {"protocols", protocols},
                      {"interfaces", interfaces},
                      {"implementations_reserved", implementations_reserved},
                      {"functions", functions}};

  for (const auto &check : count_checks) {
    const double published =
        NumberMember(source_counts, check.name, result, "/source_counts");
    if (static_cast<std::size_t>(published) != check.expected) {
      AddDiagnostic(result, "source_counts drift for " + std::string(check.name),
                    std::string("/source_counts/") + check.name);
    }
  }
}

void ValidateSourceTruthPolicy(
    const JsonObject &payload,
    Objc3StandaloneTextualInterfaceImportResult &result) {
  (void)ArrayMember(payload, "issue_refs", result, "/");
  if (!ArrayContainsNumber(payload, "issue_refs", 8238.0) ||
      !ArrayContainsNumber(payload, "issue_refs", 8233.0) ||
      !ArrayContainsNumber(payload, "issue_refs", 8234.0) ||
      !ArrayContainsNumber(payload, "issue_refs", 8208.0)) {
    AddDiagnostic(result,
                  "payload issue_refs must include #8238, #8233, #8234, and #8208",
                  "/issue_refs");
  }

  const JsonObject &policy =
      ObjectMember(payload, "source_truth_policy", result, "/");
  if (StringMember(policy, "source_truth", result,
                   "/source_truth_policy") !=
      "native-artifact-schema-fixture-public-command") {
    AddDiagnostic(result, "source truth policy identity drifted",
                  "/source_truth_policy/source_truth");
  }
  if (BoolMember(policy, "local_temp_source_truth_allowed", result,
                 "/source_truth_policy")) {
    AddDiagnostic(result, "local temp source truth must be rejected",
                  "/source_truth_policy/local_temp_source_truth_allowed");
  }
  if (BoolMember(policy, "generated_output_source_truth_allowed", result,
                 "/source_truth_policy")) {
    AddDiagnostic(result, "generated output source truth must be rejected",
                  "/source_truth_policy/generated_output_source_truth_allowed");
  }
  if (BoolMember(policy, "fallback_success_allowed", result,
                 "/source_truth_policy")) {
    AddDiagnostic(result, "fallback success must be rejected",
                  "/source_truth_policy/fallback_success_allowed");
  }
  if (!BoolMember(policy, "schema_registry_required", result,
                  "/source_truth_policy")) {
    AddDiagnostic(result, "schema registry ownership is required",
                  "/source_truth_policy/schema_registry_required");
  }
  if (!BoolMember(policy, "public_command_evidence_required", result,
                  "/source_truth_policy")) {
    AddDiagnostic(result, "public command evidence is required",
                  "/source_truth_policy/public_command_evidence_required");
  }
  if (StringMember(policy, "capability_truth_row", result,
                   "/source_truth_policy") !=
      "modules.standalone-textual-interface-payload") {
    AddDiagnostic(result, "capability truth row drifted",
                  "/source_truth_policy/capability_truth_row");
  }
  if (NumberMember(policy, "umbrella_readiness_issue_ref", result,
                   "/source_truth_policy") != 8208.0) {
    AddDiagnostic(result, "umbrella readiness issue ref must be #8208",
                  "/source_truth_policy/umbrella_readiness_issue_ref");
  }
}

void ValidateNegativeCases(
    const JsonObject &payload,
    Objc3StandaloneTextualInterfaceImportResult &result) {
  const JsonArray &negative_cases =
      ArrayMember(payload, "negative_cases", result, "/");
  std::set<std::string> missing = {"stale-schema", "unlocked-import",
                                   "hidden-declaration", "count-drift",
                                   "reserved-roundtrip",
                                   "typed-throws-abi-lowering",
                                   "typed-throws-interface-contract-drift",
                                   "value-optional-lowering",
                                   "value-optional-layout-drift"};
  for (std::size_t i = 0; i < negative_cases.size(); ++i) {
    const std::string json_path = "/negative_cases/" + std::to_string(i);
    if (!negative_cases[i].IsObject()) {
      AddDiagnostic(result, "negative case record must be an object",
                    json_path);
      continue;
    }
    const JsonObject &record = negative_cases[i].AsObject();
    const std::string case_id =
        StringMember(record, "case_id", result, json_path);
    missing.erase(case_id);
    (void)StringMember(record, "target", result, json_path);
    (void)StringMember(record, "expected_failure", result, json_path);
    if (!BoolMember(record, "fail_closed", result, json_path)) {
      AddDiagnostic(result, "negative case must fail closed",
                    json_path + "/fail_closed");
    }
  }
  for (const std::string &case_id : missing) {
    AddDiagnostic(result, "missing negative case contract: " + case_id,
                  "/negative_cases");
  }
}

std::string BuildReplayKey(
    const Objc3StandaloneTextualInterfaceImportResult &result) {
  std::ostringstream out;
  out << "objc3c.standalone-textual-interface-import.v1"
      << ";payload=" << result.payload_id << ";module=" << result.module_id
      << ";package=" << result.package_id << ";imports="
      << result.import_count << ";declarations=" << result.declaration_count
      << ";reserved=" << result.reserved_metadata_count
      << ";diagnostics=" << result.diagnostic_count;
  return out.str();
}

}  // namespace

Objc3StandaloneTextualInterfaceImportResult
ValidateObjc3StandaloneTextualInterfacePayloadImport(
    std::string_view payload_json) {
  Objc3StandaloneTextualInterfaceImportResult result;
  const JsonParseResult parsed = objc3::io::json::ParseJson(payload_json);
  if (!parsed.ok() || !parsed.value.IsObject()) {
    AddDiagnostic(result, "standalone textual interface payload is not JSON",
                  "/");
    result.diagnostic_count = result.diagnostics.size();
    result.replay_key = BuildReplayKey(result);
    return result;
  }

  const JsonObject &payload = parsed.value.AsObject();
  if (StringMember(payload, "payload_kind", result, "/") !=
      kObjc3StandaloneTextualInterfacePayloadKind) {
    AddDiagnostic(result, "payload_kind does not match textual interface v1",
                  "/payload_kind");
  }
  if (StringMember(payload, "schema_version", result, "/") !=
      kObjc3StandaloneTextualInterfacePayloadSchemaId) {
    AddDiagnostic(result, "schema_version does not match registered schema",
                  "/schema_version");
  }
  result.module_id = StringMember(payload, "module_id", result, "/");
  result.package_id = StringMember(payload, "package_id", result, "/");
  result.payload_id =
      StringMember(payload, "interface_payload_id", result, "/");
  if (result.payload_id != result.module_id + ":standalone-textual-interface:v1") {
    AddDiagnostic(result, "interface_payload_id must be module deterministic",
                  "/interface_payload_id");
  }

  (void)StringMember(payload, "producer_version", result, "/");
  ValidateSourceAnchor(payload, result, "/");
  const JsonObject &target_constraints =
      ObjectMember(payload, "target_constraints", result, "/");
  if (StringMember(target_constraints, "language_profile", result,
                   "/target_constraints") != "canonical") {
    AddDiagnostic(result, "interface import only accepts canonical profile",
                  "/target_constraints/language_profile");
  }
  (void)NumberMember(target_constraints, "language_version", result,
                     "/target_constraints");
  (void)StringMember(target_constraints, "arc_mode", result,
                     "/target_constraints");
  (void)StringMember(target_constraints, "lowering_contract", result,
                     "/target_constraints");

  const JsonArray &imports = ArrayMember(payload, "imports", result, "/");
  for (std::size_t i = 0; i < imports.size(); ++i) {
    ValidateImport(imports[i], result, "/imports/" + std::to_string(i),
                   result.module_id);
  }

  const JsonArray &declarations =
      ArrayMember(payload, "declarations", result, "/");
  std::set<std::string> declaration_ids;
  for (std::size_t i = 0; i < declarations.size(); ++i) {
    ValidateDeclaration(declarations[i], result,
                        "/declarations/" + std::to_string(i),
                        declaration_ids);
  }

  const JsonArray &reserved_metadata =
      ArrayMember(payload, "reserved_metadata", result, "/");
  for (std::size_t i = 0; i < reserved_metadata.size(); ++i) {
    ValidateReservedMetadata(reserved_metadata[i], result,
                             "/reserved_metadata/" + std::to_string(i));
  }

  const JsonObject &roundtrip =
      ObjectMember(payload, "interface_roundtrip", result, "/");
  if (StringMember(roundtrip, "payload_id", result, "/interface_roundtrip") !=
      result.payload_id) {
    AddDiagnostic(result, "roundtrip payload_id must match payload identity",
                  "/interface_roundtrip/payload_id");
  }
  if (StringMember(roundtrip, "parse_status", result,
                   "/interface_roundtrip") != "supported") {
    AddDiagnostic(result, "interface importer parse support must be supported",
                  "/interface_roundtrip/parse_status");
  }
  if (StringMember(roundtrip, "semantic_equivalence_status", result,
                   "/interface_roundtrip") != "supported") {
    AddDiagnostic(result,
                  "interface semantic equivalence support must be supported",
                  "/interface_roundtrip/semantic_equivalence_status");
  }
  if (StringMember(roundtrip, "drift_diagnostic", result,
                   "/interface_roundtrip")
          .rfind("O3", 0) != 0) {
    AddDiagnostic(result, "roundtrip drift diagnostic must be stable O3 code",
                  "/interface_roundtrip/drift_diagnostic");
  }

  const JsonObject &source_counts =
      ObjectMember(payload, "source_counts", result, "/");
  ValidateSourceCounts(source_counts, declarations, reserved_metadata, result);
  ValidateSourceTruthPolicy(payload, result);
  ValidateNegativeCases(payload, result);
  ValidateStringArray(payload, "capability_requirements", result, "/");
  if (!ArrayContainsString(payload, "capability_requirements",
                           "modules.public-import-lookup")) {
    AddDiagnostic(result,
                  "payload missing public module lookup capability",
                  "/capability_requirements");
  }
  if (!ArrayContainsString(payload, "capability_requirements",
                           "modules.standalone-textual-interface-payload")) {
    AddDiagnostic(result,
                  "payload missing standalone textual interface capability",
                  "/capability_requirements");
  }
  ValidateStringArray(payload, "public_commands", result, "/");
  if (!ArrayContainsString(
          payload, "public_commands",
          "npm run objc3c -- validate-standalone-textual-interface-payload")) {
    AddDiagnostic(result, "payload missing public validation command",
                  "/public_commands");
  }

  result.import_count = imports.size();
  result.declaration_count = declarations.size();
  result.reserved_metadata_count = reserved_metadata.size();
  result.diagnostic_count = result.diagnostics.size();
  result.parse_status_supported = result.diagnostics.empty();
  result.semantic_equivalence_supported = result.diagnostics.empty();
  result.replay_key = BuildReplayKey(result);
  return result;
}

}  // namespace objc3::artifacts::frontend
