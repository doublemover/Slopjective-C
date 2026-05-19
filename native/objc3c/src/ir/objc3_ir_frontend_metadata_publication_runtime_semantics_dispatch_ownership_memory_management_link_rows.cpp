#include "ir/objc3_ir_frontend_metadata_publication_runtime_semantics_dispatch_ownership_memory_management_link_rows.h"

#include <sstream>

#include "ir/objc3_ir_frontend_metadata_publication_runtime_semantics_dispatch_ownership_memory_management_row_helpers.h"
#include "lower/objc3_lowering_contract.h"

void EmitObjc3IRMemoryManagementRuntimeApiLinkFields(std::ostringstream &out) {
  EmitObjc3IRMemoryManagementRuntimeStringField(kObjc3RuntimeRetainI32Symbol,
                                                out);
  EmitObjc3IRMemoryManagementRuntimeStringField(kObjc3RuntimeReleaseI32Symbol,
                                                out);
  EmitObjc3IRMemoryManagementRuntimeStringField(
      kObjc3RuntimeAutoreleaseI32Symbol, out);
  EmitObjc3IRMemoryManagementRuntimeStringField(
      kObjc3RuntimeReadCurrentPropertyI32Symbol, out);
  EmitObjc3IRMemoryManagementRuntimeStringField(
      kObjc3RuntimeWriteCurrentPropertyI32Symbol, out);
  EmitObjc3IRMemoryManagementRuntimeStringField(
      kObjc3RuntimeExchangeCurrentPropertyI32Symbol, out);
  EmitObjc3IRMemoryManagementRuntimeStringField(
      kObjc3RuntimeLoadWeakCurrentPropertyI32Symbol, out);
  EmitObjc3IRMemoryManagementRuntimeStringField(
      kObjc3RuntimeStoreWeakCurrentPropertyI32Symbol, out);
}

void EmitObjc3IRMemoryManagementRuntimeImplementationLinkFields(
    std::ostringstream &out) {
  EmitObjc3IRMemoryManagementRuntimeStringField(kObjc3RuntimeRetainI32Symbol,
                                                out);
  EmitObjc3IRMemoryManagementRuntimeStringField(kObjc3RuntimeReleaseI32Symbol,
                                                out);
  EmitObjc3IRMemoryManagementRuntimeStringField(
      kObjc3RuntimeAutoreleaseI32Symbol, out);
  EmitObjc3IRMemoryManagementRuntimeStringField(
      kObjc3RuntimePushAutoreleasepoolScopeSymbol, out);
  EmitObjc3IRMemoryManagementRuntimeStringField(
      kObjc3RuntimePopAutoreleasepoolScopeSymbol, out);
  EmitObjc3IRMemoryManagementRuntimeStringField(
      kObjc3RuntimeLoadWeakCurrentPropertyI32Symbol, out);
  EmitObjc3IRMemoryManagementRuntimeStringField(
      kObjc3RuntimeStoreWeakCurrentPropertyI32Symbol, out);
}
