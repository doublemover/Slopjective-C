#include "ir/objc3_ir_frontend_metadata_publication_runtime_semantics_dispatch_ownership_runtime_hooks.h"

#include <sstream>

#include "ir/objc3_ir_c_string.h"
#include "lower/objc3_lowering_contract.h"

void EmitObjc3IROwnershipRuntimeHookEmissionMetadataNode(
    std::size_t synthesized_property_accessor_count, std::ostringstream &out) {
  out << "!69 = !{!\""
      << EscapeCStringLiteral(kObjc3OwnershipRuntimeHookEmissionContractId)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3OwnershipRuntimeHookEmissionAccessorModel)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3OwnershipRuntimeHookEmissionPropertyContextModel)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3OwnershipRuntimeHookEmissionAutoreleaseModel)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3OwnershipRuntimeHookEmissionFailClosedModel)
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
}
