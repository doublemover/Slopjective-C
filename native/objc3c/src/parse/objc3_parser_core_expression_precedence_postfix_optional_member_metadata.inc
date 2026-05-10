  void ConfigurePostfixOptionalMemberDispatchAbi(Expr &message) {
    message.dispatch_abi_receiver_slots_marshaled = 1u;
    message.dispatch_abi_selector_slots_marshaled = 1u;
    message.dispatch_abi_argument_value_slots_marshaled = 0u;
    message.dispatch_abi_runtime_arg_slots =
        kDispatchAbiMarshallingRuntimeArgSlots;
    message.dispatch_abi_argument_padding_slots_marshaled =
        ComputeDispatchAbiArgumentPaddingSlots(
            0u, message.dispatch_abi_runtime_arg_slots);
    message.dispatch_abi_argument_total_slots_marshaled =
        message.dispatch_abi_argument_padding_slots_marshaled;
    message.dispatch_abi_total_slots_marshaled =
        message.dispatch_abi_receiver_slots_marshaled +
        message.dispatch_abi_selector_slots_marshaled +
        message.dispatch_abi_argument_total_slots_marshaled;
    message.dispatch_abi_marshalling_symbol =
        BuildDispatchAbiMarshallingSymbol(
            message.dispatch_abi_receiver_slots_marshaled,
            message.dispatch_abi_selector_slots_marshaled,
            message.dispatch_abi_argument_value_slots_marshaled,
            message.dispatch_abi_argument_padding_slots_marshaled,
            message.dispatch_abi_argument_total_slots_marshaled,
            message.dispatch_abi_total_slots_marshaled,
            message.dispatch_abi_runtime_arg_slots);
    message.dispatch_abi_marshalling_is_normalized = true;
  }

  void ConfigurePostfixOptionalMemberNilReceiverSemantics(Expr &message) {
    message.nil_receiver_semantics_enabled =
        message.receiver->kind == Expr::Kind::NilLiteral;
    message.nil_receiver_foldable =
        message.nil_receiver_semantics_enabled;
    message.nil_receiver_requires_runtime_dispatch =
        !message.nil_receiver_foldable;
    message.nil_receiver_folding_symbol =
        BuildNilReceiverFoldingSymbol(
            message.nil_receiver_foldable,
            message.nil_receiver_requires_runtime_dispatch,
            message.message_send_form);
    message.nil_receiver_semantics_is_normalized = true;
  }
