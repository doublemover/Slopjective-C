#include "ir/objc3_ir_frontend_metadata_publication_dispatch_lowering_selector.h"

#include <sstream>

#include "ir/objc3_ir_frontend_metadata.h"

void EmitObjc3IRDispatchLoweringSelectorCounterNode(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  out << "!9 = !{i64 " << static_cast<unsigned long long>(metadata.message_send_selector_lowering_sites)
      << ", i64 " << static_cast<unsigned long long>(metadata.message_send_selector_lowering_unary_sites)
      << ", i64 "
      << static_cast<unsigned long long>(metadata.message_send_selector_lowering_keyword_sites)
      << ", i64 "
      << static_cast<unsigned long long>(metadata.message_send_selector_lowering_selector_piece_sites)
      << ", i64 "
      << static_cast<unsigned long long>(metadata.message_send_selector_lowering_argument_expression_sites)
      << ", i64 "
      << static_cast<unsigned long long>(metadata.message_send_selector_lowering_receiver_sites)
      << ", i64 "
      << static_cast<unsigned long long>(metadata.message_send_selector_lowering_selector_literal_entries)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.message_send_selector_lowering_selector_literal_characters)
      << ", i1 " << (metadata.deterministic_message_send_selector_lowering_handoff ? 1 : 0) << "}\n";
}
