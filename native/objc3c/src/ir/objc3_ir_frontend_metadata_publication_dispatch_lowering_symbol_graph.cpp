#include "ir/objc3_ir_frontend_metadata_publication_dispatch_lowering_symbol_graph.h"

#include <sstream>

#include "ir/objc3_ir_c_string.h"
#include "ir/objc3_ir_frontend_metadata.h"

void EmitObjc3IRDispatchLoweringSymbolGraphCounterNode(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  out << "!6 = !{i64 " << static_cast<unsigned long long>(metadata.global_symbol_nodes) << ", i64 "
      << static_cast<unsigned long long>(metadata.function_symbol_nodes) << ", i64 "
      << static_cast<unsigned long long>(metadata.interface_symbol_nodes) << ", i64 "
      << static_cast<unsigned long long>(metadata.implementation_symbol_nodes) << ", i64 "
      << static_cast<unsigned long long>(metadata.interface_property_symbol_nodes) << ", i64 "
      << static_cast<unsigned long long>(metadata.implementation_property_symbol_nodes) << ", i64 "
      << static_cast<unsigned long long>(metadata.interface_method_symbol_nodes) << ", i64 "
      << static_cast<unsigned long long>(metadata.implementation_method_symbol_nodes) << ", i64 "
      << static_cast<unsigned long long>(metadata.top_level_scope_symbols) << ", i64 "
      << static_cast<unsigned long long>(metadata.nested_scope_symbols) << ", i64 "
      << static_cast<unsigned long long>(metadata.scope_frames_total) << ", i64 "
      << static_cast<unsigned long long>(metadata.implementation_interface_resolution_sites) << ", i64 "
      << static_cast<unsigned long long>(metadata.implementation_interface_resolution_hits) << ", i64 "
      << static_cast<unsigned long long>(metadata.implementation_interface_resolution_misses) << ", i64 "
      << static_cast<unsigned long long>(metadata.method_resolution_sites) << ", i64 "
      << static_cast<unsigned long long>(metadata.method_resolution_hits) << ", i64 "
      << static_cast<unsigned long long>(metadata.method_resolution_misses) << ", i1 "
      << (metadata.deterministic_symbol_graph_handoff ? 1 : 0) << ", i1 "
      << (metadata.deterministic_scope_resolution_handoff ? 1 : 0) << ", !\""
      << EscapeCStringLiteral(metadata.deterministic_symbol_graph_scope_resolution_handoff_key)
      << "\"}\n";
}
