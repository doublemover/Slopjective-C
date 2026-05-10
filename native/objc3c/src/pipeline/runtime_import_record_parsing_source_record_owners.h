#pragma once

#include <string>

#include "pipeline/runtime_import_json_helpers.h"
#include "runtime/metadata/class_metadata.h"

namespace objc3c::pipeline::runtime_import_preservation {

bool ParseImportedRuntimeClassSourceRecord(
    const RuntimeImportJsonValue::Object &object,
    Objc3RuntimeMetadataClassSourceRecord &record,
    std::string &error);

bool ParseImportedRuntimeProtocolSourceRecord(
    const RuntimeImportJsonValue::Object &object,
    Objc3RuntimeMetadataProtocolSourceRecord &record,
    std::string &error);

bool ParseImportedRuntimeCategorySourceRecord(
    const RuntimeImportJsonValue::Object &object,
    Objc3RuntimeMetadataCategorySourceRecord &record,
    std::string &error);

bool ParseImportedRuntimePropertySourceRecord(
    const RuntimeImportJsonValue::Object &object,
    Objc3RuntimeMetadataPropertySourceRecord &record,
    std::string &error);

bool ParseImportedRuntimeMethodSourceRecord(
    const RuntimeImportJsonValue::Object &object,
    Objc3RuntimeMetadataMethodSourceRecord &record,
    std::string &error);

bool ParseImportedRuntimeIvarSourceRecord(
    const RuntimeImportJsonValue::Object &object,
    Objc3RuntimeMetadataIvarSourceRecord &record,
    std::string &error);

}  // namespace objc3c::pipeline::runtime_import_preservation
