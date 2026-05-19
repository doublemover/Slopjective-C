#include "ir/objc3_ir_frontend_metadata_publication_block_module_lowering_block_storage_row_helpers.h"

#include <sstream>

void BeginObjc3IRBlockStorageLoweringCounterRow(
    const char *metadata_node_id, std::size_t first_counter,
    std::ostringstream &out) {
  out << metadata_node_id << " = !{i64 "
      << static_cast<unsigned long long>(first_counter);
}

void EmitObjc3IRBlockStorageLoweringSizeField(std::size_t field_value,
                                              std::ostringstream &out) {
  out << ", i64 " << static_cast<unsigned long long>(field_value);
}

void EmitObjc3IRBlockStorageLoweringBoolField(bool field_value,
                                              std::ostringstream &out) {
  out << ", i1 " << (field_value ? 1 : 0);
}

void EndObjc3IRBlockStorageLoweringCounterRow(std::ostringstream &out) {
  out << "}\n\n";
}
