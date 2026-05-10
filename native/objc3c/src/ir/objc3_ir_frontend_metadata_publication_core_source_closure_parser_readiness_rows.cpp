#include "ir/objc3_ir_frontend_metadata_publication_core_source_closure_parser_readiness_rows.h"

#include <sstream>

#include "ir/objc3_ir_frontend_metadata.h"
#include "ir/objc3_ir_frontend_metadata_publication_core_source_closure_closure_rows.h"
#include "ir/objc3_ir_frontend_metadata_publication_core_source_closure_core_source_rows.h"

void EmitObjc3IRFrontendClassMetaclassSourceClosureIfReady(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  if (metadata.executable_class_metaclass_source_closure_ready) {
    EmitObjc3IRFrontendClassMetaclassSourceClosureRow(metadata, out);
  }
}

void EmitObjc3IRFrontendProtocolCategorySourceClosureIfReady(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  if (metadata.executable_protocol_category_source_closure_ready) {
    EmitObjc3IRFrontendProtocolCategorySourceClosureRow(metadata, out);
  }
}
