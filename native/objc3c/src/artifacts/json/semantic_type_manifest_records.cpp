#include "artifacts/json/semantic_type_manifest_records.h"

#include <cstdint>
#include <sstream>

#include "artifacts/json/artifact_record_array_json.h"
#include "io/json/json_writer.h"
#include "support/objc3_value_type_names.h"

namespace objc3::artifacts::json {
namespace {

using objc3::io::json::JsonObjectWriter;

}  // namespace

std::string RenderSemanticCanonicalType(
    const Objc3SemanticCanonicalType &type) {
  std::ostringstream out;
  WriteSemanticCanonicalTypeManifestRecord(out, type);
  return out.str();
}

std::string RenderSemanticCanonicalTypeArray(
    const std::vector<Objc3SemanticCanonicalType> &types) {
  return RenderArtifactRecordArray(types,
                                   WriteSemanticCanonicalTypeManifestRecord);
}

void WriteSemanticCanonicalTypeManifestRecord(
    std::ostream &out,
    const Objc3SemanticCanonicalType &type) {
  JsonObjectWriter object(out);
  object.StringField("value_type",
                     objc3c::support::ValueTypeName(type.value_type));
  object.UnsignedField("kind", static_cast<std::uint64_t>(type.kind));
  object.UnsignedField("nullability",
                       static_cast<std::uint64_t>(type.nullability));
  object.UnsignedField("ownership", static_cast<std::uint64_t>(type.ownership));
  object.StringField("canonical_spelling", type.canonical_spelling);
  object.StringField("object_pointer_type_name", type.object_pointer_type_name);
  object.StringArrayField("generic_arguments_source_order",
                          type.generic_arguments_source_order);
  object.StringArrayField("generic_arguments_lexicographic",
                          type.generic_arguments_lexicographic);
  object.StringField("replay_key", type.replay_key);
  object.BoolField("deterministic", type.deterministic);
  object.End();
}

void WriteSemanticMethodTypeManifestRecord(
    std::ostream &out,
    const Objc3SemanticMethodTypeMetadata &metadata) {
  JsonObjectWriter object(out);
  object.StringField("selector", metadata.selector_normalized);
  object.BoolField("is_class_method", metadata.is_class_method);
  object.RawJsonField("return_canonical_type",
                      RenderSemanticCanonicalType(
                          metadata.return_canonical_type));
  object.RawJsonField("param_canonical_types",
                      RenderSemanticCanonicalTypeArray(
                          metadata.param_canonical_types));
  object.End();
}

void WriteSemanticPropertyTypeManifestRecord(
    std::ostream &out,
    const Objc3SemanticPropertyTypeMetadata &metadata) {
  JsonObjectWriter object(out);
  object.StringField("name", metadata.name);
  object.BoolField("property_behavior_declared",
                   metadata.property_behavior_declared);
  object.StringField("property_behavior_name", metadata.property_behavior_name);
  object.RawJsonField("canonical_type",
                      RenderSemanticCanonicalType(metadata.canonical_type));
  object.End();
}

void WriteSemanticFunctionTypeManifestRecord(
    std::ostream &out,
    const Objc3SemanticFunctionTypeMetadata &metadata) {
  JsonObjectWriter object(out);
  object.StringField("name", metadata.name);
  object.RawJsonField("return_canonical_type",
                      RenderSemanticCanonicalType(
                          metadata.return_canonical_type));
  object.RawJsonField("param_canonical_types",
                      RenderSemanticCanonicalTypeArray(
                          metadata.param_canonical_types));
  object.End();
}

void WriteSemanticInterfaceTypeManifestRecord(
    std::ostream &out,
    const Objc3SemanticInterfaceTypeMetadata &metadata) {
  JsonObjectWriter object(out);
  object.StringField("name", metadata.name);
  object.StringArrayField("generic_parameter_names_source_order",
                          metadata.generic_parameter_names_source_order);
  object.StringArrayField("generic_parameter_variance_source_order",
                          metadata.generic_parameter_variance_source_order);
  object.StringArrayField("adopted_protocols_lexicographic",
                          metadata.adopted_protocols_lexicographic);
  object.RawJsonField("properties",
                      RenderArtifactRecordArray(
                          metadata.properties_lexicographic,
                          WriteSemanticPropertyTypeManifestRecord));
  object.RawJsonField("methods",
                      RenderArtifactRecordArray(
                          metadata.methods_lexicographic,
                          WriteSemanticMethodTypeManifestRecord));
  object.End();
}

void WriteSemanticImplementationTypeManifestRecord(
    std::ostream &out,
    const Objc3SemanticImplementationTypeMetadata &metadata) {
  JsonObjectWriter object(out);
  object.StringField("name", metadata.name);
  object.RawJsonField("properties",
                      RenderArtifactRecordArray(
                          metadata.properties_lexicographic,
                          WriteSemanticPropertyTypeManifestRecord));
  object.RawJsonField("methods",
                      RenderArtifactRecordArray(
                          metadata.methods_lexicographic,
                          WriteSemanticMethodTypeManifestRecord));
  object.End();
}

}  // namespace objc3::artifacts::json
