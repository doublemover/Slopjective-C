#include "artifacts/json/semantic_type_manifest_json.h"

#include <ostream>

#include "artifacts/json/artifact_record_array_json.h"
#include "artifacts/json/semantic_type_manifest_records.h"
#include "io/json/json_writer.h"
#include "sema/objc3_sema_contract_semantic_type_metadata_handoff.h"

namespace objc3::artifacts::json {

using objc3::io::json::JsonObjectWriter;

void WriteSemanticTypeMetadataHandoffManifestObject(
    std::ostream &out, const Objc3SemanticTypeMetadataHandoff &handoff) {
  JsonObjectWriter object(out);
  object.RawJsonField(
      "functions",
      RenderArtifactRecordArray(handoff.functions_lexicographic,
                                WriteSemanticFunctionTypeManifestRecord));
  object.RawJsonField(
      "interfaces",
      RenderArtifactRecordArray(handoff.interfaces_lexicographic,
                                WriteSemanticInterfaceTypeManifestRecord));
  object.RawJsonField(
      "implementations",
      RenderArtifactRecordArray(handoff.implementations_lexicographic,
                                WriteSemanticImplementationTypeManifestRecord));
  object.End();
}

}  // namespace objc3::artifacts::json
