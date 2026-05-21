#include "ir/objc3_ir_expression_emission.h"

#include <cstddef>
#include <string>

#include "ast/objc3_ast.h"
#include "ir/objc3_ir_block_runtime_contracts.h"
#include "ir/objc3_ir_expression_emission_call.h"

namespace {

constexpr const char *kObjc3RuntimeStdlibTextUtf8StorageI32Symbol =
    "objc3_runtime_stdlib_text_utf8_storage_i32";
constexpr const char *kObjc3RuntimeCollectionsArray3I32Symbol =
    "objc3_runtime_stdlib_collections_array3_i32";
constexpr const char *kObjc3RuntimeCollectionsArrayStorageI32Symbol =
    "objc3_runtime_stdlib_collections_array_storage_i32";
constexpr const char *kObjc3RuntimeCollectionsMutableArrayI32Symbol =
    "objc3_runtime_stdlib_collections_mutable_array_i32";
constexpr const char *kObjc3RuntimeCollectionsMutableArrayAppendI32Symbol =
    "objc3_runtime_stdlib_collections_mutable_array_append_i32";
constexpr const char *kObjc3RuntimeCollectionsArrayGetOrI32Symbol =
    "objc3_runtime_stdlib_collections_array_get_or_i32";
constexpr const char *kObjc3RuntimeCollectionsMapEmptyI32Symbol =
    "objc3_runtime_stdlib_collections_map_empty_i32";
constexpr const char *kObjc3RuntimeCollectionsMapInsertI32Symbol =
    "objc3_runtime_stdlib_collections_map_insert_i32";
constexpr const char *kObjc3RuntimeCollectionsMapLookupOrI32Symbol =
    "objc3_runtime_stdlib_collections_map_lookup_or_i32";
constexpr const char *kObjc3RuntimeCollectionsSet3I32Symbol =
    "objc3_runtime_stdlib_collections_set3_i32";
constexpr const char *kObjc3RuntimeCollectionsSetStorageI32Symbol =
    "objc3_runtime_stdlib_collections_set_storage_i32";

std::string EmitObjc3IRExprImpl(
    const Expr *expr, FunctionContext &ctx,
    const Objc3IRExpressionEmissionCallbacks &callbacks);

std::string LookupObjc3IRLocalPtr(const FunctionContext &ctx,
                                  const std::string &name) {
  for (auto it = ctx.scopes.rbegin(); it != ctx.scopes.rend(); ++it) {
    const auto found = it->find(name);
    if (found != it->end()) {
      return found->second;
    }
  }
  return "";
}

Expr::CollectionLiteralKind CollectionKindForExpr(const Expr *expr,
                                                  const FunctionContext &ctx) {
  if (expr == nullptr) {
    return Expr::CollectionLiteralKind::None;
  }
  if (expr->kind == Expr::Kind::CollectionLiteral) {
    return expr->collection_literal_kind;
  }
  if (expr->kind == Expr::Kind::Identifier) {
    const std::string ptr = LookupObjc3IRLocalPtr(ctx, expr->ident);
    const auto found = ctx.collection_kind_by_ptr.find(ptr);
    if (found != ctx.collection_kind_by_ptr.end()) {
      return found->second;
    }
  }
  return Expr::CollectionLiteralKind::None;
}

std::string EmitObjc3IRTextStorageLiteral(
    const Expr &expr, FunctionContext &ctx,
    const Objc3IRExpressionEmissionCallbacks &callbacks) {
  const std::string tmp = callbacks.new_temp(ctx);
  const std::string byte_count =
      std::to_string(expr.string_literal_byte_count);
  if (expr.string_literal_value.empty()) {
    ctx.code_lines.push_back(
        "  " + tmp + " = call i32 @" +
        std::string(kObjc3RuntimeStdlibTextUtf8StorageI32Symbol) +
        "(ptr null, i32 0)");
    return tmp;
  }

  const std::string storage =
      "%text.literal.bytes." + std::to_string(ctx.temp_counter++);
  const std::string base =
      "%text.literal.ptr." + std::to_string(ctx.temp_counter++);
  ctx.entry_lines.push_back("  " + storage + " = alloca [" + byte_count +
                            " x i8], align 1");
  ctx.code_lines.push_back("  " + base + " = getelementptr inbounds [" +
                           byte_count + " x i8], ptr " + storage +
                           ", i32 0, i32 0");
  for (std::size_t index = 0; index < expr.string_literal_value.size();
       ++index) {
    const std::string byte_ptr =
        "%text.literal.byte." + std::to_string(ctx.temp_counter++);
    const int byte_value = static_cast<int>(
        static_cast<unsigned char>(expr.string_literal_value[index]));
    ctx.code_lines.push_back("  " + byte_ptr +
                             " = getelementptr inbounds i8, ptr " + base +
                             ", i32 " + std::to_string(index));
    ctx.code_lines.push_back("  store i8 " + std::to_string(byte_value) +
                             ", ptr " + byte_ptr + ", align 1");
  }
  ctx.code_lines.push_back(
      "  " + tmp + " = call i32 @" +
      std::string(kObjc3RuntimeStdlibTextUtf8StorageI32Symbol) + "(ptr " +
      base + ", i32 " + byte_count + ")");
  return tmp;
}

std::string EmitObjc3IRValuesStorageCall(
    const std::vector<std::unique_ptr<Expr>> &values, const std::string &symbol,
    FunctionContext &ctx, const Objc3IRExpressionEmissionCallbacks &callbacks) {
  const std::string tmp = callbacks.new_temp(ctx);
  if (values.empty()) {
    ctx.code_lines.push_back("  " + tmp + " = call i32 @" + symbol +
                             "(ptr null, i32 0)");
    return tmp;
  }
  const std::string count = std::to_string(values.size());
  const std::string storage =
      "%collection.literal.values." + std::to_string(ctx.temp_counter++);
  const std::string base =
      "%collection.literal.ptr." + std::to_string(ctx.temp_counter++);
  ctx.entry_lines.push_back("  " + storage + " = alloca [" + count +
                            " x i32], align 4");
  ctx.code_lines.push_back("  " + base + " = getelementptr inbounds [" +
                           count + " x i32], ptr " + storage +
                           ", i32 0, i32 0");
  for (std::size_t index = 0; index < values.size(); ++index) {
    const std::string value =
        EmitObjc3IRExprImpl(values[index].get(), ctx, callbacks);
    const std::string slot =
        "%collection.literal.slot." + std::to_string(ctx.temp_counter++);
    ctx.code_lines.push_back("  " + slot +
                             " = getelementptr inbounds i32, ptr " + base +
                             ", i32 " + std::to_string(index));
    ctx.code_lines.push_back("  store i32 " + value + ", ptr " + slot +
                             ", align 4");
  }
  ctx.code_lines.push_back("  " + tmp + " = call i32 @" + symbol +
                           "(ptr " + base + ", i32 " + count + ")");
  return tmp;
}

std::string EmitObjc3IRCollectionLiteral(
    const Expr &expr, FunctionContext &ctx,
    const Objc3IRExpressionEmissionCallbacks &callbacks) {
  if (expr.collection_literal_kind == Expr::CollectionLiteralKind::Array) {
    if (expr.collection_literal_mutable) {
      const std::string handle = callbacks.new_temp(ctx);
      ctx.code_lines.push_back("  " + handle + " = call i32 @" +
                               std::string(
                                   kObjc3RuntimeCollectionsMutableArrayI32Symbol) +
                               "()");
      for (const auto &value_expr : expr.collection_values) {
        const std::string value =
            EmitObjc3IRExprImpl(value_expr.get(), ctx, callbacks);
        const std::string ignored = callbacks.new_temp(ctx);
        ctx.code_lines.push_back("  " + ignored + " = call i32 @" +
                                 std::string(
                                     kObjc3RuntimeCollectionsMutableArrayAppendI32Symbol) +
                                 "(i32 " + handle + ", i32 " + value + ")");
      }
      return handle;
    }
    if (expr.collection_values.size() <= 3u) {
      std::string inputs[3] = {"0", "0", "0"};
      for (std::size_t index = 0; index < expr.collection_values.size();
           ++index) {
        inputs[index] =
            EmitObjc3IRExprImpl(expr.collection_values[index].get(), ctx,
                                callbacks);
      }
      const std::string tmp = callbacks.new_temp(ctx);
      ctx.code_lines.push_back("  " + tmp + " = call i32 @" +
                               std::string(kObjc3RuntimeCollectionsArray3I32Symbol) +
                               "(i32 " + inputs[0] + ", i32 " + inputs[1] +
                               ", i32 " + inputs[2] + ", i32 " +
                               std::to_string(expr.collection_values.size()) +
                               ")");
      return tmp;
    }
    return EmitObjc3IRValuesStorageCall(
        expr.collection_values, kObjc3RuntimeCollectionsArrayStorageI32Symbol,
        ctx, callbacks);
  }
  if (expr.collection_literal_kind == Expr::CollectionLiteralKind::Set) {
    if (expr.collection_values.size() <= 3u) {
      std::string inputs[3] = {"0", "0", "0"};
      for (std::size_t index = 0; index < expr.collection_values.size();
           ++index) {
        inputs[index] =
            EmitObjc3IRExprImpl(expr.collection_values[index].get(), ctx,
                                callbacks);
      }
      const std::string tmp = callbacks.new_temp(ctx);
      ctx.code_lines.push_back("  " + tmp + " = call i32 @" +
                               std::string(kObjc3RuntimeCollectionsSet3I32Symbol) +
                               "(i32 " + inputs[0] + ", i32 " + inputs[1] +
                               ", i32 " + inputs[2] + ", i32 " +
                               std::to_string(expr.collection_values.size()) +
                               ")");
      return tmp;
    }
    return EmitObjc3IRValuesStorageCall(
        expr.collection_values, kObjc3RuntimeCollectionsSetStorageI32Symbol,
        ctx, callbacks);
  }
  if (expr.collection_literal_kind == Expr::CollectionLiteralKind::Map) {
    const std::string handle = callbacks.new_temp(ctx);
    ctx.code_lines.push_back("  " + handle + " = call i32 @" +
                             std::string(kObjc3RuntimeCollectionsMapEmptyI32Symbol) +
                             "()");
    for (std::size_t index = 0; index < expr.collection_values.size();
         ++index) {
      const std::string key =
          EmitObjc3IRExprImpl(expr.collection_keys[index].get(), ctx,
                              callbacks);
      const std::string value =
          EmitObjc3IRExprImpl(expr.collection_values[index].get(), ctx,
                              callbacks);
      const std::string ignored = callbacks.new_temp(ctx);
      ctx.code_lines.push_back("  " + ignored + " = call i32 @" +
                               std::string(kObjc3RuntimeCollectionsMapInsertI32Symbol) +
                               "(i32 " + handle + ", i32 " + key +
                               ", i32 " + value + ")");
    }
    return handle;
  }
  return callbacks.emit_unsupported_i32_value(
      "unsupported collection literal kind reached IR lowering");
}

std::string EmitObjc3IRExprImpl(
    const Expr *expr, FunctionContext &ctx,
    const Objc3IRExpressionEmissionCallbacks &callbacks) {
  if (expr == nullptr) {
    return callbacks.emit_unsupported_i32_value(
        "null expression reached IR lowering");
  }
  switch (expr->kind) {
    case Expr::Kind::Number:
      return std::to_string(expr->number);
    case Expr::Kind::BoolLiteral:
      return expr->bool_value ? "1" : "0";
    case Expr::Kind::NilLiteral:
      return "0";
    case Expr::Kind::StringLiteral: {
      return EmitObjc3IRTextStorageLiteral(*expr, ctx, callbacks);
    }
    case Expr::Kind::CollectionLiteral:
      return EmitObjc3IRCollectionLiteral(*expr, ctx, callbacks);
    case Expr::Kind::BlockLiteral:
      if (BlockLiteralSupportsEscapingRuntimeHookLowering(*expr)) {
        const std::string storage_ptr =
            callbacks.emit_block_literal_storage(*expr, ctx);
        if (storage_ptr == "poison") {
          return storage_ptr;
        }
        return callbacks.emit_promoted_block_handle(*expr, storage_ptr, ctx);
      }
      return callbacks.emit_unsupported_i32_value(
          "block literal values must be bound to a local name before use");
    case Expr::Kind::Identifier: {
      return callbacks.emit_identifier_value(expr->ident, ctx);
    }
    case Expr::Kind::IndexAccess: {
      const Expr::CollectionLiteralKind kind =
          CollectionKindForExpr(expr->left.get(), ctx);
      const std::string collection =
          EmitObjc3IRExprImpl(expr->left.get(), ctx, callbacks);
      const std::string index =
          EmitObjc3IRExprImpl(expr->right.get(), ctx, callbacks);
      const std::string tmp = callbacks.new_temp(ctx);
      if (kind == Expr::CollectionLiteralKind::Array) {
        ctx.code_lines.push_back("  " + tmp + " = call i32 @" +
                                 std::string(
                                     kObjc3RuntimeCollectionsArrayGetOrI32Symbol) +
                                 "(i32 " + collection + ", i32 " + index +
                                 ", i32 0)");
        return tmp;
      }
      if (kind == Expr::CollectionLiteralKind::Map) {
        ctx.code_lines.push_back("  " + tmp + " = call i32 @" +
                                 std::string(
                                     kObjc3RuntimeCollectionsMapLookupOrI32Symbol) +
                                 "(i32 " + collection + ", i32 " + index +
                                 ", i32 0)");
        return tmp;
      }
      return callbacks.emit_unsupported_i32_value(
          "collection index access requires an array or map handle with parser-visible origin");
    }
    case Expr::Kind::KeyPathLiteral:
      return callbacks.emit_typed_keypath_literal_value(*expr);
    case Expr::Kind::Binary: {
      if (expr->op == "&&" || expr->op == "||") {
        const std::string lhs = EmitObjc3IRExprImpl(expr->left.get(), ctx,
                                                    callbacks);
        const std::string lhs_i1 = callbacks.new_temp(ctx);
        const std::string rhs_label = callbacks.new_label(
            ctx, expr->op == "&&" ? "and_rhs_" : "or_rhs_");
        const std::string rhs_done_label = callbacks.new_label(
            ctx, expr->op == "&&" ? "and_rhs_done_" : "or_rhs_done_");
        const std::string short_label = callbacks.new_label(
            ctx, expr->op == "&&" ? "and_short_" : "or_short_");
        const std::string merge_label = callbacks.new_label(
            ctx, expr->op == "&&" ? "and_merge_" : "or_merge_");
        const std::string rhs_i1 = callbacks.new_temp(ctx);
        const std::string logical_i1 = callbacks.new_temp(ctx);
        const std::string out_i32 = callbacks.new_temp(ctx);
        const std::string short_value = expr->op == "&&" ? "0" : "1";

        ctx.code_lines.push_back("  " + lhs_i1 + " = icmp ne i32 " + lhs +
                                 ", 0");
        if (expr->op == "&&") {
          ctx.code_lines.push_back("  br i1 " + lhs_i1 + ", label %" +
                                   rhs_label + ", label %" + short_label);
        } else {
          ctx.code_lines.push_back("  br i1 " + lhs_i1 + ", label %" +
                                   short_label + ", label %" + rhs_label);
        }

        ctx.code_lines.push_back(rhs_label + ":");
        const std::string rhs = EmitObjc3IRExprImpl(expr->right.get(), ctx,
                                                    callbacks);
        ctx.code_lines.push_back("  br label %" + rhs_done_label);
        ctx.code_lines.push_back(rhs_done_label + ":");
        ctx.code_lines.push_back("  " + rhs_i1 + " = icmp ne i32 " + rhs +
                                 ", 0");
        ctx.code_lines.push_back("  br label %" + merge_label);

        ctx.code_lines.push_back(short_label + ":");
        ctx.code_lines.push_back("  br label %" + merge_label);

        ctx.code_lines.push_back(merge_label + ":");
        ctx.code_lines.push_back("  " + logical_i1 + " = phi i1 [" +
                                 short_value + ", %" + short_label + "], [" +
                                 rhs_i1 + ", %" + rhs_done_label + "]");
        ctx.code_lines.push_back("  " + out_i32 + " = zext i1 " +
                                 logical_i1 + " to i32");
        return out_i32;
      }
      if (expr->op == "??") {
        const std::string lhs =
            EmitObjc3IRExprImpl(expr->left.get(), ctx, callbacks);
        const std::string lhs_i1 = callbacks.new_temp(ctx);
        const std::string rhs_label =
            callbacks.new_label(ctx, "coalesce_rhs_");
        const std::string lhs_label =
            callbacks.new_label(ctx, "coalesce_lhs_");
        const std::string merge_label =
            callbacks.new_label(ctx, "coalesce_merge_");
        const std::string rhs_value_name = callbacks.new_temp(ctx);
        const std::string out_value = callbacks.new_temp(ctx);
        ctx.code_lines.push_back("  " + lhs_i1 + " = icmp ne i32 " + lhs +
                                 ", 0");
        ctx.code_lines.push_back("  br i1 " + lhs_i1 + ", label %" +
                                 lhs_label + ", label %" + rhs_label);
        ctx.code_lines.push_back(rhs_label + ":");
        const std::string rhs =
            EmitObjc3IRExprImpl(expr->right.get(), ctx, callbacks);
        ctx.code_lines.push_back("  " + rhs_value_name + " = add i32 " +
                                 rhs + ", 0");
        ctx.code_lines.push_back("  br label %" + merge_label);
        ctx.code_lines.push_back(lhs_label + ":");
        ctx.code_lines.push_back("  br label %" + merge_label);
        ctx.code_lines.push_back(merge_label + ":");
        ctx.code_lines.push_back("  " + out_value + " = phi i32 [" + lhs +
                                 ", %" + lhs_label + "], [" +
                                 rhs_value_name + ", %" + rhs_label + "]");
        return out_value;
      }

      const std::string lhs =
          EmitObjc3IRExprImpl(expr->left.get(), ctx, callbacks);
      const std::string rhs =
          EmitObjc3IRExprImpl(expr->right.get(), ctx, callbacks);
      if (expr->op == "+" || expr->op == "-" || expr->op == "*" ||
          expr->op == "/" || expr->op == "%") {
        const std::string tmp = callbacks.new_temp(ctx);
        std::string op = "add";
        if (expr->op == "+") {
          op = "add";
        } else if (expr->op == "-") {
          op = "sub";
        } else if (expr->op == "*") {
          op = "mul";
        } else if (expr->op == "/") {
          op = "sdiv";
        } else if (expr->op == "%") {
          op = "srem";
        }
        ctx.code_lines.push_back("  " + tmp + " = " + op + " i32 " + lhs +
                                 ", " + rhs);
        return tmp;
      }

      if (expr->op == "&" || expr->op == "|" || expr->op == "^" ||
          expr->op == "<<" || expr->op == ">>") {
        const std::string tmp = callbacks.new_temp(ctx);
        std::string op = "and";
        if (expr->op == "&") {
          op = "and";
        } else if (expr->op == "|") {
          op = "or";
        } else if (expr->op == "^") {
          op = "xor";
        } else if (expr->op == "<<") {
          op = "shl";
        } else if (expr->op == ">>") {
          op = "ashr";
        }
        ctx.code_lines.push_back("  " + tmp + " = " + op + " i32 " + lhs +
                                 ", " + rhs);
        return tmp;
      }

      std::string pred;
      if (expr->op == "==") {
        pred = "eq";
      } else if (expr->op == "!=") {
        pred = "ne";
      } else if (expr->op == "<") {
        pred = "slt";
      } else if (expr->op == "<=") {
        pred = "sle";
      } else if (expr->op == ">") {
        pred = "sgt";
      } else if (expr->op == ">=") {
        pred = "sge";
      } else {
        return callbacks.emit_unsupported_i32_value(
            "unsupported binary operator '" + expr->op + "'");
      }
      const std::string cmp_i1 = callbacks.new_temp(ctx);
      const std::string out_i32 = callbacks.new_temp(ctx);
      ctx.code_lines.push_back("  " + cmp_i1 + " = icmp " + pred +
                               " i32 " + lhs + ", " + rhs);
      ctx.code_lines.push_back("  " + out_i32 + " = zext i1 " + cmp_i1 +
                               " to i32");
      return out_i32;
    }
    case Expr::Kind::Conditional: {
      const std::string cond_value =
          EmitObjc3IRExprImpl(expr->left.get(), ctx, callbacks);
      const std::string cond_i1 = callbacks.new_temp(ctx);
      const std::string true_label = callbacks.new_label(ctx, "cond_true_");
      const std::string false_label = callbacks.new_label(ctx, "cond_false_");
      const std::string merge_label = callbacks.new_label(ctx, "cond_merge_");
      const std::string result_ptr =
          "%cond.addr." + std::to_string(ctx.temp_counter++);
      ctx.entry_lines.push_back("  " + result_ptr + " = alloca i32, align 4");
      ctx.code_lines.push_back("  " + cond_i1 + " = icmp ne i32 " +
                               cond_value + ", 0");
      ctx.code_lines.push_back("  br i1 " + cond_i1 + ", label %" +
                               true_label + ", label %" + false_label);

      ctx.code_lines.push_back(true_label + ":");
      const std::string true_value =
          EmitObjc3IRExprImpl(expr->right.get(), ctx, callbacks);
      ctx.code_lines.push_back("  store i32 " + true_value + ", ptr " +
                               result_ptr + ", align 4");
      ctx.code_lines.push_back("  br label %" + merge_label);

      ctx.code_lines.push_back(false_label + ":");
      const std::string false_value =
          EmitObjc3IRExprImpl(expr->third.get(), ctx, callbacks);
      ctx.code_lines.push_back("  store i32 " + false_value + ", ptr " +
                               result_ptr + ", align 4");
      ctx.code_lines.push_back("  br label %" + merge_label);

      ctx.code_lines.push_back(merge_label + ":");
      const std::string out_value = callbacks.new_temp(ctx);
      ctx.code_lines.push_back("  " + out_value + " = load i32, ptr " +
                               result_ptr + ", align 4");
      return out_value;
    }
    case Expr::Kind::Call: {
      return EmitObjc3IRCallExpression(expr, ctx, callbacks,
                                       EmitObjc3IRExprImpl);
    }
    case Expr::Kind::Try:
    case Expr::Kind::Throw:
      return EmitObjc3IRCallExpression(expr, ctx, callbacks,
                                       EmitObjc3IRExprImpl);
    case Expr::Kind::MessageSend: {
      return callbacks.emit_message_send_expr(expr, ctx);
    }
  }
  return callbacks.emit_unsupported_i32_value(
      "unsupported expression kind reached IR lowering");
}

}  // namespace

std::string EmitObjc3IRExpr(
    const Expr *expr, FunctionContext &ctx,
    const Objc3IRExpressionEmissionCallbacks &callbacks) {
  return EmitObjc3IRExprImpl(expr, ctx, callbacks);
}
