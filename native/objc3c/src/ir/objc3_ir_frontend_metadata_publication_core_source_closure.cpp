#include "ir/objc3_ir_frontend_metadata_publication_core_source_closure.h"

#include <sstream>

#include "ir/objc3_ir_frontend_metadata.h"

void EmitObjc3IRFrontendSourceClosureAnchorComments(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  // executable source-closure freeze anchor: IR currently publishes
  // interface/protocol/category/linking metadata as the canonical
  // source-closure proof surface only. Later realization work must preserve
  // these identities while adding runnable class/category/protocol behavior.
  // executable class/metaclass source-closure anchor: the IR handoff now
  // carries declaration-owned parent identities, method-owner identities, and
  // class/metaclass object identities so later realization work consumes the
  // same fail-closed source model.
  if (metadata.executable_class_metaclass_source_closure_ready) {
    out << "; executable_class_metaclass_source_closure = contract="
        << metadata.executable_class_metaclass_source_closure_contract_id
        << ";parent_identity_model="
        << metadata.executable_class_metaclass_parent_identity_model
        << ";method_owner_identity_model="
        << metadata.executable_class_metaclass_method_owner_identity_model
        << ";class_object_identity_model="
        << metadata.executable_class_metaclass_object_identity_model
        << ";declaration_node_count="
        << metadata.executable_class_metaclass_declaration_node_count
        << ";parent_identity_edge_count="
        << metadata.executable_class_metaclass_parent_identity_edge_count
        << ";method_owner_identity_edge_count="
        << metadata.executable_class_metaclass_method_owner_identity_edge_count
        << ";class_object_identity_edge_count="
        << metadata.executable_class_metaclass_object_identity_edge_count
        << "\n";
  }
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
  if (metadata.executable_protocol_category_source_closure_ready) {
    out << "; executable_protocol_category_source_closure = contract="
        << metadata.executable_protocol_category_source_closure_contract_id
        << ";protocol_inheritance_model="
        << metadata.executable_protocol_inheritance_identity_model
        << ";category_attachment_model="
        << metadata.executable_category_attachment_identity_model
        << ";protocol_category_conformance_model="
        << metadata.executable_protocol_category_conformance_identity_model
        << ";protocol_node_count="
        << metadata.executable_protocol_category_protocol_node_count
        << ";category_node_count="
        << metadata.executable_protocol_category_category_node_count
        << ";protocol_inheritance_edge_count="
        << metadata.executable_protocol_inheritance_identity_edge_count
        << ";category_attachment_edge_count="
        << metadata.executable_category_attachment_identity_edge_count
        << ";protocol_category_conformance_edge_count="
        << metadata.executable_protocol_category_conformance_identity_edge_count
        << "\n";
  }
}
