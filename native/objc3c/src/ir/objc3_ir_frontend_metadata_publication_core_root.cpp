#include "ir/objc3_ir_frontend_metadata_publication_core_root.h"

#include <sstream>

#include "ir/objc3_ir_frontend_core_metadata_nodes.h"
#include "ir/objc3_ir_frontend_metadata.h"

void EmitObjc3IRFrontendMetadataRootNodes(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  out << BuildObjc3IRFrontendNamedMetadataTable();
  out << BuildObjc3IRFrontendMetadataNode(metadata) << "\n";
}
