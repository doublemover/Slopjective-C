#include "ir/objc3_ir_frontend_metadata_publication_dispatch_lowering_dispatch_ownership.h"

#include "ir/objc3_ir_frontend_metadata_publication_dispatch_lowering_dispatch_semantics.h"
#include "ir/objc3_ir_frontend_metadata_publication_dispatch_lowering_ownership_arc.h"

void EmitObjc3IRDispatchOwnershipCounterNodes(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  EmitObjc3IRDispatchSemanticsCounterNodes(metadata, out);
  EmitObjc3IROwnershipArcLoweringCounterNodes(metadata, out);
}
