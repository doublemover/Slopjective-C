#include "ir/objc3_ir_frontend_metadata_publication_core_source_closure_core_source_rows.h"

#include <sstream>

#include "ir/objc3_ir_frontend_metadata.h"
#include "ir/objc3_ir_frontend_metadata_publication_core_source_closure_row_helpers.h"

void EmitObjc3IRFrontendClassMetaclassSourceClosureRow(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  // executable source-closure freeze anchor: IR currently publishes
  // interface/protocol/category/linking metadata as the canonical
  // source-closure proof surface only. Later realization work must preserve
  // these identities while adding runnable class/category/protocol behavior.
  // executable class/metaclass source-closure anchor: the IR handoff now
  // carries declaration-owned parent identities, method-owner identities, and
  // class/metaclass object identities so later realization work consumes the
  // same fail-closed source model.
  BeginObjc3IRFrontendSourceClosureAnchorComment(
      "executable_class_metaclass_source_closure",
      metadata.executable_class_metaclass_source_closure_contract_id, out);
  EmitObjc3IRFrontendSourceClosureStringField(
      "parent_identity_model",
      metadata.executable_class_metaclass_parent_identity_model, out);
  EmitObjc3IRFrontendSourceClosureStringField(
      "method_owner_identity_model",
      metadata.executable_class_metaclass_method_owner_identity_model, out);
  EmitObjc3IRFrontendSourceClosureStringField(
      "class_object_identity_model",
      metadata.executable_class_metaclass_object_identity_model, out);
  EmitObjc3IRFrontendSourceClosureSizeField(
      "declaration_node_count",
      metadata.executable_class_metaclass_declaration_node_count, out);
  EmitObjc3IRFrontendSourceClosureSizeField(
      "parent_identity_edge_count",
      metadata.executable_class_metaclass_parent_identity_edge_count, out);
  EmitObjc3IRFrontendSourceClosureSizeField(
      "method_owner_identity_edge_count",
      metadata.executable_class_metaclass_method_owner_identity_edge_count, out);
  EmitObjc3IRFrontendSourceClosureSizeField(
      "class_object_identity_edge_count",
      metadata.executable_class_metaclass_object_identity_edge_count, out);
  EndObjc3IRFrontendSourceClosureAnchorComment(out);
}
