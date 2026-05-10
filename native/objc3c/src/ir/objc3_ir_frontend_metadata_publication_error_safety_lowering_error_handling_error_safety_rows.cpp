#include "ir/objc3_ir_frontend_metadata_publication_error_safety_lowering_error_handling_error_safety_rows.h"

#include <sstream>

#include "ir/objc3_ir_frontend_metadata.h"
#include "ir/objc3_ir_frontend_metadata_publication_error_safety_lowering_error_handling_row_helpers.h"

void EmitObjc3IRNSErrorBridgingLoweringCounterNode(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  EmitObjc3IRErrorHandlingLoweringCounterRow(
      "!36",
      {
          metadata.ns_error_bridging_lowering_sites,
          metadata.ns_error_bridging_lowering_ns_error_parameter_sites,
          metadata.ns_error_bridging_lowering_ns_error_out_parameter_sites,
          metadata.ns_error_bridging_lowering_ns_error_bridge_path_sites,
          metadata.ns_error_bridging_lowering_failable_call_sites,
          metadata.ns_error_bridging_lowering_normalized_sites,
          metadata.ns_error_bridging_lowering_bridge_boundary_sites,
          metadata.ns_error_bridging_lowering_contract_violation_sites,
      },
      metadata.deterministic_ns_error_bridging_lowering_handoff, out);
}
