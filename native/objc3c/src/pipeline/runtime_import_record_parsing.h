#pragma once

#include <string>

#include "pipeline/objc3_frontend_types.h"
#include "pipeline/runtime_import_json_helpers.h"

struct Objc3ImportedRuntimeModuleSurface;

namespace objc3c::pipeline {

bool ParseRuntimeMetadataSourceRecordSet(
    const RuntimeImportJsonValue::Object &root,
    const std::string &declarations_name,
    Objc3RuntimeMetadataSourceRecordSet &record_set,
    std::string &error);

bool ParseSerializedRuntimeMetadataReusePayload(
    const RuntimeImportJsonValue::Object &root,
    Objc3ImportedRuntimeModuleSurface &surface,
    std::string &error);

}  // namespace objc3c::pipeline
