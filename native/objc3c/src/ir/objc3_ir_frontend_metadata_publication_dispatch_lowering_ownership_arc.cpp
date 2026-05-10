#include "ir/objc3_ir_frontend_metadata_publication_dispatch_lowering_ownership_arc.h"

#include <sstream>

#include "ir/objc3_ir_frontend_metadata_publication_dispatch_lowering_ownership_arc_arc_rows.h"
#include "ir/objc3_ir_frontend_metadata_publication_dispatch_lowering_ownership_arc_dispatch_rows.h"
#include "ir/objc3_ir_frontend_metadata_publication_dispatch_lowering_ownership_arc_ownership_rows.h"

void EmitObjc3IROwnershipArcLoweringCounterNodes(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  EmitObjc3IRDispatchOwnershipQualifierLoweringCounterNode(metadata, out);
  EmitObjc3IROwnershipLoweringCounterNodes(metadata, out);
  EmitObjc3IRArcDiagnosticsFixitLoweringCounterNode(metadata, out);
}
