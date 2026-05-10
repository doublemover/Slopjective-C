#include "ir/objc3_ir_frontend_metadata_publication_runtime_semantics_block_arc.h"

#include "ir/objc3_ir_frontend_metadata_publication_runtime_semantics_block_arc_arc_runtime_helpers.h"
#include "ir/objc3_ir_frontend_metadata_publication_runtime_semantics_block_arc_arc_semantics.h"
#include "ir/objc3_ir_frontend_metadata_publication_runtime_semantics_block_arc_block_runtime.h"
#include "ir/objc3_ir_frontend_metadata_publication_runtime_semantics_block_arc_runnable_arc_gate.h"

void EmitObjc3IRBlockArcMetadataNodes(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  EmitObjc3IRBlockRuntimeGateExecutionMetadataNodes(out);
  EmitObjc3IRArcSemanticMetadataNodes(metadata, out);
  EmitObjc3IRArcLoweringHelperRuntimeMetadataNodes(out);
  EmitObjc3IRRunnableArcGateMetadataNodes(out);
}
