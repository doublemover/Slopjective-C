#include "ir/objc3_ir_frontend_metadata_publication_dispatch_lowering_class_protocol_symbol.h"

#include <sstream>

#include "ir/objc3_ir_frontend_metadata.h"

void EmitObjc3IRDispatchLoweringClassProtocolSymbolCounterNode(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  out << "!7 = !{i64 " << static_cast<unsigned long long>(metadata.declared_class_interfaces) << ", i64 "
      << static_cast<unsigned long long>(metadata.declared_class_implementations) << ", i64 "
      << static_cast<unsigned long long>(metadata.resolved_class_interfaces) << ", i64 "
      << static_cast<unsigned long long>(metadata.resolved_class_implementations) << ", i64 "
      << static_cast<unsigned long long>(metadata.linked_class_method_symbols) << ", i64 "
      << static_cast<unsigned long long>(metadata.linked_category_method_symbols) << ", i64 "
      << static_cast<unsigned long long>(metadata.protocol_composition_sites) << ", i64 "
      << static_cast<unsigned long long>(metadata.protocol_composition_symbols) << ", i64 "
      << static_cast<unsigned long long>(metadata.category_composition_sites) << ", i64 "
      << static_cast<unsigned long long>(metadata.category_composition_symbols) << ", i64 "
      << static_cast<unsigned long long>(metadata.invalid_protocol_composition_sites) << ", i1 "
      << (metadata.deterministic_class_protocol_category_linking_handoff ? 1 : 0) << "}\n";
}
