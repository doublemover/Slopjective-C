#include "pipeline/runtime_import_record_parsing.h"

#include "pipeline/runtime_import_preservation_owners.h"

namespace objc3c::pipeline {

bool ParseRuntimeMetadataSourceRecordSet(
    const RuntimeImportJsonValue::Object &root,
    const std::string &declarations_name,
    Objc3RuntimeMetadataSourceRecordSet &record_set,
    std::string &error) {
  return runtime_import_preservation::
      ParseRuntimeMetadataSourceRecordSetContents(root, declarations_name,
                                                  record_set, error);
}

bool ParseSerializedRuntimeMetadataReusePayload(
    const RuntimeImportJsonValue::Object &root,
    Objc3ImportedRuntimeModuleSurface &surface,
    std::string &error) {
  return runtime_import_preservation::
      ParseSerializedRuntimeMetadataReusePayloadContents(root, surface, error);
}

}  // namespace objc3c::pipeline
