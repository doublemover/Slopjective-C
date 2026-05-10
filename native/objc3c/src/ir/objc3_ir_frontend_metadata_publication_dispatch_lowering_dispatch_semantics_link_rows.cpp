#include "ir/objc3_ir_frontend_metadata_publication_dispatch_lowering_dispatch_semantics_link_rows.h"

#include <sstream>

#include "ir/objc3_ir_frontend_metadata.h"
#include "ir/objc3_ir_frontend_metadata_publication_dispatch_lowering_dispatch_semantics_row_helpers.h"

void EmitObjc3IRDispatchRuntimeLinkWiringCounterNode(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  BeginObjc3IRDispatchSemanticsCounterRow(
      "!13", metadata.runtime_link_host_link_message_send_sites, out);
  EmitObjc3IRDispatchSemanticsSizeField(
      metadata.runtime_link_host_link_required_sites, out);
  EmitObjc3IRDispatchSemanticsSizeField(
      metadata.runtime_link_host_link_elided_sites, out);
  EmitObjc3IRDispatchSemanticsSizeField(
      metadata.runtime_link_host_link_runtime_dispatch_arg_slots, out);
  EmitObjc3IRDispatchSemanticsSizeField(
      metadata
          .runtime_link_host_link_runtime_dispatch_declaration_parameter_count,
      out);
  EmitObjc3IRDispatchSemanticsStringField(
      metadata.runtime_link_host_link_runtime_dispatch_symbol, out);
  EmitObjc3IRDispatchSemanticsBoolField(
      metadata.runtime_link_host_link_default_runtime_dispatch_symbol_binding,
      out);
  EmitObjc3IRDispatchSemanticsSizeField(
      metadata.runtime_link_host_link_contract_violation_sites, out);
  EmitObjc3IRDispatchSemanticsBoolField(
      metadata.deterministic_runtime_link_host_link_handoff, out);
  EndObjc3IRDispatchSemanticsCounterRow(out);
}
