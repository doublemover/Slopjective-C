    message.nil_receiver_semantics_enabled = message.receiver->kind == Expr::Kind::NilLiteral;
    message.nil_receiver_foldable = message.nil_receiver_semantics_enabled;
    message.nil_receiver_requires_runtime_dispatch = !message.nil_receiver_foldable;
    message.nil_receiver_folding_symbol = BuildNilReceiverFoldingSymbol(
        message.nil_receiver_foldable, message.nil_receiver_requires_runtime_dispatch, message.message_send_form);
    message.nil_receiver_semantics_is_normalized = true;
