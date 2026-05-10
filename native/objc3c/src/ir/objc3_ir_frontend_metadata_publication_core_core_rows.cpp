#include "ir/objc3_ir_frontend_metadata_publication_core_core_rows.h"

#include <sstream>

#include "ir/objc3_ir_frontend_metadata_publication_core_core_rows_count_rows.h"
#include "ir/objc3_ir_frontend_metadata_publication_core_core_rows_identity_rows.h"

void EmitObjc3IRFrontendCoreCounterNodes(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  EmitObjc3IRFrontendInterfaceImplementationCoreCounterNode(metadata, out);
  EmitObjc3IRFrontendProtocolCategoryCoreCounterNode(metadata, out);
  EmitObjc3IRFrontendSelectorNormalizationCoreCounterNode(metadata, out);
  EmitObjc3IRFrontendPropertyAttributeCoreCounterNode(metadata, out);
}
