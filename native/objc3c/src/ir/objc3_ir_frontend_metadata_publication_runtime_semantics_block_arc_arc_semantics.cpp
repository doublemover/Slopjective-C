#include "ir/objc3_ir_frontend_metadata_publication_runtime_semantics_block_arc_arc_semantics.h"

#include <sstream>

#include "ir/objc3_ir_frontend_metadata_publication_runtime_semantics_block_arc_arc_inference_lifetime.h"
#include "ir/objc3_ir_frontend_metadata_publication_runtime_semantics_block_arc_arc_interaction_semantics.h"
#include "ir/objc3_ir_frontend_metadata_publication_runtime_semantics_block_arc_arc_semantic_rules.h"
#include "ir/objc3_ir_frontend_metadata_publication_runtime_semantics_block_arc_arc_source_mode.h"

void EmitObjc3IRArcSemanticMetadataNodes(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  EmitObjc3IRArcSourceModeMetadataNodes(metadata, out);
  EmitObjc3IRArcSemanticRulesMetadataNode(out);
  EmitObjc3IRArcInferenceLifetimeMetadataNode(out);
  EmitObjc3IRArcInteractionSemanticsMetadataNode(out);
}
