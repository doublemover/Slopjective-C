#include "ir/objc3_ir_frontend_metadata_publication_core_source_closure_closure_rows_anchor.h"

#include <sstream>

#include "ir/objc3_ir_frontend_metadata.h"
#include "ir/objc3_ir_frontend_metadata_publication_core_source_closure_row_helpers.h"

void BeginObjc3IRFrontendProtocolCategorySourceClosureAnchor(
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
}

void EndObjc3IRFrontendProtocolCategorySourceClosureAnchor(
    std::ostringstream &out) {
  EndObjc3IRFrontendSourceClosureAnchorComment(out);
}
