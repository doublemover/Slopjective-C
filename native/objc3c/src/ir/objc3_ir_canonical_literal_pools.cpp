#include "ir/objc3_ir_canonical_literal_pools.h"

#include <cstddef>
#include <utility>

#include "ast/objc3_ast.h"
#include "ir/objc3_ir_frontend_metadata.h"
#include "ir/objc3_ir_module_identity.h"
#include "ir/objc3_ir_runtime_metadata_emission.h"
#include "support/objc3_string_predicates.h"

namespace {

class Objc3IRCanonicalLiteralPoolCollector {
 public:
  Objc3IRCanonicalLiteralPoolCollector(
      const Objc3Program &program,
      const Objc3IRFrontendMetadata &frontend_metadata)
      : program_(program), frontend_metadata_(frontend_metadata) {}

  Objc3IRCanonicalLiteralPools Collect() {
    for (const auto &global : program_.globals) {
      CollectSelectorExpr(global.value.get());
    }
    for (const auto &fn : program_.functions) {
      for (const auto &stmt : fn.body) {
        CollectSelectorStmt(stmt.get());
      }
    }
    CollectRuntimeMetadataPoolLiterals();
    AssignCanonicalPoolGlobalNames();
    AssignTypedKeyPathArtifactOrdinals();
    return std::move(pools_);
  }

 private:
  void RegisterSelectorLiteral(const std::string &selector) {
    if (selector.empty() ||
        pools_.selector_pool_globals.find(selector) !=
            pools_.selector_pool_globals.end()) {
      return;
    }
    pools_.selector_pool_globals.emplace(selector, "");
  }

  void RegisterRuntimeStringLiteral(const std::string &value) {
    if (value.empty() ||
        pools_.runtime_string_pool_globals.find(value) !=
            pools_.runtime_string_pool_globals.end()) {
      return;
    }
    pools_.runtime_string_pool_globals.emplace(value, "");
  }

  void AssignCanonicalPoolGlobalNames() {
    std::size_t index = 0;
    for (auto &entry : pools_.selector_pool_globals) {
      entry.second = "@__objc3_sel_pool_" +
                     FormatObjc3IRRuntimeMetadataDescriptorOrdinal(index++);
    }
    index = 0;
    for (auto &entry : pools_.runtime_string_pool_globals) {
      entry.second = "@__objc3_str_pool_" +
                     FormatObjc3IRRuntimeMetadataDescriptorOrdinal(index++);
    }
  }

  void RegisterTypedKeyPathLiteral(const Expr &expr) {
    if (!expr.typed_keypath_literal_enabled ||
        !expr.typed_keypath_literal_is_normalized ||
        expr.typed_keypath_components.empty()) {
      return;
    }
    const std::string profile =
        expr.typed_keypath_literal_profile.empty()
            ? std::string("typed-keypath:root=") +
                  expr.typed_keypath_root_name
            : expr.typed_keypath_literal_profile;
    if (pools_.typed_keypath_artifacts.find(profile) !=
        pools_.typed_keypath_artifacts.end()) {
      return;
    }
    TypedKeyPathArtifact artifact;
    artifact.root_is_self = expr.typed_keypath_root_is_self;
    artifact.root_name = expr.typed_keypath_root_name;
    artifact.component_path =
        JoinStringParts(expr.typed_keypath_components, ".");
    artifact.profile = profile;
    pools_.typed_keypath_artifacts.emplace(profile, std::move(artifact));
    RegisterRuntimeStringLiteral(expr.typed_keypath_root_name);
    RegisterRuntimeStringLiteral(
        JoinStringParts(expr.typed_keypath_components, "."));
    RegisterRuntimeStringLiteral(profile);
    if (!frontend_metadata_.lowering_generic_metadata_abi_replay_key.empty()) {
      RegisterRuntimeStringLiteral(
          frontend_metadata_.lowering_generic_metadata_abi_replay_key);
    }
  }

  void AssignTypedKeyPathArtifactOrdinals() {
    std::size_t ordinal = 0;
    for (auto &entry : pools_.typed_keypath_artifacts) {
      entry.second.ordinal = ordinal;
      entry.second.descriptor_symbol =
          "@__objc3_keypath_desc_" +
          FormatObjc3IRRuntimeMetadataDescriptorOrdinal(ordinal);
      ++ordinal;
    }
  }

