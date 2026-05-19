#include "ir/objc3_ir_statement_block_emission.h"

#include <cctype>
#include <cstddef>
#include <string>

#include "ast/objc3_ast.h"
#include "lower/contracts/error_handling_runtime_bridge_contracts.h"
#include "lower/contracts/ownership_runtime_memory_management_contracts.h"

namespace {

int Objc3IRErrorHandlingCatchKindForTypeSpelling(
    const std::string &spelling) {
  std::string normalized;
  normalized.reserve(spelling.size());
  for (unsigned char ch : spelling) {
    if (!std::isspace(ch)) {
      normalized.push_back(
          static_cast<char>(std::tolower(static_cast<unsigned char>(ch))));
    }
  }
  if (normalized == "nserror" || normalized == "nserror*" ||
      normalized == "nserror**") {
    return 1;
  }
  if (normalized == "id<error>" || normalized == "id<error>*") {
    return 2;
  }
  return 0;
}

}  // namespace

void EmitObjc3IRBlockStatement(
    const BlockStmt *block_stmt, FunctionContext &ctx,
    const Objc3IRStatementEmissionCallbacks &callbacks) {
  if (block_stmt == nullptr) {
    return;
  }
  if (block_stmt->is_do_catch_scope) {
    const std::size_t autoreleasepool_depth =
        ctx.autoreleasepool_scope_symbols.size();
    const std::string dispatch_label =
        callbacks.new_label(ctx, "do_catch_dispatch_");
    const std::string merge_label =
        callbacks.new_label(ctx, "do_catch_end_");
    const std::string error_slot =
        callbacks.build_throws_error_slot_alloca(ctx, "do_catch");
    ctx.code_lines.push_back("  store i32 0, ptr " + error_slot +
                             ", align 4");
    callbacks.push_scope(ctx);
    ctx.error_handler_stack.push_back(
        {error_slot,
         dispatch_label,
         ctx.scopes.size() - 1u,
         ctx.autoreleasepool_scope_symbols.size(),
         ctx.pending_block_dispose_calls.size(),
         ctx.pending_ownership_cleanup_calls.size(),
         ctx.arc_owned_cleanup_ptrs.size()});
    for (const auto &nested_stmt : block_stmt->body) {
      EmitObjc3IRStatement(nested_stmt.get(), ctx, callbacks);
      if (ctx.terminated) {
        break;
      }
    }
    const bool body_terminated = ctx.terminated;
    ctx.error_handler_stack.pop_back();
    callbacks.pop_scope(ctx, !body_terminated);
    if (!body_terminated) {
      callbacks.emit_autoreleasepool_unwind_to_depth(
          ctx, autoreleasepool_depth);
      ctx.code_lines.push_back("  br label %" + merge_label);
    }
    ctx.code_lines.push_back(dispatch_label + ":");
    const std::string loaded_error =
        callbacks.emit_load_thrown_error(error_slot, ctx);
    const std::string no_match_label =
        callbacks.new_label(ctx, "do_catch_no_match_");
    for (std::size_t clause_index = 0;
         clause_index < block_stmt->catch_clauses.size(); ++clause_index) {
      const auto &clause = block_stmt->catch_clauses[clause_index];
      const std::string catch_label =
          callbacks.new_label(ctx,
                              clause.catch_all ? "catch_all_" : "catch_");
      const std::string next_label =
          clause_index + 1u < block_stmt->catch_clauses.size()
              ? callbacks.new_label(ctx, "catch_next_")
              : no_match_label;
      const std::string catches = callbacks.new_temp(ctx);
      const std::string catches_bool = callbacks.new_temp(ctx);
      ctx.code_lines.push_back(
          "  " + catches + " = call i32 @" +
          std::string(kObjc3RuntimeCatchMatchesErrorI32Symbol) + "(i32 " +
          loaded_error + ", i32 " +
          std::to_string(Objc3IRErrorHandlingCatchKindForTypeSpelling(
              clause.binding_type_spelling)) +
          ", i32 " + (clause.catch_all ? "1" : "0") + ")");
      ctx.code_lines.push_back("  " + catches_bool + " = icmp ne i32 " +
                               catches + ", 0");
      ctx.code_lines.push_back("  br i1 " + catches_bool + ", label %" +
                               catch_label + ", label %" + next_label);
      ctx.code_lines.push_back(catch_label + ":");
      callbacks.push_scope(ctx);
      if (clause.has_binding && !clause.binding_name.empty()) {
        const std::string ptr =
            "%" + clause.binding_name + ".addr." +
            std::to_string(ctx.temp_counter++);
        ctx.entry_lines.push_back("  " + ptr + " = alloca i32, align 4");
        ctx.code_lines.push_back("  store i32 " + loaded_error +
                                 ", ptr " + ptr + ", align 4");
        ctx.scopes.back()[clause.binding_name] = ptr;
      }
      ctx.terminated = false;
      for (const auto &catch_stmt : clause.body) {
        EmitObjc3IRStatement(catch_stmt.get(), ctx, callbacks);
        if (ctx.terminated) {
          break;
        }
      }
      const bool catch_terminated = ctx.terminated;
      callbacks.pop_scope(ctx, !catch_terminated);
      if (!catch_terminated) {
        ctx.code_lines.push_back("  br label %" + merge_label);
      }
      ctx.code_lines.push_back(next_label + ":");
    }
    callbacks.emit_propagate_thrown_error(loaded_error, ctx);
    ctx.code_lines.push_back(merge_label + ":");
    ctx.terminated = false;
    return;
  }
  const std::size_t autoreleasepool_depth =
      ctx.autoreleasepool_scope_symbols.size();
  if (block_stmt->is_autoreleasepool_scope) {
    // integration anchor: the current non-suspending async
    // slice reuses the existing autoreleasepool scope hooks and later
    // composes them with deferred cleanup emission on scope exit. There
    // is still no separate suspension-frame cleanup runtime here.
    ctx.code_lines.push_back(
        "  call void @" +
        std::string(kObjc3RuntimePushAutoreleasepoolScopeSymbol) + "()");
    ctx.autoreleasepool_scope_symbols.push_back(
        block_stmt->autoreleasepool_scope_symbol);
  }
  callbacks.push_scope(ctx);
  for (const auto &nested_stmt : block_stmt->body) {
    EmitObjc3IRStatement(nested_stmt.get(), ctx, callbacks);
  }
  callbacks.pop_scope(ctx, !ctx.terminated);
  if (!ctx.terminated) {
    callbacks.emit_autoreleasepool_unwind_to_depth(
        ctx, autoreleasepool_depth);
  }
}
