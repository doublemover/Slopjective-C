  void FinalizeMessageSelectorLoweringMetadata(Expr &message) {
    message.message_send_form_symbol = BuildMessageSendFormSymbol(message.message_send_form);
    message.selector_lowering_symbol =
        BuildMessageSendSelectorLoweringSymbol(message.selector_lowering_pieces);
    message.selector_lowering_is_normalized = true;
  }
