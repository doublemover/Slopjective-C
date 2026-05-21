#include "ir/objc3_ir_prototype_declarations_runtime_helpers.h"

#include <sstream>
#include <string>
#include <unordered_set>

#include "ast/objc3_ast.h"
#include "ast/objc3_ast_contracts.h"
#include "ir/objc3_ir_concurrency_identity.h"
#include "ir/objc3_ir_frontend_metadata.h"
#include "ir/objc3_ir_prototype_declarations.h"
#include "lower/contracts/block_runtime_helper_contracts.h"
#include "lower/contracts/concurrency_actor_contracts.h"
#include "lower/contracts/concurrency_continuation_runtime_contracts.h"
#include "lower/contracts/concurrency_task_runtime_helper_contracts.h"
#include "lower/contracts/error_handling_runtime_bridge_contracts.h"
#include "lower/contracts/ownership_runtime_accessor_helper_contracts.h"
#include "lower/contracts/ownership_runtime_memory_management_contracts.h"

namespace {

constexpr const char *kObjc3RuntimeStdlibTextUtf8StorageI32Symbol =
    "objc3_runtime_stdlib_text_utf8_storage_i32";
constexpr const char *kObjc3RuntimeStdlibCollectionsArray3I32Symbol =
    "objc3_runtime_stdlib_collections_array3_i32";
constexpr const char *kObjc3RuntimeStdlibCollectionsArrayStorageI32Symbol =
    "objc3_runtime_stdlib_collections_array_storage_i32";
constexpr const char *kObjc3RuntimeStdlibCollectionsMutableArrayI32Symbol =
    "objc3_runtime_stdlib_collections_mutable_array_i32";
constexpr const char *kObjc3RuntimeStdlibCollectionsMutableArrayAppendI32Symbol =
    "objc3_runtime_stdlib_collections_mutable_array_append_i32";
constexpr const char *kObjc3RuntimeStdlibCollectionsMutableArraySetI32Symbol =
    "objc3_runtime_stdlib_collections_mutable_array_set_i32";
constexpr const char *kObjc3RuntimeStdlibCollectionsMutableArrayRemoveAtI32Symbol =
    "objc3_runtime_stdlib_collections_mutable_array_remove_at_i32";
constexpr const char *kObjc3RuntimeStdlibCollectionsArrayGetOrI32Symbol =
    "objc3_runtime_stdlib_collections_array_get_or_i32";
constexpr const char *kObjc3RuntimeStdlibCollectionsArrayIteratorI32Symbol =
    "objc3_runtime_stdlib_collections_array_iterator_i32";
constexpr const char *kObjc3RuntimeStdlibCollectionsMapEmptyI32Symbol =
    "objc3_runtime_stdlib_collections_map_empty_i32";
constexpr const char *kObjc3RuntimeStdlibCollectionsMapInsertI32Symbol =
    "objc3_runtime_stdlib_collections_map_insert_i32";
constexpr const char *kObjc3RuntimeStdlibCollectionsMapDeleteI32Symbol =
    "objc3_runtime_stdlib_collections_map_delete_i32";
constexpr const char *kObjc3RuntimeStdlibCollectionsMapLookupOrI32Symbol =
    "objc3_runtime_stdlib_collections_map_lookup_or_i32";
constexpr const char *kObjc3RuntimeStdlibCollectionsMapKeyIteratorI32Symbol =
    "objc3_runtime_stdlib_collections_map_key_iterator_i32";
constexpr const char *kObjc3RuntimeStdlibCollectionsMapValueIteratorI32Symbol =
    "objc3_runtime_stdlib_collections_map_value_iterator_i32";
constexpr const char *kObjc3RuntimeStdlibCollectionsSet3I32Symbol =
    "objc3_runtime_stdlib_collections_set3_i32";
constexpr const char *kObjc3RuntimeStdlibCollectionsSetStorageI32Symbol =
    "objc3_runtime_stdlib_collections_set_storage_i32";
constexpr const char *kObjc3RuntimeStdlibCollectionsSetInsertI32Symbol =
    "objc3_runtime_stdlib_collections_set_insert_i32";
constexpr const char *kObjc3RuntimeStdlibCollectionsSetDeleteI32Symbol =
    "objc3_runtime_stdlib_collections_set_delete_i32";
constexpr const char *kObjc3RuntimeStdlibCollectionsSetIteratorI32Symbol =
    "objc3_runtime_stdlib_collections_set_iterator_i32";
constexpr const char *kObjc3RuntimeStdlibCollectionsIteratorNextOrI32Symbol =
    "objc3_runtime_stdlib_collections_iterator_next_or_i32";
constexpr const char *kObjc3RuntimeStdlibCollectionsLastStatusI32Symbol =
    "objc3_runtime_stdlib_collections_last_status_i32";

bool Objc3IRFunctionRequiresArcHelperDeclarations(
    const FunctionDecl &fn, const Objc3IRFrontendMetadata &frontend_metadata) {
  if (EffectiveArcReturnInsertRetain(fn, frontend_metadata.arc_mode_enabled) ||
      fn.return_ownership_insert_release ||
      EffectiveArcReturnInsertAutorelease(fn)) {
    return true;
  }
  for (const auto &param : fn.params) {
    if (EffectiveArcParamInsertRetain(param, frontend_metadata.arc_mode_enabled) ||
        EffectiveArcParamInsertRelease(param, frontend_metadata.arc_mode_enabled) ||
        param.ownership_insert_autorelease) {
      return true;
    }
  }
  return false;
}

bool Objc3IRMethodRequiresArcHelperDeclarations(
    const Objc3IRMethodDefinition &method_def,
    const Objc3IRFrontendMetadata &frontend_metadata) {
  if (method_def.method == nullptr) {
    return false;
  }
  const Objc3MethodDecl &method = *method_def.method;
  if (EffectiveArcReturnInsertRetain(method, frontend_metadata.arc_mode_enabled) ||
      method.return_ownership_insert_release ||
      EffectiveArcReturnInsertAutorelease(method)) {
    return true;
  }
  for (const auto &param : method.params) {
    if (EffectiveArcParamInsertRetain(param, frontend_metadata.arc_mode_enabled) ||
        EffectiveArcParamInsertRelease(param, frontend_metadata.arc_mode_enabled) ||
        param.ownership_insert_autorelease) {
      return true;
    }
  }
  return false;
}

bool Objc3IRRequiresArcHelperDeclarations(
    const Objc3IRPrototypeDeclarationOptions &options) {
  if (options.frontend_metadata.arc_mode_enabled &&
      options.frontend_metadata
              .super_dispatch_method_family_returns_retained_result_sites > 0u) {
    return true;
  }
  for (const auto &fn : options.program.functions) {
    if (Objc3IRFunctionRequiresArcHelperDeclarations(fn,
                                                     options.frontend_metadata)) {
      return true;
    }
  }
  for (const auto &method_def : options.method_definitions) {
    if (Objc3IRMethodRequiresArcHelperDeclarations(
            method_def, options.frontend_metadata)) {
      return true;
    }
  }
  return false;
}

bool Objc3IRFunctionRequiresAsyncRuntimeHelperDeclarations(
    const FunctionDecl &fn) {
  return fn.async_declared && fn.return_type != ValueType::Void &&
         Objc3IRExecutorAffinityTag(fn) != 0;
}

bool Objc3IRMethodRequiresAsyncRuntimeHelperDeclarations(
    const Objc3IRMethodDefinition &method_def) {
  return method_def.method != nullptr && method_def.method->async_declared &&
         method_def.method->return_type != ValueType::Void &&
         Objc3IRExecutorAffinityTag(*method_def.method) != 0;
}

bool Objc3IRRequiresAsyncRuntimeHelperDeclarations(
    const Objc3IRPrototypeDeclarationOptions &options) {
  for (const auto &fn : options.program.functions) {
    if (Objc3IRFunctionRequiresAsyncRuntimeHelperDeclarations(fn)) {
      return true;
    }
  }
  for (const auto &method_def : options.method_definitions) {
    if (Objc3IRMethodRequiresAsyncRuntimeHelperDeclarations(method_def)) {
      return true;
    }
  }
  return false;
}

bool Objc3IRExprRequiresTextLiteralHelperDeclarations(const Expr *expr) {
  if (expr == nullptr) {
    return false;
  }
  switch (expr->kind) {
    case Expr::Kind::StringLiteral:
      return true;
    case Expr::Kind::Binary:
      return Objc3IRExprRequiresTextLiteralHelperDeclarations(
                 expr->left.get()) ||
             Objc3IRExprRequiresTextLiteralHelperDeclarations(
                 expr->right.get());
    case Expr::Kind::Conditional:
      return Objc3IRExprRequiresTextLiteralHelperDeclarations(
                 expr->left.get()) ||
             Objc3IRExprRequiresTextLiteralHelperDeclarations(
                 expr->right.get()) ||
             Objc3IRExprRequiresTextLiteralHelperDeclarations(
                 expr->third.get());
    case Expr::Kind::Call:
    case Expr::Kind::Try:
    case Expr::Kind::Throw:
      for (const auto &arg : expr->args) {
        if (Objc3IRExprRequiresTextLiteralHelperDeclarations(arg.get())) {
          return true;
        }
      }
      return false;
    case Expr::Kind::MessageSend:
      if (Objc3IRExprRequiresTextLiteralHelperDeclarations(
              expr->receiver.get())) {
        return true;
      }
      for (const auto &arg : expr->args) {
        if (Objc3IRExprRequiresTextLiteralHelperDeclarations(arg.get())) {
          return true;
        }
      }
      return false;
    case Expr::Kind::CollectionLiteral:
      for (const auto &key : expr->collection_keys) {
        if (Objc3IRExprRequiresTextLiteralHelperDeclarations(key.get())) {
          return true;
        }
      }
      for (const auto &value : expr->collection_values) {
        if (Objc3IRExprRequiresTextLiteralHelperDeclarations(value.get())) {
          return true;
        }
      }
      return false;
    case Expr::Kind::IndexAccess:
      return Objc3IRExprRequiresTextLiteralHelperDeclarations(
                 expr->left.get()) ||
             Objc3IRExprRequiresTextLiteralHelperDeclarations(
                 expr->right.get());
    case Expr::Kind::Number:
    case Expr::Kind::BoolLiteral:
    case Expr::Kind::NilLiteral:
    case Expr::Kind::Identifier:
    case Expr::Kind::KeyPathLiteral:
    case Expr::Kind::BlockLiteral:
      return false;
  }
  return false;
}

bool Objc3IRExprRequiresCollectionHelperDeclarations(const Expr *expr) {
  if (expr == nullptr) {
    return false;
  }
  switch (expr->kind) {
    case Expr::Kind::CollectionLiteral:
    case Expr::Kind::IndexAccess:
      return true;
    case Expr::Kind::Binary:
      return Objc3IRExprRequiresCollectionHelperDeclarations(
                 expr->left.get()) ||
             Objc3IRExprRequiresCollectionHelperDeclarations(
                 expr->right.get());
    case Expr::Kind::Conditional:
      return Objc3IRExprRequiresCollectionHelperDeclarations(
                 expr->left.get()) ||
             Objc3IRExprRequiresCollectionHelperDeclarations(
                 expr->right.get()) ||
             Objc3IRExprRequiresCollectionHelperDeclarations(
                 expr->third.get());
    case Expr::Kind::Call:
    case Expr::Kind::Try:
    case Expr::Kind::Throw:
      for (const auto &arg : expr->args) {
        if (Objc3IRExprRequiresCollectionHelperDeclarations(arg.get())) {
          return true;
        }
      }
      return false;
    case Expr::Kind::MessageSend:
      if (Objc3IRExprRequiresCollectionHelperDeclarations(
              expr->receiver.get())) {
        return true;
      }
      for (const auto &arg : expr->args) {
        if (Objc3IRExprRequiresCollectionHelperDeclarations(arg.get())) {
          return true;
        }
      }
      return false;
    case Expr::Kind::Number:
    case Expr::Kind::BoolLiteral:
    case Expr::Kind::NilLiteral:
    case Expr::Kind::StringLiteral:
    case Expr::Kind::Identifier:
    case Expr::Kind::KeyPathLiteral:
    case Expr::Kind::BlockLiteral:
      return false;
  }
  return false;
}

bool Objc3IRForClauseRequiresTextLiteralHelperDeclarations(
    const ForClause &clause) {
  return Objc3IRExprRequiresTextLiteralHelperDeclarations(clause.value.get());
}

bool Objc3IRForClauseRequiresCollectionHelperDeclarations(
    const ForClause &clause) {
  return Objc3IRExprRequiresCollectionHelperDeclarations(clause.value.get());
}

bool Objc3IRStmtRequiresTextLiteralHelperDeclarations(const Stmt *stmt) {
  if (stmt == nullptr) {
    return false;
  }
  switch (stmt->kind) {
    case Stmt::Kind::Let:
      return stmt->let_stmt != nullptr &&
             Objc3IRExprRequiresTextLiteralHelperDeclarations(
                 stmt->let_stmt->value.get());
    case Stmt::Kind::Assign:
      return stmt->assign_stmt != nullptr &&
             Objc3IRExprRequiresTextLiteralHelperDeclarations(
                 stmt->assign_stmt->value.get());
    case Stmt::Kind::Return:
      return stmt->return_stmt != nullptr &&
             Objc3IRExprRequiresTextLiteralHelperDeclarations(
                 stmt->return_stmt->value.get());
    case Stmt::Kind::Expr:
      return stmt->expr_stmt != nullptr &&
             Objc3IRExprRequiresTextLiteralHelperDeclarations(
                 stmt->expr_stmt->value.get());
    case Stmt::Kind::If:
      if (stmt->if_stmt == nullptr) {
        return false;
      }
      if (Objc3IRExprRequiresTextLiteralHelperDeclarations(
              stmt->if_stmt->condition.get())) {
        return true;
      }
      for (const auto &then_stmt : stmt->if_stmt->then_body) {
        if (Objc3IRStmtRequiresTextLiteralHelperDeclarations(
                then_stmt.get())) {
          return true;
        }
      }
      for (const auto &else_stmt : stmt->if_stmt->else_body) {
        if (Objc3IRStmtRequiresTextLiteralHelperDeclarations(
                else_stmt.get())) {
          return true;
        }
      }
      return false;
    case Stmt::Kind::DoWhile:
      if (stmt->do_while_stmt == nullptr) {
        return false;
      }
      for (const auto &loop_stmt : stmt->do_while_stmt->body) {
        if (Objc3IRStmtRequiresTextLiteralHelperDeclarations(
                loop_stmt.get())) {
          return true;
        }
      }
      return Objc3IRExprRequiresTextLiteralHelperDeclarations(
          stmt->do_while_stmt->condition.get());
    case Stmt::Kind::For:
      if (stmt->for_stmt == nullptr) {
        return false;
      }
      if (Objc3IRForClauseRequiresTextLiteralHelperDeclarations(
              stmt->for_stmt->init) ||
          Objc3IRExprRequiresTextLiteralHelperDeclarations(
              stmt->for_stmt->condition.get()) ||
          Objc3IRForClauseRequiresTextLiteralHelperDeclarations(
              stmt->for_stmt->step)) {
        return true;
      }
      for (const auto &loop_stmt : stmt->for_stmt->body) {
        if (Objc3IRStmtRequiresTextLiteralHelperDeclarations(
                loop_stmt.get())) {
          return true;
        }
      }
      return false;
    case Stmt::Kind::ForIn:
      if (stmt->for_in_stmt == nullptr) {
        return false;
      }
      if (Objc3IRExprRequiresTextLiteralHelperDeclarations(
              stmt->for_in_stmt->collection.get())) {
        return true;
      }
      for (const auto &loop_stmt : stmt->for_in_stmt->body) {
        if (Objc3IRStmtRequiresTextLiteralHelperDeclarations(
                loop_stmt.get())) {
          return true;
        }
      }
      return false;
    case Stmt::Kind::CollectionMutation:
      return stmt->collection_mutation_stmt != nullptr &&
             (Objc3IRExprRequiresTextLiteralHelperDeclarations(
                  stmt->collection_mutation_stmt->key_or_index.get()) ||
              Objc3IRExprRequiresTextLiteralHelperDeclarations(
                  stmt->collection_mutation_stmt->value.get()));
    case Stmt::Kind::Switch:
      if (stmt->switch_stmt == nullptr) {
        return false;
      }
      if (Objc3IRExprRequiresTextLiteralHelperDeclarations(
              stmt->switch_stmt->condition.get())) {
        return true;
      }
      for (const auto &case_stmt : stmt->switch_stmt->cases) {
        for (const auto &case_body_stmt : case_stmt.body) {
          if (Objc3IRStmtRequiresTextLiteralHelperDeclarations(
                  case_body_stmt.get())) {
            return true;
          }
        }
      }
      return false;
    case Stmt::Kind::While:
      if (stmt->while_stmt == nullptr) {
        return false;
      }
      if (Objc3IRExprRequiresTextLiteralHelperDeclarations(
              stmt->while_stmt->condition.get())) {
        return true;
      }
      for (const auto &loop_stmt : stmt->while_stmt->body) {
        if (Objc3IRStmtRequiresTextLiteralHelperDeclarations(
                loop_stmt.get())) {
          return true;
        }
      }
      return false;
    case Stmt::Kind::Block:
    case Stmt::Kind::Defer:
      if (stmt->block_stmt == nullptr) {
        return false;
      }
      for (const auto &nested_stmt : stmt->block_stmt->body) {
        if (Objc3IRStmtRequiresTextLiteralHelperDeclarations(
                nested_stmt.get())) {
          return true;
        }
      }
      return false;
    case Stmt::Kind::Break:
    case Stmt::Kind::Continue:
    case Stmt::Kind::Empty:
      return false;
  }
  return false;
}

bool Objc3IRStmtRequiresCollectionHelperDeclarations(const Stmt *stmt) {
  if (stmt == nullptr) {
    return false;
  }
  switch (stmt->kind) {
    case Stmt::Kind::Let:
      return stmt->let_stmt != nullptr &&
             Objc3IRExprRequiresCollectionHelperDeclarations(
                 stmt->let_stmt->value.get());
    case Stmt::Kind::Assign:
      return stmt->assign_stmt != nullptr &&
             Objc3IRExprRequiresCollectionHelperDeclarations(
                 stmt->assign_stmt->value.get());
    case Stmt::Kind::CollectionMutation:
      return true;
    case Stmt::Kind::Return:
      return stmt->return_stmt != nullptr &&
             Objc3IRExprRequiresCollectionHelperDeclarations(
                 stmt->return_stmt->value.get());
    case Stmt::Kind::Expr:
      return stmt->expr_stmt != nullptr &&
             Objc3IRExprRequiresCollectionHelperDeclarations(
                 stmt->expr_stmt->value.get());
    case Stmt::Kind::ForIn:
      return true;
    case Stmt::Kind::For:
      if (stmt->for_stmt == nullptr) {
        return false;
      }
      if (Objc3IRForClauseRequiresCollectionHelperDeclarations(
              stmt->for_stmt->init) ||
          Objc3IRExprRequiresCollectionHelperDeclarations(
              stmt->for_stmt->condition.get()) ||
          Objc3IRForClauseRequiresCollectionHelperDeclarations(
              stmt->for_stmt->step)) {
        return true;
      }
      for (const auto &loop_stmt : stmt->for_stmt->body) {
        if (Objc3IRStmtRequiresCollectionHelperDeclarations(loop_stmt.get())) {
          return true;
        }
      }
      return false;
    case Stmt::Kind::If:
      if (stmt->if_stmt == nullptr) {
        return false;
      }
      if (Objc3IRExprRequiresCollectionHelperDeclarations(
              stmt->if_stmt->condition.get())) {
        return true;
      }
      for (const auto &then_stmt : stmt->if_stmt->then_body) {
        if (Objc3IRStmtRequiresCollectionHelperDeclarations(then_stmt.get())) {
          return true;
        }
      }
      for (const auto &else_stmt : stmt->if_stmt->else_body) {
        if (Objc3IRStmtRequiresCollectionHelperDeclarations(else_stmt.get())) {
          return true;
        }
      }
      return false;
    case Stmt::Kind::DoWhile:
      if (stmt->do_while_stmt == nullptr) {
        return false;
      }
      for (const auto &loop_stmt : stmt->do_while_stmt->body) {
        if (Objc3IRStmtRequiresCollectionHelperDeclarations(loop_stmt.get())) {
          return true;
        }
      }
      return Objc3IRExprRequiresCollectionHelperDeclarations(
          stmt->do_while_stmt->condition.get());
    case Stmt::Kind::Switch:
      if (stmt->switch_stmt == nullptr) {
        return false;
      }
      if (Objc3IRExprRequiresCollectionHelperDeclarations(
              stmt->switch_stmt->condition.get())) {
        return true;
      }
      for (const auto &case_stmt : stmt->switch_stmt->cases) {
        for (const auto &case_body_stmt : case_stmt.body) {
          if (Objc3IRStmtRequiresCollectionHelperDeclarations(
                  case_body_stmt.get())) {
            return true;
          }
        }
      }
      return false;
    case Stmt::Kind::While:
      if (stmt->while_stmt == nullptr) {
        return false;
      }
      if (Objc3IRExprRequiresCollectionHelperDeclarations(
              stmt->while_stmt->condition.get())) {
        return true;
      }
      for (const auto &loop_stmt : stmt->while_stmt->body) {
        if (Objc3IRStmtRequiresCollectionHelperDeclarations(loop_stmt.get())) {
          return true;
        }
      }
      return false;
    case Stmt::Kind::Block:
    case Stmt::Kind::Defer:
      if (stmt->block_stmt == nullptr) {
        return false;
      }
      for (const auto &nested_stmt : stmt->block_stmt->body) {
        if (Objc3IRStmtRequiresCollectionHelperDeclarations(
                nested_stmt.get())) {
          return true;
        }
      }
      return false;
    case Stmt::Kind::Break:
    case Stmt::Kind::Continue:
    case Stmt::Kind::Empty:
      return false;
  }
  return false;
}

bool Objc3IRRequiresTextLiteralHelperDeclarations(
    const Objc3IRPrototypeDeclarationOptions &options) {
  for (const auto &global : options.program.globals) {
    if (Objc3IRExprRequiresTextLiteralHelperDeclarations(global.value.get())) {
      return true;
    }
  }
  for (const auto &fn : options.program.functions) {
    for (const auto &stmt : fn.body) {
      if (Objc3IRStmtRequiresTextLiteralHelperDeclarations(stmt.get())) {
        return true;
      }
    }
  }
  for (const auto &implementation : options.program.implementations) {
    for (const auto &method : implementation.methods) {
      for (const auto &stmt : method.body) {
        if (Objc3IRStmtRequiresTextLiteralHelperDeclarations(stmt.get())) {
          return true;
        }
      }
    }
  }
  return false;
}

bool Objc3IRRequiresCollectionHelperDeclarations(
    const Objc3IRPrototypeDeclarationOptions &options) {
  for (const auto &global : options.program.globals) {
    if (Objc3IRExprRequiresCollectionHelperDeclarations(global.value.get())) {
      return true;
    }
  }
  for (const auto &fn : options.program.functions) {
    for (const auto &stmt : fn.body) {
      if (Objc3IRStmtRequiresCollectionHelperDeclarations(stmt.get())) {
        return true;
      }
    }
  }
  for (const auto &implementation : options.program.implementations) {
    for (const auto &method : implementation.methods) {
      for (const auto &stmt : method.body) {
        if (Objc3IRStmtRequiresCollectionHelperDeclarations(stmt.get())) {
          return true;
        }
      }
    }
  }
  return false;
}

}  // namespace

