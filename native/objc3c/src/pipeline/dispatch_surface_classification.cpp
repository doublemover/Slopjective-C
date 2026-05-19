#include "pipeline/dispatch_surface_classification.h"

#include <memory>
#include <string>
#include <unordered_set>
#include <vector>

#include "lower/objc3_lowering_contract.h"

namespace {

const char *DispatchSurfaceFamilySymbol(Expr::DispatchSurfaceKind kind) {
  switch (kind) {
    case Expr::DispatchSurfaceKind::Instance:
      return kObjc3DispatchSurfaceInstanceFamily;
    case Expr::DispatchSurfaceKind::Class:
      return kObjc3DispatchSurfaceClassFamily;
    case Expr::DispatchSurfaceKind::Super:
      return kObjc3DispatchSurfaceSuperFamily;
    case Expr::DispatchSurfaceKind::Direct:
      return kObjc3DispatchSurfaceDirectFamily;
    case Expr::DispatchSurfaceKind::Dynamic:
      return kObjc3DispatchSurfaceDynamicFamily;
    case Expr::DispatchSurfaceKind::Unclassified:
    default:
      return "unclassified";
  }
}

const char *DispatchEntrypointFamilySymbol(Expr::DispatchSurfaceKind kind) {
  return kind == Expr::DispatchSurfaceKind::Direct
             ? kObjc3DispatchSurfaceDirectDispatchBinding
             : kObjc3DispatchSurfaceLiveRuntimeEntrypointFamily;
}

void CollectKnownClassNames(const Objc3Program &program,
                            std::unordered_set<std::string> &class_names) {
  for (const auto &interface_decl : program.interfaces) {
    class_names.insert(interface_decl.name);
  }
  for (const auto &implementation_decl : program.implementations) {
    class_names.insert(implementation_decl.name);
  }
}

Expr::DispatchSurfaceKind ClassifyDispatchSurfaceKind(
    const Expr &receiver,
    const std::unordered_set<std::string> &class_names,
    bool inside_method,
    bool is_class_method) {
  if (receiver.kind == Expr::Kind::NilLiteral) {
    return Expr::DispatchSurfaceKind::Instance;
  }
  if (receiver.kind != Expr::Kind::Identifier) {
    return Expr::DispatchSurfaceKind::Dynamic;
  }
  if (inside_method && receiver.ident == "super") {
    return Expr::DispatchSurfaceKind::Super;
  }
  if (class_names.find(receiver.ident) != class_names.end()) {
    return Expr::DispatchSurfaceKind::Class;
  }
  if (inside_method && receiver.ident == "self") {
    return is_class_method ? Expr::DispatchSurfaceKind::Class
                           : Expr::DispatchSurfaceKind::Instance;
  }
  return Expr::DispatchSurfaceKind::Instance;
}

void NormalizeDispatchSurfaceExpr(Expr *expr,
                                  const std::unordered_set<std::string> &class_names,
                                  bool inside_method,
                                  bool is_class_method);

void NormalizeDispatchSurfaceStatements(
    const std::vector<std::unique_ptr<Stmt>> &statements,
    const std::unordered_set<std::string> &class_names,
    bool inside_method,
    bool is_class_method) {
  for (const auto &stmt : statements) {
    if (stmt == nullptr) {
      continue;
    }
    switch (stmt->kind) {
      case Stmt::Kind::Let:
        if (stmt->let_stmt != nullptr) {
          NormalizeDispatchSurfaceExpr(stmt->let_stmt->value.get(), class_names,
                                       inside_method, is_class_method);
        }
        break;
      case Stmt::Kind::Assign:
        if (stmt->assign_stmt != nullptr) {
          NormalizeDispatchSurfaceExpr(stmt->assign_stmt->value.get(), class_names,
                                       inside_method, is_class_method);
        }
        break;
      case Stmt::Kind::Return:
        if (stmt->return_stmt != nullptr) {
          NormalizeDispatchSurfaceExpr(stmt->return_stmt->value.get(), class_names,
                                       inside_method, is_class_method);
        }
        break;
      case Stmt::Kind::If:
        if (stmt->if_stmt != nullptr) {
          NormalizeDispatchSurfaceExpr(stmt->if_stmt->condition.get(), class_names,
                                       inside_method, is_class_method);
          NormalizeDispatchSurfaceStatements(stmt->if_stmt->then_body, class_names,
                                             inside_method, is_class_method);
          NormalizeDispatchSurfaceStatements(stmt->if_stmt->else_body, class_names,
                                             inside_method, is_class_method);
        }
        break;
      case Stmt::Kind::DoWhile:
        if (stmt->do_while_stmt != nullptr) {
          NormalizeDispatchSurfaceStatements(stmt->do_while_stmt->body, class_names,
                                             inside_method, is_class_method);
          NormalizeDispatchSurfaceExpr(stmt->do_while_stmt->condition.get(), class_names,
                                       inside_method, is_class_method);
        }
        break;
      case Stmt::Kind::For:
        if (stmt->for_stmt != nullptr) {
          NormalizeDispatchSurfaceExpr(stmt->for_stmt->init.value.get(), class_names,
                                       inside_method, is_class_method);
          NormalizeDispatchSurfaceExpr(stmt->for_stmt->condition.get(), class_names,
                                       inside_method, is_class_method);
          NormalizeDispatchSurfaceExpr(stmt->for_stmt->step.value.get(), class_names,
                                       inside_method, is_class_method);
          NormalizeDispatchSurfaceStatements(stmt->for_stmt->body, class_names,
                                             inside_method, is_class_method);
        }
        break;
      case Stmt::Kind::Switch:
        if (stmt->switch_stmt != nullptr) {
          NormalizeDispatchSurfaceExpr(stmt->switch_stmt->condition.get(), class_names,
                                       inside_method, is_class_method);
          for (const auto &case_stmt : stmt->switch_stmt->cases) {
            NormalizeDispatchSurfaceStatements(case_stmt.body, class_names,
                                               inside_method, is_class_method);
          }
        }
        break;
      case Stmt::Kind::While:
        if (stmt->while_stmt != nullptr) {
          NormalizeDispatchSurfaceExpr(stmt->while_stmt->condition.get(), class_names,
                                       inside_method, is_class_method);
          NormalizeDispatchSurfaceStatements(stmt->while_stmt->body, class_names,
                                             inside_method, is_class_method);
        }
        break;
      case Stmt::Kind::Block:
      case Stmt::Kind::Defer:
        if (stmt->block_stmt != nullptr) {
          NormalizeDispatchSurfaceStatements(stmt->block_stmt->body, class_names,
                                             inside_method, is_class_method);
        }
        break;
      case Stmt::Kind::Expr:
        if (stmt->expr_stmt != nullptr) {
          NormalizeDispatchSurfaceExpr(stmt->expr_stmt->value.get(), class_names,
                                       inside_method, is_class_method);
        }
        break;
      case Stmt::Kind::Break:
      case Stmt::Kind::Continue:
      case Stmt::Kind::Empty:
        break;
    }
  }
}

void NormalizeDispatchSurfaceExpr(Expr *expr,
                                  const std::unordered_set<std::string> &class_names,
                                  bool inside_method,
                                  bool is_class_method) {
  if (expr == nullptr) {
    return;
  }

  NormalizeDispatchSurfaceExpr(expr->receiver.get(), class_names, inside_method,
                               is_class_method);
  NormalizeDispatchSurfaceExpr(expr->left.get(), class_names, inside_method,
                               is_class_method);
  NormalizeDispatchSurfaceExpr(expr->right.get(), class_names, inside_method,
                               is_class_method);
  NormalizeDispatchSurfaceExpr(expr->third.get(), class_names, inside_method,
                               is_class_method);
  for (const auto &arg : expr->args) {
    NormalizeDispatchSurfaceExpr(arg.get(), class_names, inside_method,
                                 is_class_method);
  }

  if (expr->kind != Expr::Kind::MessageSend || expr->receiver == nullptr) {
    return;
  }

  const Expr::DispatchSurfaceKind dispatch_kind =
      ClassifyDispatchSurfaceKind(*expr->receiver, class_names, inside_method,
                                  is_class_method);
  expr->dispatch_surface_kind = dispatch_kind;
  expr->dispatch_surface_family_symbol = DispatchSurfaceFamilySymbol(dispatch_kind);
  expr->dispatch_surface_entrypoint_family_symbol =
      DispatchEntrypointFamilySymbol(dispatch_kind);
  expr->runtime_dispatch_bridge_symbol =
      Objc3DispatchSurfaceRuntimeEntrypointSymbol(
          expr->dispatch_surface_family_symbol);
  expr->dispatch_surface_is_normalized = true;
  expr->super_dispatch_enabled =
      dispatch_kind == Expr::DispatchSurfaceKind::Super;
  expr->super_dispatch_requires_class_context = expr->super_dispatch_enabled;
}

}  // namespace

void NormalizeProgramDispatchSurfaceClassification(Objc3Program &program) {
  std::unordered_set<std::string> class_names;
  CollectKnownClassNames(program, class_names);

  for (auto &global : program.globals) {
    NormalizeDispatchSurfaceExpr(global.value.get(), class_names, false, false);
  }
  for (auto &function_decl : program.functions) {
    NormalizeDispatchSurfaceStatements(function_decl.body, class_names, false,
                                       false);
  }
  for (auto &implementation_decl : program.implementations) {
    for (auto &method_decl : implementation_decl.methods) {
      if (!method_decl.has_body) {
        continue;
      }
      NormalizeDispatchSurfaceStatements(method_decl.body, class_names, true,
                                         method_decl.is_class_method);
    }
  }
}
