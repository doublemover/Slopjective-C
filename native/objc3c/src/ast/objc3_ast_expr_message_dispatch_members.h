#pragma once

#ifndef OBJC3_AST_EXPR_NODE_MEMBERS_IN_EXPR
#error "objc3_ast_expr_message_dispatch_members.h must be included inside struct Expr"
#endif

  Kind kind = Kind::Number;
  int number = 0;
  bool bool_value = false;
  std::string ident;
  std::string selector;
  MessageSendForm message_send_form = MessageSendForm::None;
  std::string message_send_form_symbol;
  bool optional_send_enabled = false;
  bool optional_member_access_enabled = false;
  std::string optional_send_symbol;
  bool optional_send_is_normalized = false;
  std::vector<MessageSendSelectorPiece> selector_lowering_pieces;
  std::string selector_lowering_symbol;
  bool selector_lowering_is_normalized = false;
  unsigned dispatch_abi_receiver_slots_marshaled = 0;
  unsigned dispatch_abi_selector_slots_marshaled = 0;
  unsigned dispatch_abi_argument_value_slots_marshaled = 0;
  unsigned dispatch_abi_argument_padding_slots_marshaled = 0;
  unsigned dispatch_abi_argument_total_slots_marshaled = 0;
  unsigned dispatch_abi_total_slots_marshaled = 0;
  unsigned dispatch_abi_runtime_arg_slots = 0;
  std::string dispatch_abi_marshalling_symbol;
  bool dispatch_abi_marshalling_is_normalized = false;
  bool nil_receiver_semantics_enabled = false;
  bool nil_receiver_foldable = false;
  bool nil_receiver_requires_runtime_dispatch = true;
  std::string nil_receiver_folding_symbol;
  bool nil_receiver_semantics_is_normalized = false;
  bool super_dispatch_enabled = false;
  bool super_dispatch_requires_class_context = false;
  std::string super_dispatch_symbol;
  bool super_dispatch_semantics_is_normalized = false;
  std::string method_family_name;
  bool method_family_returns_retained_result = false;
  bool method_family_returns_related_result = false;
  std::string method_family_semantics_symbol;
  bool method_family_semantics_is_normalized = false;
  bool runtime_link_host_link_required = true;
  bool runtime_link_host_link_elided = false;
  unsigned runtime_link_host_link_declaration_parameter_count = 0;
  std::string runtime_dispatch_bridge_symbol;
  std::string runtime_link_host_link_symbol;
  bool runtime_link_host_link_is_normalized = false;
  DispatchSurfaceKind dispatch_surface_kind = DispatchSurfaceKind::Unclassified;
  std::string dispatch_surface_family_symbol;
  std::string dispatch_surface_entrypoint_family_symbol;
  bool dispatch_surface_is_normalized = false;
