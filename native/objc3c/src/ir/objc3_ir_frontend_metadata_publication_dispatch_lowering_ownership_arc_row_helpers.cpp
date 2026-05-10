#include "ir/objc3_ir_frontend_metadata_publication_dispatch_lowering_ownership_arc_row_helpers.h"

#include <sstream>

void EmitObjc3IRDispatchOwnershipArcCounterRow(
    const char *metadata_node_id, std::initializer_list<std::size_t> counters,
    bool deterministic_handoff, std::ostringstream &out) {
  out << metadata_node_id << " = !{";
  bool first_counter = true;
  for (std::size_t counter : counters) {
    if (!first_counter) {
      out << ", ";
    }
    out << "i64 " << static_cast<unsigned long long>(counter);
    first_counter = false;
  }
  out << ", i1 " << (deterministic_handoff ? 1 : 0) << "}\n\n";
}
