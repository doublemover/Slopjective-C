#include "ir/objc3_ir_frontend_metadata_publication_core_core_rows_row_helpers.h"

#include <sstream>

void BeginObjc3IRFrontendCoreCounterRow(const char *metadata_node_id,
                                        std::size_t first_counter,
                                        std::ostringstream &out) {
  out << metadata_node_id << " = !{i64 "
      << static_cast<unsigned long long>(first_counter);
}

void EmitObjc3IRFrontendCoreSizeField(std::size_t field_value,
                                      std::ostringstream &out) {
  out << ", i64 " << static_cast<unsigned long long>(field_value);
}

void EmitObjc3IRFrontendCoreBoolField(bool field_value,
                                      std::ostringstream &out) {
  out << ", i1 " << (field_value ? 1 : 0);
}

void EndObjc3IRFrontendCoreCounterRow(bool trailing_blank_line,
                                      std::ostringstream &out) {
  out << "}\n";
  if (trailing_blank_line) {
    out << "\n";
  }
}
