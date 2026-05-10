#include "ir/objc3_ir_frontend_metadata_publication_dispatch_lowering.h"

#include "ir/objc3_ir_frontend_metadata_publication_dispatch_lowering_dispatch_ownership.h"
#include "ir/objc3_ir_frontend_metadata_publication_dispatch_lowering_type_symbol.h"

void EmitObjc3IRTypeSymbolDispatchCounterNodes(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  EmitObjc3IRTypeSymbolDispatchLoweringCounterNodes(metadata, out);
}

void EmitObjc3IRDispatchOwnershipLoweringCounterNodes(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  EmitObjc3IRDispatchOwnershipCounterNodes(metadata, out);
}
