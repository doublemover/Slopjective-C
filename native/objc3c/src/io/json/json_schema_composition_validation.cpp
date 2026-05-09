#include "io/json/json_schema_composition_validation.h"

#include <cstddef>
#include <utility>

#include "io/json/json_schema_errors.h"
#include "io/json/json_schema_validation.h"

namespace objc3::io::json {

void ValidateJsonSchemaCompositionKeywords(const JsonValue &schema_root,
                                           const JsonValue &schema,
                                           const JsonValue &payload,
                                           const std::string &instance_path,
                                           const std::string &schema_path,
                                           JsonSchemaResult &result) {
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
  if (any_of == nullptr) {
    return;
  }
  if (!any_of->IsArray()) {
    AddJsonSchemaContractError(
        result, "invalid_any_of", JsonSchemaKeywordPath(schema_path, "anyOf"),
        "anyOf must be an array of schema objects");
    return;
  }

  bool matched = false;
  bool schema_failed = false;
  const JsonValue::Array &candidates = any_of->AsArray();
  for (std::size_t i = 0; i < candidates.size(); ++i) {
    JsonSchemaResult probe;
    ValidateJsonSchemaNode(schema_root, candidates[i], payload, instance_path,
                           JsonSchemaArrayElementPath(schema_path, "anyOf", i),
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

}  // namespace objc3::io::json
