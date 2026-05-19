#include "ir/objc3_ir_frontend_metadata_publication_block_module_lowering_type_rows.h"

#include "ir/objc3_ir_frontend_metadata_publication_block_module_lowering_type_generic_constraints.h"
#include "ir/objc3_ir_frontend_metadata_publication_block_module_lowering_type_generic_metadata_abi.h"
#include "ir/objc3_ir_frontend_metadata_publication_block_module_lowering_type_nullability_protocol.h"
#include "ir/objc3_ir_frontend_metadata_publication_block_module_lowering_type_variance_bridge.h"

void EmitObjc3IRTypeLoweringCounterNodes(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  EmitObjc3IRTypeGenericConstraintLoweringCounterNode(metadata, out);
  EmitObjc3IRTypeNullabilityProtocolLoweringCounterNodes(metadata, out);
  EmitObjc3IRTypeVarianceBridgeLoweringCounterNode(metadata, out);
  EmitObjc3IRTypeGenericMetadataAbiLoweringCounterNode(metadata, out);
}
