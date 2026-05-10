#include "ir/objc3_ir_frontend_metadata_publication_core_source_closure_closure_rows.h"

#include <sstream>

#include "ir/objc3_ir_frontend_metadata_publication_core_source_closure_closure_rows_anchor.h"
#include "ir/objc3_ir_frontend_metadata_publication_core_source_closure_closure_rows_counts.h"
#include "ir/objc3_ir_frontend_metadata_publication_core_source_closure_closure_rows_identity.h"

void EmitObjc3IRFrontendProtocolCategorySourceClosureRow(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  BeginObjc3IRFrontendProtocolCategorySourceClosureAnchor(metadata, out);
  EmitObjc3IRFrontendProtocolCategorySourceClosureIdentityFields(metadata, out);
  EmitObjc3IRFrontendProtocolCategorySourceClosureCountFields(metadata, out);
  EndObjc3IRFrontendProtocolCategorySourceClosureAnchor(out);
}
