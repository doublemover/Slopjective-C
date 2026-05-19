#include "ir/objc3_ir_frontend_metadata_publication_block_module_lowering_module_governance_conformance_rows.h"

#include <sstream>

#include "ir/objc3_ir_frontend_metadata.h"
#include "ir/objc3_ir_frontend_metadata_publication_block_module_lowering_module_governance_row_helpers.h"

void EmitObjc3IRModuleGovernanceConformanceLoweringCounterNode(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  EmitObjc3IRModuleGovernanceLoweringCounterRow(
      "!33",
      {metadata.cross_module_conformance_lowering_sites,
       metadata.cross_module_conformance_lowering_namespace_segment_sites,
       metadata.cross_module_conformance_lowering_import_edge_candidate_sites,
       metadata.cross_module_conformance_lowering_object_pointer_type_sites,
       metadata.cross_module_conformance_lowering_pointer_declarator_sites,
       metadata.cross_module_conformance_lowering_normalized_sites,
       metadata.cross_module_conformance_lowering_cache_invalidation_candidate_sites,
       metadata.cross_module_conformance_lowering_contract_violation_sites},
      metadata.deterministic_cross_module_conformance_lowering_handoff, out);
}
