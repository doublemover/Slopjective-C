#include "ir/objc3_ir_frontend_metadata_publication_dispatch_lowering_dispatch_semantics_semantic_rows.h"

#include <sstream>

#include "ir/objc3_ir_frontend_metadata.h"
#include "ir/objc3_ir_frontend_metadata_publication_dispatch_lowering_dispatch_semantics_row_helpers.h"

void EmitObjc3IRDispatchSuperSemanticCounterNode(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  BeginObjc3IRDispatchSemanticsCounterRow(
      "!12", metadata.super_dispatch_method_family_message_send_sites, out);
  EmitObjc3IRDispatchSemanticsSizeField(
      metadata.super_dispatch_method_family_receiver_super_identifier_sites,
      out);
  EmitObjc3IRDispatchSemanticsSizeField(
      metadata.super_dispatch_method_family_enabled_sites, out);
  EmitObjc3IRDispatchSemanticsSizeField(
      metadata.super_dispatch_method_family_requires_class_context_sites, out);
  EmitObjc3IRDispatchSemanticsSizeField(
      metadata.super_dispatch_method_family_init_sites, out);
  EmitObjc3IRDispatchSemanticsSizeField(
      metadata.super_dispatch_method_family_copy_sites, out);
  EmitObjc3IRDispatchSemanticsSizeField(
      metadata.super_dispatch_method_family_mutable_copy_sites, out);
  EmitObjc3IRDispatchSemanticsSizeField(
      metadata.super_dispatch_method_family_new_sites, out);
  EmitObjc3IRDispatchSemanticsSizeField(
      metadata.super_dispatch_method_family_none_sites, out);
  EmitObjc3IRDispatchSemanticsSizeField(
      metadata.super_dispatch_method_family_returns_retained_result_sites, out);
  EmitObjc3IRDispatchSemanticsSizeField(
      metadata.super_dispatch_method_family_returns_related_result_sites, out);
  EmitObjc3IRDispatchSemanticsSizeField(
      metadata.super_dispatch_method_family_contract_violation_sites, out);
  EmitObjc3IRDispatchSemanticsBoolField(
      metadata.deterministic_super_dispatch_method_family_handoff, out);
  EndObjc3IRDispatchSemanticsCounterRow(out);
}
