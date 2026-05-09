#include "artifacts/json/semantic_type_manifest_json.h"

#include <cstdint>
#include <ostream>
#include <sstream>
#include <vector>

#include "io/json/json_writer.h"
#include "support/objc3_value_type_names.h"

namespace objc3::artifacts::json {
namespace {

using objc3::io::json::JsonObjectWriter;

void WriteArraySeparator(std::ostream &out, bool &first) {
  if (!first) {
    out << ',';
  }
  first = false;
}

template <typename RecordT, typename WriteRecordFn>
std::string RenderRecordArray(const std::vector<RecordT> &records,
                              WriteRecordFn write_record) {
  std::ostringstream out;
  out << '[';
  bool first = true;
  for (const auto &record : records) {
    WriteArraySeparator(out, first);
    write_record(out, record);
  }
  out << ']';
  return out.str();
}

void WriteCanonicalType(std::ostream &out,
                        const Objc3SemanticCanonicalType &type);

std::string RenderCanonicalType(const Objc3SemanticCanonicalType &type) {
  std::ostringstream out;
  WriteCanonicalType(out, type);
  return out.str();
}

std::string RenderCanonicalTypeArray(
    const std::vector<Objc3SemanticCanonicalType> &types) {
  return RenderRecordArray(types, WriteCanonicalType);
}

void WriteCanonicalType(std::ostream &out,
                        const Objc3SemanticCanonicalType &type) {
  JsonObjectWriter object(out);
  object.StringField("value_type", objc3c::support::ValueTypeName(type.value_type));
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

void WriteMethodTypeMetadata(std::ostream &out,
                             const Objc3SemanticMethodTypeMetadata &metadata) {
  JsonObjectWriter object(out);
  object.StringField("selector", metadata.selector_normalized);
  object.BoolField("is_class_method", metadata.is_class_method);
  object.RawJsonField("return_canonical_type",
                      RenderCanonicalType(metadata.return_canonical_type));
  object.RawJsonField("param_canonical_types",
                      RenderCanonicalTypeArray(metadata.param_canonical_types));
  object.End();
}

void WritePropertyTypeMetadata(
    std::ostream &out, const Objc3SemanticPropertyTypeMetadata &metadata) {
  JsonObjectWriter object(out);
  object.StringField("name", metadata.name);
  object.RawJsonField("canonical_type",
                      RenderCanonicalType(metadata.canonical_type));
  object.End();
}

void WriteFunctionTypeMetadata(
    std::ostream &out, const Objc3SemanticFunctionTypeMetadata &metadata) {
  JsonObjectWriter object(out);
  object.StringField("name", metadata.name);
  object.RawJsonField("return_canonical_type",
                      RenderCanonicalType(metadata.return_canonical_type));
  object.RawJsonField("param_canonical_types",
                      RenderCanonicalTypeArray(metadata.param_canonical_types));
  object.End();
}

void WriteInterfaceTypeMetadata(
    std::ostream &out, const Objc3SemanticInterfaceTypeMetadata &metadata) {
  JsonObjectWriter object(out);
  object.StringField("name", metadata.name);
  object.StringArrayField("generic_parameter_names_source_order",
                          metadata.generic_parameter_names_source_order);
  object.StringArrayField("generic_parameter_variance_source_order",
                          metadata.generic_parameter_variance_source_order);
  object.StringArrayField("adopted_protocols_lexicographic",
                          metadata.adopted_protocols_lexicographic);
  object.RawJsonField("properties",
                      RenderRecordArray(metadata.properties_lexicographic,
                                        WritePropertyTypeMetadata));
  object.RawJsonField("methods",
                      RenderRecordArray(metadata.methods_lexicographic,
                                        WriteMethodTypeMetadata));
  object.End();
}

void WriteImplementationTypeMetadata(
    std::ostream &out,
    const Objc3SemanticImplementationTypeMetadata &metadata) {
  JsonObjectWriter object(out);
  object.StringField("name", metadata.name);
  object.RawJsonField("properties",
                      RenderRecordArray(metadata.properties_lexicographic,
                                        WritePropertyTypeMetadata));
  object.RawJsonField("methods",
                      RenderRecordArray(metadata.methods_lexicographic,
                                        WriteMethodTypeMetadata));
  object.End();
}

}  // namespace

void WriteSemanticTypeMetadataHandoffManifestObject(
    std::ostream &out, const Objc3SemanticTypeMetadataHandoff &handoff) {
  JsonObjectWriter object(out);
  object.RawJsonField("functions",
                      RenderRecordArray(handoff.functions_lexicographic,
                                        WriteFunctionTypeMetadata));
  object.RawJsonField("interfaces",
                      RenderRecordArray(handoff.interfaces_lexicographic,
                                        WriteInterfaceTypeMetadata));
  object.RawJsonField("implementations",
                      RenderRecordArray(handoff.implementations_lexicographic,
                                        WriteImplementationTypeMetadata));
  object.End();
}

}  // namespace objc3::artifacts::json
