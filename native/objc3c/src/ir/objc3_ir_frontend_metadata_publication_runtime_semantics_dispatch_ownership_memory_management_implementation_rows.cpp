#include "ir/objc3_ir_frontend_metadata_publication_runtime_semantics_dispatch_ownership_memory_management_implementation_rows.h"

#include <sstream>

#include "ir/objc3_ir_frontend_metadata.h"
#include "ir/objc3_ir_frontend_metadata_publication_runtime_semantics_dispatch_ownership_memory_management_link_rows.h"
#include "ir/objc3_ir_frontend_metadata_publication_runtime_semantics_dispatch_ownership_memory_management_row_helpers.h"
#include "lower/objc3_lowering_contract.h"

void EmitObjc3IRMemoryManagementRuntimeImplementationMetadataNode(
    const Objc3IRFrontendMetadata &metadata,
    std::size_t synthesized_property_accessor_count, std::ostringstream &out) {
  BeginObjc3IRMemoryManagementRuntimeMetadataNode(
      "!71", kObjc3RuntimeMemoryManagementImplementationContractId, out);
  EmitObjc3IRMemoryManagementRuntimeStringField(
      kObjc3RuntimeMemoryManagementImplementationRefcountModel, out);
  EmitObjc3IRMemoryManagementRuntimeStringField(
      kObjc3RuntimeMemoryManagementImplementationWeakModel, out);
  EmitObjc3IRMemoryManagementRuntimeStringField(
      kObjc3RuntimeMemoryManagementImplementationAutoreleasepoolModel, out);
  EmitObjc3IRMemoryManagementRuntimeStringField(
      kObjc3RuntimeMemoryManagementImplementationFailClosedModel, out);
  EmitObjc3IRMemoryManagementRuntimeImplementationLinkFields(out);
  EmitObjc3IRMemoryManagementRuntimeSizeField(
      synthesized_property_accessor_count, out);
  EmitObjc3IRMemoryManagementRuntimeSizeField(
      metadata.autoreleasepool_scope_lowering_scope_sites, out);
  EndObjc3IRMemoryManagementRuntimeMetadataNode(out);
}
