#include "ir/objc3_ir_frontend_metadata_publication_block_module_lowering_module_governance_governance_rows.h"

#include <sstream>

#include "ir/objc3_ir_frontend_metadata.h"
#include "ir/objc3_ir_frontend_metadata_publication_block_module_lowering_module_governance_row_helpers.h"

void EmitObjc3IRModuleGovernancePartitionLoweringCounterNode(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  EmitObjc3IRModuleGovernanceLoweringCounterRow(
      "!31",
      {metadata.public_private_api_partition_lowering_sites,
       metadata.public_private_api_partition_lowering_namespace_segment_sites,
       metadata
           .public_private_api_partition_lowering_import_edge_candidate_sites,
       metadata.public_private_api_partition_lowering_object_pointer_type_sites,
       metadata.public_private_api_partition_lowering_pointer_declarator_sites,
       metadata.public_private_api_partition_lowering_normalized_sites,
       metadata.public_private_api_partition_lowering_contract_violation_sites},
      metadata.deterministic_public_private_api_partition_lowering_handoff,
      out);
}
