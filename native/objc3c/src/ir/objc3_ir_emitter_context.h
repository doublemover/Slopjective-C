#pragma once

#include <cstddef>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "ast/objc3_ast.h"
#include "ir/objc3_ir_receiver_dispatch_policy.h"
#include "lower/contracts/runtime_dispatch_lowering_contracts.h"

struct FunctionEffectInfo {
  bool has_global_write = false;
  bool has_message_send = false;
  std::unordered_set<std::string> called_functions;
};

struct LoweredMessageSend {
  std::string receiver = "0";
  Objc3IRReceiverDispatchFacts receiver_dispatch_facts;
  std::vector<std::string> args;
  std::size_t explicit_arg_count = 0;
  std::string selector;
  std::string method_family_name;
  bool method_family_returns_retained_result = false;
  bool method_family_returns_related_result = false;
  std::string dispatch_surface_family;
  std::string dispatch_surface_entrypoint_family;
  std::string dispatch_symbol = kObjc3RuntimeDispatchSymbol;
  ValueType runtime_return_type = ValueType::I32;
  bool uses_from_class_dispatch = false;
  std::string lookup_start_class_name;
  std::string lookup_start_class_ptr;
  std::string direct_call_symbol;
  ValueType direct_call_return_type = ValueType::I32;
  std::vector<ValueType> direct_call_param_types;
};

struct ControlLabels {
  std::string continue_label;
  std::string break_label;
  bool continue_allowed = false;
  std::size_t scope_depth = 0;
  std::size_t autoreleasepool_depth = 0;
  std::size_t pending_block_dispose_depth = 0;
  std::size_t ownership_cleanup_depth = 0;
  std::size_t arc_cleanup_depth = 0;
};

struct BlockBinding {
  std::string storage_ptr;
  const Expr *literal = nullptr;
  std::string promoted_handle_ptr;
};

struct TypedKeyPathArtifact {
  std::size_t ordinal = 0;
  bool root_is_self = false;
  std::string root_name;
  std::string component_path;
  std::string profile;
  std::string descriptor_symbol;
};

struct PendingBlockDisposeCall {
  std::string helper_symbol;
  std::string storage_ptr;
};

struct PendingOwnershipCleanupCall {
  std::string binding_name;
  std::string storage_ptr;
  std::string cleanup_function_symbol;
  std::string resource_close_symbol;
  bool has_resource_invalid_value = false;
  int resource_invalid_value = 0;
  bool active = true;
};

struct FunctionContext {
  std::vector<std::string> entry_lines;
  std::vector<std::string> code_lines;
  std::vector<std::unordered_map<std::string, std::string>> scopes;
  std::unordered_map<std::string, BlockBinding> block_bindings;
  std::vector<PendingBlockDisposeCall> pending_block_dispose_calls;
  std::vector<PendingOwnershipCleanupCall> pending_ownership_cleanup_calls;
  std::vector<ControlLabels> control_stack;
  std::vector<std::string> autoreleasepool_scope_symbols;
  std::vector<std::vector<const BlockStmt *>> pending_defer_scope_blocks;
  std::vector<std::size_t> pending_block_dispose_scope_depths;
  std::vector<std::size_t> pending_ownership_cleanup_scope_depths;
  std::vector<std::size_t> arc_cleanup_scope_depths;
  std::unordered_set<std::string> nil_bound_ptrs;
  std::unordered_set<std::string> nonzero_bound_ptrs;
  std::unordered_map<std::string, int> const_value_ptrs;
  std::unordered_map<std::string, int> immediate_identifiers;
  std::vector<std::string> arc_owned_cleanup_ptrs;
  std::unordered_set<std::string> arc_owned_cleanup_ptr_set;
  std::unordered_set<std::string> arc_owned_storage_ptrs;
  std::unordered_map<std::string, std::string>
      arc_method_family_cleanup_ptr_by_value;
  std::unordered_map<std::string, std::size_t> ownership_cleanup_call_indices;
  struct ErrorHandlerFrame {
    std::string error_slot_ptr;
    std::string dispatch_label;
    std::size_t scope_depth = 0;
    std::size_t autoreleasepool_depth = 0;
    std::size_t pending_block_dispose_depth = 0;
    std::size_t ownership_cleanup_depth = 0;
    std::size_t arc_cleanup_depth = 0;
  };
  std::vector<ErrorHandlerFrame> error_handler_stack;
  std::string function_error_out_param;
  ValueType return_type = ValueType::I32;
  bool async_runtime_helper_enabled = false;
  bool actor_runtime_helper_enabled = false;
  bool actor_nonisolated_entry_enabled = false;
  std::string current_implementation_name;
  std::string current_superclass_name;
  bool current_method_is_class_method = false;
  int async_resume_entry_tag = 0;
  int async_executor_tag = 0;
  int temp_counter = 0;
  int label_counter = 0;
  bool terminated = false;
  bool global_proofs_invalidated = false;
  bool arc_return_insert_retain = false;
  bool arc_return_insert_autorelease = false;
};
