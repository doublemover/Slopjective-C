#include "artifacts/objc3_frontend_runtime_import_artifacts.h"

#include <sstream>
#include <string>
#include <vector>

#include "io/objc3_json.h"
#include "support/objc3_runtime_metadata_record_set.h"

namespace objc3::artifacts::frontend {
namespace {

using objc3::io::EscapeJsonString;

#include "artifacts/objc3_frontend_runtime_import_metadata_record_json_common.inc"
#include "artifacts/objc3_frontend_runtime_import_metadata_record_json_owned_declarations.inc"
#include "artifacts/objc3_frontend_runtime_import_metadata_record_json_references.inc"

}  // namespace

std::string RenderRuntimeOwnedDeclarationsJson(
    const Objc3RuntimeMetadataSourceRecordSet &runtime_metadata_source_records) {
  std::ostringstream out;
  out << "{\n"
      << "    \"classes\": [\n";
  WriteRuntimeOwnedClassDeclarationsJson(out, runtime_metadata_source_records);
  out << "    ],\n"
      << "    \"protocols\": [\n";
  WriteRuntimeOwnedProtocolDeclarationsJson(out,
                                            runtime_metadata_source_records);
  out << "    ],\n"
      << "    \"categories\": [\n";
  WriteRuntimeOwnedCategoryDeclarationsJson(out,
                                            runtime_metadata_source_records);
  out << "    ],\n"
      << "    \"properties\": [\n";
  WriteRuntimeOwnedPropertyDeclarationsJson(out,
                                            runtime_metadata_source_records);
  out << "    ],\n"
      << "    \"methods\": [\n";
  WriteRuntimeOwnedMethodDeclarationsJson(out, runtime_metadata_source_records);
  out << "    ],\n"
      << "    \"ivars\": [\n";
  WriteRuntimeOwnedIvarDeclarationsJson(out, runtime_metadata_source_records);
  out << "    ]\n"
      << "  }";
  return out.str();
}

std::string RenderRuntimeMetadataReferencesJson(
    const Objc3RuntimeMetadataSourceRecordSet &runtime_metadata_source_records) {
  std::ostringstream out;
  out << "[\n";
  RuntimeMetadataReferenceJsonEmitter reference_emitter(out);
  AppendClassRuntimeMetadataReferencesJson(reference_emitter,
                                           runtime_metadata_source_records);
  AppendProtocolRuntimeMetadataReferencesJson(reference_emitter,
                                              runtime_metadata_source_records);
  AppendCategoryRuntimeMetadataReferencesJson(reference_emitter,
                                              runtime_metadata_source_records);
  AppendPropertyRuntimeMetadataReferencesJson(reference_emitter,
                                              runtime_metadata_source_records);
  AppendMethodRuntimeMetadataReferencesJson(reference_emitter,
                                            runtime_metadata_source_records);
  if (reference_emitter.HasReferences()) {
    out << "\n";
  }
  out << "  ]";
  return out.str();
}

}  // namespace objc3::artifacts::frontend
