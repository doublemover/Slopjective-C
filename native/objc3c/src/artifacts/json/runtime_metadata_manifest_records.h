#pragma once

#include <iosfwd>

#include "artifacts/json/runtime_metadata_manifest_json.h"
#include "io/json/json_writer.h"

namespace objc3::artifacts::json {

void WriteRuntimeMetadataLocationFields(
    objc3::io::json::JsonObjectWriter &object,
    unsigned line,
    unsigned column);
void WriteRuntimeMetadataInterfaceManifestRecord(
    std::ostream &out,
    const Objc3RuntimeMetadataClassSourceRecord &record);
void WriteRuntimeMetadataImplementationManifestRecord(
    std::ostream &out,
    const Objc3RuntimeMetadataClassSourceRecord &record);
void WriteRuntimeMetadataProtocolManifestRecord(
    std::ostream &out,
    const Objc3RuntimeMetadataProtocolSourceRecord &record);
void WriteRuntimeMetadataCategoryManifestRecord(
    std::ostream &out,
    const Objc3RuntimeMetadataCategorySourceRecord &record);
void WriteRuntimeMetadataPropertyManifestRecord(
    std::ostream &out,
    const Objc3RuntimeMetadataPropertySourceRecord &record);
void WriteRuntimeMetadataMethodManifestRecord(
    std::ostream &out,
    const Objc3RuntimeMetadataMethodSourceRecord &record);
void WriteRuntimeMetadataIvarManifestRecord(
    std::ostream &out,
    const Objc3RuntimeMetadataIvarSourceRecord &record);

}  // namespace objc3::artifacts::json