bool EmitObjc3IRDeclarationOnce(std::unordered_set<std::string> &declared_symbols,
                                bool &emitted, std::ostringstream &out,
                                const std::string &symbol,
                                const std::string &declaration) {
  if (symbol.empty() || !declared_symbols.insert(symbol).second) {
    return false;
  }
  out << declaration;
  emitted = true;
  return true;
}

bool Objc3IRRequiresRuntimeHelperDeclarations(
    const Objc3IRPrototypeDeclarationOptions &options) {
  const Objc3IRFrontendMetadata &frontend_metadata = options.frontend_metadata;
  return options.synthesized_property_accessor_count > 0u ||
         !options.method_definitions.empty() ||
         Objc3IRRequiresArcHelperDeclarations(options) ||
         !frontend_metadata.lowering_error_handling_throws_abi_propagation_replay_key
              .empty() ||
         Objc3IRRequiresAsyncRuntimeHelperDeclarations(options) ||
         !frontend_metadata.lowering_async_continuation_replay_key.empty() ||
         !frontend_metadata.lowering_await_lowering_suspension_state_replay_key
              .empty() ||
         frontend_metadata.autoreleasepool_scope_lowering_scope_sites > 0u ||
         frontend_metadata.block_storage_escape_lowering_escape_to_heap_sites >
             0u ||
         frontend_metadata.block_copy_dispose_lowering_copy_helper_required_sites >
             0u ||
         frontend_metadata
                 .block_copy_dispose_lowering_dispose_helper_required_sites >
             0u ||
         Objc3IRRequiresTextLiteralHelperDeclarations(options) ||
         Objc3IRRequiresCollectionHelperDeclarations(options);
}

