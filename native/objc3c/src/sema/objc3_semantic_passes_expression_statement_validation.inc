      }
      if (AreObjCReferenceTypesAssignmentCompatible(then_type, else_type)) {
        if (then_type.type == else_type.type) {
          return then_type;
        }
        return MakeScalarSemanticType(ValueType::ObjCId);
      }
      if (!IsSameSemanticType(then_type, else_type)) {
        diagnostics.push_back(MakeDiag(expr->line, expr->column, "O3S206",
                                       "type mismatch: conditional branches must be type-compatible"));
      }
      return IsSameSemanticType(then_type, else_type) ? then_type : MakeScalarSemanticType(ValueType::Unknown);
    }
#include "sema/objc3_semantic_passes_expression_statement_validation_match_expression_case.inc"
#include "sema/objc3_semantic_passes_expression_statement_validation_block_literal_case.inc"
#include "sema/objc3_semantic_passes_expression_statement_validation_callable_checks.inc"
    case Expr::Kind::CollectionLiteral: {
      bool literal_type_valid = true;
      for (const auto &key : expr->collection_keys) {
        const SemanticTypeInfo key_type =
            ValidateExpr(key.get(), scopes, globals, functions, diagnostics,
                         max_message_send_args, message_send_context,
                         allow_throwing_call);
        if (!IsUnknownSemanticType(key_type) &&
            !IsObjc3CollectionLiteralRuntimeI32Type(key_type)) {
          diagnostics.push_back(MakeDiag(
              key->line, key->column, "O3S206",
              "type mismatch: collection literal keys are currently limited to i32 runtime descriptors"));
          literal_type_valid = false;
        }
      }
      for (const auto &value : expr->collection_values) {
        const SemanticTypeInfo value_type =
            ValidateExpr(value.get(), scopes, globals, functions, diagnostics,
                         max_message_send_args, message_send_context,
                         allow_throwing_call);
        if (!IsUnknownSemanticType(value_type) &&
            !IsObjc3CollectionLiteralRuntimeI32Type(value_type)) {
          diagnostics.push_back(MakeDiag(
              value->line, value->column, "O3S206",
              "type mismatch: collection literal values are currently limited to i32 runtime descriptors"));
          literal_type_valid = false;
        }
      }
      if (!literal_type_valid ||
          expr->collection_literal_kind == Expr::CollectionLiteralKind::None) {
        return MakeScalarSemanticType(ValueType::Unknown);
      }
      const char *type_name =
          Objc3MutableCollectionTypeNameForLiteral(expr->collection_literal_kind);
      if (expr->collection_literal_kind == Expr::CollectionLiteralKind::Map) {
        return MakeGenericCollectionSemanticType(type_name, {"i32", "i32"});
      }
      return MakeGenericCollectionSemanticType(type_name, {"i32"});
    }
    case Expr::Kind::IndexAccess: {
      const SemanticTypeInfo collection_type =
          ValidateExpr(expr->left.get(), scopes, globals, functions,
                       diagnostics, max_message_send_args, message_send_context,
                       allow_throwing_call);
      const SemanticTypeInfo index_type =
          ValidateExpr(expr->right.get(), scopes, globals, functions,
                       diagnostics, max_message_send_args, message_send_context,
                       allow_throwing_call);
      if (collection_type.is_generic_collection &&
          IsReadyObjc3GenericCollectionTypeModel(
              collection_type.generic_collection_model)) {
        const Objc3GenericCollectionTypeModel &model =
            collection_type.generic_collection_model;
        if (IsObjc3CollectionArrayLikeKind(model.kind)) {
          if (!IsUnknownSemanticType(index_type) &&
              !IsObjc3CollectionLiteralRuntimeI32Type(index_type)) {
            diagnostics.push_back(MakeDiag(
                expr->right->line, expr->right->column, "O3S206",
                "type mismatch: array index must be i32"));
          }
          return MakeSemanticTypeFromObjc3CollectionIdentity(
              model.element_type_identity);
        }
        if (IsObjc3CollectionMapKind(model.kind)) {
          if (!IsUnknownSemanticType(index_type) &&
              !IsObjc3CollectionValueCompatibleWithIdentity(
                  model.key_type_identity, index_type)) {
            diagnostics.push_back(MakeDiag(
                expr->right->line, expr->right->column, "O3S206",
                "type mismatch: map index must match key type '" +
                    model.key_type_identity + "'"));
          }
          return MakeSemanticTypeFromObjc3CollectionIdentity(
              model.value_type_identity);
        }
        if (IsObjc3CollectionSetKind(model.kind)) {
          diagnostics.push_back(MakeDiag(
              expr->line, expr->column, "O3S206",
              "type mismatch: set values are not index-addressable"));
        }
      }
      return MakeScalarSemanticType(ValueType::Unknown);
    }
    case Expr::Kind::MessageSend: {
      return ValidateMessageSendExpr(expr, scopes, globals, functions,
                                     diagnostics, max_message_send_args,
                                     message_send_context);
    }
  }
  return MakeScalarSemanticType(ValueType::Unknown);
}

#include "sema/objc3_semantic_passes_expression_statement_validation_message_send.inc"

#include "sema/objc3_semantic_passes_expression_statement_validation_statement_iteration.inc"

#include "sema/objc3_semantic_passes_expression_statement_validation_assignment.inc"

#include "sema/objc3_semantic_passes_expression_statement_validation_atomic_mappings.inc"

#include "sema/objc3_semantic_passes_expression_statement_validation_statement_flow.inc"

static void ValidateStatement(const Stmt *stmt, std::vector<SemanticScope> &scopes,
                              const std::unordered_map<std::string, ValueType> &globals,
                              const std::unordered_map<std::string, FunctionInfo> &functions,
                              const SemanticTypeInfo &expected_return_type, const std::string &function_name,
                              std::vector<std::string> &diagnostics, int loop_depth, int switch_depth,
                              int defer_loop_base_depth, int defer_switch_base_depth,
                              bool inside_defer_body,
                              std::size_t max_message_send_args,
                              const Objc3MessageSendResolutionContext &message_send_context) {
  if (stmt == nullptr) {
    return;
  }
#include "sema/objc3_semantic_passes_expression_statement_validation_statement_local_helpers.inc"

  switch (stmt->kind) {
#include "sema/objc3_semantic_passes_expression_statement_validation_statement_basic_cases.inc"
#include "sema/objc3_semantic_passes_expression_statement_validation_statement_if_case.inc"
