#include "ir/objc3_ir_frontend_metadata_publication_dispatch_lowering_abi_marshalling.h"

#include <sstream>

#include "ir/objc3_ir_frontend_metadata.h"

void EmitObjc3IRDispatchLoweringAbiMarshallingCounterNode(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  out << "!10 = !{i64 " << static_cast<unsigned long long>(metadata.dispatch_abi_marshalling_message_send_sites)
      << ", i64 "
      << static_cast<unsigned long long>(metadata.dispatch_abi_marshalling_receiver_slots_marshaled)
      << ", i64 "
      << static_cast<unsigned long long>(metadata.dispatch_abi_marshalling_selector_slots_marshaled)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.dispatch_abi_marshalling_argument_value_slots_marshaled)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.dispatch_abi_marshalling_argument_padding_slots_marshaled)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.dispatch_abi_marshalling_argument_total_slots_marshaled)
      << ", i64 " << static_cast<unsigned long long>(metadata.dispatch_abi_marshalling_total_marshaled_slots)
      << ", i64 "
      << static_cast<unsigned long long>(metadata.dispatch_abi_marshalling_runtime_dispatch_arg_slots)
      << ", i1 " << (metadata.deterministic_dispatch_abi_marshalling_handoff ? 1 : 0) << "}\n";
}
