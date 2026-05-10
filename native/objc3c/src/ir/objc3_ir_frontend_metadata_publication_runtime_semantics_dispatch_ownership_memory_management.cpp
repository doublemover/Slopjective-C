#include "ir/objc3_ir_frontend_metadata_publication_runtime_semantics_dispatch_ownership_memory_management.h"

#include <sstream>

#include "ir/objc3_ir_c_string.h"
#include "ir/objc3_ir_frontend_metadata.h"
#include "lower/objc3_lowering_contract.h"

void EmitObjc3IRMemoryManagementRuntimeMetadataNodes(
    const Objc3IRFrontendMetadata &metadata,
    std::size_t synthesized_property_accessor_count, std::ostringstream &out) {
  out << "!70 = !{!\""
      << EscapeCStringLiteral(kObjc3RuntimeMemoryManagementApiContractId)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeMemoryManagementApiReferenceModel)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeMemoryManagementApiWeakModel)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeMemoryManagementApiAutoreleasepoolModel)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeMemoryManagementApiFailClosedModel)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeRetainI32Symbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeReleaseI32Symbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeAutoreleaseI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeReadCurrentPropertyI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeWriteCurrentPropertyI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeExchangeCurrentPropertyI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeLoadWeakCurrentPropertyI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeStoreWeakCurrentPropertyI32Symbol)
      << "\", i64 "
      << static_cast<unsigned long long>(synthesized_property_accessor_count)
      << "}\n";
  out << "!71 = !{!\""
      << EscapeCStringLiteral(kObjc3RuntimeMemoryManagementImplementationContractId)
      << "\", !\""
      << EscapeCStringLiteral(
             kObjc3RuntimeMemoryManagementImplementationRefcountModel)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeMemoryManagementImplementationWeakModel)
      << "\", !\""
      << EscapeCStringLiteral(
             kObjc3RuntimeMemoryManagementImplementationAutoreleasepoolModel)
      << "\", !\""
      << EscapeCStringLiteral(
             kObjc3RuntimeMemoryManagementImplementationFailClosedModel)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeRetainI32Symbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeReleaseI32Symbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeAutoreleaseI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimePushAutoreleasepoolScopeSymbol)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimePopAutoreleasepoolScopeSymbol)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeLoadWeakCurrentPropertyI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeStoreWeakCurrentPropertyI32Symbol)
      << "\", i64 "
      << static_cast<unsigned long long>(synthesized_property_accessor_count)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.autoreleasepool_scope_lowering_scope_sites)
      << "}\n";
}
