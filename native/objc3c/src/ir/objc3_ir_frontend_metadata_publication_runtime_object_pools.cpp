#include "ir/objc3_ir_frontend_metadata_publication_runtime_object_pools.h"

#include <sstream>

#include "ir/objc3_ir_c_string.h"
#include "ir/objc3_ir_runtime_metadata_emission.h"
#include "lower/objc3_lowering_contract.h"

void EmitObjc3IRRuntimeObjectPoolMetadataNode(
    std::size_t selector_pool_global_count,
    std::size_t runtime_string_pool_global_count, std::ostringstream &out) {
  out << "!59 = !{!\""
      << EscapeCStringLiteral(kObjc3RuntimeSelectorStringPoolEmissionContractId)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeSelectorPoolEmissionPayloadModel)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeStringPoolEmissionPayloadModel)
      << "\", i64 "
      << static_cast<unsigned long long>(selector_pool_global_count)
      << ", i64 "
      << static_cast<unsigned long long>(runtime_string_pool_global_count)
      << ", !\""
      << EscapeCStringLiteral(Objc3RuntimeMetadataHostSectionForLogicalName(
             kObjc3RuntimeSelectorPoolLogicalSection))
      << "\", !\""
      << EscapeCStringLiteral(Objc3RuntimeMetadataHostSectionForLogicalName(
             kObjc3RuntimeStringPoolLogicalSection))
      << "\"}\n";
}
