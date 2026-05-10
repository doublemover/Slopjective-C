#include "ir/objc3_ir_frontend_metadata_publication_emitter_groups.h"

#include <sstream>

#include "ir/objc3_ir_frontend_metadata_publication_error_safety_lowering.h"
#include "ir/objc3_ir_lowering_extension_metadata_publication_dispatch.h"
#include "ir/objc3_ir_lowering_extension_metadata_publication_interop.h"
#include "ir/objc3_ir_lowering_extension_metadata_publication_metaprogramming.h"
#include "ir/objc3_ir_lowering_extension_metadata_publication_ownership.h"

void EmitObjc3IRFrontendExtensionPublicationGroup(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  EmitObjc3IRActorDispatchControlMetadataNodes(metadata, out);
  EmitObjc3IRInteropLoweringMetadataNodes(metadata, out);
  EmitObjc3IRMetaprogrammingLoweringMetadataNodes(metadata, out);
  EmitObjc3IRDispatchMetadataPreservationNodes(metadata, out);
  EmitObjc3IROwnershipExtensionMetadataNodes(metadata, out);
  EmitObjc3IRAsyncDiagnosticLoweringCounterNodes(metadata, out);
}
