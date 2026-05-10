#include "ir/objc3_ir_frontend_metadata_publication_runtime_support_library.h"

#include "ir/objc3_ir_frontend_metadata_publication_runtime_support_library_base.h"
#include "ir/objc3_ir_frontend_metadata_publication_runtime_support_library_core_feature.h"
#include "ir/objc3_ir_frontend_metadata_publication_runtime_support_library_link_wiring.h"

void EmitObjc3IRRuntimeSupportLibraryMetadataNodes(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  EmitObjc3IRRuntimeSupportLibraryBaseMetadataNode(metadata, out);
  EmitObjc3IRRuntimeSupportLibraryCoreFeatureMetadataNode(metadata, out);
  EmitObjc3IRRuntimeSupportLibraryLinkWiringMetadataNode(metadata, out);
}
