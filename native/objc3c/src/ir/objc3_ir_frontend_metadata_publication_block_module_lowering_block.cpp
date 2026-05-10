#include "ir/objc3_ir_frontend_metadata_publication_block_module_lowering_block.h"

#include "ir/objc3_ir_frontend_metadata_publication_block_module_lowering_block_abi.h"
#include "ir/objc3_ir_frontend_metadata_publication_block_module_lowering_block_capture.h"
#include "ir/objc3_ir_frontend_metadata_publication_block_module_lowering_block_determinism.h"
#include "ir/objc3_ir_frontend_metadata_publication_block_module_lowering_block_storage.h"

void EmitObjc3IRBlockLoweringCounterNodes(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  EmitObjc3IRBlockCaptureLoweringCounterNodes(metadata, out);
  EmitObjc3IRBlockAbiLoweringCounterNodes(metadata, out);
  EmitObjc3IRBlockStorageLoweringCounterNodes(metadata, out);
  EmitObjc3IRBlockDeterminismLoweringCounterNodes(metadata, out);
}
