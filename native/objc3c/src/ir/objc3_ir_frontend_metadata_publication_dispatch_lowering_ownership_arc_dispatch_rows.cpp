#include "ir/objc3_ir_frontend_metadata_publication_dispatch_lowering_ownership_arc_dispatch_rows.h"

#include <sstream>

#include "ir/objc3_ir_frontend_metadata.h"
#include "ir/objc3_ir_frontend_metadata_publication_dispatch_lowering_ownership_arc_row_helpers.h"

void EmitObjc3IRDispatchOwnershipQualifierLoweringCounterNode(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  EmitObjc3IRDispatchOwnershipArcCounterRow(
      "!14",
      {metadata.ownership_qualifier_lowering_ownership_qualifier_sites,
       metadata.ownership_qualifier_lowering_invalid_ownership_qualifier_sites,
       metadata
           .ownership_qualifier_lowering_object_pointer_type_annotation_sites},
      metadata.deterministic_ownership_qualifier_lowering_handoff, out);
}
