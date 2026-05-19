#include "ir/objc3_ir_frontend_metadata_publication_block_module_lowering_block_storage.h"

#include <sstream>

#include "ir/objc3_ir_frontend_metadata_publication_block_module_lowering_block_storage_block_rows.h"
#include "ir/objc3_ir_frontend_metadata_publication_block_module_lowering_block_storage_storage_rows.h"

void EmitObjc3IRBlockStorageLoweringCounterNodes(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  EmitObjc3IRBlockStorageEscapeLoweringCounterNode(metadata, out);
  EmitObjc3IRBlockCopyDisposeLoweringCounterNode(metadata, out);
}
