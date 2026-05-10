#include "ir/objc3_ir_frontend_metadata_publication_emitter_groups.h"

#include <sstream>

#include "ir/objc3_ir_frontend_metadata_publication_block_module_lowering.h"
#include "ir/objc3_ir_frontend_metadata_publication_dispatch_lowering.h"
#include "ir/objc3_ir_frontend_metadata_publication_error_safety_lowering.h"

void EmitObjc3IRFrontendLoweringPublicationGroup(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  EmitObjc3IRTypeSymbolDispatchCounterNodes(metadata, out);
  EmitObjc3IRDispatchOwnershipLoweringCounterNodes(metadata, out);
  EmitObjc3IRBlockLoweringCounterNodes(metadata, out);
  EmitObjc3IRTypeModuleLoweringCounterNodes(metadata, out);
  EmitObjc3IRModuleGovernanceLoweringCounterNodes(metadata, out);
  EmitObjc3IRErrorHandlingLoweringCounterNodes(metadata, out);
  EmitObjc3IRSafetyConcurrencyLoweringCounterNodes(metadata, out);
}
