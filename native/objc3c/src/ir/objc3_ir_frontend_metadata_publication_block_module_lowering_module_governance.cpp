#include "ir/objc3_ir_frontend_metadata_publication_block_module_lowering_module_governance.h"

#include <sstream>

#include "ir/objc3_ir_frontend_metadata_publication_block_module_lowering_module_governance_conformance_rows.h"
#include "ir/objc3_ir_frontend_metadata_publication_block_module_lowering_module_governance_governance_rows.h"
#include "ir/objc3_ir_frontend_metadata_publication_block_module_lowering_module_governance_module_rows.h"

void EmitObjc3IRModuleGovernanceLoweringCounterNodes(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  EmitObjc3IRModuleGovernancePartitionLoweringCounterNode(metadata, out);
  EmitObjc3IRModuleGovernanceCacheLoweringCounterNode(metadata, out);
  EmitObjc3IRModuleGovernanceConformanceLoweringCounterNode(metadata, out);
}
