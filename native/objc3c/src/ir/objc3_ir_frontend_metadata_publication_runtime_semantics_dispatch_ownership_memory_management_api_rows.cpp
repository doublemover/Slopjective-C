#include "ir/objc3_ir_frontend_metadata_publication_runtime_semantics_dispatch_ownership_memory_management_api_rows.h"

#include <sstream>

#include "ir/objc3_ir_frontend_metadata_publication_runtime_semantics_dispatch_ownership_memory_management_link_rows.h"
#include "ir/objc3_ir_frontend_metadata_publication_runtime_semantics_dispatch_ownership_memory_management_row_helpers.h"
#include "lower/objc3_lowering_contract.h"

void EmitObjc3IRMemoryManagementRuntimeApiMetadataNode(
    std::size_t synthesized_property_accessor_count, std::ostringstream &out) {
  BeginObjc3IRMemoryManagementRuntimeMetadataNode(
      "!70", kObjc3RuntimeMemoryManagementApiContractId, out);
  EmitObjc3IRMemoryManagementRuntimeStringField(
      kObjc3RuntimeMemoryManagementApiReferenceModel, out);
  EmitObjc3IRMemoryManagementRuntimeStringField(
      kObjc3RuntimeMemoryManagementApiWeakModel, out);
  EmitObjc3IRMemoryManagementRuntimeStringField(
      kObjc3RuntimeMemoryManagementApiAutoreleasepoolModel, out);
  EmitObjc3IRMemoryManagementRuntimeStringField(
      kObjc3RuntimeMemoryManagementApiFailClosedModel, out);
  EmitObjc3IRMemoryManagementRuntimeApiLinkFields(out);
  EmitObjc3IRMemoryManagementRuntimeSizeField(
      synthesized_property_accessor_count, out);
  EndObjc3IRMemoryManagementRuntimeMetadataNode(out);
}
