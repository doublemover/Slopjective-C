#include "artifacts/json/runtime_metadata_manifest_json.h"

#include <ostream>
#include <sstream>

#include "artifacts/json/artifact_record_array_json.h"
#include "artifacts/json/runtime_metadata_manifest_records.h"
#include "io/json/json_writer.h"

namespace objc3::artifacts::json {
namespace {

using objc3::io::json::JsonObjectWriter;
using objc3::io::json::JsonArrayWriter;

template <typename Predicate>
std::string RenderFilteredClassRecordArray(
    const std::vector<Objc3RuntimeMetadataClassSourceRecord> &records,
    Predicate include_record,
    void (*write_record)(std::ostream &,
                         const Objc3RuntimeMetadataClassSourceRecord &)) {
  std::ostringstream out;
  JsonArrayWriter array(out);
  for (const auto &record : records) {
    if (!include_record(record)) {
      continue;
    }
    array.BeginElement();
    write_record(out, record);
  }
  array.End();
  return out.str();
}

}  // namespace

void WriteRuntimeMetadataInterfaceManifestArray(
    std::ostream &out, const Objc3RuntimeMetadataSourceRecordSet &records) {
  out << RenderFilteredClassRecordArray(
      records.classes_lexicographic,
      [](const Objc3RuntimeMetadataClassSourceRecord &record) {
        return record.record_kind == "interface";
      },
      WriteRuntimeMetadataInterfaceManifestRecord);
}

void WriteRuntimeMetadataImplementationManifestArray(
    std::ostream &out, const Objc3RuntimeMetadataSourceRecordSet &records) {
  out << RenderFilteredClassRecordArray(
      records.classes_lexicographic,
      [](const Objc3RuntimeMetadataClassSourceRecord &record) {
        return record.record_kind == "implementation";
      },
      WriteRuntimeMetadataImplementationManifestRecord);
}

void WriteRuntimeMetadataProtocolManifestArray(
    std::ostream &out, const Objc3RuntimeMetadataSourceRecordSet &records) {
  out << RenderArtifactRecordArray(records.protocols_lexicographic,
                                   WriteRuntimeMetadataProtocolManifestRecord);
}

void WriteRuntimeMetadataCategoryManifestArray(
    std::ostream &out, const Objc3RuntimeMetadataSourceRecordSet &records) {
  out << RenderArtifactRecordArray(records.categories_lexicographic,
                                   WriteRuntimeMetadataCategoryManifestRecord);
}

void WriteRuntimeMetadataSourceRecordSetManifestObject(
    std::ostream &out, const Objc3RuntimeMetadataSourceRecordSet &records) {
  JsonObjectWriter object(out);
  object.BoolField("deterministic", records.deterministic);
  object.RawJsonField(
      "properties",
      RenderArtifactRecordArray(records.properties_lexicographic,
                                WriteRuntimeMetadataPropertyManifestRecord));
  object.RawJsonField("methods",
                      RenderArtifactRecordArray(records.methods_lexicographic,
                                                WriteRuntimeMetadataMethodManifestRecord));
  object.RawJsonField("ivars",
                      RenderArtifactRecordArray(records.ivars_lexicographic,
                                                WriteRuntimeMetadataIvarManifestRecord));
  object.End();
}

}  // namespace objc3::artifacts::json
