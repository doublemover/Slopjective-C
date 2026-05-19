#include "ir/objc3_ir_frontend_metadata_publication_dispatch_lowering_dispatch_semantics_dispatch_rows.h"

#include <sstream>

#include "ir/objc3_ir_frontend_metadata.h"
#include "ir/objc3_ir_frontend_metadata_publication_dispatch_lowering_dispatch_semantics_row_helpers.h"

void EmitObjc3IRDispatchNilReceiverLoweringCounterNode(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  BeginObjc3IRDispatchSemanticsCounterRow(
      "!11", metadata.nil_receiver_semantics_foldability_message_send_sites,
      out);
  EmitObjc3IRDispatchSemanticsSizeField(
      metadata.nil_receiver_semantics_foldability_receiver_nil_literal_sites,
      out);
  EmitObjc3IRDispatchSemanticsSizeField(
      metadata.nil_receiver_semantics_foldability_enabled_sites, out);
  EmitObjc3IRDispatchSemanticsSizeField(
      metadata.nil_receiver_semantics_foldability_foldable_sites, out);
  EmitObjc3IRDispatchSemanticsSizeField(
      metadata.nil_receiver_semantics_foldability_runtime_dispatch_required_sites,
      out);
  EmitObjc3IRDispatchSemanticsSizeField(
      metadata.nil_receiver_semantics_foldability_non_nil_receiver_sites, out);
  EmitObjc3IRDispatchSemanticsSizeField(
      metadata.nil_receiver_semantics_foldability_contract_violation_sites,
      out);
  EmitObjc3IRDispatchSemanticsBoolField(
      metadata.deterministic_nil_receiver_semantics_foldability_handoff, out);
  EndObjc3IRDispatchSemanticsCounterRow(out);
}
