#include "ir/objc3_ir_frontend_metadata_publication_block_module_lowering_module_governance_module_rows.h"

#include <sstream>

#include "ir/objc3_ir_frontend_metadata.h"
#include "ir/objc3_ir_frontend_metadata_publication_block_module_lowering_module_governance_row_helpers.h"

void EmitObjc3IRModuleGovernanceCacheLoweringCounterNode(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  EmitObjc3IRModuleGovernanceLoweringCounterRow(
      "!32",
      {metadata.incremental_module_cache_invalidation_lowering_sites,
       metadata
           .incremental_module_cache_invalidation_lowering_namespace_segment_sites,
       metadata
           .incremental_module_cache_invalidation_lowering_import_edge_candidate_sites,
       metadata
           .incremental_module_cache_invalidation_lowering_object_pointer_type_sites,
       metadata
           .incremental_module_cache_invalidation_lowering_pointer_declarator_sites,
       metadata.incremental_module_cache_invalidation_lowering_normalized_sites,
       metadata
           .incremental_module_cache_invalidation_lowering_cache_invalidation_candidate_sites,
       metadata
           .incremental_module_cache_invalidation_lowering_contract_violation_sites},
      metadata.deterministic_incremental_module_cache_invalidation_lowering_handoff,
      out);
}
