#include "ir/objc3_ir_frontend_metadata_publication_block_module_lowering_type_module.h"

#include "ir/objc3_ir_frontend_metadata_publication_block_module_lowering_module_rows.h"
#include "ir/objc3_ir_frontend_metadata_publication_block_module_lowering_type_rows.h"

void EmitObjc3IRTypeModuleLoweringCounterNodes(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  EmitObjc3IRTypeLoweringCounterNodes(metadata, out);
  EmitObjc3IRModuleLoweringCounterNodes(metadata, out);
}
