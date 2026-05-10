#include "ir/objc3_ir_frontend_metadata_publication_runtime_support_library_base.h"

#include <sstream>

#include "ir/objc3_ir_frontend_metadata_publication_runtime_support_library_base_artifact_rows.h"
#include "ir/objc3_ir_frontend_metadata_publication_runtime_support_library_base_identity_rows.h"
#include "ir/objc3_ir_frontend_metadata_publication_runtime_support_library_base_link_rows.h"
#include "ir/objc3_ir_frontend_metadata_publication_runtime_support_library_base_state_rows.h"

void EmitObjc3IRRuntimeSupportLibraryBaseMetadataNode(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  BeginObjc3IRRuntimeSupportLibraryBaseMetadataNode(metadata, out);
  EmitObjc3IRRuntimeSupportLibraryBaseStateFields(metadata, out);
  EmitObjc3IRRuntimeSupportLibraryBaseArtifactFields(metadata, out);
  EmitObjc3IRRuntimeSupportLibraryBaseLinkFields(metadata, out);
  EndObjc3IRRuntimeSupportLibraryBaseMetadataNode(out);
}
