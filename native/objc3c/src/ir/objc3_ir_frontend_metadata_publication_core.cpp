#include "ir/objc3_ir_frontend_metadata_publication_core.h"

#include "ir/objc3_ir_frontend_metadata_publication_core_core_rows.h"
#include "ir/objc3_ir_frontend_metadata_publication_core_root.h"
#include "ir/objc3_ir_frontend_metadata_publication_core_source_closure.h"

void EmitObjc3IRFrontendCoreMetadataPublication(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  EmitObjc3IRFrontendSourceClosureAnchorComments(metadata, out);
  EmitObjc3IRFrontendMetadataRootNodes(metadata, out);
  EmitObjc3IRFrontendCoreCounterNodes(metadata, out);
}
