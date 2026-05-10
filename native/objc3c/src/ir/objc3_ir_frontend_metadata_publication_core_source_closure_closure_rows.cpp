#include "ir/objc3_ir_frontend_metadata_publication_core_source_closure_closure_rows.h"

#include <sstream>

#include "ir/objc3_ir_frontend_metadata.h"
#include "ir/objc3_ir_frontend_metadata_publication_core_source_closure_row_helpers.h"

void EmitObjc3IRFrontendProtocolCategorySourceClosureRow(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  // executable protocol/category source-closure anchor: the IR handoff now
  // carries protocol inheritance, category attachment, and adopted-protocol
  // conformance identities so later object-model semantic issues consume the
  // same fail-closed source model.
  // object-model semantic-rule freeze anchor: IR stays evidence-only for the
  // frozen semantic boundary covering realization legality, inheritance
  // legality, override compatibility, protocol conformance, and deterministic
  // category merge behavior; executable enforcement begins in later lane-B
  // work.
  // protocol-conformance implementation anchor: IR remains an evidence-only
  // consumer of the sema-owned protocol conformance result while publishing the
  // same protocol/category source identities after sema starts enforcing
  // required-vs-optional protocol member coverage with fail-closed diagnostics.
  // category-merge implementation anchor: IR remains downstream of the
  // sema-owned realized-class category merge/conflict decision and must not
  // reinterpret attachment legality or concrete message resolution.
  // inheritance/override legality anchor: IR remains downstream of the
  // sema-owned realized-class inheritance and override legality result and must
  // not reinterpret superclass cycles, missing realization closure, or inherited
  // member compatibility.
  BeginObjc3IRFrontendSourceClosureAnchorComment(
      "executable_protocol_category_source_closure",
      metadata.executable_protocol_category_source_closure_contract_id, out);
  EmitObjc3IRFrontendSourceClosureStringField(
      "protocol_inheritance_model",
      metadata.executable_protocol_inheritance_identity_model, out);
  EmitObjc3IRFrontendSourceClosureStringField(
      "category_attachment_model",
      metadata.executable_category_attachment_identity_model, out);
  EmitObjc3IRFrontendSourceClosureStringField(
      "protocol_category_conformance_model",
      metadata.executable_protocol_category_conformance_identity_model, out);
  EmitObjc3IRFrontendSourceClosureSizeField(
      "protocol_node_count",
      metadata.executable_protocol_category_protocol_node_count, out);
  EmitObjc3IRFrontendSourceClosureSizeField(
      "category_node_count",
      metadata.executable_protocol_category_category_node_count, out);
  EmitObjc3IRFrontendSourceClosureSizeField(
      "protocol_inheritance_edge_count",
      metadata.executable_protocol_inheritance_identity_edge_count, out);
  EmitObjc3IRFrontendSourceClosureSizeField(
      "category_attachment_edge_count",
      metadata.executable_category_attachment_identity_edge_count, out);
  EmitObjc3IRFrontendSourceClosureSizeField(
      "protocol_category_conformance_edge_count",
      metadata.executable_protocol_category_conformance_identity_edge_count,
      out);
  EndObjc3IRFrontendSourceClosureAnchorComment(out);
}
