#pragma once

#include <cstddef>
#include <iosfwd>
#include <string>
#include <vector>

#include "ast/objc3_ast_contracts.h"
#include "runtime/metadata/runtime_metadata_model.h"

namespace objc3::artifacts::json {

void WriteRuntimeMetadataInterfaceManifestArray(
    std::ostream &out, const Objc3RuntimeMetadataSourceRecordSet &records);
void WriteRuntimeMetadataImplementationManifestArray(
    std::ostream &out, const Objc3RuntimeMetadataSourceRecordSet &records);
void WriteRuntimeMetadataProtocolManifestArray(
    std::ostream &out, const Objc3RuntimeMetadataSourceRecordSet &records);
void WriteRuntimeMetadataCategoryManifestArray(
    std::ostream &out, const Objc3RuntimeMetadataSourceRecordSet &records);
void WriteRuntimeMetadataSourceRecordSetManifestObject(
    std::ostream &out, const Objc3RuntimeMetadataSourceRecordSet &records);

}  // namespace objc3::artifacts::json
