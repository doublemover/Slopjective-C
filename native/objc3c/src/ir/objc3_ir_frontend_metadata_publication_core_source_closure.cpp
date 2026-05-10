#include "ir/objc3_ir_frontend_metadata_publication_core_source_closure.h"

#include <sstream>

#include "ir/objc3_ir_frontend_metadata_publication_core_source_closure_parser_readiness_rows.h"

void EmitObjc3IRFrontendSourceClosureAnchorComments(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  EmitObjc3IRFrontendClassMetaclassSourceClosureIfReady(metadata, out);
  EmitObjc3IRFrontendProtocolCategorySourceClosureIfReady(metadata, out);
}