  void CollectSelectorExpr(const Expr *expr) {
    if (expr == nullptr) {
      return;
    }
    if (expr->typed_keypath_literal_enabled) {
      RegisterTypedKeyPathLiteral(*expr);
    }
    switch (expr->kind) {
      case Expr::Kind::MessageSend:
        RegisterSelectorLiteral(expr->selector);
        CollectSelectorExpr(expr->receiver.get());
        for (const auto &arg : expr->args) {
          CollectSelectorExpr(arg.get());
        }
        return;
      case Expr::Kind::Binary:
        CollectSelectorExpr(expr->left.get());
        CollectSelectorExpr(expr->right.get());
        return;
      case Expr::Kind::Conditional:
        CollectSelectorExpr(expr->left.get());
        CollectSelectorExpr(expr->right.get());
        CollectSelectorExpr(expr->third.get());
        return;
      case Expr::Kind::CollectionLiteral:
        for (const auto &key : expr->collection_keys) {
          CollectSelectorExpr(key.get());
        }
        for (const auto &value : expr->collection_values) {
          CollectSelectorExpr(value.get());
        }
        return;
      case Expr::Kind::IndexAccess:
        CollectSelectorExpr(expr->left.get());
        CollectSelectorExpr(expr->right.get());
        return;
      case Expr::Kind::Call:
      case Expr::Kind::Try:
      case Expr::Kind::Throw:
        for (const auto &arg : expr->args) {
          CollectSelectorExpr(arg.get());
        }
        return;
      case Expr::Kind::KeyPathLiteral:
        return;
      default:
        return;
    }
  }

  void CollectSelectorStmt(const Stmt *stmt) {
    if (stmt == nullptr) {
      return;
    }
    switch (stmt->kind) {
      case Stmt::Kind::Let:
        if (stmt->let_stmt != nullptr) {
          CollectSelectorExpr(stmt->let_stmt->value.get());
        }
        return;
      case Stmt::Kind::Assign:
        if (stmt->assign_stmt != nullptr) {
          CollectSelectorExpr(stmt->assign_stmt->value.get());
        }
        return;
      case Stmt::Kind::CollectionMutation:
        if (stmt->collection_mutation_stmt != nullptr) {
          CollectSelectorExpr(
              stmt->collection_mutation_stmt->key_or_index.get());
          CollectSelectorExpr(stmt->collection_mutation_stmt->value.get());
        }
        return;
      case Stmt::Kind::Return:
        if (stmt->return_stmt != nullptr) {
          CollectSelectorExpr(stmt->return_stmt->value.get());
        }
        return;
      case Stmt::Kind::Expr:
        if (stmt->expr_stmt != nullptr) {
          CollectSelectorExpr(stmt->expr_stmt->value.get());
        }
        return;
      case Stmt::Kind::If:
        if (stmt->if_stmt == nullptr) {
          return;
        }
        CollectSelectorExpr(stmt->if_stmt->condition.get());
        for (const auto &then_stmt : stmt->if_stmt->then_body) {
          CollectSelectorStmt(then_stmt.get());
        }
        for (const auto &else_stmt : stmt->if_stmt->else_body) {
          CollectSelectorStmt(else_stmt.get());
        }
        return;
      case Stmt::Kind::DoWhile:
        if (stmt->do_while_stmt == nullptr) {
          return;
        }
        for (const auto &loop_stmt : stmt->do_while_stmt->body) {
          CollectSelectorStmt(loop_stmt.get());
        }
        CollectSelectorExpr(stmt->do_while_stmt->condition.get());
        return;
      case Stmt::Kind::For:
        if (stmt->for_stmt == nullptr) {
          return;
        }
        CollectSelectorExpr(stmt->for_stmt->init.value.get());
        CollectSelectorExpr(stmt->for_stmt->condition.get());
        CollectSelectorExpr(stmt->for_stmt->step.value.get());
        for (const auto &loop_stmt : stmt->for_stmt->body) {
          CollectSelectorStmt(loop_stmt.get());
        }
        return;
      case Stmt::Kind::ForIn:
        if (stmt->for_in_stmt == nullptr) {
          return;
        }
        CollectSelectorExpr(stmt->for_in_stmt->collection.get());
        for (const auto &loop_stmt : stmt->for_in_stmt->body) {
          CollectSelectorStmt(loop_stmt.get());
        }
        return;
      case Stmt::Kind::Switch:
        if (stmt->switch_stmt == nullptr) {
          return;
        }
        CollectSelectorExpr(stmt->switch_stmt->condition.get());
        for (const auto &case_stmt : stmt->switch_stmt->cases) {
          for (const auto &case_body_stmt : case_stmt.body) {
            CollectSelectorStmt(case_body_stmt.get());
          }
        }
        return;
      case Stmt::Kind::While:
        if (stmt->while_stmt == nullptr) {
          return;
        }
        CollectSelectorExpr(stmt->while_stmt->condition.get());
        for (const auto &loop_stmt : stmt->while_stmt->body) {
          CollectSelectorStmt(loop_stmt.get());
        }
        return;
      case Stmt::Kind::Block:
      case Stmt::Kind::Defer:
        if (stmt->block_stmt == nullptr) {
          return;
        }
        for (const auto &nested_stmt : stmt->block_stmt->body) {
          CollectSelectorStmt(nested_stmt.get());
        }
        return;
      case Stmt::Kind::Break:
      case Stmt::Kind::Continue:
      case Stmt::Kind::Empty:
        return;
    }
  }

