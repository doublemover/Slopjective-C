#pragma once

#include <cstddef>
#include <string>

#include "sema/objc3_sema_contract_type_canonical.h"

struct Objc3MessageSendSelectorLoweringSiteMetadata {
  std::string selector;
  std::string selector_lowering_symbol;
  std::size_t argument_count = 0;
  std::size_t selector_piece_count = 0;
  std::size_t selector_argument_piece_count = 0;
  bool unary_form = false;
  bool keyword_form = false;
  bool selector_lowering_is_normalized = false;
  bool receiver_is_nil_literal = false;
  bool nil_receiver_semantics_enabled = false;
  bool nil_receiver_foldable = false;
  bool nil_receiver_requires_runtime_dispatch = true;
  bool nil_receiver_semantics_is_normalized = false;
  bool runtime_link_host_link_required = true;
  bool runtime_link_host_link_elided = false;
  std::size_t runtime_link_host_link_runtime_dispatch_arg_slots = 0;
  std::size_t runtime_link_host_link_declaration_parameter_count = 0;
  std::string runtime_dispatch_bridge_symbol;
  std::string runtime_link_host_link_symbol;
  bool runtime_link_host_link_is_normalized = false;
  bool receiver_is_super_identifier = false;
  bool super_dispatch_enabled = false;
  bool super_dispatch_requires_class_context = false;
  bool super_dispatch_semantics_is_normalized = false;
  std::string method_family_name;
  bool method_family_returns_retained_result = false;
  bool method_family_returns_related_result = false;
  bool method_family_semantics_is_normalized = false;
  unsigned line = 1;
  unsigned column = 1;
};

struct Objc3MessageSendSelectorLoweringSummary {
  std::size_t message_send_sites = 0;
  std::size_t unary_form_sites = 0;
  std::size_t keyword_form_sites = 0;
  std::size_t selector_lowering_symbol_sites = 0;
  std::size_t selector_lowering_piece_entries = 0;
  std::size_t selector_lowering_argument_piece_entries = 0;
  std::size_t selector_lowering_normalized_sites = 0;
  std::size_t selector_lowering_form_mismatch_sites = 0;
  std::size_t selector_lowering_arity_mismatch_sites = 0;
  std::size_t selector_lowering_symbol_mismatch_sites = 0;
  std::size_t selector_lowering_missing_symbol_sites = 0;
  std::size_t selector_lowering_contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3DispatchAbiMarshallingSummary {
  std::size_t message_send_sites = 0;
  std::size_t receiver_slots = 0;
  std::size_t selector_symbol_slots = 0;
  std::size_t argument_slots = 0;
  std::size_t keyword_argument_slots = 0;
  std::size_t unary_argument_slots = 0;
  std::size_t arity_mismatch_sites = 0;
  std::size_t missing_selector_symbol_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3NilReceiverSemanticsFoldabilitySummary {
  std::size_t message_send_sites = 0;
  std::size_t receiver_nil_literal_sites = 0;
  std::size_t nil_receiver_semantics_enabled_sites = 0;
  std::size_t nil_receiver_foldable_sites = 0;
  std::size_t nil_receiver_runtime_dispatch_required_sites = 0;
  std::size_t non_nil_receiver_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3SuperDispatchMethodFamilySummary {
  std::size_t message_send_sites = 0;
  std::size_t receiver_super_identifier_sites = 0;
  std::size_t super_dispatch_enabled_sites = 0;
  std::size_t super_dispatch_requires_class_context_sites = 0;
  std::size_t method_family_init_sites = 0;
  std::size_t method_family_copy_sites = 0;
  std::size_t method_family_mutable_copy_sites = 0;
  std::size_t method_family_new_sites = 0;
  std::size_t method_family_none_sites = 0;
  std::size_t method_family_returns_retained_result_sites = 0;
  std::size_t method_family_returns_related_result_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3RuntimeLinkHostLinkSummary {
  std::size_t message_send_sites = 0;
  std::size_t runtime_link_required_sites = 0;
  std::size_t runtime_link_elided_sites = 0;
  std::size_t runtime_dispatch_arg_slots = 0;
  std::size_t runtime_dispatch_declaration_parameter_count = 0;
  std::size_t contract_violation_sites = 0;
  std::string runtime_dispatch_symbol = kObjc3RuntimeLinkHostLinkDefaultDispatchSymbol;
  bool default_runtime_dispatch_symbol_binding = true;
  bool deterministic = true;
};
