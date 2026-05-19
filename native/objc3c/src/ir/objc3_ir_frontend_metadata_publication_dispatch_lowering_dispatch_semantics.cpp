#include "ir/objc3_ir_frontend_metadata_publication_dispatch_lowering_dispatch_semantics.h"

#include <sstream>

#include "ir/objc3_ir_frontend_metadata_publication_dispatch_lowering_dispatch_semantics_dispatch_rows.h"
#include "ir/objc3_ir_frontend_metadata_publication_dispatch_lowering_dispatch_semantics_link_rows.h"
#include "ir/objc3_ir_frontend_metadata_publication_dispatch_lowering_dispatch_semantics_semantic_rows.h"

void EmitObjc3IRDispatchSemanticsCounterNodes(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  EmitObjc3IRDispatchNilReceiverLoweringCounterNode(metadata, out);
  EmitObjc3IRDispatchSuperSemanticCounterNode(metadata, out);
  EmitObjc3IRDispatchRuntimeLinkWiringCounterNode(metadata, out);
}
