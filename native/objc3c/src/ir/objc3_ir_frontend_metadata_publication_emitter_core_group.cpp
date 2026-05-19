#include "ir/objc3_ir_frontend_metadata_publication_emitter_groups.h"

#include <sstream>

#include "ir/objc3_ir_frontend_metadata_publication_core.h"

void EmitObjc3IRFrontendCorePublicationGroup(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  EmitObjc3IRFrontendCoreMetadataPublication(metadata, out);
}