  void CollectRuntimeMetadataPoolLiterals() {
    for (const auto &bundle :
         frontend_metadata_
             .runtime_metadata_class_metaclass_bundles_lexicographic) {
      RegisterRuntimeStringLiteral(bundle.class_name);
      RegisterRuntimeStringLiteral(bundle.owner_identity);
      if (bundle.has_super &&
          objc3c::support::StartsWith(bundle.super_class_owner_identity,
                                      "class:")) {
        RegisterRuntimeStringLiteral(
            bundle.super_class_owner_identity.substr(6));
      }
    }
    for (const auto &bundle :
         frontend_metadata_.runtime_metadata_protocol_bundles_lexicographic) {
      RegisterRuntimeStringLiteral(bundle.protocol_name);
      RegisterRuntimeStringLiteral(bundle.owner_identity);
    }
    for (const auto &bundle :
         frontend_metadata_.runtime_metadata_category_bundles_lexicographic) {
      RegisterRuntimeStringLiteral(bundle.class_name);
      RegisterRuntimeStringLiteral(bundle.category_name);
      RegisterRuntimeStringLiteral(bundle.owner_identity);
      RegisterRuntimeStringLiteral(bundle.record_kind);
      RegisterRuntimeStringLiteral(bundle.category_owner_identity);
      RegisterRuntimeStringLiteral(bundle.class_owner_identity);
    }
    for (const auto &bundle :
         frontend_metadata_
             .runtime_metadata_method_list_bundles_lexicographic) {
      RegisterRuntimeStringLiteral(bundle.declaration_owner_identity);
      RegisterRuntimeStringLiteral(bundle.export_owner_identity);
      for (const auto &entry : bundle.entries_lexicographic) {
        RegisterSelectorLiteral(entry.selector);
        RegisterRuntimeStringLiteral(entry.owner_identity);
        RegisterRuntimeStringLiteral(entry.return_type_name);
      }
    }
    for (const auto &bundle :
         frontend_metadata_.runtime_metadata_property_bundles_lexicographic) {
      RegisterRuntimeStringLiteral(bundle.property_name);
      RegisterRuntimeStringLiteral(bundle.type_name);
      RegisterRuntimeStringLiteral(bundle.owner_identity);
      RegisterRuntimeStringLiteral(bundle.declaration_owner_identity);
      RegisterRuntimeStringLiteral(bundle.export_owner_identity);
      if (bundle.has_getter) {
        RegisterSelectorLiteral(bundle.getter_selector);
      }
      if (bundle.has_setter) {
        RegisterSelectorLiteral(bundle.setter_selector);
      }
      if (!bundle.ivar_binding_symbol.empty()) {
        RegisterRuntimeStringLiteral(bundle.ivar_binding_symbol);
      }
    }
    for (const auto &bundle :
         frontend_metadata_.runtime_metadata_ivar_bundles_lexicographic) {
      RegisterRuntimeStringLiteral(bundle.owner_identity);
      RegisterRuntimeStringLiteral(bundle.declaration_owner_identity);
      RegisterRuntimeStringLiteral(bundle.export_owner_identity);
      RegisterRuntimeStringLiteral(bundle.property_owner_identity);
      RegisterRuntimeStringLiteral(bundle.property_name);
      RegisterRuntimeStringLiteral(bundle.ivar_binding_symbol);
    }
  }

  const Objc3Program &program_;
  const Objc3IRFrontendMetadata &frontend_metadata_;
  Objc3IRCanonicalLiteralPools pools_;
};

}  // namespace

Objc3IRCanonicalLiteralPools BuildObjc3IRCanonicalLiteralPools(
    const Objc3Program &program,
    const Objc3IRFrontendMetadata &frontend_metadata) {
  Objc3IRCanonicalLiteralPoolCollector collector(program, frontend_metadata);
  return collector.Collect();
}