void EmitObjc3IRRuntimeHelperDeclarations(
    std::unordered_set<std::string> &declared_symbols, bool &emitted,
    std::ostringstream &out) {
  EmitObjc3IRDeclarationOnce(
      declared_symbols, emitted, out, kObjc3RuntimeReadCurrentPropertyI32Symbol,
      "declare i32 @" + std::string(kObjc3RuntimeReadCurrentPropertyI32Symbol) +
          "()\n");
  EmitObjc3IRDeclarationOnce(
      declared_symbols, emitted, out, kObjc3RuntimeWriteCurrentPropertyI32Symbol,
      "declare void @" +
          std::string(kObjc3RuntimeWriteCurrentPropertyI32Symbol) + "(i32)\n");
  EmitObjc3IRDeclarationOnce(
      declared_symbols, emitted, out,
      kObjc3RuntimeExchangeCurrentPropertyI32Symbol,
      "declare i32 @" +
          std::string(kObjc3RuntimeExchangeCurrentPropertyI32Symbol) +
          "(i32)\n");
  EmitObjc3IRDeclarationOnce(
      declared_symbols, emitted, out,
      kObjc3RuntimeCurrentDispatchReceiverI32Symbol,
      "declare i32 @" +
          std::string(kObjc3RuntimeCurrentDispatchReceiverI32Symbol) + "()\n");
  EmitObjc3IRDeclarationOnce(
      declared_symbols, emitted, out, kObjc3RuntimeStoreThrownErrorI32Symbol,
      "declare void @" + std::string(kObjc3RuntimeStoreThrownErrorI32Symbol) +
          "(ptr, i32)\n");
  EmitObjc3IRDeclarationOnce(
      declared_symbols, emitted, out, kObjc3RuntimeLoadThrownErrorI32Symbol,
      "declare i32 @" + std::string(kObjc3RuntimeLoadThrownErrorI32Symbol) +
          "(ptr)\n");
  EmitObjc3IRDeclarationOnce(
      declared_symbols, emitted, out, kObjc3RuntimeBridgeStatusErrorI32Symbol,
      "declare i32 @" + std::string(kObjc3RuntimeBridgeStatusErrorI32Symbol) +
          "(i32, i32)\n");
  EmitObjc3IRDeclarationOnce(
      declared_symbols, emitted, out, kObjc3RuntimeBridgeNSErrorErrorI32Symbol,
      "declare i32 @" + std::string(kObjc3RuntimeBridgeNSErrorErrorI32Symbol) +
          "(i32)\n");
  EmitObjc3IRDeclarationOnce(
      declared_symbols, emitted, out,
      kObjc3RuntimeBridgeForeignExceptionErrorI32Symbol,
      "declare i32 @" +
          std::string(kObjc3RuntimeBridgeForeignExceptionErrorI32Symbol) +
          "(i32, i32, i32)\n");
  EmitObjc3IRDeclarationOnce(
      declared_symbols, emitted, out, kObjc3RuntimeCatchMatchesErrorI32Symbol,
      "declare i32 @" + std::string(kObjc3RuntimeCatchMatchesErrorI32Symbol) +
          "(i32, i32, i32)\n");
  EmitObjc3IRDeclarationOnce(
      declared_symbols, emitted, out,
      kObjc3RuntimeAllocateAsyncContinuationI32Symbol,
      "declare i32 @" +
          std::string(kObjc3RuntimeAllocateAsyncContinuationI32Symbol) +
          "(i32, i32)\n");
  EmitObjc3IRDeclarationOnce(
      declared_symbols, emitted, out,
      kObjc3RuntimeHandoffAsyncContinuationToExecutorI32Symbol,
      "declare i32 @" +
          std::string(kObjc3RuntimeHandoffAsyncContinuationToExecutorI32Symbol) +
          "(i32, i32)\n");
  EmitObjc3IRDeclarationOnce(
      declared_symbols, emitted, out, kObjc3RuntimeResumeAsyncContinuationI32Symbol,
      "declare i32 @" +
          std::string(kObjc3RuntimeResumeAsyncContinuationI32Symbol) +
          "(i32, i32)\n");
  EmitObjc3IRDeclarationOnce(
      declared_symbols, emitted, out, kObjc3RuntimeSpawnTaskI32Symbol,
      "declare i32 @" + std::string(kObjc3RuntimeSpawnTaskI32Symbol) +
          "(i32, i32)\n");
  EmitObjc3IRDeclarationOnce(
      declared_symbols, emitted, out, kObjc3RuntimeEnterTaskGroupScopeI32Symbol,
      "declare i32 @" + std::string(kObjc3RuntimeEnterTaskGroupScopeI32Symbol) +
          "(i32)\n");
  EmitObjc3IRDeclarationOnce(
      declared_symbols, emitted, out, kObjc3RuntimeAddTaskGroupTaskI32Symbol,
      "declare i32 @" + std::string(kObjc3RuntimeAddTaskGroupTaskI32Symbol) +
          "(i32)\n");
  EmitObjc3IRDeclarationOnce(
      declared_symbols, emitted, out, kObjc3RuntimeWaitTaskGroupNextI32Symbol,
      "declare i32 @" + std::string(kObjc3RuntimeWaitTaskGroupNextI32Symbol) +
          "(i32)\n");
  EmitObjc3IRDeclarationOnce(
      declared_symbols, emitted, out, kObjc3RuntimeCancelTaskGroupI32Symbol,
      "declare i32 @" + std::string(kObjc3RuntimeCancelTaskGroupI32Symbol) +
          "(i32)\n");
  EmitObjc3IRDeclarationOnce(
      declared_symbols, emitted, out, kObjc3RuntimeTaskIsCancelledI32Symbol,
      "declare i32 @" + std::string(kObjc3RuntimeTaskIsCancelledI32Symbol) +
          "(i32)\n");
  EmitObjc3IRDeclarationOnce(
      declared_symbols, emitted, out, kObjc3RuntimeTaskOnCancelI32Symbol,
      "declare i32 @" + std::string(kObjc3RuntimeTaskOnCancelI32Symbol) +
          "(i32)\n");
  EmitObjc3IRDeclarationOnce(
      declared_symbols, emitted, out, kObjc3RuntimeExecutorHopI32Symbol,
      "declare i32 @" + std::string(kObjc3RuntimeExecutorHopI32Symbol) +
          "(i32, i32)\n");
  EmitObjc3IRDeclarationOnce(
      declared_symbols, emitted, out,
      kObjc3RuntimeActorEnterIsolationThunkI32Symbol,
      "declare i32 @" +
          std::string(kObjc3RuntimeActorEnterIsolationThunkI32Symbol) +
          "(i32)\n");
  EmitObjc3IRDeclarationOnce(
      declared_symbols, emitted, out, kObjc3RuntimeActorEnterNonisolatedI32Symbol,
      "declare i32 @" +
          std::string(kObjc3RuntimeActorEnterNonisolatedI32Symbol) +
          "(i32, i32)\n");
  EmitObjc3IRDeclarationOnce(
      declared_symbols, emitted, out, kObjc3RuntimeActorHopToExecutorI32Symbol,
      "declare i32 @" +
          std::string(kObjc3RuntimeActorHopToExecutorI32Symbol) +
          "(i32, i32)\n");
  EmitObjc3IRDeclarationOnce(
      declared_symbols, emitted, out,
      kObjc3RuntimeActorRecordReplayProofI32Symbol,
      "declare i32 @" +
          std::string(kObjc3RuntimeActorRecordReplayProofI32Symbol) + "(i32)\n");
  EmitObjc3IRDeclarationOnce(
      declared_symbols, emitted, out, kObjc3RuntimeActorRecordRaceGuardI32Symbol,
      "declare i32 @" +
          std::string(kObjc3RuntimeActorRecordRaceGuardI32Symbol) + "(i32)\n");
  EmitObjc3IRDeclarationOnce(
      declared_symbols, emitted, out, kObjc3RuntimeActorBindExecutorI32Symbol,
      "declare i32 @" + std::string(kObjc3RuntimeActorBindExecutorI32Symbol) +
          "(i32, i32)\n");
  EmitObjc3IRDeclarationOnce(
      declared_symbols, emitted, out, kObjc3RuntimeActorMailboxEnqueueI32Symbol,
      "declare i32 @" +
          std::string(kObjc3RuntimeActorMailboxEnqueueI32Symbol) +
          "(i32, i32, i32)\n");
  EmitObjc3IRDeclarationOnce(
      declared_symbols, emitted, out, kObjc3RuntimeActorMailboxDrainNextI32Symbol,
      "declare i32 @" +
          std::string(kObjc3RuntimeActorMailboxDrainNextI32Symbol) +
          "(i32, i32)\n");
  EmitObjc3IRDeclarationOnce(
      declared_symbols, emitted, out, kObjc3RuntimeLoadWeakCurrentPropertyI32Symbol,
      "declare i32 @" +
          std::string(kObjc3RuntimeLoadWeakCurrentPropertyI32Symbol) + "()\n");
  EmitObjc3IRDeclarationOnce(
      declared_symbols, emitted, out,
      kObjc3RuntimeStoreWeakCurrentPropertyI32Symbol,
      "declare void @" +
          std::string(kObjc3RuntimeStoreWeakCurrentPropertyI32Symbol) +
          "(i32)\n");
  EmitObjc3IRDeclarationOnce(
      declared_symbols, emitted, out, kObjc3RuntimeRetainI32Symbol,
      "declare i32 @" + std::string(kObjc3RuntimeRetainI32Symbol) + "(i32)\n");
  EmitObjc3IRDeclarationOnce(
      declared_symbols, emitted, out, kObjc3RuntimeReleaseI32Symbol,
      "declare i32 @" + std::string(kObjc3RuntimeReleaseI32Symbol) + "(i32)\n");
  EmitObjc3IRDeclarationOnce(
      declared_symbols, emitted, out, kObjc3RuntimeAutoreleaseI32Symbol,
      "declare i32 @" + std::string(kObjc3RuntimeAutoreleaseI32Symbol) +
          "(i32)\n");
  EmitObjc3IRDeclarationOnce(
      declared_symbols, emitted, out, kObjc3RuntimePromoteBlockI32Symbol,
      "declare i32 @" + std::string(kObjc3RuntimePromoteBlockI32Symbol) +
          "(ptr, i64, i32)\n");
  EmitObjc3IRDeclarationOnce(
      declared_symbols, emitted, out, kObjc3RuntimeInvokeBlockI32Symbol,
      "declare i32 @" + std::string(kObjc3RuntimeInvokeBlockI32Symbol) +
          "(i32, i32, i32, i32, i32)\n");
  EmitObjc3IRDeclarationOnce(
      declared_symbols, emitted, out, kObjc3RuntimePushAutoreleasepoolScopeSymbol,
      "declare void @" +
          std::string(kObjc3RuntimePushAutoreleasepoolScopeSymbol) + "()\n");
  EmitObjc3IRDeclarationOnce(
      declared_symbols, emitted, out, kObjc3RuntimePopAutoreleasepoolScopeSymbol,
      "declare void @" +
          std::string(kObjc3RuntimePopAutoreleasepoolScopeSymbol) + "()\n");
  EmitObjc3IRDeclarationOnce(
      declared_symbols, emitted, out,
      kObjc3RuntimeStdlibTextUtf8StorageI32Symbol,
      "declare i32 @" +
          std::string(kObjc3RuntimeStdlibTextUtf8StorageI32Symbol) +
          "(ptr, i32)\n");
  EmitObjc3IRDeclarationOnce(
      declared_symbols, emitted, out, kObjc3RuntimeStdlibCollectionsArray3I32Symbol,
      "declare i32 @" + std::string(kObjc3RuntimeStdlibCollectionsArray3I32Symbol) +
          "(i32, i32, i32, i32)\n");
  EmitObjc3IRDeclarationOnce(
      declared_symbols, emitted, out,
      kObjc3RuntimeStdlibCollectionsArrayStorageI32Symbol,
      "declare i32 @" +
          std::string(kObjc3RuntimeStdlibCollectionsArrayStorageI32Symbol) +
          "(ptr, i32)\n");
  EmitObjc3IRDeclarationOnce(
      declared_symbols, emitted, out,
      kObjc3RuntimeStdlibCollectionsMutableArrayI32Symbol,
      "declare i32 @" +
          std::string(kObjc3RuntimeStdlibCollectionsMutableArrayI32Symbol) +
          "()\n");
  EmitObjc3IRDeclarationOnce(
      declared_symbols, emitted, out,
      kObjc3RuntimeStdlibCollectionsMutableArrayAppendI32Symbol,
      "declare i32 @" +
          std::string(kObjc3RuntimeStdlibCollectionsMutableArrayAppendI32Symbol) +
          "(i32, i32)\n");
  EmitObjc3IRDeclarationOnce(
      declared_symbols, emitted, out,
      kObjc3RuntimeStdlibCollectionsMutableArraySetI32Symbol,
      "declare i32 @" +
          std::string(kObjc3RuntimeStdlibCollectionsMutableArraySetI32Symbol) +
          "(i32, i32, i32)\n");
  EmitObjc3IRDeclarationOnce(
      declared_symbols, emitted, out,
      kObjc3RuntimeStdlibCollectionsMutableArrayRemoveAtI32Symbol,
      "declare i32 @" +
          std::string(kObjc3RuntimeStdlibCollectionsMutableArrayRemoveAtI32Symbol) +
          "(i32, i32)\n");
  EmitObjc3IRDeclarationOnce(
      declared_symbols, emitted, out,
      kObjc3RuntimeStdlibCollectionsArrayGetOrI32Symbol,
      "declare i32 @" +
          std::string(kObjc3RuntimeStdlibCollectionsArrayGetOrI32Symbol) +
          "(i32, i32, i32)\n");
  EmitObjc3IRDeclarationOnce(
      declared_symbols, emitted, out,
      kObjc3RuntimeStdlibCollectionsArrayIteratorI32Symbol,
      "declare i32 @" +
          std::string(kObjc3RuntimeStdlibCollectionsArrayIteratorI32Symbol) +
          "(i32)\n");
  EmitObjc3IRDeclarationOnce(
      declared_symbols, emitted, out, kObjc3RuntimeStdlibCollectionsMapEmptyI32Symbol,
      "declare i32 @" + std::string(kObjc3RuntimeStdlibCollectionsMapEmptyI32Symbol) +
          "()\n");
  EmitObjc3IRDeclarationOnce(
      declared_symbols, emitted, out,
      kObjc3RuntimeStdlibCollectionsMapInsertI32Symbol,
      "declare i32 @" +
          std::string(kObjc3RuntimeStdlibCollectionsMapInsertI32Symbol) +
          "(i32, i32, i32)\n");
  EmitObjc3IRDeclarationOnce(
      declared_symbols, emitted, out,
      kObjc3RuntimeStdlibCollectionsMapDeleteI32Symbol,
      "declare i32 @" +
          std::string(kObjc3RuntimeStdlibCollectionsMapDeleteI32Symbol) +
          "(i32, i32)\n");
  EmitObjc3IRDeclarationOnce(
      declared_symbols, emitted, out,
      kObjc3RuntimeStdlibCollectionsMapLookupOrI32Symbol,
      "declare i32 @" +
          std::string(kObjc3RuntimeStdlibCollectionsMapLookupOrI32Symbol) +
          "(i32, i32, i32)\n");
  EmitObjc3IRDeclarationOnce(
      declared_symbols, emitted, out,
      kObjc3RuntimeStdlibCollectionsMapKeyIteratorI32Symbol,
      "declare i32 @" +
          std::string(kObjc3RuntimeStdlibCollectionsMapKeyIteratorI32Symbol) +
          "(i32)\n");
  EmitObjc3IRDeclarationOnce(
      declared_symbols, emitted, out,
      kObjc3RuntimeStdlibCollectionsMapValueIteratorI32Symbol,
      "declare i32 @" +
          std::string(kObjc3RuntimeStdlibCollectionsMapValueIteratorI32Symbol) +
          "(i32)\n");
  EmitObjc3IRDeclarationOnce(
      declared_symbols, emitted, out, kObjc3RuntimeStdlibCollectionsSet3I32Symbol,
      "declare i32 @" + std::string(kObjc3RuntimeStdlibCollectionsSet3I32Symbol) +
          "(i32, i32, i32, i32)\n");
  EmitObjc3IRDeclarationOnce(
      declared_symbols, emitted, out,
      kObjc3RuntimeStdlibCollectionsSetStorageI32Symbol,
      "declare i32 @" +
          std::string(kObjc3RuntimeStdlibCollectionsSetStorageI32Symbol) +
          "(ptr, i32)\n");
  EmitObjc3IRDeclarationOnce(
      declared_symbols, emitted, out,
      kObjc3RuntimeStdlibCollectionsSetInsertI32Symbol,
      "declare i32 @" +
          std::string(kObjc3RuntimeStdlibCollectionsSetInsertI32Symbol) +
          "(i32, i32)\n");
  EmitObjc3IRDeclarationOnce(
      declared_symbols, emitted, out,
      kObjc3RuntimeStdlibCollectionsSetDeleteI32Symbol,
      "declare i32 @" +
          std::string(kObjc3RuntimeStdlibCollectionsSetDeleteI32Symbol) +
          "(i32, i32)\n");
  EmitObjc3IRDeclarationOnce(
      declared_symbols, emitted, out,
      kObjc3RuntimeStdlibCollectionsSetIteratorI32Symbol,
      "declare i32 @" +
          std::string(kObjc3RuntimeStdlibCollectionsSetIteratorI32Symbol) +
          "(i32)\n");
  EmitObjc3IRDeclarationOnce(
      declared_symbols, emitted, out,
      kObjc3RuntimeStdlibCollectionsIteratorNextOrI32Symbol,
      "declare i32 @" +
          std::string(kObjc3RuntimeStdlibCollectionsIteratorNextOrI32Symbol) +
          "(i32, i32)\n");
  EmitObjc3IRDeclarationOnce(
      declared_symbols, emitted, out,
      kObjc3RuntimeStdlibCollectionsLastStatusI32Symbol,
      "declare i32 @" +
          std::string(kObjc3RuntimeStdlibCollectionsLastStatusI32Symbol) +
          "()\n");
}
