  void ApplyMessageDispatchAbiLoweringAnnotations(Expr &message) {
    message.dispatch_abi_receiver_slots_marshaled = 1u;
    message.dispatch_abi_selector_slots_marshaled = 1u;
    message.dispatch_abi_argument_value_slots_marshaled = static_cast<unsigned>(message.args.size());
    message.dispatch_abi_runtime_arg_slots = kDispatchAbiMarshallingRuntimeArgSlots;
    message.dispatch_abi_argument_padding_slots_marshaled = ComputeDispatchAbiArgumentPaddingSlots(
        message.args.size(), message.dispatch_abi_runtime_arg_slots);
    message.dispatch_abi_argument_total_slots_marshaled = message.dispatch_abi_argument_value_slots_marshaled +
                                                         message.dispatch_abi_argument_padding_slots_marshaled;
    message.dispatch_abi_total_slots_marshaled = message.dispatch_abi_receiver_slots_marshaled +
                                                message.dispatch_abi_selector_slots_marshaled +
                                                message.dispatch_abi_argument_total_slots_marshaled;
    message.dispatch_abi_marshalling_symbol = BuildDispatchAbiMarshallingSymbol(
        message.dispatch_abi_receiver_slots_marshaled, message.dispatch_abi_selector_slots_marshaled,
        message.dispatch_abi_argument_value_slots_marshaled, message.dispatch_abi_argument_padding_slots_marshaled,
        message.dispatch_abi_argument_total_slots_marshaled, message.dispatch_abi_total_slots_marshaled,
        message.dispatch_abi_runtime_arg_slots);
    message.dispatch_abi_marshalling_is_normalized = true;
  }
