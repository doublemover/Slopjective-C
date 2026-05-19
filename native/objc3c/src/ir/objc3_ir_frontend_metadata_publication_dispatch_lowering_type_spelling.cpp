#include "ir/objc3_ir_frontend_metadata_publication_dispatch_lowering_type_spelling.h"

#include <sstream>

#include "ir/objc3_ir_frontend_metadata.h"

void EmitObjc3IRDispatchLoweringTypeSpellingCounterNode(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  out << "!5 = !{i64 " << static_cast<unsigned long long>(metadata.object_pointer_type_spellings)
      << ", i64 " << static_cast<unsigned long long>(metadata.pointer_declarator_entries) << ", i64 "
      << static_cast<unsigned long long>(metadata.pointer_declarator_depth_total) << ", i64 "
      << static_cast<unsigned long long>(metadata.pointer_declarator_token_entries) << ", i64 "
      << static_cast<unsigned long long>(metadata.nullability_suffix_entries) << ", i64 "
      << static_cast<unsigned long long>(metadata.generic_suffix_entries) << ", i64 "
      << static_cast<unsigned long long>(metadata.terminated_generic_suffix_entries) << ", i64 "
      << static_cast<unsigned long long>(metadata.unterminated_generic_suffix_entries) << ", i1 "
      << (metadata.deterministic_object_pointer_nullability_generics_handoff ? 1 : 0) << "}\n";
}
