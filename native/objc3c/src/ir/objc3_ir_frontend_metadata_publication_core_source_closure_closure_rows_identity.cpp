#include "ir/objc3_ir_frontend_metadata_publication_core_source_closure_closure_rows_identity.h"

#include <sstream>

#include "ir/objc3_ir_frontend_metadata.h"
#include "ir/objc3_ir_frontend_metadata_publication_core_source_closure_row_helpers.h"

void EmitObjc3IRFrontendProtocolCategorySourceClosureIdentityFields(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  EmitObjc3IRFrontendSourceClosureStringField(
      "protocol_inheritance_model",
      metadata.executable_protocol_inheritance_identity_model, out);
  EmitObjc3IRFrontendSourceClosureStringField(
      "category_attachment_model",
      metadata.executable_category_attachment_identity_model, out);
  EmitObjc3IRFrontendSourceClosureStringField(
      "protocol_category_conformance_model",
      metadata.executable_protocol_category_conformance_identity_model, out);
}
