#include "parse/objc3_message_send_profiles.h"

#include <sstream>

namespace objc3c::parse {

unsigned ComputeDispatchAbiArgumentPaddingSlots(std::size_t argument_count,
                                                unsigned runtime_arg_slots) {
  if (runtime_arg_slots == 0u) {
    return 0u;
  }
  const std::size_t remainder = argument_count % runtime_arg_slots;
  if (remainder == 0u) {
    return 0u;
  }
  return static_cast<unsigned>(
      runtime_arg_slots - static_cast<unsigned>(remainder));
}

std::string BuildDispatchAbiMarshallingSymbol(unsigned receiver_slots,
                                              unsigned selector_slots,
                                              unsigned argument_value_slots,
                                              unsigned argument_padding_slots,
                                              unsigned argument_total_slots,
                                              unsigned total_slots,
                                              unsigned runtime_arg_slots) {
  std::ostringstream out;
  out << "dispatch-abi-marshalling:recv=" << receiver_slots
      << ";sel=" << selector_slots
      << ";arg-values=" << argument_value_slots
      << ";arg-padding=" << argument_padding_slots
      << ";arg-total=" << argument_total_slots
      << ";total=" << total_slots
      << ";runtime-slots=" << runtime_arg_slots;
  return out.str();
}

}  // namespace objc3c::parse
