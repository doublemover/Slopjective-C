#include "ir/objc3_ir_frontend_metadata_publication_emitter_groups.h"

#include <sstream>

#include "ir/objc3_ir_frontend_metadata_publication_concurrency.h"
#include "ir/objc3_ir_frontend_metadata_publication_runtime_semantics.h"

void EmitObjc3IRFrontendRuntimeSemanticsPublicationGroup(
    const Objc3IRFrontendMetadata &metadata,
    std::size_t synthesized_property_accessor_count, std::ostringstream &out) {
  EmitObjc3IRDispatchOwnershipMetadataNodes(
      metadata, synthesized_property_accessor_count, out);
  EmitObjc3IRBlockArcMetadataNodes(metadata, out);
  EmitObjc3IRErrorHandlingMetadataNodes(metadata, out);
  EmitObjc3IRConcurrencyRuntimeMetadataNodes(out);
}
