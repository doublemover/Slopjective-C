#include "io/json/json_schema_validation.h"

#include "io/json/json_equivalence.h"
#include "io/json/json_pointer.h"
#include "io/json/json_schema_array_validation.h"
#include "io/json/json_schema_errors.h"
#include "io/json/json_schema_object_validation.h"
#include "io/json/json_schema_scalar_validation.h"
#include "io/json/json_schema_subschema.h"
#include "io/json/json_schema_type.h"

#include <cstddef>
#include <sstream>
#include <utility>

namespace objc3::io::json {

void ValidateJsonSchemaNode(const JsonValue &schema_root,
                            const JsonValue &schema,
                            const JsonValue &payload,
                            std::string instance_path,
                            std::string schema_path,
                            JsonSchemaResult &result) {
  if (!schema.IsObject()) {
    AddJsonSchemaContractError(result, "invalid_schema_node", schema_path,
                               "schema node must be a JSON object");
    return;
  }
  if (const JsonValue *ref = schema.Find("$ref"); ref != nullptr) {
    if (!ref->IsString()) {
      AddJsonSchemaContractError(
          result, "invalid_ref", JsonSchemaKeywordPath(schema_path, "$ref"),
          "$ref must be a local JSON pointer string");
      return;
    }
    const JsonValue *resolved =
        ResolveLocalJsonPointerRef(schema_root, ref->AsString());
    if (resolved == nullptr) {
      AddJsonSchemaContractError(
          result, "unresolved_ref", JsonSchemaKeywordPath(schema_path, "$ref"),
          "unresolved schema reference " + ref->AsString());
      return;
    }
    ValidateJsonSchemaNode(schema_root, *resolved, payload, instance_path,
                           ref->AsString(), result);
    return;
  }
  const JsonValue *all_of = schema.Find("allOf");
  if (all_of != nullptr) {
    if (!all_of->IsArray()) {
      AddJsonSchemaContractError(
          result, "invalid_all_of", JsonSchemaKeywordPath(schema_path, "allOf"),
          "allOf must be an array of schema objects");
    } else {
      const JsonValue::Array &candidates = all_of->AsArray();
      for (std::size_t i = 0; i < candidates.size(); ++i) {
        ValidateJsonSchemaNode(schema_root, candidates[i], payload,
                               instance_path,
                               JsonSchemaArrayElementPath(schema_path, "allOf",
                                                          i),
                               result);
      }
    }
  }
  const JsonValue *any_of = schema.Find("anyOf");
  if (any_of != nullptr) {
    if (!any_of->IsArray()) {
      AddJsonSchemaContractError(
          result, "invalid_any_of", JsonSchemaKeywordPath(schema_path, "anyOf"),
          "anyOf must be an array of schema objects");
    } else {
      bool matched = false;
      bool schema_failed = false;
      const JsonValue::Array &candidates = any_of->AsArray();
      for (std::size_t i = 0; i < candidates.size(); ++i) {
        JsonSchemaResult probe;
        ValidateJsonSchemaNode(schema_root, candidates[i], payload,
                               instance_path,
                               JsonSchemaArrayElementPath(schema_path, "anyOf",
                                                          i),
                               probe);
        if (HasJsonSchemaContractIssue(probe)) {
          schema_failed = true;
          for (JsonSchemaIssue &issue : probe.errors) {
            if (issue.domain == "schema") {
              AppendJsonSchemaIssue(result, std::move(issue));
            }
          }
          continue;
        }
        if (probe.ok) {
          matched = true;
          break;
        }
      }
      if (!matched && !schema_failed) {
        AddJsonSchemaPayloadError(
            result, "any_of", instance_path,
            JsonSchemaKeywordPath(schema_path, "anyOf"),
            "value did not match any allowed schema");
      }
    }
  }
  if (const JsonValue *schema_type = schema.Find("type");
      schema_type != nullptr) {
    if (!JsonSchemaMatchesType(*schema_type, payload)) {
      std::ostringstream out;
      out << "expected " << DescribeExpectedJsonSchemaType(*schema_type)
          << " but found "
          << JsonSchemaValueTypeName(payload);
      AddJsonSchemaPayloadError(
          result, "type_mismatch", instance_path,
          JsonSchemaKeywordPath(schema_path, "type"), out.str());
      return;
    }
  }
  const JsonValue *const_value = schema.Find("const");
  if (const_value != nullptr && !JsonEquals(*const_value, payload)) {
    AddJsonSchemaPayloadError(
        result, "const_mismatch", instance_path,
        JsonSchemaKeywordPath(schema_path, "const"),
        "value did not match const value");
  }
  const JsonValue *enum_values = schema.Find("enum");
  if (enum_values != nullptr && enum_values->IsArray()) {
    bool matched = false;
    for (const JsonValue &candidate : enum_values->AsArray()) {
      if (JsonEquals(candidate, payload)) {
        matched = true;
        break;
      }
    }
    if (!matched) {
      AddJsonSchemaPayloadError(
          result, "enum_mismatch", instance_path,
          JsonSchemaKeywordPath(schema_path, "enum"),
          "value did not match enum values");
    }
  }
  const JsonValue *properties = schema.Find("properties");
  ValidateJsonSchemaObjectFields(schema_root, schema, payload, properties,
                                 instance_path, schema_path, result);
  ValidateJsonSchemaArrayFields(schema_root, schema, payload, instance_path,
                                schema_path, result);
  ValidateJsonSchemaScalarFields(schema, payload, instance_path, schema_path,
                                 result);
}

}  // namespace objc3::io::json
