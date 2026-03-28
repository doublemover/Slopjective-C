#include "pipeline/objc3_frontend_pipeline.h"

#include <algorithm>
#include <cctype>
#include <sstream>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include "lex/objc3_lexer.h"
#include "lower/objc3_lowering_contract.h"
#include "parse/objc3_ast_builder_contract.h"
#include "parse/objc3_parse_support.h"
#include "pipeline/objc3_lowering_runtime_diagnostics_surfacing_edge_case_compatibility_surface.h"
#include "pipeline/objc3_lowering_runtime_diagnostics_surfacing_edge_case_expansion_and_robustness_surface.h"
#include "pipeline/objc3_lowering_runtime_diagnostics_surfacing_diagnostics_hardening_surface.h"
#include "pipeline/objc3_lowering_runtime_diagnostics_surfacing_conformance_matrix_implementation_surface.h"
#include "pipeline/objc3_lowering_runtime_diagnostics_surfacing_recovery_determinism_hardening_surface.h"
#include "pipeline/objc3_lowering_runtime_diagnostics_surfacing_core_feature_expansion_surface.h"
#include "pipeline/objc3_lowering_runtime_diagnostics_surfacing_core_feature_implementation_surface.h"
#include "pipeline/objc3_lowering_runtime_stability_core_feature_implementation_surface.h"
#include "pipeline/objc3_lowering_runtime_stability_invariant_scaffold.h"
#include "pipeline/objc3_ir_emission_completeness_scaffold.h"
#include "pipeline/objc3_lowering_runtime_diagnostics_surfacing_scaffold.h"
#include "pipeline/objc3_lowering_pipeline_pass_graph_core_feature_surface.h"
#include "pipeline/objc3_lowering_pipeline_pass_graph_scaffold.h"
#include "pipeline/objc3_parse_lowering_readiness_surface.h"
#include "pipeline/objc3_semantic_diagnostic_taxonomy_and_fix_it_synthesis_scaffold.h"
#include "pipeline/objc3_semantic_diagnostic_taxonomy_and_fix_it_synthesis_core_feature_implementation_surface.h"
#include "pipeline/objc3_semantic_diagnostic_taxonomy_and_fix_it_synthesis_core_feature_expansion_surface.h"
#include "pipeline/objc3_semantic_diagnostic_taxonomy_and_fix_it_synthesis_edge_case_compatibility_surface.h"
#include "pipeline/objc3_semantic_diagnostic_taxonomy_and_fix_it_synthesis_edge_case_expansion_and_robustness_surface.h"
#include "pipeline/objc3_semantic_diagnostic_taxonomy_and_fix_it_synthesis_diagnostics_hardening_surface.h"
#include "pipeline/objc3_semantic_diagnostic_taxonomy_and_fix_it_synthesis_recovery_determinism_hardening_surface.h"
#include "pipeline/objc3_semantic_diagnostic_taxonomy_and_fix_it_synthesis_conformance_matrix_implementation_surface.h"
#include "pipeline/objc3_semantic_diagnostic_taxonomy_and_fix_it_synthesis_conformance_corpus_expansion_surface.h"
#include "pipeline/objc3_semantic_diagnostic_taxonomy_and_fix_it_synthesis_performance_quality_guardrails_surface.h"
#include "pipeline/objc3_semantic_stability_core_feature_implementation_surface.h"
#include "sema/objc3_semantic_passes.h"
#include "pipeline/objc3_semantic_stability_spec_delta_closure_scaffold.h"
#include "pipeline/objc3_typed_sema_to_lowering_contract_surface.h"
#include "sema/objc3_sema_pass_manager.h"

using objc3c::parse::support::MakeDiag;

namespace {

template <typename T, typename = void>
struct HasProtocolsMember : std::false_type {};

template <typename T>
struct HasProtocolsMember<T, std::void_t<decltype(std::declval<const T &>().protocols)>> : std::true_type {};

template <typename T, typename = void>
struct HasCategoriesMember : std::false_type {};

template <typename T>
struct HasCategoriesMember<T, std::void_t<decltype(std::declval<const T &>().categories)>> : std::true_type {};

template <typename T, typename = void>
struct HasMethodsMember : std::false_type {};

template <typename T>
struct HasMethodsMember<T, std::void_t<decltype(std::declval<const T &>().methods)>> : std::true_type {};

template <typename T, typename = void>
struct HasMethodsLexicographicMember : std::false_type {};

template <typename T>
struct HasMethodsLexicographicMember<T, std::void_t<decltype(std::declval<const T &>().methods_lexicographic)>>
    : std::true_type {};

template <typename T, typename = void>
struct HasHasMatchingInterfaceMember : std::false_type {};

template <typename T>
struct HasHasMatchingInterfaceMember<T, std::void_t<decltype(std::declval<const T &>().has_matching_interface)>>
    : std::true_type {};

template <typename T, typename = void>
struct HasProtocolsLexicographicMember : std::false_type {};

template <typename T>
struct HasProtocolsLexicographicMember<T, std::void_t<decltype(std::declval<const T &>().protocols_lexicographic)>>
    : std::true_type {};

template <typename T, typename = void>
struct HasCategoriesLexicographicMember : std::false_type {};

template <typename T>
struct HasCategoriesLexicographicMember<T, std::void_t<decltype(std::declval<const T &>().categories_lexicographic)>>
    : std::true_type {};

template <typename T>
std::size_t CountProtocols(const T &value) {
  if constexpr (HasProtocolsMember<T>::value) {
    return value.protocols.size();
  }
  return 0;
}

template <typename T>
std::size_t CountCategories(const T &value) {
  if constexpr (HasCategoriesMember<T>::value) {
    return value.categories.size();
  }
  return 0;
}

template <typename Surface>
std::size_t CountProtocolMethodsFromSymbolTable(const Surface &surface) {
  if constexpr (HasProtocolsMember<Surface>::value) {
    std::size_t total = 0;
    for (const auto &entry : surface.protocols) {
      const auto &metadata = entry.second;
      if constexpr (HasMethodsMember<std::decay_t<decltype(metadata)>>::value) {
        total += metadata.methods.size();
      }
    }
    return total;
  }
  return 0;
}

template <typename Surface>
std::size_t CountCategoryMethodsFromSymbolTable(const Surface &surface) {
  if constexpr (HasCategoriesMember<Surface>::value) {
    std::size_t total = 0;
    for (const auto &entry : surface.categories) {
      const auto &metadata = entry.second;
      if constexpr (HasMethodsMember<std::decay_t<decltype(metadata)>>::value) {
        total += metadata.methods.size();
      }
    }
    return total;
  }
  return 0;
}

template <typename Surface>
std::size_t CountLinkedCategorySymbolsFromSymbolTable(const Surface &surface) {
  if constexpr (HasCategoriesMember<Surface>::value) {
    std::size_t total = 0;
    for (const auto &entry : surface.categories) {
      const auto &metadata = entry.second;
      if constexpr (HasHasMatchingInterfaceMember<std::decay_t<decltype(metadata)>>::value &&
                    HasMethodsMember<std::decay_t<decltype(metadata)>>::value) {
        if (metadata.has_matching_interface) {
          total += metadata.methods.size();
        }
      }
    }
    return total;
  }
  return 0;
}

template <typename Handoff>
std::size_t CountProtocolMethodsFromTypeMetadata(const Handoff &handoff) {
  if constexpr (HasProtocolsLexicographicMember<Handoff>::value) {
    std::size_t total = 0;
    for (const auto &metadata : handoff.protocols_lexicographic) {
      if constexpr (HasMethodsLexicographicMember<std::decay_t<decltype(metadata)>>::value) {
        total += metadata.methods_lexicographic.size();
      }
    }
    return total;
  }
  return 0;
}

template <typename Handoff>
std::size_t CountCategoryMethodsFromTypeMetadata(const Handoff &handoff) {
  if constexpr (HasCategoriesLexicographicMember<Handoff>::value) {
    std::size_t total = 0;
    for (const auto &metadata : handoff.categories_lexicographic) {
      if constexpr (HasMethodsLexicographicMember<std::decay_t<decltype(metadata)>>::value) {
        total += metadata.methods_lexicographic.size();
      }
    }
    return total;
  }
  return 0;
}

template <typename Handoff>
std::size_t CountLinkedCategorySymbolsFromTypeMetadata(const Handoff &handoff) {
  if constexpr (HasCategoriesLexicographicMember<Handoff>::value) {
    std::size_t total = 0;
    for (const auto &metadata : handoff.categories_lexicographic) {
      if constexpr (HasHasMatchingInterfaceMember<std::decay_t<decltype(metadata)>>::value &&
                    HasMethodsLexicographicMember<std::decay_t<decltype(metadata)>>::value) {
        if (metadata.has_matching_interface) {
          total += metadata.methods_lexicographic.size();
        }
      }
    }
    return total;
  }
  return 0;
}

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

const char *RuntimeMetadataTypeName(ValueType type) {
  switch (type) {
    case ValueType::I32:
      return "i32";
    case ValueType::Bool:
      return "bool";
    case ValueType::Void:
      return "void";
    case ValueType::ObjCId:
      return "id";
    case ValueType::ObjCClass:
      return "Class";
    case ValueType::ObjCSel:
      return "SEL";
    case ValueType::ObjCProtocol:
      return "Protocol";
    case ValueType::ObjCInstancetype:
      return "instancetype";
    case ValueType::ObjCObjectPtr:
      return "object-pointer";
    default:
      return "unknown";
  }
}

std::string BuildCategoryOwnerName(const std::string &class_name,
                                   const std::string &category_name) {
  return class_name + "(" + category_name + ")";
}

bool IsClassSourceRecordLess(const Objc3RuntimeMetadataClassSourceRecord &lhs,
                             const Objc3RuntimeMetadataClassSourceRecord &rhs) {
  return std::tie(lhs.name, lhs.record_kind, lhs.objc_final_declared,
                  lhs.objc_sealed_declared, lhs.line, lhs.column) <
         std::tie(rhs.name, rhs.record_kind, rhs.objc_final_declared,
                  rhs.objc_sealed_declared, rhs.line, rhs.column);
}

bool IsProtocolSourceRecordLess(const Objc3RuntimeMetadataProtocolSourceRecord &lhs,
                                const Objc3RuntimeMetadataProtocolSourceRecord &rhs) {
  return std::tie(lhs.name, lhs.line, lhs.column) <
         std::tie(rhs.name, rhs.line, rhs.column);
}

bool IsCategorySourceRecordLess(const Objc3RuntimeMetadataCategorySourceRecord &lhs,
                                const Objc3RuntimeMetadataCategorySourceRecord &rhs) {
  return std::tie(lhs.class_name, lhs.category_name, lhs.record_kind, lhs.line, lhs.column) <
         std::tie(rhs.class_name, rhs.category_name, rhs.record_kind, rhs.line, rhs.column);
}

bool IsPropertySourceRecordLess(const Objc3RuntimeMetadataPropertySourceRecord &lhs,
                                const Objc3RuntimeMetadataPropertySourceRecord &rhs) {
  return std::tie(lhs.owner_kind, lhs.owner_name, lhs.property_name, lhs.line, lhs.column) <
         std::tie(rhs.owner_kind, rhs.owner_name, rhs.property_name, rhs.line, rhs.column);
}

bool IsMethodSourceRecordLess(const Objc3RuntimeMetadataMethodSourceRecord &lhs,
                              const Objc3RuntimeMetadataMethodSourceRecord &rhs) {
  return std::tie(lhs.owner_kind, lhs.owner_name, lhs.selector,
                  lhs.effective_direct_dispatch, lhs.objc_final_declared,
                  lhs.line, lhs.column) <
         std::tie(rhs.owner_kind, rhs.owner_name, rhs.selector,
                  rhs.effective_direct_dispatch, rhs.objc_final_declared,
                  rhs.line, rhs.column);
}

bool IsIvarSourceRecordLess(const Objc3RuntimeMetadataIvarSourceRecord &lhs,
                            const Objc3RuntimeMetadataIvarSourceRecord &rhs) {
  return std::tie(lhs.owner_kind, lhs.owner_name, lhs.property_name, lhs.ivar_binding_symbol, lhs.line, lhs.column) <
         std::tie(rhs.owner_kind, rhs.owner_name, rhs.property_name, rhs.ivar_binding_symbol, rhs.line, rhs.column);
}

std::string BuildRuntimeClassOwnerIdentity(const std::string &class_name) {
  return "class:" + class_name;
}

std::string BuildRuntimeMetaclassOwnerIdentity(const std::string &class_name) {
  return "metaclass:" + class_name;
}

std::string BuildRuntimeCategoryOwnerIdentity(const std::string &class_name,
                                              const std::string &category_name) {
  return "category:" + class_name + "(" + category_name + ")";
}

std::string BuildPropertyNodeOwnerIdentity(
    const std::string &declaration_owner_identity,
    const Objc3PropertyDecl &property) {
  return property.scope_path_symbol.empty()
             ? declaration_owner_identity + "::property:" + property.name
             : property.scope_path_symbol;
}

std::string BuildMethodNodeOwnerIdentity(
    const std::string &declaration_owner_identity,
    const Objc3MethodDecl &method) {
  return method.scope_path_symbol.empty()
             ? declaration_owner_identity + "::" +
                   (method.is_class_method ? "class_method:" : "instance_method:") +
                   method.selector
             : method.scope_path_symbol;
}

std::string BuildIvarNodeOwnerIdentity(
    const std::string &declaration_owner_identity,
    const Objc3PropertyDecl &property) {
  if (!property.ivar_binding_symbol.empty()) {
    if (property.ivar_binding_symbol.find("::") != std::string::npos) {
      return property.ivar_binding_symbol;
    }
    return declaration_owner_identity + "::" + property.ivar_binding_symbol;
  }
  return declaration_owner_identity + "::ivar:" + property.name;
}

std::size_t CountClassMethods(const std::vector<Objc3MethodDecl> &methods) {
  return static_cast<std::size_t>(
      std::count_if(methods.begin(), methods.end(), [](const Objc3MethodDecl &method) {
        return method.is_class_method;
      }));
}

bool IsExecutableMetadataInterfaceNodeLess(
    const Objc3ExecutableMetadataInterfaceGraphNode &lhs,
    const Objc3ExecutableMetadataInterfaceGraphNode &rhs) {
  return std::tie(lhs.class_name, lhs.owner_identity, lhs.line, lhs.column) <
         std::tie(rhs.class_name, rhs.owner_identity, rhs.line, rhs.column);
}

bool IsExecutableMetadataImplementationNodeLess(
    const Objc3ExecutableMetadataImplementationGraphNode &lhs,
    const Objc3ExecutableMetadataImplementationGraphNode &rhs) {
  return std::tie(lhs.class_name, lhs.owner_identity, lhs.line, lhs.column) <
         std::tie(rhs.class_name, rhs.owner_identity, rhs.line, rhs.column);
}

bool IsExecutableMetadataClassNodeLess(
    const Objc3ExecutableMetadataClassGraphNode &lhs,
    const Objc3ExecutableMetadataClassGraphNode &rhs) {
  return std::tie(lhs.class_name, lhs.owner_identity, lhs.line, lhs.column) <
         std::tie(rhs.class_name, rhs.owner_identity, rhs.line, rhs.column);
}

bool IsExecutableMetadataMetaclassNodeLess(
    const Objc3ExecutableMetadataMetaclassGraphNode &lhs,
    const Objc3ExecutableMetadataMetaclassGraphNode &rhs) {
  return std::tie(lhs.class_name, lhs.owner_identity, lhs.line, lhs.column) <
         std::tie(rhs.class_name, rhs.owner_identity, rhs.line, rhs.column);
}

bool IsExecutableMetadataProtocolNodeLess(
    const Objc3ExecutableMetadataProtocolGraphNode &lhs,
    const Objc3ExecutableMetadataProtocolGraphNode &rhs) {
  return std::tie(lhs.protocol_name, lhs.owner_identity, lhs.line, lhs.column) <
         std::tie(rhs.protocol_name, rhs.owner_identity, rhs.line, rhs.column);
}

bool IsExecutableMetadataCategoryNodeLess(
    const Objc3ExecutableMetadataCategoryGraphNode &lhs,
    const Objc3ExecutableMetadataCategoryGraphNode &rhs) {
  return std::tie(lhs.class_name, lhs.category_name, lhs.owner_identity, lhs.line,
                  lhs.column) <
         std::tie(rhs.class_name, rhs.category_name, rhs.owner_identity, rhs.line,
                  rhs.column);
}

bool IsExecutableMetadataPropertyNodeLess(
    const Objc3ExecutableMetadataPropertyGraphNode &lhs,
    const Objc3ExecutableMetadataPropertyGraphNode &rhs) {
  return std::tie(lhs.owner_kind, lhs.owner_name, lhs.property_name,
                  lhs.owner_identity, lhs.line, lhs.column) <
         std::tie(rhs.owner_kind, rhs.owner_name, rhs.property_name,
                  rhs.owner_identity, rhs.line, rhs.column);
}

bool IsExecutableMetadataMethodNodeLess(
    const Objc3ExecutableMetadataMethodGraphNode &lhs,
    const Objc3ExecutableMetadataMethodGraphNode &rhs) {
  return std::tie(lhs.owner_kind, lhs.owner_name, lhs.selector,
                  lhs.is_class_method, lhs.owner_identity, lhs.line, lhs.column) <
         std::tie(rhs.owner_kind, rhs.owner_name, rhs.selector,
                  rhs.is_class_method, rhs.owner_identity, rhs.line, rhs.column);
}

bool IsExecutableMetadataIvarNodeLess(
    const Objc3ExecutableMetadataIvarGraphNode &lhs,
    const Objc3ExecutableMetadataIvarGraphNode &rhs) {
  return std::tie(lhs.owner_kind, lhs.owner_name, lhs.property_name,
                  lhs.ivar_binding_symbol, lhs.owner_identity, lhs.line,
                  lhs.column) <
         std::tie(rhs.owner_kind, rhs.owner_name, rhs.property_name,
                  rhs.ivar_binding_symbol, rhs.owner_identity, rhs.line,
                  rhs.column);
}

bool IsExecutableMetadataGraphEdgeLess(
    const Objc3ExecutableMetadataGraphEdge &lhs,
    const Objc3ExecutableMetadataGraphEdge &rhs) {
  return std::tie(lhs.edge_kind, lhs.source_owner_identity, lhs.target_owner_identity,
                  lhs.line, lhs.column) <
         std::tie(rhs.edge_kind, rhs.source_owner_identity, rhs.target_owner_identity,
                  rhs.line, rhs.column);
}

std::string BuildExecutablePropertyOwnerKey(const std::string &owner_name,
                                            const std::string &property_name) {
  return owner_name + "|" + property_name;
}

bool UsesWeakCurrentPropertyRuntimeHelper(
    const std::string &ownership_runtime_hook_profile) {
  return ownership_runtime_hook_profile == "objc-weak-side-table";
}

bool UsesStrongOwnedCurrentPropertyExchange(
    const std::string &ownership_lifetime_profile,
    const std::string &accessor_ownership_profile) {
  return ownership_lifetime_profile == "strong-owned" ||
         accessor_ownership_profile.find("ownership_lifetime=strong-owned") !=
             std::string::npos;
}

bool ShouldSynthesizeExecutablePropertyAccessors(
    const std::string &owner_kind, const std::string &owner_name,
    const std::string &property_name,
    const std::unordered_set<std::string> &class_implementation_names,
    const std::unordered_set<std::string> &implementation_property_keys) {
  if (owner_kind == "class-implementation" ||
      owner_kind == "category-implementation") {
    return true;
  }
  if (owner_kind != "class-interface") {
    return false;
  }
  return class_implementation_names.find(owner_name) !=
             class_implementation_names.end() &&
         implementation_property_keys.find(
             BuildExecutablePropertyOwnerKey(owner_name, property_name)) ==
             implementation_property_keys.end();
}

std::string BuildGetterStorageRuntimeHelperSymbol(
    bool synthesizes_executable_accessors,
    const std::string &ownership_runtime_hook_profile) {
  if (!synthesizes_executable_accessors) {
    return {};
  }
  return UsesWeakCurrentPropertyRuntimeHelper(ownership_runtime_hook_profile)
             ? kObjc3RuntimeLoadWeakCurrentPropertyI32Symbol
             : kObjc3RuntimeReadCurrentPropertyI32Symbol;
}

std::string BuildSetterStorageRuntimeHelperSymbol(
    bool synthesizes_executable_accessors, bool effective_setter_available,
    const std::string &ownership_lifetime_profile,
    const std::string &ownership_runtime_hook_profile,
    const std::string &accessor_ownership_profile) {
  if (!synthesizes_executable_accessors || !effective_setter_available) {
    return {};
  }
  if (UsesWeakCurrentPropertyRuntimeHelper(ownership_runtime_hook_profile)) {
    return kObjc3RuntimeStoreWeakCurrentPropertyI32Symbol;
  }
  return UsesStrongOwnedCurrentPropertyExchange(ownership_lifetime_profile,
                                                accessor_ownership_profile)
             ? kObjc3RuntimeExchangeCurrentPropertyI32Symbol
             : kObjc3RuntimeWriteCurrentPropertyI32Symbol;
}

Objc3RuntimeMetadataSourceRecordSet BuildRuntimeMetadataSourceRecordSet(
    const Objc3Program &program) {
  Objc3RuntimeMetadataSourceRecordSet records;
  std::unordered_set<std::string> class_implementation_names;
  class_implementation_names.reserve(program.implementations.size());
  std::unordered_set<std::string> implementation_property_keys;
  for (const auto &implementation_decl : program.implementations) {
    if (!implementation_decl.has_category) {
      class_implementation_names.insert(implementation_decl.name);
    }
    const std::string owner_name =
        implementation_decl.has_category
            ? BuildCategoryOwnerName(implementation_decl.name,
                                     implementation_decl.category_name)
            : implementation_decl.name;
    for (const auto &property : implementation_decl.properties) {
      implementation_property_keys.insert(
          BuildExecutablePropertyOwnerKey(owner_name, property.name));
    }
  }

  const auto apply_arc_property_interaction_metadata =
      [](Objc3RuntimeMetadataPropertySourceRecord &property_record) {
        const auto profile_contains =
            [&property_record](std::string_view needle) {
              return property_record.property_attribute_profile.find(
                         std::string(needle)) != std::string::npos;
            };
        const auto rebuild_accessor_profile = [&property_record]() {
          std::ostringstream out;
          out << "getter=" << property_record.effective_getter_selector
              << ";setter_available="
              << (property_record.effective_setter_available ? 1 : 0)
              << ";setter=";
          if (property_record.effective_setter_available) {
            out << property_record.effective_setter_selector;
          } else {
            out << "<none>";
          }
          out << ";ownership_lifetime="
              << property_record.ownership_lifetime_profile
              << ";runtime_hook="
              << property_record.ownership_runtime_hook_profile;
          property_record.accessor_ownership_profile = out.str();
        };

        if (property_record.ownership_lifetime_profile.empty()) {
          if (profile_contains("weak=1")) {
            property_record.ownership_lifetime_profile = "weak";
            property_record.ownership_runtime_hook_profile =
                "objc-weak-side-table";
          } else if (profile_contains("strong=1") ||
                     profile_contains("copy=1")) {
            property_record.ownership_lifetime_profile = "strong-owned";
            property_record.ownership_runtime_hook_profile.clear();
          }
        }

        if ((!property_record.ownership_lifetime_profile.empty() ||
             !property_record.ownership_runtime_hook_profile.empty()) &&
            (property_record.accessor_ownership_profile.empty() ||
             (property_record.accessor_ownership_profile.find(
                  "ownership_lifetime=") != std::string::npos &&
              property_record.accessor_ownership_profile.find(
                  "ownership_lifetime=;") != std::string::npos))) {
          rebuild_accessor_profile();
        }
      };

  const auto append_property_records =
      [&records, &apply_arc_property_interaction_metadata,
       &class_implementation_names, &implementation_property_keys](
          const auto &properties, const std::string &owner_kind,
          const std::string &owner_name) {
        for (const auto &property : properties) {
          Objc3RuntimeMetadataPropertySourceRecord property_record;
          property_record.owner_kind = owner_kind;
          property_record.owner_name = owner_name;
          property_record.property_name = property.name;
          property_record.type_name = RuntimeMetadataTypeName(property.type);
          property_record.has_getter = property.has_getter;
          property_record.getter_selector = property.getter_selector;
          property_record.has_setter = property.has_setter;
          property_record.setter_selector = property.setter_selector;
          property_record.ivar_binding_symbol = property.ivar_binding_symbol;
          property_record.executable_synthesized_binding_kind =
              property.executable_synthesized_binding_kind;
          property_record.executable_synthesized_binding_symbol =
              property.executable_synthesized_binding_symbol;
          property_record.property_attribute_profile =
              property.property_attribute_profile;
          property_record.ownership_lifetime_profile =
              property.ownership_lifetime_profile;
          property_record.ownership_runtime_hook_profile =
              property.ownership_runtime_hook_profile;
          property_record.effective_getter_selector =
              property.effective_getter_selector;
          property_record.effective_setter_available =
              property.effective_setter_available;
          property_record.effective_setter_selector =
              property.effective_setter_selector;
          property_record.accessor_ownership_profile =
              property.accessor_ownership_profile;
          apply_arc_property_interaction_metadata(property_record);
          property_record.synthesizes_executable_accessors =
              ShouldSynthesizeExecutablePropertyAccessors(
                  owner_kind, owner_name, property.name,
                  class_implementation_names, implementation_property_keys);
          property_record.getter_storage_runtime_helper_symbol =
              BuildGetterStorageRuntimeHelperSymbol(
                  property_record.synthesizes_executable_accessors,
                  property_record.ownership_runtime_hook_profile);
          property_record.setter_storage_runtime_helper_symbol =
              BuildSetterStorageRuntimeHelperSymbol(
                  property_record.synthesizes_executable_accessors,
                  property_record.effective_setter_available,
                  property_record.ownership_lifetime_profile,
                  property_record.ownership_runtime_hook_profile,
                  property_record.accessor_ownership_profile);
          property_record.executable_ivar_layout_symbol =
              property.executable_ivar_layout_symbol;
          property_record.executable_ivar_layout_slot_index =
              property.executable_ivar_layout_slot_index;
          property_record.executable_ivar_layout_size_bytes =
              property.executable_ivar_layout_size_bytes;
          property_record.executable_ivar_layout_alignment_bytes =
              property.executable_ivar_layout_alignment_bytes;
          property_record.executable_ivar_init_order_index =
              property.executable_ivar_init_order_index;
          property_record.executable_ivar_destroy_order_index =
              property.executable_ivar_destroy_order_index;
          property_record.line = property.line;
          property_record.column = property.column;
          records.properties_lexicographic.push_back(std::move(property_record));

          if (!property.ivar_binding_symbol.empty()) {
            Objc3RuntimeMetadataIvarSourceRecord ivar_record;
            ivar_record.owner_kind = owner_kind;
            ivar_record.owner_name = owner_name;
            ivar_record.property_name = property.name;
            ivar_record.ivar_binding_symbol = property.ivar_binding_symbol;
            ivar_record.executable_synthesized_binding_kind =
                property.executable_synthesized_binding_kind;
            ivar_record.executable_synthesized_binding_symbol =
                property.executable_synthesized_binding_symbol;
            ivar_record.executable_ivar_layout_symbol =
                property.executable_ivar_layout_symbol;
            ivar_record.executable_ivar_layout_slot_index =
                property.executable_ivar_layout_slot_index;
            ivar_record.executable_ivar_layout_size_bytes =
                property.executable_ivar_layout_size_bytes;
            ivar_record.executable_ivar_layout_alignment_bytes =
                property.executable_ivar_layout_alignment_bytes;
            ivar_record.executable_ivar_init_order_index =
                property.executable_ivar_init_order_index;
            ivar_record.executable_ivar_destroy_order_index =
                property.executable_ivar_destroy_order_index;
            ivar_record.line = property.line;
            ivar_record.column = property.column;
            records.ivars_lexicographic.push_back(std::move(ivar_record));
          }
        }
      };

  const auto append_method_records =
      [&records](const auto &methods, const std::string &owner_kind,
                 const std::string &owner_name,
                 bool direct_members_declared = false) {
        for (const auto &method : methods) {
          Objc3RuntimeMetadataMethodSourceRecord method_record;
          method_record.owner_kind = owner_kind;
          method_record.owner_name = owner_name;
          method_record.selector = method.selector;
          method_record.is_class_method = method.is_class_method;
          method_record.has_body = method.has_body;
          method_record.effective_direct_dispatch =
              method.objc_direct_declared ||
              (direct_members_declared && !method.objc_dynamic_declared);
          method_record.objc_final_declared = method.objc_final_declared;
          method_record.parameter_count = method.params.size();
          method_record.return_type_name = RuntimeMetadataTypeName(method.return_type);
          method_record.line = method.line;
          method_record.column = method.column;
          records.methods_lexicographic.push_back(std::move(method_record));
        }
      };

  struct Objc3ClassDispatchProfile {
    bool objc_direct_members_declared = false;
    bool objc_final_declared = false;
    bool objc_sealed_declared = false;
  };
  std::unordered_map<std::string, Objc3ClassDispatchProfile> class_dispatch_profiles;
  class_dispatch_profiles.reserve(program.interfaces.size());
  for (const auto &interface_decl : program.interfaces) {
    if (interface_decl.has_category) {
      continue;
    }
    class_dispatch_profiles[interface_decl.name] = Objc3ClassDispatchProfile{
        interface_decl.objc_direct_members_declared,
        interface_decl.objc_final_declared,
        interface_decl.objc_sealed_declared};
  }

  for (const auto &protocol : program.protocols) {
    Objc3RuntimeMetadataProtocolSourceRecord record;
    record.name = protocol.name;
    record.inherited_protocols_lexicographic = protocol.inherited_protocols_lexicographic;
    record.is_forward_declaration = protocol.is_forward_declaration;
    record.property_count = protocol.properties.size();
    record.method_count = protocol.methods.size();
    record.line = protocol.line;
    record.column = protocol.column;
    records.protocols_lexicographic.push_back(record);
    append_property_records(protocol.properties, "protocol", protocol.name);
    append_method_records(protocol.methods, "protocol", protocol.name);
  }

  for (const auto &interface_decl : program.interfaces) {
    if (interface_decl.has_category) {
      Objc3RuntimeMetadataCategorySourceRecord record;
      record.record_kind = "interface";
      record.class_name = interface_decl.name;
      record.category_name = interface_decl.category_name;
      record.adopted_protocols_lexicographic =
          interface_decl.adopted_protocols_lexicographic;
      record.property_count = interface_decl.properties.size();
      record.method_count = interface_decl.methods.size();
      record.line = interface_decl.line;
      record.column = interface_decl.column;
      records.categories_lexicographic.push_back(record);
      const std::string owner_name =
          BuildCategoryOwnerName(interface_decl.name, interface_decl.category_name);
      append_property_records(interface_decl.properties, "category-interface", owner_name);
      append_method_records(interface_decl.methods, "category-interface", owner_name);
      continue;
    }

    Objc3RuntimeMetadataClassSourceRecord record;
    record.record_kind = "interface";
    record.name = interface_decl.name;
    record.super_name = interface_decl.super_name;
    record.adopted_protocols_lexicographic =
        interface_decl.adopted_protocols_lexicographic;
    record.has_super = !interface_decl.super_name.empty();
    record.objc_final_declared = interface_decl.objc_final_declared;
    record.objc_sealed_declared = interface_decl.objc_sealed_declared;
    record.property_count = interface_decl.properties.size();
    record.method_count = interface_decl.methods.size();
    record.line = interface_decl.line;
    record.column = interface_decl.column;
    records.classes_lexicographic.push_back(record);
    append_property_records(interface_decl.properties, "class-interface", interface_decl.name);
    append_method_records(interface_decl.methods, "class-interface",
                          interface_decl.name,
                          interface_decl.objc_direct_members_declared);
  }

  for (const auto &implementation : program.implementations) {
    if (implementation.has_category) {
      Objc3RuntimeMetadataCategorySourceRecord record;
      record.record_kind = "implementation";
      record.class_name = implementation.name;
      record.category_name = implementation.category_name;
      record.property_count = implementation.properties.size();
      record.method_count = implementation.methods.size();
      record.line = implementation.line;
      record.column = implementation.column;
      records.categories_lexicographic.push_back(record);
      const std::string owner_name =
          BuildCategoryOwnerName(implementation.name, implementation.category_name);
      append_property_records(implementation.properties, "category-implementation", owner_name);
      append_method_records(implementation.methods, "category-implementation", owner_name);
      continue;
    }

    Objc3RuntimeMetadataClassSourceRecord record;
    const auto profile_it = class_dispatch_profiles.find(implementation.name);
    record.record_kind = "implementation";
    record.name = implementation.name;
    if (profile_it != class_dispatch_profiles.end()) {
      record.objc_final_declared = profile_it->second.objc_final_declared;
      record.objc_sealed_declared = profile_it->second.objc_sealed_declared;
    }
    record.property_count = implementation.properties.size();
    record.method_count = implementation.methods.size();
    record.line = implementation.line;
    record.column = implementation.column;
    records.classes_lexicographic.push_back(record);
    append_property_records(implementation.properties, "class-implementation", implementation.name);
    append_method_records(
        implementation.methods, "class-implementation", implementation.name,
        profile_it != class_dispatch_profiles.end() &&
            profile_it->second.objc_direct_members_declared);
  }

  std::sort(records.classes_lexicographic.begin(),
            records.classes_lexicographic.end(),
            IsClassSourceRecordLess);
  std::sort(records.protocols_lexicographic.begin(),
            records.protocols_lexicographic.end(),
            IsProtocolSourceRecordLess);
  std::sort(records.categories_lexicographic.begin(),
            records.categories_lexicographic.end(),
            IsCategorySourceRecordLess);
  std::sort(records.properties_lexicographic.begin(),
            records.properties_lexicographic.end(),
            IsPropertySourceRecordLess);
  std::sort(records.methods_lexicographic.begin(),
            records.methods_lexicographic.end(),
            IsMethodSourceRecordLess);
  std::sort(records.ivars_lexicographic.begin(),
            records.ivars_lexicographic.end(),
            IsIvarSourceRecordLess);

  records.deterministic = std::is_sorted(records.classes_lexicographic.begin(),
                                         records.classes_lexicographic.end(),
                                         IsClassSourceRecordLess) &&
                          std::is_sorted(records.protocols_lexicographic.begin(),
                                         records.protocols_lexicographic.end(),
                                         IsProtocolSourceRecordLess) &&
                          std::is_sorted(records.categories_lexicographic.begin(),
                                         records.categories_lexicographic.end(),
                                         IsCategorySourceRecordLess) &&
                          std::is_sorted(records.properties_lexicographic.begin(),
                                         records.properties_lexicographic.end(),
                                         IsPropertySourceRecordLess) &&
                          std::is_sorted(records.methods_lexicographic.begin(),
                                         records.methods_lexicographic.end(),
                                         IsMethodSourceRecordLess) &&
                          std::is_sorted(records.ivars_lexicographic.begin(),
                                         records.ivars_lexicographic.end(),
                                         IsIvarSourceRecordLess);
  return records;
}

Objc3ExecutableMetadataSourceGraph BuildExecutableMetadataSourceGraph(
    const Objc3Program &program,
    const Objc3RuntimeMetadataSourceRecordSet &runtime_metadata_source_records) {
  Objc3ExecutableMetadataSourceGraph graph;

  struct AggregatedClassSurface {
    bool has_interface = false;
    bool has_implementation = false;
    std::string interface_owner_identity;
    std::string implementation_owner_identity;
    std::string super_class_owner_identity;
    std::vector<std::string> adopted_protocol_owner_identities_lexicographic;
    bool objc_direct_members_declared = false;
    bool objc_final_declared = false;
    bool objc_sealed_declared = false;
    std::size_t interface_property_count = 0;
    std::size_t implementation_property_count = 0;
    std::size_t interface_method_count = 0;
    std::size_t implementation_method_count = 0;
    std::size_t interface_class_method_count = 0;
    std::size_t implementation_class_method_count = 0;
    unsigned line = 1;
    unsigned column = 1;
  };

  struct AggregatedCategorySurface {
    bool has_interface = false;
    bool has_implementation = false;
    std::string interface_owner_identity;
    std::string implementation_owner_identity;
    std::string class_owner_identity;
    std::vector<std::string> adopted_protocol_owner_identities_lexicographic;
    std::size_t interface_property_count = 0;
    std::size_t implementation_property_count = 0;
    std::size_t interface_method_count = 0;
    std::size_t implementation_method_count = 0;
    std::size_t interface_class_method_count = 0;
    std::size_t implementation_class_method_count = 0;
    unsigned line = 1;
    unsigned column = 1;
  };

  std::unordered_map<std::string, AggregatedClassSurface> aggregated_classes;
  aggregated_classes.reserve(program.interfaces.size() + program.implementations.size());
  std::unordered_map<std::string, AggregatedCategorySurface> aggregated_categories;
  aggregated_categories.reserve(program.interfaces.size() + program.implementations.size());

  auto &owner_edges = graph.owner_edges_lexicographic;
  const auto add_owner_edge = [&owner_edges](const std::string &edge_kind,
                                             const std::string &source_owner_identity,
                                             const std::string &target_owner_identity,
                                             unsigned line,
                                             unsigned column) {
    if (source_owner_identity.empty() || target_owner_identity.empty()) {
      return;
    }
    Objc3ExecutableMetadataGraphEdge edge;
    edge.edge_kind = edge_kind;
    edge.source_owner_identity = source_owner_identity;
    edge.target_owner_identity = target_owner_identity;
    edge.line = line;
    edge.column = column;
    owner_edges.push_back(std::move(edge));
  };

  struct MethodEdgeRecord {
    std::string owner_identity;
    std::string export_owner_identity;
    std::string selector;
    bool is_class_method = false;
  };
  std::vector<MethodEdgeRecord> method_edge_records;
  std::unordered_set<std::string> class_implementation_names;
  class_implementation_names.reserve(program.implementations.size());
  std::unordered_set<std::string> implementation_property_keys;
  for (const auto &implementation_decl : program.implementations) {
    if (!implementation_decl.has_category) {
      class_implementation_names.insert(implementation_decl.name);
    }
    const std::string owner_name =
        implementation_decl.has_category
            ? BuildCategoryOwnerName(implementation_decl.name,
                                     implementation_decl.category_name)
            : implementation_decl.name;
    for (const auto &property : implementation_decl.properties) {
      implementation_property_keys.insert(
          BuildExecutablePropertyOwnerKey(owner_name, property.name));
    }
  }

  const auto apply_arc_property_interaction_metadata =
      [](Objc3ExecutableMetadataPropertyGraphNode &node) {
        const auto profile_contains = [&node](std::string_view needle) {
          return node.property_attribute_profile.find(std::string(needle)) !=
                 std::string::npos;
        };
        const auto rebuild_accessor_profile = [&node]() {
          std::ostringstream out;
          out << "getter=" << node.effective_getter_selector
              << ";setter_available="
              << (node.effective_setter_available ? 1 : 0) << ";setter=";
          if (node.effective_setter_available) {
            out << node.effective_setter_selector;
          } else {
            out << "<none>";
          }
          out << ";ownership_lifetime=" << node.ownership_lifetime_profile
              << ";runtime_hook=" << node.ownership_runtime_hook_profile;
          node.accessor_ownership_profile = out.str();
        };

        if (node.ownership_lifetime_profile.empty()) {
          if (profile_contains("weak=1")) {
            node.ownership_lifetime_profile = "weak";
            node.ownership_runtime_hook_profile = "objc-weak-side-table";
          } else if (profile_contains("strong=1") ||
                     profile_contains("copy=1")) {
            node.ownership_lifetime_profile = "strong-owned";
            node.ownership_runtime_hook_profile.clear();
          }
        }
        if ((!node.ownership_lifetime_profile.empty() ||
             !node.ownership_runtime_hook_profile.empty()) &&
            (node.accessor_ownership_profile.empty() ||
             (node.accessor_ownership_profile.find("ownership_lifetime=") !=
                  std::string::npos &&
              node.accessor_ownership_profile.find("ownership_lifetime=;") !=
                  std::string::npos))) {
          rebuild_accessor_profile();
        }
      };

  const auto add_property_nodes =
      [&graph, &add_owner_edge, &apply_arc_property_interaction_metadata,
       &class_implementation_names, &implementation_property_keys](
          const auto &properties, const std::string &owner_kind,
          const std::string &owner_name,
          const std::string &declaration_owner_identity,
          const std::string &export_owner_identity) {
        for (const auto &property : properties) {
          Objc3ExecutableMetadataPropertyGraphNode node;
          node.owner_kind = owner_kind;
          node.owner_name = owner_name;
          node.declaration_owner_identity = declaration_owner_identity;
          node.export_owner_identity = export_owner_identity;
          node.owner_identity =
              BuildPropertyNodeOwnerIdentity(declaration_owner_identity, property);
          node.property_name = property.name;
          node.type_name = RuntimeMetadataTypeName(property.type);
          node.has_getter = property.has_getter;
          node.getter_selector = property.getter_selector;
          node.has_setter = property.has_setter;
          node.setter_selector = property.setter_selector;
          node.ivar_binding_symbol = property.ivar_binding_symbol;
          node.executable_synthesized_binding_kind =
              property.executable_synthesized_binding_kind;
          node.executable_synthesized_binding_symbol =
              property.executable_synthesized_binding_symbol;
          node.property_attribute_profile = property.property_attribute_profile;
          node.ownership_lifetime_profile =
              property.ownership_lifetime_profile;
          node.ownership_runtime_hook_profile =
              property.ownership_runtime_hook_profile;
          node.effective_getter_selector =
              property.effective_getter_selector;
          node.effective_setter_available =
              property.effective_setter_available;
          node.effective_setter_selector =
              property.effective_setter_selector;
          node.accessor_ownership_profile =
              property.accessor_ownership_profile;
          apply_arc_property_interaction_metadata(node);
          node.synthesizes_executable_accessors =
              ShouldSynthesizeExecutablePropertyAccessors(
                  owner_kind, owner_name, property.name,
                  class_implementation_names, implementation_property_keys);
          node.getter_storage_runtime_helper_symbol =
              BuildGetterStorageRuntimeHelperSymbol(
                  node.synthesizes_executable_accessors,
                  node.ownership_runtime_hook_profile);
          node.setter_storage_runtime_helper_symbol =
              BuildSetterStorageRuntimeHelperSymbol(
                  node.synthesizes_executable_accessors,
                  node.effective_setter_available,
                  node.ownership_lifetime_profile,
                  node.ownership_runtime_hook_profile,
                  node.accessor_ownership_profile);
          node.executable_ivar_layout_symbol =
              property.executable_ivar_layout_symbol;
          node.executable_ivar_layout_slot_index =
              property.executable_ivar_layout_slot_index;
          node.executable_ivar_layout_size_bytes =
              property.executable_ivar_layout_size_bytes;
          node.executable_ivar_layout_alignment_bytes =
              property.executable_ivar_layout_alignment_bytes;
          node.executable_ivar_init_order_index =
              property.executable_ivar_init_order_index;
          node.executable_ivar_destroy_order_index =
              property.executable_ivar_destroy_order_index;
          node.line = property.line;
          node.column = property.column;
          graph.property_nodes_lexicographic.push_back(node);

          add_owner_edge("property-to-declaration-owner", node.owner_identity,
                         declaration_owner_identity, node.line, node.column);
          add_owner_edge("property-to-export-owner", node.owner_identity,
                         export_owner_identity, node.line, node.column);

          if (!property.ivar_binding_symbol.empty()) {
            Objc3ExecutableMetadataIvarGraphNode ivar_node;
            ivar_node.owner_kind = owner_kind;
            ivar_node.owner_name = owner_name;
            ivar_node.declaration_owner_identity = declaration_owner_identity;
            ivar_node.export_owner_identity = export_owner_identity;
            ivar_node.property_owner_identity = node.owner_identity;
            ivar_node.owner_identity =
                BuildIvarNodeOwnerIdentity(declaration_owner_identity, property);
            ivar_node.property_name = property.name;
            ivar_node.ivar_binding_symbol = property.ivar_binding_symbol;
            ivar_node.executable_synthesized_binding_kind =
                property.executable_synthesized_binding_kind;
            ivar_node.executable_synthesized_binding_symbol =
                property.executable_synthesized_binding_symbol;
            ivar_node.executable_ivar_layout_symbol =
                property.executable_ivar_layout_symbol;
            ivar_node.executable_ivar_layout_slot_index =
                property.executable_ivar_layout_slot_index;
            ivar_node.executable_ivar_layout_size_bytes =
                property.executable_ivar_layout_size_bytes;
            ivar_node.executable_ivar_layout_alignment_bytes =
                property.executable_ivar_layout_alignment_bytes;
            ivar_node.executable_ivar_init_order_index =
                property.executable_ivar_init_order_index;
            ivar_node.executable_ivar_destroy_order_index =
                property.executable_ivar_destroy_order_index;
            ivar_node.line = property.line;
            ivar_node.column = property.column;
            graph.ivar_nodes_lexicographic.push_back(ivar_node);

            add_owner_edge("ivar-to-declaration-owner", ivar_node.owner_identity,
                           declaration_owner_identity, ivar_node.line,
                           ivar_node.column);
            add_owner_edge("ivar-to-export-owner", ivar_node.owner_identity,
                           export_owner_identity, ivar_node.line, ivar_node.column);
            add_owner_edge("ivar-to-property", ivar_node.owner_identity,
                           node.owner_identity, ivar_node.line, ivar_node.column);
            add_owner_edge("property-to-ivar", node.owner_identity,
                           ivar_node.owner_identity, node.line, node.column);
          }
        }
      };

  const auto add_method_nodes =
      [&graph, &add_owner_edge, &method_edge_records](const auto &methods,
                                                      const std::string &owner_kind,
                                                      const std::string &owner_name,
                                                      const std::string &declaration_owner_identity,
                                                      const std::string &instance_export_owner_identity,
                                                      const std::string &class_export_owner_identity,
                                                      bool direct_members_declared) {
        for (const auto &method : methods) {
          Objc3ExecutableMetadataMethodGraphNode node;
          node.owner_kind = owner_kind;
          node.owner_name = owner_name;
          node.declaration_owner_identity = declaration_owner_identity;
          node.export_owner_identity =
              method.is_class_method ? class_export_owner_identity
                                     : instance_export_owner_identity;
          node.owner_identity =
              BuildMethodNodeOwnerIdentity(declaration_owner_identity, method);
          node.selector = method.selector;
          node.is_class_method = method.is_class_method;
          node.has_body = method.has_body;
          node.effective_direct_dispatch =
              method.objc_direct_declared ||
              (direct_members_declared && !method.objc_dynamic_declared);
          node.objc_final_declared = method.objc_final_declared;
          node.parameter_count = method.params.size();
          node.return_type_name = RuntimeMetadataTypeName(method.return_type);
          node.line = method.line;
          node.column = method.column;
          graph.method_nodes_lexicographic.push_back(node);

          add_owner_edge("method-to-declaration-owner", node.owner_identity,
                         declaration_owner_identity, node.line, node.column);
          add_owner_edge("method-to-export-owner", node.owner_identity,
                         node.export_owner_identity, node.line, node.column);

          method_edge_records.push_back(
              {node.owner_identity, node.export_owner_identity, node.selector,
               node.is_class_method});
        }
      };

  for (const auto &interface_decl : program.interfaces) {
    if (interface_decl.has_category) {
      const std::string category_owner_name =
          BuildCategoryOwnerName(interface_decl.name, interface_decl.category_name);
      AggregatedCategorySurface &aggregate =
          aggregated_categories[category_owner_name];
      if (!aggregate.has_interface && !aggregate.has_implementation) {
        aggregate.line = interface_decl.line;
        aggregate.column = interface_decl.column;
      }
      aggregate.has_interface = true;
      aggregate.interface_owner_identity = interface_decl.semantic_link_symbol;
      aggregate.class_owner_identity =
          BuildRuntimeClassOwnerIdentity(interface_decl.name);
      aggregate.adopted_protocol_owner_identities_lexicographic =
          interface_decl.adopted_protocols_lexicographic;
      aggregate.interface_property_count = interface_decl.properties.size();
      aggregate.interface_method_count = interface_decl.methods.size();
      aggregate.interface_class_method_count =
          CountClassMethods(interface_decl.methods);

      add_property_nodes(interface_decl.properties, "category-interface",
                         category_owner_name, interface_decl.semantic_link_symbol,
                         BuildRuntimeCategoryOwnerIdentity(
                             interface_decl.name, interface_decl.category_name));
      add_method_nodes(interface_decl.methods, "category-interface",
                       category_owner_name, interface_decl.semantic_link_symbol,
                       BuildRuntimeCategoryOwnerIdentity(
                           interface_decl.name, interface_decl.category_name),
                       BuildRuntimeCategoryOwnerIdentity(
                           interface_decl.name, interface_decl.category_name),
                       false);
      continue;
    }

    Objc3ExecutableMetadataInterfaceGraphNode node;
    node.class_name = interface_decl.name;
    node.owner_identity = interface_decl.semantic_link_symbol;
    node.class_owner_identity = BuildRuntimeClassOwnerIdentity(interface_decl.name);
    node.metaclass_owner_identity =
        BuildRuntimeMetaclassOwnerIdentity(interface_decl.name);
    node.super_class_owner_identity =
        interface_decl.super_name.empty()
            ? std::string{}
            : BuildRuntimeClassOwnerIdentity(interface_decl.super_name);
    node.super_metaclass_owner_identity =
        interface_decl.super_name.empty()
            ? std::string{}
            : BuildRuntimeMetaclassOwnerIdentity(interface_decl.super_name);
    node.instance_method_owner_identity = node.class_owner_identity;
    node.class_method_owner_identity = node.metaclass_owner_identity;
    node.has_super = !interface_decl.super_name.empty();
    node.declaration_complete =
        !node.owner_identity.empty() && !node.class_owner_identity.empty() &&
        !node.metaclass_owner_identity.empty() &&
        !node.instance_method_owner_identity.empty() &&
        !node.class_method_owner_identity.empty() &&
        (!node.has_super ||
         (!node.super_class_owner_identity.empty() &&
          !node.super_metaclass_owner_identity.empty()));
    node.property_count = interface_decl.properties.size();
    node.method_count = interface_decl.methods.size();
    node.class_method_count = CountClassMethods(interface_decl.methods);
    node.instance_method_count = node.method_count - node.class_method_count;
    node.line = interface_decl.line;
    node.column = interface_decl.column;
    graph.interface_nodes_lexicographic.push_back(node);

    AggregatedClassSurface &aggregate = aggregated_classes[interface_decl.name];
    if (!aggregate.has_interface && !aggregate.has_implementation) {
      aggregate.line = interface_decl.line;
      aggregate.column = interface_decl.column;
    }
    aggregate.has_interface = true;
    aggregate.interface_owner_identity = interface_decl.semantic_link_symbol;
    aggregate.super_class_owner_identity = node.super_class_owner_identity;
    aggregate.adopted_protocol_owner_identities_lexicographic =
        interface_decl.adopted_protocols_lexicographic;
    aggregate.objc_direct_members_declared =
        interface_decl.objc_direct_members_declared;
    aggregate.objc_final_declared = interface_decl.objc_final_declared;
    aggregate.objc_sealed_declared = interface_decl.objc_sealed_declared;
    aggregate.interface_property_count = node.property_count;
    aggregate.interface_method_count = node.method_count;
    aggregate.interface_class_method_count = node.class_method_count;

    add_owner_edge("interface-to-class", node.owner_identity,
                   node.class_owner_identity, node.line, node.column);
    add_owner_edge("interface-to-metaclass", node.owner_identity,
                   node.metaclass_owner_identity, node.line, node.column);
    add_owner_edge("interface-to-instance-method-owner", node.owner_identity,
                   node.instance_method_owner_identity, node.line, node.column);
    add_owner_edge("interface-to-class-method-owner", node.owner_identity,
                   node.class_method_owner_identity, node.line, node.column);
    add_owner_edge("class-to-superclass", node.class_owner_identity,
                   node.super_class_owner_identity, node.line, node.column);
    add_owner_edge("interface-to-superclass", node.owner_identity,
                   node.super_class_owner_identity, node.line, node.column);
    add_owner_edge("interface-to-super-metaclass", node.owner_identity,
                   node.super_metaclass_owner_identity, node.line, node.column);

    add_property_nodes(interface_decl.properties, "class-interface",
                       interface_decl.name, interface_decl.semantic_link_symbol,
                       node.class_owner_identity);
    add_method_nodes(interface_decl.methods, "class-interface", interface_decl.name,
                     interface_decl.semantic_link_symbol, node.class_owner_identity,
                     node.metaclass_owner_identity,
                     interface_decl.objc_direct_members_declared);
  }

  for (const auto &implementation_decl : program.implementations) {
    if (implementation_decl.has_category) {
      const std::string category_owner_name =
          BuildCategoryOwnerName(implementation_decl.name,
                                 implementation_decl.category_name);
      AggregatedCategorySurface &aggregate =
          aggregated_categories[category_owner_name];
      if (!aggregate.has_interface && !aggregate.has_implementation) {
        aggregate.line = implementation_decl.line;
        aggregate.column = implementation_decl.column;
      }
      aggregate.has_implementation = true;
      aggregate.implementation_owner_identity =
          implementation_decl.semantic_link_symbol;
      aggregate.class_owner_identity =
          BuildRuntimeClassOwnerIdentity(implementation_decl.name);
      aggregate.implementation_property_count =
          implementation_decl.properties.size();
      aggregate.implementation_method_count = implementation_decl.methods.size();
      aggregate.implementation_class_method_count =
          CountClassMethods(implementation_decl.methods);

      add_property_nodes(
          implementation_decl.properties, "category-implementation",
          category_owner_name, implementation_decl.semantic_link_symbol,
          BuildRuntimeCategoryOwnerIdentity(implementation_decl.name,
                                            implementation_decl.category_name));
      add_method_nodes(
          implementation_decl.methods, "category-implementation",
          category_owner_name, implementation_decl.semantic_link_symbol,
          BuildRuntimeCategoryOwnerIdentity(implementation_decl.name,
                                            implementation_decl.category_name),
          BuildRuntimeCategoryOwnerIdentity(implementation_decl.name,
                                            implementation_decl.category_name),
          false);
      continue;
    }

    AggregatedClassSurface &aggregate = aggregated_classes[implementation_decl.name];
    Objc3ExecutableMetadataImplementationGraphNode node;
    node.class_name = implementation_decl.name;
    node.owner_identity = implementation_decl.semantic_link_symbol;
    node.interface_owner_identity =
        implementation_decl.semantic_link_interface_symbol;
    node.class_owner_identity =
        BuildRuntimeClassOwnerIdentity(implementation_decl.name);
    node.metaclass_owner_identity =
        BuildRuntimeMetaclassOwnerIdentity(implementation_decl.name);
    node.super_class_owner_identity = aggregate.super_class_owner_identity;
    node.super_metaclass_owner_identity =
        aggregate.super_class_owner_identity.empty()
            ? std::string{}
            : BuildRuntimeMetaclassOwnerIdentity(
                  aggregate.super_class_owner_identity.substr(6u));
    node.instance_method_owner_identity = node.class_owner_identity;
    node.class_method_owner_identity = node.metaclass_owner_identity;
    node.has_matching_interface = !node.interface_owner_identity.empty();
    node.has_super = !node.super_class_owner_identity.empty();
    node.declaration_complete =
        !node.owner_identity.empty() && !node.class_owner_identity.empty() &&
        !node.metaclass_owner_identity.empty() &&
        !node.instance_method_owner_identity.empty() &&
        !node.class_method_owner_identity.empty() &&
        (!node.has_super ||
         (!node.super_class_owner_identity.empty() &&
          !node.super_metaclass_owner_identity.empty()));
    node.property_count = implementation_decl.properties.size();
    node.method_count = implementation_decl.methods.size();
    node.class_method_count = CountClassMethods(implementation_decl.methods);
    node.instance_method_count = node.method_count - node.class_method_count;
    node.line = implementation_decl.line;
    node.column = implementation_decl.column;
    graph.implementation_nodes_lexicographic.push_back(node);

    if (!aggregate.has_interface && !aggregate.has_implementation) {
      aggregate.line = implementation_decl.line;
      aggregate.column = implementation_decl.column;
    }
    aggregate.has_implementation = true;
    aggregate.implementation_owner_identity = implementation_decl.semantic_link_symbol;
    aggregate.implementation_property_count = node.property_count;
    aggregate.implementation_method_count = node.method_count;
    aggregate.implementation_class_method_count = node.class_method_count;

    add_owner_edge("implementation-to-class", node.owner_identity,
                   node.class_owner_identity, node.line, node.column);
    add_owner_edge("implementation-to-metaclass", node.owner_identity,
                   node.metaclass_owner_identity, node.line, node.column);
    add_owner_edge("implementation-to-interface", node.owner_identity,
                   node.interface_owner_identity, node.line, node.column);
    add_owner_edge("implementation-to-instance-method-owner",
                   node.owner_identity, node.instance_method_owner_identity,
                   node.line, node.column);
    add_owner_edge("implementation-to-class-method-owner", node.owner_identity,
                   node.class_method_owner_identity, node.line, node.column);
    add_owner_edge("implementation-to-superclass", node.owner_identity,
                   node.super_class_owner_identity, node.line, node.column);
    add_owner_edge("implementation-to-super-metaclass", node.owner_identity,
                   node.super_metaclass_owner_identity, node.line, node.column);

    add_property_nodes(implementation_decl.properties, "class-implementation",
                       implementation_decl.name,
                       implementation_decl.semantic_link_symbol,
                       node.class_owner_identity);
    add_method_nodes(implementation_decl.methods, "class-implementation",
                     implementation_decl.name,
                     implementation_decl.semantic_link_symbol,
                     node.class_owner_identity, node.metaclass_owner_identity,
                     aggregate.objc_direct_members_declared);
  }

  for (const auto &protocol_decl : program.protocols) {
    Objc3ExecutableMetadataProtocolGraphNode node;
    node.protocol_name = protocol_decl.name;
    node.owner_identity = protocol_decl.semantic_link_symbol;
    node.inherited_protocol_owner_identities_lexicographic =
        protocol_decl.inherited_protocols_lexicographic;
    node.property_count = protocol_decl.properties.size();
    node.method_count = protocol_decl.methods.size();
    node.is_forward_declaration = protocol_decl.is_forward_declaration;
    node.declaration_complete =
        !node.protocol_name.empty() && !node.owner_identity.empty();
    node.line = protocol_decl.line;
    node.column = protocol_decl.column;
    std::sort(node.inherited_protocol_owner_identities_lexicographic.begin(),
              node.inherited_protocol_owner_identities_lexicographic.end());
    node.inherited_protocol_owner_identities_lexicographic.erase(
        std::unique(node.inherited_protocol_owner_identities_lexicographic.begin(),
                    node.inherited_protocol_owner_identities_lexicographic.end()),
        node.inherited_protocol_owner_identities_lexicographic.end());
    node.inherited_protocol_identity_complete =
        std::all_of(node.inherited_protocol_owner_identities_lexicographic.begin(),
                    node.inherited_protocol_owner_identities_lexicographic.end(),
                    [](const std::string &owner_identity) {
                      return !owner_identity.empty();
                    });
    graph.protocol_nodes_lexicographic.push_back(node);

    for (const auto &target : node.inherited_protocol_owner_identities_lexicographic) {
      add_owner_edge("protocol-to-inherited-protocol", node.owner_identity,
                     target, node.line, node.column);
    }

    add_property_nodes(protocol_decl.properties, "protocol", protocol_decl.name,
                       protocol_decl.semantic_link_symbol,
                       protocol_decl.semantic_link_symbol);
    add_method_nodes(protocol_decl.methods, "protocol", protocol_decl.name,
                     protocol_decl.semantic_link_symbol,
                     protocol_decl.semantic_link_symbol,
                     protocol_decl.semantic_link_symbol, false);
  }

  std::vector<std::string> class_names;
  class_names.reserve(aggregated_classes.size());
  for (const auto &entry : aggregated_classes) {
    class_names.push_back(entry.first);
  }
  std::sort(class_names.begin(), class_names.end());

  graph.class_nodes_lexicographic.reserve(class_names.size());
  graph.metaclass_nodes_lexicographic.reserve(class_names.size());
  for (const std::string &class_name : class_names) {
    const AggregatedClassSurface &aggregate = aggregated_classes.at(class_name);

    Objc3ExecutableMetadataClassGraphNode class_node;
    class_node.class_name = class_name;
    class_node.owner_identity = BuildRuntimeClassOwnerIdentity(class_name);
    class_node.interface_owner_identity = aggregate.interface_owner_identity;
    class_node.implementation_owner_identity =
        aggregate.implementation_owner_identity;
    class_node.metaclass_owner_identity =
        BuildRuntimeMetaclassOwnerIdentity(class_name);
    class_node.super_class_owner_identity = aggregate.super_class_owner_identity;
    class_node.super_metaclass_owner_identity =
        aggregate.super_class_owner_identity.empty()
            ? std::string{}
            : BuildRuntimeMetaclassOwnerIdentity(
                  aggregate.super_class_owner_identity.substr(6u));
    class_node.adopted_protocol_owner_identities_lexicographic =
        aggregate.adopted_protocol_owner_identities_lexicographic;
    class_node.instance_method_owner_identity = class_node.owner_identity;
    class_node.class_method_owner_identity = class_node.metaclass_owner_identity;
    class_node.has_interface = aggregate.has_interface;
    class_node.has_implementation = aggregate.has_implementation;
    class_node.has_super = !aggregate.super_class_owner_identity.empty();
    class_node.objc_final_declared = aggregate.objc_final_declared;
    class_node.objc_sealed_declared = aggregate.objc_sealed_declared;
    class_node.realization_identity_complete =
        !class_node.owner_identity.empty() &&
        !class_node.metaclass_owner_identity.empty() &&
        !class_node.instance_method_owner_identity.empty() &&
        !class_node.class_method_owner_identity.empty() &&
        (!class_node.has_super ||
         (!class_node.super_class_owner_identity.empty() &&
          !class_node.super_metaclass_owner_identity.empty()));
    class_node.interface_property_count = aggregate.interface_property_count;
    class_node.implementation_property_count =
        aggregate.implementation_property_count;
    class_node.interface_method_count = aggregate.interface_method_count;
    class_node.implementation_method_count = aggregate.implementation_method_count;
    class_node.interface_class_method_count =
        aggregate.interface_class_method_count;
    class_node.implementation_class_method_count =
        aggregate.implementation_class_method_count;
    class_node.interface_instance_method_count =
        aggregate.interface_method_count -
        aggregate.interface_class_method_count;
    class_node.implementation_instance_method_count =
        aggregate.implementation_method_count -
        aggregate.implementation_class_method_count;
    class_node.line = aggregate.line;
    class_node.column = aggregate.column;
    graph.class_nodes_lexicographic.push_back(class_node);

    if (aggregate.has_interface) {
      Objc3ExecutableMetadataMetaclassGraphNode metaclass_node;
      metaclass_node.class_name = class_name;
      metaclass_node.owner_identity =
          BuildRuntimeMetaclassOwnerIdentity(class_name);
      metaclass_node.class_owner_identity = class_node.owner_identity;
      metaclass_node.interface_owner_identity = aggregate.interface_owner_identity;
      metaclass_node.implementation_owner_identity =
          aggregate.implementation_owner_identity;
      metaclass_node.super_metaclass_owner_identity =
          class_node.has_super
              ? BuildRuntimeMetaclassOwnerIdentity(
                    class_node.super_class_owner_identity.substr(6u))
              : std::string{};
      metaclass_node.derived_from_interface = true;
      metaclass_node.has_implementation = aggregate.has_implementation;
      metaclass_node.has_super = class_node.has_super;
      metaclass_node.interface_class_method_count =
          aggregate.interface_class_method_count;
      metaclass_node.implementation_class_method_count =
          aggregate.implementation_class_method_count;
      metaclass_node.line = aggregate.line;
      metaclass_node.column = aggregate.column;
      graph.metaclass_nodes_lexicographic.push_back(metaclass_node);

      add_owner_edge("class-to-metaclass", class_node.owner_identity,
                     metaclass_node.owner_identity, class_node.line,
                     class_node.column);
      add_owner_edge("metaclass-to-super-metaclass",
                     metaclass_node.owner_identity,
                     metaclass_node.super_metaclass_owner_identity,
                     metaclass_node.line, metaclass_node.column);
    }
  }

  std::vector<std::string> category_names;
  category_names.reserve(aggregated_categories.size());
  for (const auto &entry : aggregated_categories) {
    category_names.push_back(entry.first);
  }
  std::sort(category_names.begin(), category_names.end());

  graph.category_nodes_lexicographic.reserve(category_names.size());
  for (const std::string &category_owner_name : category_names) {
    const AggregatedCategorySurface &aggregate =
        aggregated_categories.at(category_owner_name);
    const std::size_t open_paren = category_owner_name.find('(');
    const std::string class_name =
        open_paren == std::string::npos
            ? category_owner_name
            : category_owner_name.substr(0u, open_paren);
    const std::string category_name =
        open_paren == std::string::npos
            ? std::string{}
            : category_owner_name.substr(open_paren + 1u,
                                         category_owner_name.size() - open_paren - 2u);

    Objc3ExecutableMetadataCategoryGraphNode node;
    node.class_name = class_name;
    node.category_name = category_name;
    node.owner_identity =
        BuildRuntimeCategoryOwnerIdentity(class_name, category_name);
    node.interface_owner_identity = aggregate.interface_owner_identity;
    node.implementation_owner_identity = aggregate.implementation_owner_identity;
    node.class_owner_identity = aggregate.class_owner_identity;
    node.adopted_protocol_owner_identities_lexicographic =
        aggregate.adopted_protocol_owner_identities_lexicographic;
    node.has_interface = aggregate.has_interface;
    node.has_implementation = aggregate.has_implementation;
    node.declaration_complete =
        !node.class_name.empty() && !node.category_name.empty() &&
        !node.owner_identity.empty() && !node.class_owner_identity.empty() &&
        (!node.has_interface || !node.interface_owner_identity.empty()) &&
        (!node.has_implementation || !node.implementation_owner_identity.empty());
    node.attachment_identity_complete =
        !node.class_owner_identity.empty() &&
        (!node.has_interface || !node.interface_owner_identity.empty()) &&
        (!node.has_implementation || !node.implementation_owner_identity.empty());
    node.interface_property_count = aggregate.interface_property_count;
    node.implementation_property_count = aggregate.implementation_property_count;
    node.interface_method_count = aggregate.interface_method_count;
    node.implementation_method_count = aggregate.implementation_method_count;
    node.interface_class_method_count = aggregate.interface_class_method_count;
    node.implementation_class_method_count =
        aggregate.implementation_class_method_count;
    node.line = aggregate.line;
    node.column = aggregate.column;
    std::sort(node.adopted_protocol_owner_identities_lexicographic.begin(),
              node.adopted_protocol_owner_identities_lexicographic.end());
    node.adopted_protocol_owner_identities_lexicographic.erase(
        std::unique(node.adopted_protocol_owner_identities_lexicographic.begin(),
                    node.adopted_protocol_owner_identities_lexicographic.end()),
        node.adopted_protocol_owner_identities_lexicographic.end());
    node.conformance_identity_complete =
        std::all_of(node.adopted_protocol_owner_identities_lexicographic.begin(),
                    node.adopted_protocol_owner_identities_lexicographic.end(),
                    [](const std::string &owner_identity) {
                      return !owner_identity.empty();
                    });
    graph.category_nodes_lexicographic.push_back(node);

    add_owner_edge("category-to-class", node.owner_identity,
                   node.class_owner_identity, node.line, node.column);
    add_owner_edge("category-to-interface", node.owner_identity,
                   node.interface_owner_identity, node.line, node.column);
    add_owner_edge("category-to-implementation", node.owner_identity,
                   node.implementation_owner_identity, node.line, node.column);
    for (const auto &target : node.adopted_protocol_owner_identities_lexicographic) {
      add_owner_edge("category-to-protocol", node.owner_identity, target,
                     node.line, node.column);
    }
  }

  for (const auto &property_node : graph.property_nodes_lexicographic) {
    if (property_node.has_getter) {
      for (const auto &method_record : method_edge_records) {
        if (method_record.export_owner_identity == property_node.export_owner_identity &&
            method_record.selector == property_node.getter_selector &&
            !method_record.is_class_method) {
          add_owner_edge("property-to-getter-method", property_node.owner_identity,
                         method_record.owner_identity, property_node.line,
                         property_node.column);
        }
      }
    }

    if (property_node.has_setter) {
      for (const auto &method_record : method_edge_records) {
        if (method_record.export_owner_identity == property_node.export_owner_identity &&
            method_record.selector == property_node.setter_selector &&
            !method_record.is_class_method) {
          add_owner_edge("property-to-setter-method", property_node.owner_identity,
                         method_record.owner_identity, property_node.line,
                         property_node.column);
        }
      }
    }
  }

  std::unordered_map<std::string, const Objc3ExecutableMetadataClassGraphNode *>
      class_nodes_by_owner_identity;
  class_nodes_by_owner_identity.reserve(graph.class_nodes_lexicographic.size());
  for (const auto &class_node : graph.class_nodes_lexicographic) {
    class_nodes_by_owner_identity.emplace(class_node.owner_identity, &class_node);
  }

  std::unordered_map<std::string,
                     const Objc3ExecutableMetadataInterfaceGraphNode *>
      interface_nodes_by_owner_identity;
  interface_nodes_by_owner_identity.reserve(
      graph.interface_nodes_lexicographic.size());
  for (const auto &interface_node : graph.interface_nodes_lexicographic) {
    interface_nodes_by_owner_identity.emplace(interface_node.owner_identity,
                                              &interface_node);
  }

  std::unordered_map<std::string, std::string>
      class_interface_method_owner_identity_by_key;
  class_interface_method_owner_identity_by_key.reserve(
      graph.method_nodes_lexicographic.size());
  const auto build_interface_method_key =
      [](const std::string &declaration_owner_identity,
         const std::string &selector, bool is_class_method) {
        return declaration_owner_identity + "::" +
               (is_class_method ? "class" : "instance") + "::" + selector;
      };
  for (const auto &method_node : graph.method_nodes_lexicographic) {
    if (method_node.owner_kind != "class-interface") {
      continue;
    }
    class_interface_method_owner_identity_by_key.emplace(
        build_interface_method_key(method_node.declaration_owner_identity,
                                   method_node.selector,
                                   method_node.is_class_method),
        method_node.owner_identity);
  }

  const auto find_overridden_interface_method_owner_identity =
      [&](const Objc3ExecutableMetadataInterfaceGraphNode &interface_node,
          const Objc3ExecutableMetadataMethodGraphNode &method_node) {
        std::string next_super_owner_identity =
            interface_node.super_class_owner_identity;
        std::unordered_set<std::string> visited;
        while (!next_super_owner_identity.empty()) {
          if (!visited.insert(next_super_owner_identity).second) {
            return std::string{};
          }
          const auto class_it =
              class_nodes_by_owner_identity.find(next_super_owner_identity);
          if (class_it == class_nodes_by_owner_identity.end() ||
              class_it->second->interface_owner_identity.empty()) {
            return std::string{};
          }
          const std::string key = build_interface_method_key(
              class_it->second->interface_owner_identity, method_node.selector,
              method_node.is_class_method);
          const auto method_it =
              class_interface_method_owner_identity_by_key.find(key);
          if (method_it !=
              class_interface_method_owner_identity_by_key.end()) {
            return method_it->second;
          }
          next_super_owner_identity =
              class_it->second->super_class_owner_identity;
        }
        return std::string{};
      };

  for (const auto &method_node : graph.method_nodes_lexicographic) {
    if (method_node.owner_kind != "class-interface") {
      continue;
    }
    const auto interface_it = interface_nodes_by_owner_identity.find(
        method_node.declaration_owner_identity);
    if (interface_it == interface_nodes_by_owner_identity.end() ||
        !interface_it->second->has_super) {
      continue;
    }
    const std::string overridden_method_owner_identity =
        find_overridden_interface_method_owner_identity(*interface_it->second,
                                                        method_node);
    if (!overridden_method_owner_identity.empty()) {
      add_owner_edge("method-to-overridden-method", method_node.owner_identity,
                     overridden_method_owner_identity, method_node.line,
                     method_node.column);
    }
  }

  std::sort(graph.interface_nodes_lexicographic.begin(),
            graph.interface_nodes_lexicographic.end(),
            IsExecutableMetadataInterfaceNodeLess);
  std::sort(graph.implementation_nodes_lexicographic.begin(),
            graph.implementation_nodes_lexicographic.end(),
            IsExecutableMetadataImplementationNodeLess);
  std::sort(graph.class_nodes_lexicographic.begin(),
            graph.class_nodes_lexicographic.end(),
            IsExecutableMetadataClassNodeLess);
  std::sort(graph.metaclass_nodes_lexicographic.begin(),
            graph.metaclass_nodes_lexicographic.end(),
            IsExecutableMetadataMetaclassNodeLess);
  std::sort(graph.protocol_nodes_lexicographic.begin(),
            graph.protocol_nodes_lexicographic.end(),
            IsExecutableMetadataProtocolNodeLess);
  std::sort(graph.category_nodes_lexicographic.begin(),
            graph.category_nodes_lexicographic.end(),
            IsExecutableMetadataCategoryNodeLess);
  std::sort(graph.property_nodes_lexicographic.begin(),
            graph.property_nodes_lexicographic.end(),
            IsExecutableMetadataPropertyNodeLess);
  std::sort(graph.method_nodes_lexicographic.begin(),
            graph.method_nodes_lexicographic.end(),
            IsExecutableMetadataMethodNodeLess);
  std::sort(graph.ivar_nodes_lexicographic.begin(),
            graph.ivar_nodes_lexicographic.end(),
            IsExecutableMetadataIvarNodeLess);
  std::sort(graph.owner_edges_lexicographic.begin(),
            graph.owner_edges_lexicographic.end(),
            IsExecutableMetadataGraphEdgeLess);

  const auto has_graph_edge = [&graph](const std::string &edge_kind,
                                       const std::string &source_owner_identity,
                                       const std::string &target_owner_identity) {
    return std::any_of(
        graph.owner_edges_lexicographic.begin(),
        graph.owner_edges_lexicographic.end(),
        [&edge_kind, &source_owner_identity, &target_owner_identity](
            const Objc3ExecutableMetadataGraphEdge &edge) {
          return edge.edge_kind == edge_kind &&
                 edge.source_owner_identity == source_owner_identity &&
                 edge.target_owner_identity == target_owner_identity;
        });
  };

  const std::size_t expected_class_interface_count = static_cast<std::size_t>(
      std::count_if(program.interfaces.begin(), program.interfaces.end(),
                    [](const Objc3InterfaceDecl &decl) {
                      return !decl.has_category;
                    }));
  const std::size_t expected_class_implementation_count = static_cast<std::size_t>(
      std::count_if(program.implementations.begin(), program.implementations.end(),
                    [](const Objc3ImplementationDecl &decl) {
                      return !decl.has_category;
                    }));
  const bool interface_count_aligned =
      graph.interface_nodes_lexicographic.size() == expected_class_interface_count;
  const bool implementation_count_aligned =
      graph.implementation_nodes_lexicographic.size() ==
      expected_class_implementation_count;
  const bool metaclass_count_aligned =
      graph.metaclass_nodes_lexicographic.size() ==
      graph.interface_nodes_lexicographic.size();
  const bool class_node_floor_satisfied =
      graph.class_nodes_lexicographic.size() >=
          graph.interface_nodes_lexicographic.size() &&
      graph.class_nodes_lexicographic.size() >=
          graph.implementation_nodes_lexicographic.size();
  const bool protocol_count_aligned =
      graph.protocol_nodes_lexicographic.size() ==
      runtime_metadata_source_records.protocols_lexicographic.size();
  const bool property_count_aligned =
      graph.property_nodes_lexicographic.size() ==
      runtime_metadata_source_records.properties_lexicographic.size();
  const bool method_count_aligned =
      graph.method_nodes_lexicographic.size() ==
      runtime_metadata_source_records.methods_lexicographic.size();
  const bool ivar_count_aligned =
      graph.ivar_nodes_lexicographic.size() ==
      runtime_metadata_source_records.ivars_lexicographic.size();
  std::vector<std::string> category_record_owner_names;
  category_record_owner_names.reserve(
      runtime_metadata_source_records.categories_lexicographic.size());
  for (const auto &record :
       runtime_metadata_source_records.categories_lexicographic) {
    category_record_owner_names.push_back(
        BuildCategoryOwnerName(record.class_name, record.category_name));
  }
  std::sort(category_record_owner_names.begin(),
            category_record_owner_names.end());
  category_record_owner_names.erase(
      std::unique(category_record_owner_names.begin(),
                  category_record_owner_names.end()),
      category_record_owner_names.end());
  const bool category_count_aligned =
      graph.category_nodes_lexicographic.size() ==
      category_record_owner_names.size();

  graph.class_metaclass_declaration_closure_complete = true;
  graph.class_metaclass_parent_identity_closure_complete = true;
  graph.class_metaclass_method_owner_identity_closure_complete = true;
  graph.class_metaclass_object_identity_closure_complete = true;
  graph.protocol_category_declaration_closure_complete = true;
  graph.protocol_inheritance_identity_closure_complete = true;
  graph.category_attachment_identity_closure_complete = true;
  graph.protocol_category_conformance_identity_closure_complete = true;

  for (const auto &node : graph.interface_nodes_lexicographic) {
    graph.class_metaclass_declaration_closure_complete =
        graph.class_metaclass_declaration_closure_complete &&
        node.declaration_complete;
    graph.class_metaclass_parent_identity_closure_complete =
        graph.class_metaclass_parent_identity_closure_complete &&
        (!node.has_super ||
         (!node.super_class_owner_identity.empty() &&
          !node.super_metaclass_owner_identity.empty() &&
          has_graph_edge("interface-to-superclass", node.owner_identity,
                         node.super_class_owner_identity) &&
          has_graph_edge("interface-to-super-metaclass", node.owner_identity,
                         node.super_metaclass_owner_identity)));
    graph.class_metaclass_method_owner_identity_closure_complete =
        graph.class_metaclass_method_owner_identity_closure_complete &&
        node.instance_method_owner_identity == node.class_owner_identity &&
        node.class_method_owner_identity == node.metaclass_owner_identity &&
        has_graph_edge("interface-to-instance-method-owner", node.owner_identity,
                       node.instance_method_owner_identity) &&
        has_graph_edge("interface-to-class-method-owner", node.owner_identity,
                       node.class_method_owner_identity);
    graph.class_metaclass_object_identity_closure_complete =
        graph.class_metaclass_object_identity_closure_complete &&
        !node.class_owner_identity.empty() && !node.metaclass_owner_identity.empty() &&
        has_graph_edge("interface-to-class", node.owner_identity,
                       node.class_owner_identity) &&
        has_graph_edge("interface-to-metaclass", node.owner_identity,
                       node.metaclass_owner_identity);
  }

  for (const auto &node : graph.implementation_nodes_lexicographic) {
    graph.class_metaclass_declaration_closure_complete =
        graph.class_metaclass_declaration_closure_complete &&
        node.declaration_complete;
    graph.class_metaclass_parent_identity_closure_complete =
        graph.class_metaclass_parent_identity_closure_complete &&
        (!node.has_super ||
         (!node.super_class_owner_identity.empty() &&
          !node.super_metaclass_owner_identity.empty() &&
          has_graph_edge("implementation-to-superclass", node.owner_identity,
                         node.super_class_owner_identity) &&
          has_graph_edge("implementation-to-super-metaclass", node.owner_identity,
                         node.super_metaclass_owner_identity)));
    graph.class_metaclass_method_owner_identity_closure_complete =
        graph.class_metaclass_method_owner_identity_closure_complete &&
        node.instance_method_owner_identity == node.class_owner_identity &&
        node.class_method_owner_identity == node.metaclass_owner_identity &&
        has_graph_edge("implementation-to-instance-method-owner",
                       node.owner_identity, node.instance_method_owner_identity) &&
        has_graph_edge("implementation-to-class-method-owner",
                       node.owner_identity, node.class_method_owner_identity);
    graph.class_metaclass_object_identity_closure_complete =
        graph.class_metaclass_object_identity_closure_complete &&
        !node.class_owner_identity.empty() && !node.metaclass_owner_identity.empty() &&
        has_graph_edge("implementation-to-class", node.owner_identity,
                       node.class_owner_identity) &&
        has_graph_edge("implementation-to-metaclass", node.owner_identity,
                       node.metaclass_owner_identity);
  }

  for (const auto &node : graph.class_nodes_lexicographic) {
    graph.class_metaclass_declaration_closure_complete =
        graph.class_metaclass_declaration_closure_complete &&
        node.realization_identity_complete;
    graph.class_metaclass_parent_identity_closure_complete =
        graph.class_metaclass_parent_identity_closure_complete &&
        (!node.has_super ||
         (!node.super_class_owner_identity.empty() &&
          !node.super_metaclass_owner_identity.empty() &&
          has_graph_edge("class-to-superclass", node.owner_identity,
                         node.super_class_owner_identity)));
    graph.class_metaclass_method_owner_identity_closure_complete =
        graph.class_metaclass_method_owner_identity_closure_complete &&
        node.instance_method_owner_identity == node.owner_identity &&
        node.class_method_owner_identity == node.metaclass_owner_identity;
    graph.class_metaclass_object_identity_closure_complete =
        graph.class_metaclass_object_identity_closure_complete &&
        !node.owner_identity.empty() && !node.metaclass_owner_identity.empty() &&
        has_graph_edge("class-to-metaclass", node.owner_identity,
                       node.metaclass_owner_identity);
  }

  for (const auto &node : graph.metaclass_nodes_lexicographic) {
    graph.class_metaclass_declaration_closure_complete =
        graph.class_metaclass_declaration_closure_complete &&
        !node.owner_identity.empty() && !node.class_owner_identity.empty();
    graph.class_metaclass_parent_identity_closure_complete =
        graph.class_metaclass_parent_identity_closure_complete &&
        (!node.has_super ||
         (!node.super_metaclass_owner_identity.empty() &&
          has_graph_edge("metaclass-to-super-metaclass", node.owner_identity,
                         node.super_metaclass_owner_identity)));
    graph.class_metaclass_object_identity_closure_complete =
        graph.class_metaclass_object_identity_closure_complete &&
        !node.owner_identity.empty() && !node.class_owner_identity.empty() &&
        has_graph_edge("class-to-metaclass", node.class_owner_identity,
                       node.owner_identity);
  }

  for (const auto &node : graph.protocol_nodes_lexicographic) {
    graph.protocol_category_declaration_closure_complete =
        graph.protocol_category_declaration_closure_complete &&
        node.declaration_complete;
    graph.protocol_inheritance_identity_closure_complete =
        graph.protocol_inheritance_identity_closure_complete &&
        node.inherited_protocol_identity_complete &&
        std::all_of(
            node.inherited_protocol_owner_identities_lexicographic.begin(),
            node.inherited_protocol_owner_identities_lexicographic.end(),
            [&](const std::string &target_owner_identity) {
              return has_graph_edge("protocol-to-inherited-protocol",
                                    node.owner_identity,
                                    target_owner_identity);
            });
  }

  for (const auto &node : graph.category_nodes_lexicographic) {
    graph.protocol_category_declaration_closure_complete =
        graph.protocol_category_declaration_closure_complete &&
        node.declaration_complete;
    graph.category_attachment_identity_closure_complete =
        graph.category_attachment_identity_closure_complete &&
        node.attachment_identity_complete &&
        has_graph_edge("category-to-class", node.owner_identity,
                       node.class_owner_identity) &&
        (!node.has_interface ||
         has_graph_edge("category-to-interface", node.owner_identity,
                        node.interface_owner_identity)) &&
        (!node.has_implementation ||
         has_graph_edge("category-to-implementation", node.owner_identity,
                        node.implementation_owner_identity));
    graph.protocol_category_conformance_identity_closure_complete =
        graph.protocol_category_conformance_identity_closure_complete &&
        node.conformance_identity_complete &&
        std::all_of(
            node.adopted_protocol_owner_identities_lexicographic.begin(),
            node.adopted_protocol_owner_identities_lexicographic.end(),
            [&](const std::string &target_owner_identity) {
              return has_graph_edge("category-to-protocol", node.owner_identity,
                                    target_owner_identity);
            });
  }

  graph.deterministic =
      std::is_sorted(graph.interface_nodes_lexicographic.begin(),
                     graph.interface_nodes_lexicographic.end(),
                     IsExecutableMetadataInterfaceNodeLess) &&
      std::is_sorted(graph.implementation_nodes_lexicographic.begin(),
                     graph.implementation_nodes_lexicographic.end(),
                     IsExecutableMetadataImplementationNodeLess) &&
      std::is_sorted(graph.class_nodes_lexicographic.begin(),
                     graph.class_nodes_lexicographic.end(),
                     IsExecutableMetadataClassNodeLess) &&
      std::is_sorted(graph.metaclass_nodes_lexicographic.begin(),
                     graph.metaclass_nodes_lexicographic.end(),
                     IsExecutableMetadataMetaclassNodeLess) &&
      std::is_sorted(graph.protocol_nodes_lexicographic.begin(),
                     graph.protocol_nodes_lexicographic.end(),
                     IsExecutableMetadataProtocolNodeLess) &&
      std::is_sorted(graph.category_nodes_lexicographic.begin(),
                     graph.category_nodes_lexicographic.end(),
                     IsExecutableMetadataCategoryNodeLess) &&
      std::is_sorted(graph.property_nodes_lexicographic.begin(),
                     graph.property_nodes_lexicographic.end(),
                     IsExecutableMetadataPropertyNodeLess) &&
      std::is_sorted(graph.method_nodes_lexicographic.begin(),
                     graph.method_nodes_lexicographic.end(),
                     IsExecutableMetadataMethodNodeLess) &&
      std::is_sorted(graph.ivar_nodes_lexicographic.begin(),
                     graph.ivar_nodes_lexicographic.end(),
                     IsExecutableMetadataIvarNodeLess) &&
      std::is_sorted(graph.owner_edges_lexicographic.begin(),
                     graph.owner_edges_lexicographic.end(),
                     IsExecutableMetadataGraphEdgeLess);
  graph.source_graph_complete =
      graph.deterministic && interface_count_aligned &&
      implementation_count_aligned && metaclass_count_aligned &&
      class_node_floor_satisfied && protocol_count_aligned &&
      category_count_aligned && property_count_aligned &&
      method_count_aligned && ivar_count_aligned &&
      graph.class_metaclass_declaration_closure_complete &&
      graph.class_metaclass_parent_identity_closure_complete &&
      graph.class_metaclass_method_owner_identity_closure_complete &&
      graph.class_metaclass_object_identity_closure_complete &&
      graph.protocol_category_declaration_closure_complete &&
      graph.protocol_inheritance_identity_closure_complete &&
      graph.category_attachment_identity_closure_complete &&
      graph.protocol_category_conformance_identity_closure_complete;
  graph.ready_for_semantic_closure = graph.source_graph_complete;
  graph.ready_for_lowering = false;
  return graph;
}

Objc3ExecutableMetadataSemanticConsistencyBoundary
BuildExecutableMetadataSemanticConsistencyBoundary(
    const Objc3ExecutableMetadataSourceGraph &graph,
    const Objc3FrontendProtocolCategorySummary &protocol_category_summary,
    const Objc3FrontendClassProtocolCategoryLinkingSummary
        &class_protocol_category_linking_summary,
    const Objc3FrontendSelectorNormalizationSummary
        &selector_normalization_summary,
    const Objc3FrontendPropertyAttributeSummary &property_attribute_summary,
    const Objc3FrontendSymbolGraphScopeResolutionSummary
        &symbol_graph_scope_resolution_summary) {
  Objc3ExecutableMetadataSemanticConsistencyBoundary boundary;
  boundary.executable_metadata_source_graph_contract_id = graph.contract_id;
  boundary.source_graph_ready = IsReadyObjc3ExecutableMetadataSourceGraph(graph);
  boundary.protocol_category_handoff_deterministic =
      protocol_category_summary.deterministic_protocol_category_handoff;
  boundary.class_protocol_category_linking_deterministic =
      class_protocol_category_linking_summary
          .deterministic_class_protocol_category_linking_handoff;
  boundary.selector_normalization_deterministic =
      selector_normalization_summary.deterministic_selector_normalization_handoff;
  boundary.property_attribute_deterministic =
      property_attribute_summary.deterministic_property_attribute_handoff;
  boundary.symbol_graph_scope_resolution_deterministic =
      symbol_graph_scope_resolution_summary.deterministic_symbol_graph_handoff &&
      symbol_graph_scope_resolution_summary
          .deterministic_scope_resolution_handoff;
  boundary.protocol_node_count = graph.protocol_nodes_lexicographic.size();
  boundary.category_node_count = graph.category_nodes_lexicographic.size();
  boundary.property_node_count = graph.property_nodes_lexicographic.size();
  boundary.method_node_count = graph.method_nodes_lexicographic.size();
  boundary.ivar_node_count = graph.ivar_nodes_lexicographic.size();
  boundary.owner_edge_count = graph.owner_edges_lexicographic.size();

  const auto has_graph_edge =
      [&graph](const std::string &edge_kind, const std::string &source,
               const std::string &target) {
        return std::any_of(
            graph.owner_edges_lexicographic.begin(),
            graph.owner_edges_lexicographic.end(),
            [&](const Objc3ExecutableMetadataGraphEdge &edge) {
              return edge.edge_kind == edge_kind &&
                     edge.source_owner_identity == source &&
                     edge.target_owner_identity == target;
            });
      };

  boundary.protocol_inheritance_edges_complete = true;
  for (const auto &node : graph.protocol_nodes_lexicographic) {
    for (const auto &target :
         node.inherited_protocol_owner_identities_lexicographic) {
      if (!has_graph_edge("protocol-to-inherited-protocol", node.owner_identity,
                          target)) {
        boundary.protocol_inheritance_edges_complete = false;
      }
    }
  }

  boundary.category_attachment_edges_complete = true;
  for (const auto &node : graph.category_nodes_lexicographic) {
    if (!has_graph_edge("category-to-class", node.owner_identity,
                        node.class_owner_identity)) {
      boundary.category_attachment_edges_complete = false;
    }
    if (node.has_interface &&
        !has_graph_edge("category-to-interface", node.owner_identity,
                        node.interface_owner_identity)) {
      boundary.category_attachment_edges_complete = false;
    }
    if (node.has_implementation &&
        !has_graph_edge("category-to-implementation", node.owner_identity,
                        node.implementation_owner_identity)) {
      boundary.category_attachment_edges_complete = false;
    }
    for (const auto &target :
         node.adopted_protocol_owner_identities_lexicographic) {
      if (!has_graph_edge("category-to-protocol", node.owner_identity, target)) {
        boundary.category_attachment_edges_complete = false;
      }
    }
  }

  boundary.declaration_export_owner_split_complete = true;
  for (const auto &node : graph.property_nodes_lexicographic) {
    if (node.owner_identity.empty() || node.declaration_owner_identity.empty() ||
        node.export_owner_identity.empty() || node.owner_kind.empty() ||
        node.owner_name.empty() || node.property_name.empty()) {
      boundary.declaration_export_owner_split_complete = false;
    }
  }
  for (const auto &node : graph.method_nodes_lexicographic) {
    if (node.owner_identity.empty() || node.declaration_owner_identity.empty() ||
        node.export_owner_identity.empty() || node.owner_kind.empty() ||
        node.owner_name.empty() || node.selector.empty()) {
      boundary.declaration_export_owner_split_complete = false;
    }
  }
  for (const auto &node : graph.ivar_nodes_lexicographic) {
    if (node.owner_identity.empty() || node.declaration_owner_identity.empty() ||
        node.export_owner_identity.empty() || node.property_owner_identity.empty() ||
        node.owner_kind.empty() || node.owner_name.empty() ||
        node.ivar_binding_symbol.empty()) {
      boundary.declaration_export_owner_split_complete = false;
    }
  }

  boundary.property_method_ivar_owner_edges_complete = true;
  for (const auto &node : graph.property_nodes_lexicographic) {
    if (!has_graph_edge("property-to-declaration-owner", node.owner_identity,
                        node.declaration_owner_identity) ||
        !has_graph_edge("property-to-export-owner", node.owner_identity,
                        node.export_owner_identity)) {
      boundary.property_method_ivar_owner_edges_complete = false;
    }
    if (!node.ivar_binding_symbol.empty()) {
      const auto ivar_it = std::find_if(
          graph.ivar_nodes_lexicographic.begin(),
          graph.ivar_nodes_lexicographic.end(),
          [&](const Objc3ExecutableMetadataIvarGraphNode &ivar_node) {
            return ivar_node.property_owner_identity == node.owner_identity &&
                   ivar_node.ivar_binding_symbol == node.ivar_binding_symbol;
          });
      if (ivar_it == graph.ivar_nodes_lexicographic.end() ||
          !has_graph_edge("property-to-ivar", node.owner_identity,
                          ivar_it->owner_identity)) {
        boundary.property_method_ivar_owner_edges_complete = false;
      }
    }
    if (node.has_getter) {
      const bool getter_exists = std::any_of(
          graph.method_nodes_lexicographic.begin(),
          graph.method_nodes_lexicographic.end(),
          [&](const Objc3ExecutableMetadataMethodGraphNode &method_node) {
            return method_node.export_owner_identity ==
                       node.export_owner_identity &&
                   !method_node.is_class_method &&
                   method_node.selector == node.getter_selector;
          });
      if (getter_exists) {
        const auto getter_it = std::find_if(
            graph.method_nodes_lexicographic.begin(),
            graph.method_nodes_lexicographic.end(),
            [&](const Objc3ExecutableMetadataMethodGraphNode &method_node) {
              return method_node.export_owner_identity ==
                         node.export_owner_identity &&
                     !method_node.is_class_method &&
                     method_node.selector == node.getter_selector &&
                     has_graph_edge("property-to-getter-method",
                                    node.owner_identity,
                                    method_node.owner_identity);
            });
        if (getter_it == graph.method_nodes_lexicographic.end()) {
          boundary.property_method_ivar_owner_edges_complete = false;
        }
      }
    }
    if (node.has_setter) {
      const bool setter_exists = std::any_of(
          graph.method_nodes_lexicographic.begin(),
          graph.method_nodes_lexicographic.end(),
          [&](const Objc3ExecutableMetadataMethodGraphNode &method_node) {
            return method_node.export_owner_identity ==
                       node.export_owner_identity &&
                   !method_node.is_class_method &&
                   method_node.selector == node.setter_selector;
          });
      if (setter_exists) {
        const auto setter_it = std::find_if(
            graph.method_nodes_lexicographic.begin(),
            graph.method_nodes_lexicographic.end(),
            [&](const Objc3ExecutableMetadataMethodGraphNode &method_node) {
              return method_node.export_owner_identity ==
                         node.export_owner_identity &&
                     !method_node.is_class_method &&
                     method_node.selector == node.setter_selector &&
                     has_graph_edge("property-to-setter-method",
                                    node.owner_identity,
                                    method_node.owner_identity);
            });
        if (setter_it == graph.method_nodes_lexicographic.end()) {
          boundary.property_method_ivar_owner_edges_complete = false;
        }
      }
    }
  }

  for (const auto &node : graph.method_nodes_lexicographic) {
    if (!has_graph_edge("method-to-declaration-owner", node.owner_identity,
                        node.declaration_owner_identity) ||
        !has_graph_edge("method-to-export-owner", node.owner_identity,
                        node.export_owner_identity)) {
      boundary.property_method_ivar_owner_edges_complete = false;
    }
  }

  for (const auto &node : graph.ivar_nodes_lexicographic) {
    if (!has_graph_edge("ivar-to-declaration-owner", node.owner_identity,
                        node.declaration_owner_identity) ||
        !has_graph_edge("ivar-to-export-owner", node.owner_identity,
                        node.export_owner_identity) ||
        !has_graph_edge("ivar-to-property", node.owner_identity,
                        node.property_owner_identity)) {
      boundary.property_method_ivar_owner_edges_complete = false;
    }
  }

  if (boundary.contract_id.empty()) {
    boundary.failure_reason =
        "metadata semantic consistency contract id is empty";
  } else if (boundary.executable_metadata_source_graph_contract_id.empty()) {
    boundary.failure_reason =
        "metadata semantic consistency graph contract id is empty";
  } else if (!boundary.source_graph_ready) {
    boundary.failure_reason =
        "executable metadata source graph is not ready";
  } else if (!boundary.protocol_category_handoff_deterministic) {
    boundary.failure_reason =
        "protocol/category semantic handoff is not deterministic";
  } else if (!boundary.class_protocol_category_linking_deterministic) {
    boundary.failure_reason =
        "class/protocol/category linking handoff is not deterministic";
  } else if (!boundary.selector_normalization_deterministic) {
    boundary.failure_reason =
        "selector normalization handoff is not deterministic";
  } else if (!boundary.property_attribute_deterministic) {
    boundary.failure_reason =
        "property attribute handoff is not deterministic";
  } else if (!boundary.symbol_graph_scope_resolution_deterministic) {
    boundary.failure_reason =
        "symbol-graph/scope-resolution handoff is not deterministic";
  } else if (!boundary.protocol_inheritance_edges_complete) {
    boundary.failure_reason =
        "protocol inheritance edges are incomplete";
  } else if (!boundary.category_attachment_edges_complete) {
    boundary.failure_reason =
        "category attachment edges are incomplete";
  } else if (!boundary.declaration_export_owner_split_complete) {
    boundary.failure_reason =
        "declaration/export owner identities are incomplete";
  } else if (!boundary.property_method_ivar_owner_edges_complete) {
    boundary.failure_reason =
        "property/method/ivar owner edges are incomplete";
  }

  boundary.semantic_boundary_frozen = boundary.failure_reason.empty();
  boundary.lowering_admission_ready = false;
  boundary.fail_closed =
      boundary.semantic_boundary_frozen &&
      !boundary.lowering_admission_ready &&
      boundary.semantic_conflict_diagnostics_enforcement_pending &&
      boundary.duplicate_export_owner_enforcement_pending &&
      boundary.lowering_admission_pending;
  if (boundary.failure_reason.empty() && !boundary.fail_closed) {
    boundary.failure_reason =
        "metadata semantic consistency freeze is not fail-closed";
  }
  return boundary;
}

Objc3ExecutableMetadataSemanticValidationSurface
BuildExecutableMetadataSemanticValidationSurface(
    const Objc3ExecutableMetadataSourceGraph &graph,
    const Objc3ExecutableMetadataSemanticConsistencyBoundary
        &semantic_consistency_boundary,
    const Objc3SemanticTypeMetadataHandoff &sema_type_metadata_handoff,
    const Objc3FrontendClassProtocolCategoryLinkingSummary
        &class_protocol_category_linking_summary) {
  Objc3ExecutableMetadataSemanticValidationSurface surface;
  surface.executable_metadata_semantic_consistency_contract_id =
      semantic_consistency_boundary.contract_id;
  surface.semantic_consistency_ready =
      IsReadyObjc3ExecutableMetadataSemanticConsistencyBoundary(
          semantic_consistency_boundary);

  const Objc3MethodLookupOverrideConflictSummary
      &method_lookup_override_conflict_summary =
          sema_type_metadata_handoff.method_lookup_override_conflict_summary;
  surface.method_lookup_override_conflict_handoff_deterministic =
      method_lookup_override_conflict_summary.deterministic;
  surface.class_protocol_category_linking_deterministic =
      class_protocol_category_linking_summary
          .deterministic_class_protocol_category_linking_handoff;

  surface.override_lookup_sites =
      method_lookup_override_conflict_summary.override_lookup_sites;
  surface.override_lookup_hits =
      method_lookup_override_conflict_summary.override_lookup_hits;
  surface.override_lookup_misses =
      method_lookup_override_conflict_summary.override_lookup_misses;
  surface.override_conflicts =
      method_lookup_override_conflict_summary.override_conflicts;
  surface.unresolved_base_interfaces =
      method_lookup_override_conflict_summary.unresolved_base_interfaces;
  surface.protocol_composition_sites =
      class_protocol_category_linking_summary.protocol_composition_sites;
  surface.protocol_composition_symbols =
      class_protocol_category_linking_summary.protocol_composition_symbols;
  surface.category_composition_sites =
      class_protocol_category_linking_summary.category_composition_sites;
  surface.category_composition_symbols =
      class_protocol_category_linking_summary.category_composition_symbols;
  surface.invalid_protocol_composition_sites =
      class_protocol_category_linking_summary.invalid_protocol_composition_sites;

  const auto count_edges = [&](const std::string &edge_kind) {
    return static_cast<std::size_t>(std::count_if(
        graph.owner_edges_lexicographic.begin(),
        graph.owner_edges_lexicographic.end(),
        [&](const Objc3ExecutableMetadataGraphEdge &edge) {
          return edge.edge_kind == edge_kind;
        }));
  };
  const auto has_edge = [&](const std::string &edge_kind,
                            const std::string &source_owner_identity,
                            const std::string &target_owner_identity) {
    return std::any_of(
        graph.owner_edges_lexicographic.begin(),
        graph.owner_edges_lexicographic.end(),
        [&](const Objc3ExecutableMetadataGraphEdge &edge) {
          return edge.edge_kind == edge_kind &&
                 edge.source_owner_identity == source_owner_identity &&
                 edge.target_owner_identity == target_owner_identity;
        });
  };

  surface.class_inheritance_edge_count = count_edges("class-to-superclass");
  surface.protocol_inheritance_edge_count =
      count_edges("protocol-to-inherited-protocol");
  surface.metaclass_super_edge_count =
      count_edges("metaclass-to-super-metaclass");
  surface.override_edge_count = count_edges("method-to-overridden-method");

  std::unordered_map<std::string, const Objc3ExecutableMetadataClassGraphNode *>
      class_nodes_by_owner_identity;
  class_nodes_by_owner_identity.reserve(graph.class_nodes_lexicographic.size());
  for (const auto &class_node : graph.class_nodes_lexicographic) {
    class_nodes_by_owner_identity.emplace(class_node.owner_identity, &class_node);
  }

  std::unordered_map<
      std::string, const Objc3ExecutableMetadataMetaclassGraphNode *>
      metaclass_nodes_by_owner_identity;
  metaclass_nodes_by_owner_identity.reserve(
      graph.metaclass_nodes_lexicographic.size());
  for (const auto &metaclass_node : graph.metaclass_nodes_lexicographic) {
    metaclass_nodes_by_owner_identity.emplace(metaclass_node.owner_identity,
                                              &metaclass_node);
  }

  std::unordered_map<
      std::string, const Objc3ExecutableMetadataProtocolGraphNode *>
      protocol_nodes_by_owner_identity;
  protocol_nodes_by_owner_identity.reserve(
      graph.protocol_nodes_lexicographic.size());
  for (const auto &protocol_node : graph.protocol_nodes_lexicographic) {
    protocol_nodes_by_owner_identity.emplace(protocol_node.owner_identity,
                                             &protocol_node);
  }

  std::unordered_map<
      std::string, const Objc3ExecutableMetadataInterfaceGraphNode *>
      interface_nodes_by_owner_identity;
  interface_nodes_by_owner_identity.reserve(
      graph.interface_nodes_lexicographic.size());
  for (const auto &interface_node : graph.interface_nodes_lexicographic) {
    interface_nodes_by_owner_identity.emplace(interface_node.owner_identity,
                                              &interface_node);
  }

  std::unordered_map<std::string, const Objc3ExecutableMetadataMethodGraphNode *>
      class_interface_methods_by_key;
  class_interface_methods_by_key.reserve(graph.method_nodes_lexicographic.size());
  const auto build_interface_method_key =
      [](const std::string &declaration_owner_identity,
         const std::string &selector, bool is_class_method) {
        return declaration_owner_identity + "::" +
               (is_class_method ? "class" : "instance") + "::" + selector;
      };
  for (const auto &method_node : graph.method_nodes_lexicographic) {
    if (method_node.owner_kind != "class-interface") {
      continue;
    }
    class_interface_methods_by_key.emplace(
        build_interface_method_key(method_node.declaration_owner_identity,
                                   method_node.selector,
                                   method_node.is_class_method),
        &method_node);
  }

  surface.class_inheritance_edges_complete = true;
  surface.inheritance_chain_cycle_free = true;
  surface.superclass_targets_resolved = true;
  for (const auto &class_node : graph.class_nodes_lexicographic) {
    if (!class_node.has_super) {
      continue;
    }
    if (!has_edge("class-to-superclass", class_node.owner_identity,
                  class_node.super_class_owner_identity)) {
      surface.class_inheritance_edges_complete = false;
    }
    std::string next_super_owner_identity = class_node.super_class_owner_identity;
    std::unordered_set<std::string> visited;
    while (!next_super_owner_identity.empty()) {
      if (!visited.insert(next_super_owner_identity).second) {
        surface.inheritance_chain_cycle_free = false;
        break;
      }
      const auto class_it =
          class_nodes_by_owner_identity.find(next_super_owner_identity);
      if (class_it == class_nodes_by_owner_identity.end()) {
        surface.superclass_targets_resolved = false;
        break;
      }
      next_super_owner_identity = class_it->second->super_class_owner_identity;
    }
  }

  surface.protocol_inheritance_edges_complete = true;
  surface.protocol_inheritance_targets_resolved = true;
  for (const auto &protocol_node : graph.protocol_nodes_lexicographic) {
    for (const auto &target_owner_identity :
         protocol_node.inherited_protocol_owner_identities_lexicographic) {
      if (!has_edge("protocol-to-inherited-protocol",
                    protocol_node.owner_identity, target_owner_identity)) {
        surface.protocol_inheritance_edges_complete = false;
      }
      if (protocol_nodes_by_owner_identity.find(target_owner_identity) ==
          protocol_nodes_by_owner_identity.end()) {
        surface.protocol_inheritance_targets_resolved = false;
      }
    }
  }

  surface.metaclass_edges_complete = true;
  surface.metaclass_targets_resolved = true;
  surface.metaclass_lineage_aligned = true;
  for (const auto &metaclass_node : graph.metaclass_nodes_lexicographic) {
    const auto class_it =
        class_nodes_by_owner_identity.find(metaclass_node.class_owner_identity);
    if (class_it == class_nodes_by_owner_identity.end()) {
      surface.metaclass_targets_resolved = false;
      surface.metaclass_lineage_aligned = false;
      continue;
    }
    if (!has_edge("class-to-metaclass", metaclass_node.class_owner_identity,
                  metaclass_node.owner_identity)) {
      surface.metaclass_edges_complete = false;
    }
    if (class_it->second->metaclass_owner_identity != metaclass_node.owner_identity) {
      surface.metaclass_lineage_aligned = false;
    }
    if (metaclass_node.has_super) {
      if (!has_edge("metaclass-to-super-metaclass", metaclass_node.owner_identity,
                    metaclass_node.super_metaclass_owner_identity)) {
        surface.metaclass_edges_complete = false;
      }
      const auto super_metaclass_it =
          metaclass_nodes_by_owner_identity.find(
              metaclass_node.super_metaclass_owner_identity);
      if (super_metaclass_it == metaclass_nodes_by_owner_identity.end()) {
        surface.metaclass_targets_resolved = false;
        surface.metaclass_lineage_aligned = false;
      } else if (class_it->second->has_super &&
                 super_metaclass_it->second->class_owner_identity !=
                     class_it->second->super_class_owner_identity) {
        surface.metaclass_lineage_aligned = false;
      }
    } else if (!metaclass_node.super_metaclass_owner_identity.empty()) {
      surface.metaclass_lineage_aligned = false;
    }
  }

  surface.method_override_edges_complete = true;
  for (const auto &method_node : graph.method_nodes_lexicographic) {
    if (method_node.owner_kind != "class-interface") {
      continue;
    }
    const auto interface_it = interface_nodes_by_owner_identity.find(
        method_node.declaration_owner_identity);
    if (interface_it == interface_nodes_by_owner_identity.end() ||
        !interface_it->second->has_super) {
      continue;
    }

    std::string next_super_owner_identity =
        interface_it->second->super_class_owner_identity;
    const Objc3ExecutableMetadataMethodGraphNode *overridden_method = nullptr;
    std::unordered_set<std::string> visited;
    while (!next_super_owner_identity.empty()) {
      if (!visited.insert(next_super_owner_identity).second) {
        break;
      }
      const auto class_it =
          class_nodes_by_owner_identity.find(next_super_owner_identity);
      if (class_it == class_nodes_by_owner_identity.end() ||
          class_it->second->interface_owner_identity.empty()) {
        break;
      }
      const auto base_method_it = class_interface_methods_by_key.find(
          build_interface_method_key(class_it->second->interface_owner_identity,
                                     method_node.selector,
                                     method_node.is_class_method));
      if (base_method_it != class_interface_methods_by_key.end()) {
        overridden_method = base_method_it->second;
        break;
      }
      next_super_owner_identity = class_it->second->super_class_owner_identity;
    }
    if (overridden_method == nullptr) {
      continue;
    }
    if (!has_edge("method-to-overridden-method", method_node.owner_identity,
                  overridden_method->owner_identity)) {
      surface.method_override_edges_complete = false;
    } else if (method_node.is_class_method) {
      ++surface.class_method_override_edge_count;
    } else {
      ++surface.instance_method_override_edge_count;
    }
  }

  surface.override_lookup_complete =
      method_lookup_override_conflict_summary.unresolved_base_interfaces == 0u;
  surface.override_conflicts_absent =
      method_lookup_override_conflict_summary.override_conflicts == 0u;
  surface.protocol_composition_valid =
      class_protocol_category_linking_summary.invalid_protocol_composition_sites ==
          0u &&
      class_protocol_category_linking_summary.protocol_composition_symbols >=
          class_protocol_category_linking_summary
              .category_composition_symbols &&
      class_protocol_category_linking_summary.protocol_composition_sites >=
          class_protocol_category_linking_summary.category_composition_sites;

  surface.inheritance_validation_ready =
      surface.class_inheritance_edges_complete &&
      surface.protocol_inheritance_edges_complete &&
      surface.inheritance_chain_cycle_free &&
      surface.superclass_targets_resolved &&
      surface.protocol_inheritance_targets_resolved;
  surface.override_validation_ready =
      surface.method_lookup_override_conflict_handoff_deterministic &&
      surface.method_override_edges_complete &&
      surface.override_lookup_complete &&
      surface.override_conflicts_absent;
  surface.protocol_composition_validation_ready =
      surface.class_protocol_category_linking_deterministic &&
      surface.protocol_composition_valid;
  surface.metaclass_relationship_validation_ready =
      surface.metaclass_edges_complete &&
      surface.metaclass_targets_resolved &&
      surface.metaclass_lineage_aligned;

  if (surface.contract_id.empty()) {
    surface.failure_reason =
        "executable metadata semantic validation contract id is empty";
  } else if (surface.executable_metadata_semantic_consistency_contract_id.empty()) {
    surface.failure_reason =
        "executable metadata semantic consistency dependency contract id is empty";
  } else if (!surface.semantic_consistency_ready) {
    surface.failure_reason =
        "executable metadata semantic consistency boundary is not ready";
  } else if (!surface.method_lookup_override_conflict_handoff_deterministic) {
    surface.failure_reason =
        "method lookup/override conflict handoff is not deterministic";
  } else if (!surface.class_protocol_category_linking_deterministic) {
    surface.failure_reason =
        "class/protocol/category linking handoff is not deterministic";
  } else if (!surface.inheritance_validation_ready) {
    surface.failure_reason =
        "inheritance validation is incomplete";
  } else if (!surface.override_validation_ready) {
    surface.failure_reason = "override validation is incomplete";
  } else if (!surface.protocol_composition_validation_ready) {
    surface.failure_reason =
        "protocol composition validation is incomplete";
  } else if (!surface.metaclass_relationship_validation_ready) {
    surface.failure_reason =
        "metaclass relationship validation is incomplete";
  }

  surface.semantic_validation_complete = surface.failure_reason.empty();
  surface.lowering_admission_ready = false;
  surface.fail_closed = surface.semantic_validation_complete &&
                        !surface.lowering_admission_ready;
  if (surface.failure_reason.empty() && !surface.fail_closed) {
    surface.failure_reason =
        "executable metadata semantic validation surface is not fail-closed";
  }
  return surface;
}

static std::string BuildExecutableMetadataLoweringHandoffReplayKey(
    const Objc3ExecutableMetadataLoweringHandoffSurface &surface) {
  std::ostringstream out;
  out << "executable-metadata-lowering-handoff:v1"
      << ";source_graph_ready=" << (surface.source_graph_ready ? "true" : "false")
      << ";semantic_consistency_ready="
      << (surface.semantic_consistency_ready ? "true" : "false")
      << ";semantic_validation_ready="
      << (surface.semantic_validation_ready ? "true" : "false")
      << ";semantic_type_metadata_handoff_deterministic="
      << (surface.semantic_type_metadata_handoff_deterministic ? "true"
                                                               : "false")
      << ";protocol_category_handoff_deterministic="
      << (surface.protocol_category_handoff_deterministic ? "true" : "false")
      << ";class_protocol_category_linking_handoff_deterministic="
      << (surface.class_protocol_category_linking_handoff_deterministic ? "true"
                                                                        : "false")
      << ";selector_normalization_handoff_deterministic="
      << (surface.selector_normalization_handoff_deterministic ? "true"
                                                               : "false")
      << ";property_attribute_handoff_deterministic="
      << (surface.property_attribute_handoff_deterministic ? "true" : "false")
      << ";symbol_graph_scope_resolution_handoff_deterministic="
      << (surface.symbol_graph_scope_resolution_handoff_deterministic ? "true"
                                                                      : "false")
      << ";property_synthesis_ivar_binding_handoff_deterministic="
      << (surface.property_synthesis_ivar_binding_handoff_deterministic ? "true"
                                                                        : "false")
      << ";interface_node_count=" << surface.interface_node_count
      << ";implementation_node_count=" << surface.implementation_node_count
      << ";class_node_count=" << surface.class_node_count
      << ";metaclass_node_count=" << surface.metaclass_node_count
      << ";protocol_node_count=" << surface.protocol_node_count
      << ";category_node_count=" << surface.category_node_count
      << ";property_node_count=" << surface.property_node_count
      << ";method_node_count=" << surface.method_node_count
      << ";ivar_node_count=" << surface.ivar_node_count
      << ";owner_edge_count=" << surface.owner_edge_count
      << ";ready_for_lowering=" << (surface.ready_for_lowering ? "true"
                                                               : "false");
  return out.str();
}

Objc3ExecutableMetadataLoweringHandoffSurface
BuildExecutableMetadataLoweringHandoffSurface(
    const Objc3ExecutableMetadataSourceGraph &graph,
    const Objc3ExecutableMetadataSemanticConsistencyBoundary
        &semantic_consistency_boundary,
    const Objc3ExecutableMetadataSemanticValidationSurface
        &semantic_validation_surface,
    const Objc3SemanticTypeMetadataHandoff &sema_type_metadata_handoff,
    const Objc3FrontendProtocolCategorySummary &protocol_category_summary,
    const Objc3FrontendClassProtocolCategoryLinkingSummary
        &class_protocol_category_linking_summary,
    const Objc3FrontendSelectorNormalizationSummary
        &selector_normalization_summary,
    const Objc3FrontendPropertyAttributeSummary &property_attribute_summary,
    const Objc3FrontendSymbolGraphScopeResolutionSummary
        &symbol_graph_scope_resolution_summary,
    const Objc3SemaParityContractSurface &sema_parity_surface) {
  Objc3ExecutableMetadataLoweringHandoffSurface surface;
  surface.executable_metadata_source_graph_contract_id = graph.contract_id;
  surface.executable_metadata_semantic_consistency_contract_id =
      semantic_consistency_boundary.contract_id;
  surface.executable_metadata_semantic_validation_contract_id =
      semantic_validation_surface.contract_id;
  surface.source_graph_ready = IsReadyObjc3ExecutableMetadataSourceGraph(graph);
  surface.semantic_consistency_ready =
      IsReadyObjc3ExecutableMetadataSemanticConsistencyBoundary(
          semantic_consistency_boundary);
  surface.semantic_validation_ready =
      IsReadyObjc3ExecutableMetadataSemanticValidationSurface(
          semantic_validation_surface);
  surface.semantic_type_metadata_handoff_deterministic =
      IsDeterministicSemanticTypeMetadataHandoff(sema_type_metadata_handoff);
  surface.protocol_category_handoff_deterministic =
      protocol_category_summary.deterministic_protocol_category_handoff;
  surface.class_protocol_category_linking_handoff_deterministic =
      class_protocol_category_linking_summary
          .deterministic_class_protocol_category_linking_handoff;
  surface.selector_normalization_handoff_deterministic =
      selector_normalization_summary.deterministic_selector_normalization_handoff;
  surface.property_attribute_handoff_deterministic =
      property_attribute_summary.deterministic_property_attribute_handoff;
  surface.symbol_graph_scope_resolution_handoff_deterministic =
      symbol_graph_scope_resolution_summary.deterministic_symbol_graph_handoff &&
      symbol_graph_scope_resolution_summary
          .deterministic_scope_resolution_handoff;
  surface.property_synthesis_ivar_binding_handoff_deterministic =
      sema_parity_surface.property_synthesis_ivar_binding_summary.deterministic &&
      sema_parity_surface
          .deterministic_property_synthesis_ivar_binding_handoff;
  surface.interface_node_count = graph.interface_nodes_lexicographic.size();
  surface.implementation_node_count =
      graph.implementation_nodes_lexicographic.size();
  surface.class_node_count = graph.class_nodes_lexicographic.size();
  surface.metaclass_node_count = graph.metaclass_nodes_lexicographic.size();
  surface.protocol_node_count = graph.protocol_nodes_lexicographic.size();
  surface.category_node_count = graph.category_nodes_lexicographic.size();
  surface.property_node_count = graph.property_nodes_lexicographic.size();
  surface.method_node_count = graph.method_nodes_lexicographic.size();
  surface.ivar_node_count = graph.ivar_nodes_lexicographic.size();
  surface.owner_edge_count = graph.owner_edges_lexicographic.size();

  if (surface.contract_id.empty()) {
    surface.failure_reason =
        "metadata lowering handoff contract id is empty";
  } else if (surface.executable_metadata_source_graph_contract_id.empty()) {
    surface.failure_reason =
        "metadata lowering handoff graph contract id is empty";
  } else if (surface.executable_metadata_semantic_consistency_contract_id.empty()) {
    surface.failure_reason =
        "metadata lowering handoff semantic consistency contract id is empty";
  } else if (surface.executable_metadata_semantic_validation_contract_id.empty()) {
    surface.failure_reason =
        "metadata lowering handoff semantic validation contract id is empty";
  } else if (!surface.source_graph_ready) {
    surface.failure_reason =
        "executable metadata source graph is not ready for lowering handoff";
  } else if (!surface.semantic_consistency_ready) {
    surface.failure_reason =
        "executable metadata semantic consistency boundary is not ready";
  } else if (!surface.semantic_validation_ready) {
    surface.failure_reason =
        "executable metadata semantic validation surface is not ready";
  } else if (!surface.semantic_type_metadata_handoff_deterministic) {
    surface.failure_reason =
        "semantic type metadata handoff is not deterministic";
  } else if (!surface.protocol_category_handoff_deterministic) {
    surface.failure_reason =
        "protocol/category metadata handoff is not deterministic";
  } else if (!surface.class_protocol_category_linking_handoff_deterministic) {
    surface.failure_reason =
        "class/protocol/category metadata linking handoff is not deterministic";
  } else if (!surface.selector_normalization_handoff_deterministic) {
    surface.failure_reason =
        "selector normalization handoff is not deterministic";
  } else if (!surface.property_attribute_handoff_deterministic) {
    surface.failure_reason =
        "property attribute handoff is not deterministic";
  } else if (!surface.symbol_graph_scope_resolution_handoff_deterministic) {
    surface.failure_reason =
        "symbol graph and scope resolution handoff is not deterministic";
  } else if (!surface.property_synthesis_ivar_binding_handoff_deterministic) {
    surface.failure_reason =
        "property synthesis and ivar binding handoff is not deterministic";
  }

  surface.lowering_schema_frozen = surface.failure_reason.empty();
  surface.ready_for_lowering = false;
  surface.fail_closed =
      surface.lowering_schema_frozen && !surface.ready_for_lowering;
  surface.replay_key = BuildExecutableMetadataLoweringHandoffReplayKey(surface);
  if (surface.failure_reason.empty() && !surface.fail_closed) {
    surface.failure_reason =
        "metadata lowering handoff freeze is not fail-closed";
  }
  if (!surface.failure_reason.empty()) {
    surface.replay_key.clear();
  }
  return surface;
}

static std::string BuildExecutableMetadataTypedLoweringHandoffReplayKey(
    const Objc3ExecutableMetadataTypedLoweringHandoff &surface) {
  std::ostringstream out;
  out << "executable-metadata-typed-lowering-handoff:v1"
      << ";graph_contract=" << surface.executable_metadata_source_graph_contract_id
      << ";protocol_category_source_closure_contract="
      << surface.source_graph.protocol_category_source_closure_contract_id
      << ";protocol_inheritance_identity_model="
      << surface.source_graph.protocol_inheritance_identity_model
      << ";category_attachment_identity_model="
      << surface.source_graph.category_attachment_identity_model
      << ";protocol_category_conformance_identity_model="
      << surface.source_graph.protocol_category_conformance_identity_model
      << ";semantic_consistency_contract="
      << surface.executable_metadata_semantic_consistency_contract_id
      << ";semantic_validation_contract="
      << surface.executable_metadata_semantic_validation_contract_id
      << ";c001_contract="
      << surface.executable_metadata_lowering_handoff_contract_id
      << ";schema_ordering_model=" << surface.manifest_schema_ordering_model
      << ";deterministic=" << (surface.deterministic ? "true" : "false")
      << ";ready_for_lowering="
      << (surface.ready_for_lowering ? "true" : "false");

  const Objc3ExecutableMetadataSourceGraph &graph = surface.source_graph;
  for (const auto &node : graph.interface_nodes_lexicographic) {
    out << ";interface=" << node.class_name << "|" << node.owner_identity << "|"
        << node.class_owner_identity << "|" << node.metaclass_owner_identity
        << "|" << node.super_class_owner_identity << "|"
        << (node.has_super ? "true" : "false") << "|" << node.property_count
        << "|" << node.method_count << "|" << node.class_method_count << "|"
        << node.instance_method_count << "|" << node.line << "|"
        << node.column;
  }
  for (const auto &node : graph.implementation_nodes_lexicographic) {
    out << ";implementation=" << node.class_name << "|" << node.owner_identity
        << "|" << node.interface_owner_identity << "|"
        << node.class_owner_identity << "|" << node.metaclass_owner_identity
        << "|" << (node.has_matching_interface ? "true" : "false") << "|"
        << node.property_count << "|" << node.method_count << "|"
        << node.class_method_count << "|" << node.instance_method_count << "|"
        << node.line << "|" << node.column;
  }
  for (const auto &node : graph.class_nodes_lexicographic) {
    out << ";class=" << node.class_name << "|" << node.owner_identity << "|"
        << node.interface_owner_identity << "|"
        << node.implementation_owner_identity << "|"
        << node.metaclass_owner_identity << "|" << node.super_class_owner_identity
        << "|" << (node.has_interface ? "true" : "false") << "|"
        << (node.has_implementation ? "true" : "false") << "|"
        << (node.has_super ? "true" : "false") << "|"
        << node.interface_property_count << "|"
        << node.implementation_property_count << "|"
        << node.interface_method_count << "|" << node.implementation_method_count
        << "|" << node.interface_class_method_count << "|"
        << node.implementation_class_method_count << "|"
        << node.interface_instance_method_count << "|"
        << node.implementation_instance_method_count << "|" << node.line << "|"
        << node.column;
  }
  for (const auto &node : graph.metaclass_nodes_lexicographic) {
    out << ";metaclass=" << node.class_name << "|" << node.owner_identity << "|"
        << node.class_owner_identity << "|" << node.interface_owner_identity
        << "|" << node.implementation_owner_identity << "|"
        << node.super_metaclass_owner_identity << "|"
        << (node.derived_from_interface ? "true" : "false") << "|"
        << (node.has_implementation ? "true" : "false") << "|"
        << (node.has_super ? "true" : "false") << "|"
        << node.interface_class_method_count << "|"
        << node.implementation_class_method_count << "|" << node.line << "|"
        << node.column;
  }
  for (const auto &node : graph.protocol_nodes_lexicographic) {
    out << ";protocol=" << node.protocol_name << "|" << node.owner_identity << "|";
    for (const auto &owner : node.inherited_protocol_owner_identities_lexicographic) {
      out << owner << ",";
    }
    out << "|" << node.property_count << "|" << node.method_count << "|"
        << (node.is_forward_declaration ? "true" : "false") << "|"
        << (node.declaration_complete ? "true" : "false") << "|"
        << (node.inherited_protocol_identity_complete ? "true" : "false")
        << "|" << node.line << "|" << node.column;
  }
  for (const auto &node : graph.category_nodes_lexicographic) {
    out << ";category=" << node.class_name << "|" << node.category_name << "|"
        << node.owner_identity << "|" << node.interface_owner_identity << "|"
        << node.implementation_owner_identity << "|" << node.class_owner_identity
        << "|";
    for (const auto &owner : node.adopted_protocol_owner_identities_lexicographic) {
      out << owner << ",";
    }
    out << "|" << (node.has_interface ? "true" : "false") << "|"
        << (node.has_implementation ? "true" : "false") << "|"
        << (node.declaration_complete ? "true" : "false") << "|"
        << (node.attachment_identity_complete ? "true" : "false") << "|"
        << (node.conformance_identity_complete ? "true" : "false") << "|"
        << node.interface_property_count << "|"
        << node.implementation_property_count << "|"
        << node.interface_method_count << "|" << node.implementation_method_count
        << "|" << node.interface_class_method_count << "|"
        << node.implementation_class_method_count << "|" << node.line << "|"
        << node.column;
  }
  for (const auto &node : graph.property_nodes_lexicographic) {
    out << ";property=" << node.owner_kind << "|" << node.owner_name << "|"
        << node.owner_identity << "|" << node.declaration_owner_identity << "|"
        << node.export_owner_identity << "|" << node.property_name << "|"
        << node.type_name << "|" << (node.has_getter ? "true" : "false") << "|"
        << node.getter_selector << "|" << (node.has_setter ? "true" : "false")
        << "|" << node.setter_selector << "|" << node.ivar_binding_symbol
        << "|" << node.line << "|" << node.column;
  }
  for (const auto &node : graph.method_nodes_lexicographic) {
    out << ";method=" << node.owner_kind << "|" << node.owner_name << "|"
        << node.owner_identity << "|" << node.declaration_owner_identity << "|"
        << node.export_owner_identity << "|" << node.selector << "|"
        << (node.is_class_method ? "true" : "false") << "|"
        << (node.has_body ? "true" : "false") << "|" << node.parameter_count
        << "|" << node.return_type_name << "|" << node.line << "|"
        << node.column;
  }
  for (const auto &node : graph.ivar_nodes_lexicographic) {
    out << ";ivar=" << node.owner_kind << "|" << node.owner_name << "|"
        << node.owner_identity << "|" << node.declaration_owner_identity << "|"
        << node.export_owner_identity << "|" << node.property_owner_identity
        << "|" << node.property_name << "|" << node.ivar_binding_symbol << "|"
        << node.line << "|" << node.column;
  }
  for (const auto &edge : graph.owner_edges_lexicographic) {
    out << ";edge=" << edge.edge_kind << "|" << edge.source_owner_identity << "|"
        << edge.target_owner_identity << "|" << edge.line << "|" << edge.column;
  }
  return out.str();
}

Objc3ExecutableMetadataTypedLoweringHandoff
BuildExecutableMetadataTypedLoweringHandoff(
    const Objc3ExecutableMetadataSourceGraph &graph,
    const Objc3ExecutableMetadataSemanticConsistencyBoundary
        &semantic_consistency_boundary,
    const Objc3ExecutableMetadataSemanticValidationSurface
        &semantic_validation_surface,
    const Objc3ExecutableMetadataLoweringHandoffSurface &lowering_handoff_surface) {
  Objc3ExecutableMetadataTypedLoweringHandoff surface;
  surface.executable_metadata_lowering_handoff_contract_id =
      lowering_handoff_surface.contract_id;
  surface.executable_metadata_source_graph_contract_id = graph.contract_id;
  surface.executable_metadata_semantic_consistency_contract_id =
      semantic_consistency_boundary.contract_id;
  surface.executable_metadata_semantic_validation_contract_id =
      semantic_validation_surface.contract_id;
  surface.source_graph_ready = IsReadyObjc3ExecutableMetadataSourceGraph(graph);
  surface.semantic_consistency_ready =
      IsReadyObjc3ExecutableMetadataSemanticConsistencyBoundary(
          semantic_consistency_boundary);
  surface.semantic_validation_ready =
      IsReadyObjc3ExecutableMetadataSemanticValidationSurface(
          semantic_validation_surface);
  surface.lowering_handoff_surface_ready =
      IsReadyObjc3ExecutableMetadataLoweringHandoffSurface(
          lowering_handoff_surface);
  surface.source_graph = graph;

  if (surface.contract_id.empty()) {
    surface.failure_reason =
        "typed lowering handoff contract id is empty";
  } else if (surface.executable_metadata_lowering_handoff_contract_id.empty()) {
    surface.failure_reason =
        "typed lowering handoff C001 contract id is empty";
  } else if (surface.executable_metadata_source_graph_contract_id.empty()) {
    surface.failure_reason =
        "typed lowering handoff source graph contract id is empty";
  } else if (surface.executable_metadata_semantic_consistency_contract_id.empty()) {
    surface.failure_reason =
        "typed lowering handoff semantic consistency contract id is empty";
  } else if (surface.executable_metadata_semantic_validation_contract_id.empty()) {
    surface.failure_reason =
        "typed lowering handoff semantic validation contract id is empty";
  } else if (surface.manifest_schema_ordering_model.empty()) {
    surface.failure_reason =
        "typed lowering handoff manifest schema ordering model is empty";
  } else if (!surface.source_graph_ready) {
    surface.failure_reason =
        "typed lowering handoff source graph is not ready";
  } else if (!surface.semantic_consistency_ready) {
    surface.failure_reason =
        "typed lowering handoff semantic consistency boundary is not ready";
  } else if (!surface.semantic_validation_ready) {
    surface.failure_reason =
        "typed lowering handoff semantic validation surface is not ready";
  } else if (!surface.lowering_handoff_surface_ready) {
    surface.failure_reason =
        "typed lowering handoff C001 freeze surface is not ready";
  } else if (graph.interface_nodes_lexicographic.size() !=
             lowering_handoff_surface.interface_node_count) {
    surface.failure_reason =
        "typed lowering handoff interface node count drifted from C001 freeze";
  } else if (graph.implementation_nodes_lexicographic.size() !=
             lowering_handoff_surface.implementation_node_count) {
    surface.failure_reason =
        "typed lowering handoff implementation node count drifted from C001 freeze";
  } else if (graph.class_nodes_lexicographic.size() !=
             lowering_handoff_surface.class_node_count) {
    surface.failure_reason =
        "typed lowering handoff class node count drifted from C001 freeze";
  } else if (graph.metaclass_nodes_lexicographic.size() !=
             lowering_handoff_surface.metaclass_node_count) {
    surface.failure_reason =
        "typed lowering handoff metaclass node count drifted from C001 freeze";
  } else if (graph.protocol_nodes_lexicographic.size() !=
             lowering_handoff_surface.protocol_node_count) {
    surface.failure_reason =
        "typed lowering handoff protocol node count drifted from C001 freeze";
  } else if (graph.category_nodes_lexicographic.size() !=
             lowering_handoff_surface.category_node_count) {
    surface.failure_reason =
        "typed lowering handoff category node count drifted from C001 freeze";
  } else if (graph.property_nodes_lexicographic.size() !=
             lowering_handoff_surface.property_node_count) {
    surface.failure_reason =
        "typed lowering handoff property node count drifted from C001 freeze";
  } else if (graph.method_nodes_lexicographic.size() !=
             lowering_handoff_surface.method_node_count) {
    surface.failure_reason =
        "typed lowering handoff method node count drifted from C001 freeze";
  } else if (graph.ivar_nodes_lexicographic.size() !=
             lowering_handoff_surface.ivar_node_count) {
    surface.failure_reason =
        "typed lowering handoff ivar node count drifted from C001 freeze";
  } else if (graph.owner_edges_lexicographic.size() !=
             lowering_handoff_surface.owner_edge_count) {
    surface.failure_reason =
        "typed lowering handoff owner edge count drifted from C001 freeze";
  }

  surface.deterministic =
      surface.failure_reason.empty() && graph.deterministic &&
      !lowering_handoff_surface.replay_key.empty();
  surface.manifest_schema_frozen = surface.failure_reason.empty();
  surface.fail_closed = surface.manifest_schema_frozen;
  surface.ready_for_lowering =
      surface.manifest_schema_frozen && surface.deterministic &&
      surface.fail_closed;
  if (surface.failure_reason.empty()) {
    surface.replay_key =
        BuildExecutableMetadataTypedLoweringHandoffReplayKey(surface);
  }
  if (!surface.failure_reason.empty() || !surface.ready_for_lowering) {
    if (surface.failure_reason.empty()) {
      surface.failure_reason =
          "typed lowering handoff did not become lowering-ready";
    }
    surface.replay_key.clear();
  }
  return surface;
}

Objc3RuntimeMetadataSourceOwnershipBoundary BuildRuntimeMetadataSourceOwnershipBoundary(
    const Objc3RuntimeMetadataSourceRecordSet &records,
    const Objc3SemanticTypeMetadataHandoff &type_metadata_handoff) {
  Objc3RuntimeMetadataSourceOwnershipBoundary boundary;
  const std::size_t sema_interface_implementation_record_count =
      type_metadata_handoff.interfaces_lexicographic.size() +
      type_metadata_handoff.implementations_lexicographic.size();
  const bool sema_interface_implementation_record_count_present =
      sema_interface_implementation_record_count > 0u ||
      type_metadata_handoff.interface_implementation_summary.declared_interfaces > 0u ||
      type_metadata_handoff.interface_implementation_summary.declared_implementations > 0u;

  boundary.frontend_owns_runtime_metadata_source_records = true;
  boundary.runtime_metadata_source_records_ready_for_lowering = false;
  boundary.native_runtime_library_present = false;
  boundary.runtime_shim_test_only = true;
  boundary.class_record_count = records.classes_lexicographic.size();
  boundary.protocol_record_count = records.protocols_lexicographic.size();
  boundary.category_interface_record_count =
      static_cast<std::size_t>(std::count_if(records.categories_lexicographic.begin(),
                                             records.categories_lexicographic.end(),
                                             [](const Objc3RuntimeMetadataCategorySourceRecord &record) {
                                               return record.record_kind == "interface";
                                             }));
  boundary.category_implementation_record_count =
      static_cast<std::size_t>(std::count_if(records.categories_lexicographic.begin(),
                                             records.categories_lexicographic.end(),
                                             [](const Objc3RuntimeMetadataCategorySourceRecord &record) {
                                               return record.record_kind == "implementation";
                                             }));
  boundary.property_record_count = records.properties_lexicographic.size();
  boundary.method_record_count = records.methods_lexicographic.size();
  boundary.ivar_record_count = records.ivars_lexicographic.size();

  // diagnostic precision anchor: the semantic type-metadata handoff
  // now treats category containers as a separate runtime-metadata concern so
  // valid class-plus-category programs do not collapse into duplicate
  // class-owner diagnostics. The ownership boundary therefore validates the
  // sema surface against class records only; category records remain parser/AST
  // owned until later runtime-metadata milestones wire a dedicated sema lane.
  const std::size_t source_interface_implementation_record_count =
      boundary.class_record_count;
  const bool class_alignment_consistent =
      !sema_interface_implementation_record_count_present ||
      sema_interface_implementation_record_count ==
          source_interface_implementation_record_count;
  boundary.deterministic_source_schema =
      IsReadyObjc3RuntimeMetadataSourceRecordSet(records) &&
      class_alignment_consistent &&
      boundary.ivar_record_count <= boundary.property_record_count &&
      !boundary.contract_id.empty() &&
      !boundary.canonical_source_schema.empty() &&
      !boundary.class_record_ast_anchor.empty() &&
      !boundary.protocol_record_ast_anchor.empty() &&
      !boundary.category_record_ast_anchor.empty() &&
      !boundary.property_record_ast_anchor.empty() &&
      !boundary.method_record_ast_anchor.empty() &&
      !boundary.ivar_record_ast_anchor.empty() &&
      !boundary.ivar_record_source_model.empty();
  boundary.fail_closed =
      boundary.frontend_owns_runtime_metadata_source_records &&
      !boundary.runtime_metadata_source_records_ready_for_lowering &&
      !boundary.native_runtime_library_present &&
      boundary.runtime_shim_test_only;

  if (!class_alignment_consistent) {
    boundary.failure_reason = "AST/sema class metadata source counts diverged";
  } else if (boundary.ivar_record_count > boundary.property_record_count) {
    boundary.failure_reason = "ivar source records exceed property source records";
  } else if (!boundary.deterministic_source_schema) {
    boundary.failure_reason = "runtime metadata source schema anchors are incomplete";
  } else if (!boundary.fail_closed) {
    boundary.failure_reason = "runtime metadata source ownership boundary is not fail-closed";
  }

  return boundary;
}

Objc3RuntimeExportLegalityBoundary BuildRuntimeExportLegalityBoundary(
    const Objc3RuntimeMetadataSourceOwnershipBoundary &runtime_metadata_source_ownership,
    const Objc3TypedSemaToLoweringContractSurface &typed_surface,
    const Objc3SemanticIntegrationSurface &integration_surface,
    const Objc3FrontendProtocolCategorySummary &protocol_category_summary,
    const Objc3FrontendClassProtocolCategoryLinkingSummary &class_protocol_category_linking_summary,
    const Objc3FrontendSelectorNormalizationSummary &selector_normalization_summary,
    const Objc3FrontendPropertyAttributeSummary &property_attribute_summary,
    const Objc3FrontendObjectPointerNullabilityGenericsSummary &object_pointer_summary,
    const Objc3FrontendSymbolGraphScopeResolutionSummary &symbol_graph_scope_resolution_summary,
    const Objc3SemaParityContractSurface &sema_parity_surface) {
  Objc3RuntimeExportLegalityBoundary boundary;
  boundary.semantic_integration_surface_built = integration_surface.built;
  boundary.sema_type_metadata_handoff_deterministic =
      typed_surface.semantic_type_metadata_handoff_deterministic &&
      sema_parity_surface.deterministic_type_metadata_handoff;
  boundary.typed_sema_surface_ready =
      typed_surface.semantic_integration_surface_built;
  boundary.typed_sema_surface_deterministic =
      typed_surface.semantic_type_metadata_handoff_deterministic &&
      typed_surface.protocol_category_handoff_deterministic &&
      typed_surface.class_protocol_category_linking_handoff_deterministic &&
      typed_surface.selector_normalization_handoff_deterministic &&
      typed_surface.property_attribute_handoff_deterministic &&
      typed_surface.object_pointer_type_handoff_deterministic &&
      typed_surface.symbol_graph_handoff_deterministic &&
      typed_surface.scope_resolution_handoff_deterministic;
  boundary.runtime_metadata_source_boundary_ready =
      IsReadyObjc3RuntimeMetadataSourceOwnershipBoundary(
          runtime_metadata_source_ownership);
  boundary.protocol_category_deterministic =
      protocol_category_summary.deterministic_protocol_category_handoff;
  boundary.class_protocol_category_linking_deterministic =
      class_protocol_category_linking_summary
          .deterministic_class_protocol_category_linking_handoff;
  boundary.selector_normalization_deterministic =
      selector_normalization_summary.deterministic_selector_normalization_handoff;
  boundary.property_attribute_deterministic =
      property_attribute_summary.deterministic_property_attribute_handoff;
  boundary.object_pointer_surface_deterministic =
      object_pointer_summary
          .deterministic_object_pointer_nullability_generics_handoff;
  boundary.symbol_graph_scope_resolution_deterministic =
      symbol_graph_scope_resolution_summary.deterministic_symbol_graph_handoff &&
      symbol_graph_scope_resolution_summary.deterministic_scope_resolution_handoff;
  boundary.property_synthesis_ivar_binding_deterministic =
      sema_parity_surface.property_synthesis_ivar_binding_summary.deterministic &&
      sema_parity_surface.deterministic_property_synthesis_ivar_binding_handoff;

  boundary.class_record_count = runtime_metadata_source_ownership.class_record_count;
  boundary.protocol_record_count =
      runtime_metadata_source_ownership.protocol_record_count;
  boundary.category_record_count =
      runtime_metadata_source_ownership.category_record_count();
  boundary.property_record_count =
      runtime_metadata_source_ownership.property_record_count;
  boundary.method_record_count = runtime_metadata_source_ownership.method_record_count;
  boundary.ivar_record_count = runtime_metadata_source_ownership.ivar_record_count;
  boundary.invalid_protocol_composition_sites =
      class_protocol_category_linking_summary.invalid_protocol_composition_sites;
  boundary.property_attribute_invalid_entries =
      sema_parity_surface.property_attribute_invalid_attribute_entries_total;
  boundary.property_attribute_contract_violations =
      sema_parity_surface.property_attribute_contract_violations_total;
  boundary.invalid_type_annotation_sites =
      sema_parity_surface.type_annotation_invalid_generic_suffix_sites_total +
      sema_parity_surface.type_annotation_invalid_pointer_declarator_sites_total +
      sema_parity_surface.type_annotation_invalid_nullability_suffix_sites_total +
      sema_parity_surface.type_annotation_invalid_ownership_qualifier_sites_total;
  boundary.property_ivar_binding_missing =
      sema_parity_surface.property_synthesis_ivar_binding_summary
          .ivar_binding_missing;
  boundary.property_ivar_binding_conflicts =
      sema_parity_surface.property_synthesis_ivar_binding_summary
          .ivar_binding_conflicts;
  boundary.implementation_resolution_misses =
      symbol_graph_scope_resolution_summary.implementation_interface_resolution_misses;
  boundary.method_resolution_misses =
      symbol_graph_scope_resolution_summary.method_resolution_misses;

  if (boundary.contract_id.empty()) {
    boundary.failure_reason = "runtime export legality contract id is empty";
  } else if (!boundary.sema_type_metadata_handoff_deterministic) {
    boundary.failure_reason =
        "semantic type-metadata handoff is not deterministic";
  } else if (!boundary.typed_sema_surface_ready) {
    boundary.failure_reason =
        "typed sema runtime-export handoff is not ready";
  } else if (!boundary.typed_sema_surface_deterministic) {
    boundary.failure_reason =
        "typed sema runtime-export handoff is not deterministic";
  } else if (!boundary.runtime_metadata_source_boundary_ready) {
    boundary.failure_reason =
        "runtime metadata source ownership boundary is not ready";
  } else if (!boundary.protocol_category_deterministic) {
    boundary.failure_reason =
        "protocol/category semantic handoff is not deterministic";
  } else if (!boundary.class_protocol_category_linking_deterministic) {
    boundary.failure_reason =
        "class/protocol/category linking handoff is not deterministic";
  } else if (!boundary.selector_normalization_deterministic) {
    boundary.failure_reason = "selector normalization handoff is not deterministic";
  } else if (!boundary.property_attribute_deterministic) {
    boundary.failure_reason = "property attribute handoff is not deterministic";
  } else if (!boundary.object_pointer_surface_deterministic) {
    boundary.failure_reason =
        "object-pointer/nullability/generics handoff is not deterministic";
  } else if (!boundary.symbol_graph_scope_resolution_deterministic) {
    boundary.failure_reason =
        "symbol-graph/scope-resolution handoff is not deterministic";
  } else if (!boundary.property_synthesis_ivar_binding_deterministic) {
    boundary.failure_reason =
        "property synthesis/ivar binding handoff is not deterministic";
  } else if (boundary.invalid_protocol_composition_sites >
             boundary.protocol_record_count + boundary.category_record_count) {
    boundary.failure_reason =
        "invalid protocol composition sites exceed export-bearing records";
  } else if (boundary.ivar_record_count > boundary.property_record_count) {
    boundary.failure_reason =
        "ivar export records exceed property export records";
  }

  boundary.semantic_boundary_frozen = boundary.failure_reason.empty();
  boundary.metadata_export_enforcement_ready = false;
  boundary.fail_closed =
      boundary.semantic_boundary_frozen &&
      !boundary.metadata_export_enforcement_ready &&
      boundary.duplicate_runtime_identity_enforcement_pending &&
      boundary.incomplete_declaration_export_blocking_pending &&
      boundary.illegal_redeclaration_mix_export_blocking_pending;
  if (boundary.failure_reason.empty() && !boundary.fail_closed) {
    boundary.failure_reason =
        "runtime export legality freeze is not fail-closed";
  }
  return boundary;
}

struct Objc3RuntimeExportViolationAccumulator {
  std::size_t count = 0;
  unsigned first_line = 1;
  unsigned first_column = 1;
  bool has_location = false;

  void Add(std::size_t increment, unsigned line, unsigned column) {
    if (increment == 0) {
      return;
    }
    if (!has_location) {
      first_line = line;
      first_column = column;
      has_location = true;
    }
    count += increment;
  }

  void Merge(const Objc3RuntimeExportViolationAccumulator &other) {
    if (other.count == 0) {
      return;
    }
    Add(other.count, other.first_line, other.first_column);
  }
};

template <typename Record, typename KeyBuilder>
Objc3RuntimeExportViolationAccumulator CountDuplicateRuntimeExportIdentitySites(
    const std::vector<Record> &records,
    KeyBuilder build_key) {
  Objc3RuntimeExportViolationAccumulator violations;
  std::unordered_map<std::string, std::size_t> seen;
  seen.reserve(records.size());
  for (const auto &record : records) {
    std::size_t &count = seen[build_key(record)];
    if (count > 0u) {
      violations.Add(1u, record.line, record.column);
    }
    ++count;
  }
  return violations;
}

struct Objc3RuntimeExportPairPresence {
  std::size_t interface_records = 0;
  std::size_t implementation_records = 0;
  unsigned line = 1;
  unsigned column = 1;
  bool has_location = false;
};

struct Objc3RuntimeExportBlockingDiagnostic {
  unsigned line = 1;
  unsigned column = 1;
  std::string code;
  std::string message;
};

bool IsInterfaceRuntimePropertyOwnerKind(const std::string &owner_kind) {
  return owner_kind == "class-interface" || owner_kind == "category-interface";
}

bool IsImplementationRuntimePropertyOwnerKind(const std::string &owner_kind) {
  return owner_kind == "class-implementation" || owner_kind == "category-implementation";
}

bool IsInterfaceRuntimeMethodOwnerKind(const std::string &owner_kind) {
  return owner_kind == "class-interface" || owner_kind == "category-interface";
}

bool IsImplementationRuntimeMethodOwnerKind(const std::string &owner_kind) {
  return owner_kind == "class-implementation" || owner_kind == "category-implementation";
}

bool AreCompatibleRuntimePropertyRedeclarations(
    const Objc3RuntimeMetadataPropertySourceRecord &interface_record,
    const Objc3RuntimeMetadataPropertySourceRecord &implementation_record) {
  return interface_record.type_name == implementation_record.type_name &&
         interface_record.has_getter == implementation_record.has_getter &&
         interface_record.getter_selector == implementation_record.getter_selector &&
         interface_record.has_setter == implementation_record.has_setter &&
         interface_record.setter_selector == implementation_record.setter_selector &&
         interface_record.executable_synthesized_binding_kind ==
             implementation_record.executable_synthesized_binding_kind &&
         interface_record.property_attribute_profile ==
             implementation_record.property_attribute_profile &&
         interface_record.ownership_lifetime_profile ==
             implementation_record.ownership_lifetime_profile &&
         interface_record.ownership_runtime_hook_profile ==
             implementation_record.ownership_runtime_hook_profile &&
         interface_record.effective_getter_selector ==
             implementation_record.effective_getter_selector &&
         interface_record.effective_setter_available ==
             implementation_record.effective_setter_available &&
         interface_record.effective_setter_selector ==
             implementation_record.effective_setter_selector &&
         interface_record.accessor_ownership_profile ==
             implementation_record.accessor_ownership_profile &&
         interface_record.executable_ivar_layout_symbol ==
             implementation_record.executable_ivar_layout_symbol &&
         interface_record.executable_ivar_layout_slot_index ==
             implementation_record.executable_ivar_layout_slot_index &&
         interface_record.executable_ivar_layout_size_bytes ==
             implementation_record.executable_ivar_layout_size_bytes &&
         interface_record.executable_ivar_layout_alignment_bytes ==
             implementation_record.executable_ivar_layout_alignment_bytes;
}

bool AreCompatibleRuntimeMethodRedeclarations(
    const Objc3RuntimeMetadataMethodSourceRecord &interface_record,
    const Objc3RuntimeMetadataMethodSourceRecord &implementation_record) {
  return interface_record.is_class_method == implementation_record.is_class_method &&
         interface_record.selector == implementation_record.selector &&
         interface_record.effective_direct_dispatch ==
             implementation_record.effective_direct_dispatch &&
         interface_record.objc_final_declared ==
             implementation_record.objc_final_declared &&
         interface_record.parameter_count == implementation_record.parameter_count &&
         interface_record.return_type_name == implementation_record.return_type_name &&
         !interface_record.has_body && implementation_record.has_body;
}

std::vector<Objc3RuntimeExportBlockingDiagnostic>
BuildRuntimeExportBlockingDiagnostics(
    const Objc3RuntimeMetadataSourceRecordSet &records,
    const Objc3RuntimeExportEnforcementSummary &summary) {
  std::vector<Objc3RuntimeExportBlockingDiagnostic> diagnostics;
  struct Objc3RuntimeExportDuplicateSite {
    std::size_t count = 0;
    unsigned line = 1;
    unsigned column = 1;
    bool has_location = false;
  };
  const auto push_diagnostic =
      [&diagnostics](unsigned line, unsigned column, const std::string &code,
                     const std::string &message) {
        diagnostics.push_back({line, column, code, message});
      };
  const auto pluralize = [](std::size_t count, const std::string &singular,
                            const std::string &plural) {
    return count == 1u ? singular : plural;
  };

  if (summary.duplicate_runtime_identity_sites > 0u) {
    {
      std::unordered_map<std::string, Objc3RuntimeExportPairPresence>
          category_presence;
      category_presence.reserve(records.categories_lexicographic.size());
      for (const auto &record : records.categories_lexicographic) {
        const std::string owner_name =
            BuildCategoryOwnerName(record.class_name, record.category_name);
        Objc3RuntimeExportPairPresence &presence = category_presence[owner_name];
        if (!presence.has_location) {
          presence.line = record.line;
          presence.column = record.column;
          presence.has_location = true;
        }
        if (record.record_kind == "interface") {
          ++presence.interface_records;
        } else if (record.record_kind == "implementation") {
          ++presence.implementation_records;
        }
      }
      for (const auto &[owner_name, presence] : category_presence) {
        if (presence.interface_records > 1u) {
          push_diagnostic(
              presence.line, presence.column, "O3S261",
              "runtime metadata export blocked: category attachment collision: "
              "category '" +
                  owner_name + "' has multiple @interface declarations");
          push_diagnostic(
              presence.line, presence.column, "O3S263",
              "runtime metadata export blocked: ambiguous runtime metadata "
              "graph resolution: category '" +
                  owner_name + "' has multiple @interface attachment "
                                "candidates");
        }
        if (presence.implementation_records > 1u) {
          push_diagnostic(
              presence.line, presence.column, "O3S261",
              "runtime metadata export blocked: category attachment collision: "
              "category '" +
                  owner_name + "' has multiple @implementation declarations");
          push_diagnostic(
              presence.line, presence.column, "O3S263",
              "runtime metadata export blocked: ambiguous runtime metadata "
              "graph resolution: category '" +
                  owner_name + "' has multiple @implementation attachment "
                                "candidates");
        }
      }
    }

    {
      std::unordered_map<std::string, Objc3RuntimeExportPairPresence>
          class_presence;
      class_presence.reserve(records.classes_lexicographic.size());
      for (const auto &record : records.classes_lexicographic) {
        Objc3RuntimeExportPairPresence &presence = class_presence[record.name];
        if (!presence.has_location) {
          presence.line = record.line;
          presence.column = record.column;
          presence.has_location = true;
        }
        if (record.record_kind == "interface") {
          ++presence.interface_records;
        } else if (record.record_kind == "implementation") {
          ++presence.implementation_records;
        }
      }
      for (const auto &[name, presence] : class_presence) {
        if (presence.interface_records > 1u) {
          push_diagnostic(
              presence.line, presence.column, "O3S263",
              "runtime metadata export blocked: ambiguous runtime metadata "
              "graph resolution: class '" +
                  name + "' has multiple @interface declarations");
        }
        if (presence.implementation_records > 1u) {
          push_diagnostic(
              presence.line, presence.column, "O3S263",
              "runtime metadata export blocked: ambiguous runtime metadata "
              "graph resolution: class '" +
                  name + "' has multiple @implementation declarations");
        }
      }
    }

    {
      std::unordered_map<std::string, Objc3RuntimeExportDuplicateSite>
          property_presence;
      property_presence.reserve(records.properties_lexicographic.size());
      for (const auto &record : records.properties_lexicographic) {
        const std::string key = record.owner_kind + "\n" + record.owner_name +
                                "\n" + record.property_name;
        Objc3RuntimeExportDuplicateSite &presence = property_presence[key];
        if (!presence.has_location) {
          presence.line = record.line;
          presence.column = record.column;
          presence.has_location = true;
        }
        ++presence.count;
      }
      for (const auto &[key, presence] : property_presence) {
        if (presence.count <= 1u) {
          continue;
        }
        const std::size_t first_break = key.find('\n');
        const std::size_t second_break = key.find('\n', first_break + 1u);
        const std::string owner_kind = key.substr(0u, first_break);
        const std::string owner_name =
            key.substr(first_break + 1u, second_break - first_break - 1u);
        const std::string property_name = key.substr(second_break + 1u);
        push_diagnostic(
            presence.line, presence.column, "O3S262",
            "runtime metadata export blocked: duplicate runtime member: "
            "property '" +
                property_name + "' in " + owner_kind + " '" + owner_name +
                "' has " + std::to_string(presence.count) + " export " +
                pluralize(presence.count, "record", "records"));
      }
    }

    {
      std::unordered_map<std::string, Objc3RuntimeExportDuplicateSite>
          method_presence;
      method_presence.reserve(records.methods_lexicographic.size());
      for (const auto &record : records.methods_lexicographic) {
        const std::string key = record.owner_kind + "\n" + record.owner_name +
                                "\n" +
                                (record.is_class_method ? "+" : "-") + "\n" +
                                record.selector;
        Objc3RuntimeExportDuplicateSite &presence = method_presence[key];
        if (!presence.has_location) {
          presence.line = record.line;
          presence.column = record.column;
          presence.has_location = true;
        }
        ++presence.count;
      }
      for (const auto &[key, presence] : method_presence) {
        if (presence.count <= 1u) {
          continue;
        }
        const std::size_t first_break = key.find('\n');
        const std::size_t second_break = key.find('\n', first_break + 1u);
        const std::size_t third_break = key.find('\n', second_break + 1u);
        const std::string owner_kind = key.substr(0u, first_break);
        const std::string owner_name =
            key.substr(first_break + 1u, second_break - first_break - 1u);
        const std::string polarity =
            key.substr(second_break + 1u, third_break - second_break - 1u);
        const std::string selector = key.substr(third_break + 1u);
        push_diagnostic(
            presence.line, presence.column, "O3S262",
            "runtime metadata export blocked: duplicate runtime member: " +
                std::string(polarity == "+" ? "class" : "instance") +
                " selector '" + selector + "' in " + owner_kind + " '" +
                owner_name + "' has " + std::to_string(presence.count) +
                " export " + pluralize(presence.count, "record", "records"));
      }
    }

    {
      std::unordered_map<std::string, Objc3RuntimeExportDuplicateSite>
          ivar_presence;
      ivar_presence.reserve(records.ivars_lexicographic.size());
      for (const auto &record : records.ivars_lexicographic) {
        const std::string key = record.owner_kind + "\n" + record.owner_name +
                                "\n" + record.ivar_binding_symbol;
        Objc3RuntimeExportDuplicateSite &presence = ivar_presence[key];
        if (!presence.has_location) {
          presence.line = record.line;
          presence.column = record.column;
          presence.has_location = true;
        }
        ++presence.count;
      }
      for (const auto &[key, presence] : ivar_presence) {
        if (presence.count <= 1u) {
          continue;
        }
        const std::size_t first_break = key.find('\n');
        const std::size_t second_break = key.find('\n', first_break + 1u);
        const std::string owner_kind = key.substr(0u, first_break);
        const std::string owner_name =
            key.substr(first_break + 1u, second_break - first_break - 1u);
        const std::string ivar_symbol = key.substr(second_break + 1u);
        push_diagnostic(
            presence.line, presence.column, "O3S262",
            "runtime metadata export blocked: duplicate runtime member: "
            "ivar '" +
                ivar_symbol + "' in " + owner_kind + " '" + owner_name +
                "' has " + std::to_string(presence.count) + " export " +
                pluralize(presence.count, "record", "records"));
      }
    }
  }

  if (summary.incomplete_declaration_sites > 0u) {
    std::unordered_map<std::string, Objc3RuntimeExportPairPresence> class_presence;
    class_presence.reserve(records.classes_lexicographic.size());
    for (const auto &record : records.classes_lexicographic) {
      Objc3RuntimeExportPairPresence &presence = class_presence[record.name];
      if (!presence.has_location) {
        presence.line = record.line;
        presence.column = record.column;
        presence.has_location = true;
      }
      if (record.record_kind == "interface") {
        ++presence.interface_records;
      } else if (record.record_kind == "implementation") {
        ++presence.implementation_records;
      }
    }
    for (const auto &[name, presence] : class_presence) {
      if (presence.interface_records > 0u && presence.implementation_records == 0u) {
        diagnostics.push_back(
            {presence.line,
             presence.column,
             "O3S260",
             "runtime metadata export blocked: incomplete runtime metadata declarations are not exportable: interface '" +
                 name + "' is missing a matching @implementation"});
      } else if (presence.interface_records == 0u &&
                 presence.implementation_records > 0u) {
        diagnostics.push_back(
            {presence.line,
             presence.column,
             "O3S260",
             "runtime metadata export blocked: incomplete runtime metadata declarations are not exportable: implementation '" +
                 name + "' is missing a matching @interface"});
      }
    }
  }

  {
    std::unordered_map<std::string, Objc3RuntimeExportPairPresence>
        category_presence;
    category_presence.reserve(records.categories_lexicographic.size());
    for (const auto &record : records.categories_lexicographic) {
      const std::string owner_name =
          BuildCategoryOwnerName(record.class_name, record.category_name);
      Objc3RuntimeExportPairPresence &presence = category_presence[owner_name];
      if (!presence.has_location) {
        presence.line = record.line;
        presence.column = record.column;
        presence.has_location = true;
      }
      if (record.record_kind == "interface") {
        ++presence.interface_records;
      } else if (record.record_kind == "implementation") {
        ++presence.implementation_records;
      }
    }
    for (const auto &[owner_name, presence] : category_presence) {
      if (presence.interface_records > 0u && presence.implementation_records == 0u) {
        diagnostics.push_back(
            {presence.line,
             presence.column,
             "O3S260",
             "runtime metadata export blocked: incomplete runtime metadata declarations are not exportable: category '" +
                 owner_name + "' is missing a matching @implementation"});
      } else if (presence.interface_records == 0u &&
                 presence.implementation_records > 0u) {
        diagnostics.push_back(
            {presence.line,
             presence.column,
             "O3S260",
             "runtime metadata export blocked: incomplete runtime metadata declarations are not exportable: category '" +
                 owner_name + "' is missing a matching @interface"});
      }
    }
  }

  if (summary.duplicate_runtime_identity_sites > 0u &&
      diagnostics.empty()) {
    push_diagnostic(summary.first_failure_line, summary.first_failure_column,
                    "O3S262",
                    "runtime metadata export blocked: duplicate runtime "
                    "metadata identities are not exportable");
  }

  std::sort(diagnostics.begin(), diagnostics.end(),
            [](const Objc3RuntimeExportBlockingDiagnostic &lhs,
               const Objc3RuntimeExportBlockingDiagnostic &rhs) {
              return std::tie(lhs.line, lhs.column, lhs.code, lhs.message) <
                     std::tie(rhs.line, rhs.column, rhs.code, rhs.message);
            });
  return diagnostics;
}

bool HasRuntimeMetadataSourceRecords(const Objc3RuntimeMetadataSourceRecordSet &records) {
  return !records.classes_lexicographic.empty() ||
         !records.protocols_lexicographic.empty() ||
         !records.categories_lexicographic.empty() ||
         !records.properties_lexicographic.empty() ||
         !records.methods_lexicographic.empty() ||
         !records.ivars_lexicographic.empty();
}

Objc3RuntimeExportEnforcementSummary BuildRuntimeExportEnforcementSummary(
    const Objc3RuntimeMetadataSourceRecordSet &records,
    const Objc3RuntimeExportLegalityBoundary &runtime_export_legality) {
  Objc3RuntimeExportEnforcementSummary summary;
  summary.metadata_completeness_enforced = true;
  summary.duplicate_runtime_identity_suppression_enforced = true;
  summary.illegal_redeclaration_mix_blocking_enforced = true;
  summary.metadata_shape_drift_blocking_enforced = true;
  summary.fail_closed = true;

  Objc3RuntimeExportViolationAccumulator duplicate_violations;
  duplicate_violations.Merge(CountDuplicateRuntimeExportIdentitySites(
      records.classes_lexicographic,
      [](const Objc3RuntimeMetadataClassSourceRecord &record) {
        return record.record_kind + "\n" + record.name;
      }));
  duplicate_violations.Merge(CountDuplicateRuntimeExportIdentitySites(
      records.categories_lexicographic,
      [](const Objc3RuntimeMetadataCategorySourceRecord &record) {
        return record.record_kind + "\n" + record.class_name + "\n" +
               record.category_name;
      }));
  duplicate_violations.Merge(CountDuplicateRuntimeExportIdentitySites(
      records.properties_lexicographic,
      [](const Objc3RuntimeMetadataPropertySourceRecord &record) {
        return record.owner_kind + "\n" + record.owner_name + "\n" +
               record.property_name;
      }));
  duplicate_violations.Merge(CountDuplicateRuntimeExportIdentitySites(
      records.methods_lexicographic,
      [](const Objc3RuntimeMetadataMethodSourceRecord &record) {
        return record.owner_kind + "\n" + record.owner_name + "\n" +
               (record.is_class_method ? "+" : "-") + "\n" + record.selector;
      }));
  duplicate_violations.Merge(CountDuplicateRuntimeExportIdentitySites(
      records.ivars_lexicographic,
      [](const Objc3RuntimeMetadataIvarSourceRecord &record) {
        return record.owner_kind + "\n" + record.owner_name + "\n" +
               record.ivar_binding_symbol;
      }));
  {
    std::unordered_map<std::string, std::size_t> seen_protocols;
    seen_protocols.reserve(records.protocols_lexicographic.size());
    for (const auto &record : records.protocols_lexicographic) {
      if (record.is_forward_declaration) {
        continue;
      }
      std::size_t &count = seen_protocols[record.name];
      if (count > 0u) {
        duplicate_violations.Add(1u, record.line, record.column);
      }
      ++count;
    }
  }
  summary.duplicate_runtime_identity_sites = duplicate_violations.count;

  Objc3RuntimeExportViolationAccumulator incomplete_violations;
  // Forward protocol declarations are dependency hints for later complete
  // protocol records or composition spelling; they are not themselves
  // exportable runtime metadata units and must not block the runnable path.
  {
    std::unordered_map<std::string, Objc3RuntimeExportPairPresence> class_presence;
    class_presence.reserve(records.classes_lexicographic.size());
    for (const auto &record : records.classes_lexicographic) {
      Objc3RuntimeExportPairPresence &presence = class_presence[record.name];
      if (!presence.has_location) {
        presence.line = record.line;
        presence.column = record.column;
        presence.has_location = true;
      }
      if (record.record_kind == "interface") {
        ++presence.interface_records;
      } else if (record.record_kind == "implementation") {
        ++presence.implementation_records;
      }
    }
    for (const auto &entry : class_presence) {
      const Objc3RuntimeExportPairPresence &presence = entry.second;
      if (presence.interface_records == 0u || presence.implementation_records == 0u) {
        incomplete_violations.Add(1u, presence.line, presence.column);
      }
    }
  }
  {
    std::unordered_map<std::string, Objc3RuntimeExportPairPresence> category_presence;
    category_presence.reserve(records.categories_lexicographic.size());
    for (const auto &record : records.categories_lexicographic) {
      const std::string key = record.class_name + "\n" + record.category_name;
      Objc3RuntimeExportPairPresence &presence = category_presence[key];
      if (!presence.has_location) {
        presence.line = record.line;
        presence.column = record.column;
        presence.has_location = true;
      }
      if (record.record_kind == "interface") {
        ++presence.interface_records;
      } else if (record.record_kind == "implementation") {
        ++presence.implementation_records;
      }
    }
    for (const auto &entry : category_presence) {
      const Objc3RuntimeExportPairPresence &presence = entry.second;
      if (presence.interface_records == 0u || presence.implementation_records == 0u) {
        incomplete_violations.Add(1u, presence.line, presence.column);
      }
    }
  }
  incomplete_violations.count +=
      runtime_export_legality.implementation_resolution_misses +
      runtime_export_legality.method_resolution_misses +
      runtime_export_legality.property_ivar_binding_missing;
  summary.incomplete_declaration_sites = incomplete_violations.count;

  Objc3RuntimeExportViolationAccumulator illegal_redeclaration_violations;
  {
    struct RuntimePropertyRedeclarationPair {
      const Objc3RuntimeMetadataPropertySourceRecord *interface_record = nullptr;
      const Objc3RuntimeMetadataPropertySourceRecord *implementation_record =
          nullptr;
    };
    std::unordered_map<std::string, RuntimePropertyRedeclarationPair>
        property_pairs;
    property_pairs.reserve(records.properties_lexicographic.size());
    for (const auto &record : records.properties_lexicographic) {
      if (!IsInterfaceRuntimePropertyOwnerKind(record.owner_kind) &&
          !IsImplementationRuntimePropertyOwnerKind(record.owner_kind)) {
        continue;
      }
      const std::string key = record.owner_name + "\n" + record.property_name;
      RuntimePropertyRedeclarationPair &pair = property_pairs[key];
      if (IsInterfaceRuntimePropertyOwnerKind(record.owner_kind)) {
        pair.interface_record = &record;
      } else if (IsImplementationRuntimePropertyOwnerKind(record.owner_kind)) {
        pair.implementation_record = &record;
      }
    }
    for (const auto &entry : property_pairs) {
      const RuntimePropertyRedeclarationPair &pair = entry.second;
      if (pair.interface_record == nullptr || pair.implementation_record == nullptr) {
        continue;
      }
      if (!AreCompatibleRuntimePropertyRedeclarations(*pair.interface_record,
                                                      *pair.implementation_record)) {
        illegal_redeclaration_violations.Add(
            1u, pair.implementation_record->line,
            pair.implementation_record->column);
      }
    }
  }
  {
    struct RuntimeMethodRedeclarationPair {
      const Objc3RuntimeMetadataMethodSourceRecord *interface_record = nullptr;
      const Objc3RuntimeMetadataMethodSourceRecord *implementation_record =
          nullptr;
    };
    std::unordered_map<std::string, RuntimeMethodRedeclarationPair> method_pairs;
    method_pairs.reserve(records.methods_lexicographic.size());
    for (const auto &record : records.methods_lexicographic) {
      if (!IsInterfaceRuntimeMethodOwnerKind(record.owner_kind) &&
          !IsImplementationRuntimeMethodOwnerKind(record.owner_kind)) {
        continue;
      }
      const std::string key = record.owner_name + "\n" +
                              (record.is_class_method ? "+" : "-") + "\n" +
                              record.selector;
      RuntimeMethodRedeclarationPair &pair = method_pairs[key];
      if (IsInterfaceRuntimeMethodOwnerKind(record.owner_kind)) {
        pair.interface_record = &record;
      } else if (IsImplementationRuntimeMethodOwnerKind(record.owner_kind)) {
        pair.implementation_record = &record;
      }
    }
    for (const auto &entry : method_pairs) {
      const RuntimeMethodRedeclarationPair &pair = entry.second;
      if (pair.interface_record == nullptr || pair.implementation_record == nullptr) {
        continue;
      }
      if (!AreCompatibleRuntimeMethodRedeclarations(*pair.interface_record,
                                                    *pair.implementation_record)) {
        illegal_redeclaration_violations.Add(
            1u, pair.implementation_record->line,
            pair.implementation_record->column);
      }
    }
  }
  summary.illegal_redeclaration_mix_sites =
      illegal_redeclaration_violations.count +
      runtime_export_legality.invalid_protocol_composition_sites +
      runtime_export_legality.property_attribute_invalid_entries +
      runtime_export_legality.property_attribute_contract_violations +
      runtime_export_legality.invalid_type_annotation_sites +
      runtime_export_legality.property_ivar_binding_conflicts;

  summary.metadata_shape_drift_sites =
      (runtime_export_legality.semantic_integration_surface_built ? 0u : 1u) +
      (runtime_export_legality.sema_type_metadata_handoff_deterministic ? 0u : 1u) +
      (runtime_export_legality.typed_sema_surface_ready ? 0u : 1u) +
      (runtime_export_legality.typed_sema_surface_deterministic ? 0u : 1u) +
      (runtime_export_legality.runtime_metadata_source_boundary_ready ? 0u : 1u) +
      (runtime_export_legality.protocol_category_deterministic ? 0u : 1u) +
      (runtime_export_legality.class_protocol_category_linking_deterministic ? 0u : 1u) +
      (runtime_export_legality.selector_normalization_deterministic ? 0u : 1u) +
      (runtime_export_legality.property_attribute_deterministic ? 0u : 1u) +
      (runtime_export_legality.object_pointer_surface_deterministic ? 0u : 1u) +
      (runtime_export_legality.symbol_graph_scope_resolution_deterministic ? 0u : 1u) +
      (runtime_export_legality.property_synthesis_ivar_binding_deterministic ? 0u : 1u) +
      (runtime_export_legality.invalid_protocol_composition_sites <=
               runtime_export_legality.protocol_record_count +
                   runtime_export_legality.category_record_count
           ? 0u
           : 1u) +
      (runtime_export_legality.ivar_record_count <=
               runtime_export_legality.property_record_count
           ? 0u
           : 1u);

  summary.ready_for_runtime_export =
      summary.duplicate_runtime_identity_sites == 0u &&
      summary.incomplete_declaration_sites == 0u &&
      summary.illegal_redeclaration_mix_sites == 0u &&
      summary.metadata_shape_drift_sites == 0u;

  if (duplicate_violations.has_location) {
    summary.first_failure_line = duplicate_violations.first_line;
    summary.first_failure_column = duplicate_violations.first_column;
  } else if (incomplete_violations.has_location) {
    summary.first_failure_line = incomplete_violations.first_line;
    summary.first_failure_column = incomplete_violations.first_column;
  } else if (illegal_redeclaration_violations.has_location) {
    summary.first_failure_line = illegal_redeclaration_violations.first_line;
    summary.first_failure_column = illegal_redeclaration_violations.first_column;
  }

  if (summary.duplicate_runtime_identity_sites > 0u) {
    summary.failure_reason =
        "duplicate runtime metadata identities are not exportable";
  } else if (summary.incomplete_declaration_sites > 0u) {
    summary.failure_reason =
        "incomplete runtime metadata declarations are not exportable";
  } else if (summary.illegal_redeclaration_mix_sites > 0u) {
    summary.failure_reason =
        "illegal runtime metadata redeclaration mixes are not exportable";
  } else if (summary.metadata_shape_drift_sites > 0u) {
    summary.failure_reason =
        "runtime metadata export shape drift detected before lowering";
  }

  return summary;
}

Objc3FrontendProtocolCategorySummary BuildProtocolCategorySummary(
    const Objc3Program &program,
    const Objc3SemanticIntegrationSurface &integration_surface,
    const Objc3SemanticTypeMetadataHandoff &type_metadata_handoff) {
  Objc3FrontendProtocolCategorySummary summary;
  summary.declared_protocols = CountProtocols(program);
  summary.declared_categories = CountCategories(program);
  summary.resolved_protocol_symbols = CountProtocols(integration_surface);
  summary.resolved_category_symbols = CountCategories(integration_surface);
  summary.protocol_method_symbols = CountProtocolMethodsFromSymbolTable(integration_surface);
  summary.category_method_symbols = CountCategoryMethodsFromSymbolTable(integration_surface);
  summary.linked_category_symbols = CountLinkedCategorySymbolsFromSymbolTable(integration_surface);

  if (summary.protocol_method_symbols == 0) {
    summary.protocol_method_symbols = CountProtocolMethodsFromTypeMetadata(type_metadata_handoff);
  }
  if (summary.category_method_symbols == 0) {
    summary.category_method_symbols = CountCategoryMethodsFromTypeMetadata(type_metadata_handoff);
  }
  if (summary.linked_category_symbols == 0) {
    summary.linked_category_symbols = CountLinkedCategorySymbolsFromTypeMetadata(type_metadata_handoff);
  }

  summary.deterministic_protocol_category_handoff =
      summary.linked_category_symbols <= summary.category_method_symbols &&
      summary.resolved_protocol_symbols <= summary.declared_protocols &&
      summary.resolved_category_symbols <= summary.declared_categories;
  return summary;
}

Objc3FrontendClassProtocolCategoryLinkingSummary BuildClassProtocolCategoryLinkingSummary(
    const Objc3InterfaceImplementationSummary &interface_implementation_summary,
    const Objc3FrontendProtocolCategorySummary &protocol_category_summary,
    const Objc3SemanticIntegrationSurface &integration_surface,
    const Objc3SemanticTypeMetadataHandoff &type_metadata_handoff) {
  Objc3FrontendClassProtocolCategoryLinkingSummary summary;
  summary.declared_class_interfaces = interface_implementation_summary.declared_interfaces;
  summary.declared_class_implementations = interface_implementation_summary.declared_implementations;
  summary.resolved_class_interfaces = interface_implementation_summary.resolved_interfaces;
  summary.resolved_class_implementations = interface_implementation_summary.resolved_implementations;
  summary.linked_class_method_symbols = interface_implementation_summary.linked_implementation_symbols;
  summary.linked_category_method_symbols = protocol_category_summary.linked_category_symbols;

  const Objc3ProtocolCategoryCompositionSummary &integration_composition_summary =
      integration_surface.protocol_category_composition_summary;
  const Objc3ProtocolCategoryCompositionSummary &type_metadata_composition_summary =
      type_metadata_handoff.protocol_category_composition_summary;
  const auto select_composition_value = [&](std::size_t integration_value, std::size_t type_metadata_value) {
    if (integration_surface.built) {
      return integration_value;
    }
    return type_metadata_value;
  };

  summary.protocol_composition_sites =
      select_composition_value(integration_composition_summary.protocol_composition_sites,
                               type_metadata_composition_summary.protocol_composition_sites);
  summary.protocol_composition_symbols =
      select_composition_value(integration_composition_summary.protocol_composition_symbols,
                               type_metadata_composition_summary.protocol_composition_symbols);
  summary.category_composition_sites =
      select_composition_value(integration_composition_summary.category_composition_sites,
                               type_metadata_composition_summary.category_composition_sites);
  summary.category_composition_symbols =
      select_composition_value(integration_composition_summary.category_composition_symbols,
                               type_metadata_composition_summary.category_composition_symbols);
  summary.invalid_protocol_composition_sites =
      select_composition_value(integration_composition_summary.invalid_protocol_composition_sites,
                               type_metadata_composition_summary.invalid_protocol_composition_sites);

  const bool composition_fields_match =
      integration_composition_summary.protocol_composition_sites ==
          type_metadata_composition_summary.protocol_composition_sites &&
      integration_composition_summary.protocol_composition_symbols ==
          type_metadata_composition_summary.protocol_composition_symbols &&
      integration_composition_summary.category_composition_sites ==
          type_metadata_composition_summary.category_composition_sites &&
      integration_composition_summary.category_composition_symbols ==
          type_metadata_composition_summary.category_composition_symbols &&
      integration_composition_summary.invalid_protocol_composition_sites ==
          type_metadata_composition_summary.invalid_protocol_composition_sites;

  summary.deterministic_class_protocol_category_linking_handoff =
      interface_implementation_summary.deterministic &&
      protocol_category_summary.deterministic_protocol_category_handoff &&
      integration_composition_summary.deterministic &&
      type_metadata_composition_summary.deterministic &&
      composition_fields_match &&
      summary.resolved_class_interfaces <= summary.declared_class_interfaces &&
      summary.resolved_class_implementations <= summary.declared_class_implementations &&
      summary.linked_class_method_symbols <= interface_implementation_summary.interface_method_symbols &&
      summary.linked_class_method_symbols <= interface_implementation_summary.implementation_method_symbols &&
      summary.linked_category_method_symbols <= protocol_category_summary.category_method_symbols &&
      summary.category_composition_sites <= summary.protocol_composition_sites &&
      summary.category_composition_symbols <= summary.protocol_composition_symbols &&
      summary.invalid_protocol_composition_sites <=
          summary.protocol_composition_sites + summary.category_composition_sites;
  return summary;
}

template <typename Container>
void AccumulateSelectorNormalizationSummary(const Container &declarations,
                                           Objc3FrontendSelectorNormalizationSummary &summary) {
  for (const auto &declaration : declarations) {
    for (const auto &method : declaration.methods) {
      ++summary.method_declaration_entries;
      summary.selector_piece_entries += method.selector_pieces.size();

      std::size_t method_parameter_links = 0;
      bool method_parameter_names_complete = true;
      for (const auto &piece : method.selector_pieces) {
        if (!piece.has_parameter) {
          continue;
        }
        ++method_parameter_links;
        ++summary.selector_piece_parameter_links;
        if (piece.parameter_name.empty()) {
          method_parameter_names_complete = false;
        }
      }

      if (method.selector_is_normalized) {
        ++summary.normalized_method_declarations;
      }

      summary.deterministic_selector_normalization_handoff =
          summary.deterministic_selector_normalization_handoff &&
          (!method.selector_pieces.empty() || method.selector.empty()) &&
          (method.selector_is_normalized || method.selector_pieces.empty()) &&
          method_parameter_names_complete &&
          method_parameter_links <= method.params.size() &&
          method.params.size() <= method.selector_pieces.size();
    }
  }
}

Objc3FrontendSelectorNormalizationSummary BuildSelectorNormalizationSummary(const Objc3Program &program) {
  Objc3FrontendSelectorNormalizationSummary summary;
  AccumulateSelectorNormalizationSummary(program.protocols, summary);
  AccumulateSelectorNormalizationSummary(program.interfaces, summary);
  AccumulateSelectorNormalizationSummary(program.implementations, summary);
  summary.deterministic_selector_normalization_handoff =
      summary.deterministic_selector_normalization_handoff &&
      summary.normalized_method_declarations <= summary.method_declaration_entries &&
      summary.selector_piece_parameter_links <= summary.selector_piece_entries;
  return summary;
}

template <typename Container>
void AccumulatePropertyAttributeSummary(const Container &declarations,
                                        Objc3FrontendPropertyAttributeSummary &summary) {
  for (const auto &declaration : declarations) {
    for (const auto &property : declaration.properties) {
      ++summary.property_declaration_entries;
      summary.property_attribute_entries += property.attributes.size();

      std::size_t accessor_modifier_entries = 0;
      if (property.is_readonly) {
        ++accessor_modifier_entries;
      }
      if (property.is_readwrite) {
        ++accessor_modifier_entries;
      }
      if (property.is_atomic) {
        ++accessor_modifier_entries;
      }
      if (property.is_nonatomic) {
        ++accessor_modifier_entries;
      }
      if (property.is_copy) {
        ++accessor_modifier_entries;
      }
      if (property.is_strong) {
        ++accessor_modifier_entries;
      }
      if (property.is_weak) {
        ++accessor_modifier_entries;
      }
      if (property.is_assign) {
        ++accessor_modifier_entries;
      }
      if (property.has_getter) {
        ++accessor_modifier_entries;
        ++summary.property_getter_selector_entries;
      }
      if (property.has_setter) {
        ++accessor_modifier_entries;
        ++summary.property_setter_selector_entries;
      }
      summary.property_accessor_modifier_entries += accessor_modifier_entries;

      bool attribute_names_complete = true;
      bool attribute_values_complete = true;
      for (const auto &attribute : property.attributes) {
        if (attribute.name.empty()) {
          attribute_names_complete = false;
        }
        if (attribute.has_value) {
          ++summary.property_attribute_value_entries;
          if (attribute.value.empty()) {
            attribute_values_complete = false;
          }
        }
      }

      summary.deterministic_property_attribute_handoff =
          summary.deterministic_property_attribute_handoff &&
          !property.name.empty() &&
          (!property.is_readonly || !property.is_readwrite) &&
          (!property.is_atomic || !property.is_nonatomic) &&
          (!property.has_getter || !property.getter_selector.empty()) &&
          (!property.has_setter || !property.setter_selector.empty()) &&
          attribute_names_complete &&
          attribute_values_complete &&
          summary.property_getter_selector_entries <= summary.property_declaration_entries &&
          summary.property_setter_selector_entries <= summary.property_declaration_entries;
    }
  }
}

Objc3FrontendPropertyAttributeSummary BuildPropertyAttributeSummary(const Objc3Program &program) {
  Objc3FrontendPropertyAttributeSummary summary;
  AccumulatePropertyAttributeSummary(program.protocols, summary);
  AccumulatePropertyAttributeSummary(program.interfaces, summary);
  AccumulatePropertyAttributeSummary(program.implementations, summary);
  summary.deterministic_property_attribute_handoff =
      summary.deterministic_property_attribute_handoff &&
      summary.property_attribute_value_entries <= summary.property_attribute_entries &&
      summary.property_accessor_modifier_entries >= summary.property_getter_selector_entries &&
      summary.property_accessor_modifier_entries >= summary.property_setter_selector_entries;
  return summary;
}

void AccumulateObjectPointerNullabilityGenericsTypeAnnotation(
    bool object_pointer_type_spelling,
    const std::string &object_pointer_type_name,
    bool has_pointer_declarator,
    unsigned pointer_declarator_depth,
    const std::vector<Objc3SemaTokenMetadata> &pointer_declarator_tokens,
    const std::vector<Objc3SemaTokenMetadata> &nullability_suffix_tokens,
    bool has_generic_suffix,
    bool generic_suffix_terminated,
    const std::string &generic_suffix_text,
    Objc3FrontendObjectPointerNullabilityGenericsSummary &summary) {
  if (object_pointer_type_spelling) {
    ++summary.object_pointer_type_spellings;
  }
  summary.pointer_declarator_depth_total += pointer_declarator_depth;
  summary.pointer_declarator_token_entries += pointer_declarator_tokens.size();
  summary.nullability_suffix_entries += nullability_suffix_tokens.size();

  if (has_pointer_declarator) {
    ++summary.pointer_declarator_entries;
    summary.deterministic_object_pointer_nullability_generics_handoff =
        summary.deterministic_object_pointer_nullability_generics_handoff && pointer_declarator_depth > 0;
  } else {
    summary.deterministic_object_pointer_nullability_generics_handoff =
        summary.deterministic_object_pointer_nullability_generics_handoff && pointer_declarator_depth == 0;
  }

  summary.deterministic_object_pointer_nullability_generics_handoff =
      summary.deterministic_object_pointer_nullability_generics_handoff &&
      (!object_pointer_type_spelling || !object_pointer_type_name.empty()) &&
      pointer_declarator_tokens.size() == static_cast<std::size_t>(pointer_declarator_depth);

  for (const auto &token : pointer_declarator_tokens) {
    summary.deterministic_object_pointer_nullability_generics_handoff =
        summary.deterministic_object_pointer_nullability_generics_handoff &&
        token.kind == Objc3SemaTokenKind::PointerDeclarator && !token.text.empty();
  }
  for (const auto &token : nullability_suffix_tokens) {
    summary.deterministic_object_pointer_nullability_generics_handoff =
        summary.deterministic_object_pointer_nullability_generics_handoff &&
        token.kind == Objc3SemaTokenKind::NullabilitySuffix && !token.text.empty();
  }

  if (has_generic_suffix) {
    ++summary.generic_suffix_entries;
    if (generic_suffix_terminated) {
      ++summary.terminated_generic_suffix_entries;
    } else {
      ++summary.unterminated_generic_suffix_entries;
    }
    summary.deterministic_object_pointer_nullability_generics_handoff =
        summary.deterministic_object_pointer_nullability_generics_handoff &&
        !generic_suffix_text.empty() && generic_suffix_text.front() == '<' &&
        (!generic_suffix_terminated || generic_suffix_text.back() == '>');
    return;
  }

  summary.deterministic_object_pointer_nullability_generics_handoff =
      summary.deterministic_object_pointer_nullability_generics_handoff &&
      generic_suffix_terminated && generic_suffix_text.empty();
}

void AccumulateObjectPointerNullabilityGenericsForMethod(
    const Objc3MethodDecl &method,
    Objc3FrontendObjectPointerNullabilityGenericsSummary &summary) {
  AccumulateObjectPointerNullabilityGenericsTypeAnnotation(method.return_object_pointer_type_spelling,
                                                           method.return_object_pointer_type_name,
                                                           method.has_return_pointer_declarator,
                                                           method.return_pointer_declarator_depth,
                                                           method.return_pointer_declarator_tokens,
                                                           method.return_nullability_suffix_tokens,
                                                           method.has_return_generic_suffix,
                                                           method.return_generic_suffix_terminated,
                                                           method.return_generic_suffix_text,
                                                           summary);
  for (const auto &param : method.params) {
    AccumulateObjectPointerNullabilityGenericsTypeAnnotation(param.object_pointer_type_spelling,
                                                             param.object_pointer_type_name,
                                                             param.has_pointer_declarator,
                                                             param.pointer_declarator_depth,
                                                             param.pointer_declarator_tokens,
                                                             param.nullability_suffix_tokens,
                                                             param.has_generic_suffix,
                                                             param.generic_suffix_terminated,
                                                             param.generic_suffix_text,
                                                             summary);
  }
}

template <typename Container>
void AccumulateObjectPointerNullabilityGenericsForObjcDeclarations(
    const Container &declarations,
    Objc3FrontendObjectPointerNullabilityGenericsSummary &summary) {
  for (const auto &declaration : declarations) {
    for (const auto &property : declaration.properties) {
      AccumulateObjectPointerNullabilityGenericsTypeAnnotation(property.object_pointer_type_spelling,
                                                               property.object_pointer_type_name,
                                                               property.has_pointer_declarator,
                                                               property.pointer_declarator_depth,
                                                               property.pointer_declarator_tokens,
                                                               property.nullability_suffix_tokens,
                                                               property.has_generic_suffix,
                                                               property.generic_suffix_terminated,
                                                               property.generic_suffix_text,
                                                               summary);
    }
    for (const auto &method : declaration.methods) {
      AccumulateObjectPointerNullabilityGenericsForMethod(method, summary);
    }
  }
}

Objc3FrontendObjectPointerNullabilityGenericsSummary BuildObjectPointerNullabilityGenericsSummary(
    const Objc3Program &program) {
  Objc3FrontendObjectPointerNullabilityGenericsSummary summary;
  for (const auto &fn : program.functions) {
    AccumulateObjectPointerNullabilityGenericsTypeAnnotation(fn.return_object_pointer_type_spelling,
                                                             fn.return_object_pointer_type_name,
                                                             fn.has_return_pointer_declarator,
                                                             fn.return_pointer_declarator_depth,
                                                             fn.return_pointer_declarator_tokens,
                                                             fn.return_nullability_suffix_tokens,
                                                             fn.has_return_generic_suffix,
                                                             fn.return_generic_suffix_terminated,
                                                             fn.return_generic_suffix_text,
                                                             summary);
    for (const auto &param : fn.params) {
      AccumulateObjectPointerNullabilityGenericsTypeAnnotation(param.object_pointer_type_spelling,
                                                               param.object_pointer_type_name,
                                                               param.has_pointer_declarator,
                                                               param.pointer_declarator_depth,
                                                               param.pointer_declarator_tokens,
                                                               param.nullability_suffix_tokens,
                                                               param.has_generic_suffix,
                                                               param.generic_suffix_terminated,
                                                               param.generic_suffix_text,
                                                               summary);
    }
  }
  AccumulateObjectPointerNullabilityGenericsForObjcDeclarations(program.protocols, summary);
  AccumulateObjectPointerNullabilityGenericsForObjcDeclarations(program.interfaces, summary);
  AccumulateObjectPointerNullabilityGenericsForObjcDeclarations(program.implementations, summary);

  summary.deterministic_object_pointer_nullability_generics_handoff =
      summary.deterministic_object_pointer_nullability_generics_handoff &&
      summary.terminated_generic_suffix_entries + summary.unterminated_generic_suffix_entries ==
          summary.generic_suffix_entries &&
      summary.pointer_declarator_entries <= summary.pointer_declarator_depth_total &&
      summary.pointer_declarator_entries <= summary.pointer_declarator_token_entries;
  return summary;
}

std::string BuildPart3TypeSourceClosureReplayKey(
    const Objc3FrontendPart3TypeSourceClosureSummary &summary) {
  std::ostringstream out;
  out << summary.contract_id
      << ";protocol_required_methods=" << summary.protocol_required_method_count
      << ";protocol_optional_methods=" << summary.protocol_optional_method_count
      << ";protocol_required_properties=" << summary.protocol_required_property_count
      << ";protocol_optional_properties=" << summary.protocol_optional_property_count
      << ";object_pointer_type_spelling_sites=" << summary.object_pointer_type_spelling_sites
      << ";pointer_declarator_entries=" << summary.pointer_declarator_entries
      << ";nullability_suffix_entries=" << summary.nullability_suffix_entries
      << ";generic_suffix_entries=" << summary.generic_suffix_entries
      << ";optional_sites="
      << summary.optional_binding_sites << ":" << summary.guard_binding_sites << ":"
      << summary.optional_send_sites << ":" << summary.nil_coalescing_sites << ":"
      << summary.typed_keypath_literal_sites
      << ";optional_member_access_sites=" << summary.optional_member_access_sites
      << ";deterministic=" << (summary.deterministic_handoff ? "true" : "false");
  return out.str();
}

std::string BuildPart5ControlFlowSourceClosureReplayKey(
    const Objc3FrontendPart5ControlFlowSourceClosureSummary &summary) {
  std::ostringstream out;
  out << summary.contract_id
      << ";guard_binding_sites=" << summary.guard_binding_sites
      << ";guard_binding_clause_sites=" << summary.guard_binding_clause_sites
      << ";guard_boolean_condition_sites="
      << summary.guard_boolean_condition_sites
      << ";switch_pattern_sites=" << summary.switch_case_pattern_sites << ":"
      << summary.switch_default_pattern_sites
      << ";match_surface_sites=" << summary.match_statement_sites << ":"
      << summary.match_case_pattern_sites << ":" << summary.match_default_sites
      << ":" << summary.match_wildcard_pattern_sites << ":"
      << summary.match_literal_pattern_sites << ":"
      << summary.match_binding_pattern_sites << ":"
      << summary.match_result_case_pattern_sites
      << ";reserved_keyword_sites=" << summary.defer_keyword_sites
      << ";deterministic=" << (summary.deterministic_handoff ? "true" : "false");
  return out.str();
}

std::string BuildPart6ErrorSourceClosureReplayKey(
    const Objc3FrontendPart6ErrorSourceClosureSummary &summary) {
  std::ostringstream out;
  out << summary.contract_id
      << ";throws_sites=" << summary.function_throws_declaration_sites << ":"
      << summary.method_throws_declaration_sites
      << ";result_sites=" << summary.result_like_sites << ":"
      << summary.result_success_sites << ":" << summary.result_failure_sites
      << ":" << summary.result_branch_sites << ":"
      << summary.result_payload_sites
      << ";nserror_sites=" << summary.ns_error_bridging_sites << ":"
      << summary.ns_error_out_parameter_sites << ":"
      << summary.ns_error_bridge_path_sites
      << ";bridge_marker_sites=" << summary.objc_nserror_attribute_sites << ":"
      << summary.objc_status_code_attribute_sites << ":"
      << summary.status_code_success_clause_sites << ":"
      << summary.status_code_error_type_clause_sites << ":"
      << summary.status_code_mapping_clause_sites
      << ";reserved_keyword_sites=" << summary.try_keyword_sites << ":"
      << summary.throw_keyword_sites << ":" << summary.catch_keyword_sites
      << ";deterministic=" << (summary.deterministic_handoff ? "true" : "false");
  return out.str();
}

std::string BuildPart7AsyncSourceClosureReplayKey(
    const Objc3FrontendPart7AsyncSourceClosureSummary &summary) {
  std::ostringstream out;
  out << summary.contract_id
      << ";async_sites=" << summary.async_keyword_sites << ":"
      << summary.async_function_sites << ":" << summary.async_method_sites
      << ";await_sites=" << summary.await_keyword_sites << ":"
      << summary.await_expression_sites
      << ";executor_sites=" << summary.executor_attribute_sites << ":"
      << summary.executor_main_sites << ":" << summary.executor_global_sites
      << ":" << summary.executor_named_sites
      << ";deterministic=" << (summary.deterministic_handoff ? "true" : "false");
  return out.str();
}

std::string BuildPart7ActorMemberIsolationSourceClosureReplayKey(
    const Objc3FrontendPart7ActorMemberIsolationSourceClosureSummary &summary) {
  std::ostringstream out;
  out << summary.contract_id
      << ";actor_sites=" << summary.actor_interface_sites << ":"
      << summary.actor_method_sites << ":" << summary.actor_property_sites
      << ";nonisolated_sites=" << summary.objc_nonisolated_annotation_sites
      << ";executor_sites=" << summary.actor_member_executor_annotation_sites
      << ";async_sites=" << summary.actor_async_method_sites
      << ";metadata_sites=" << summary.actor_member_metadata_sites
      << ";deterministic=" << (summary.deterministic_handoff ? "true" : "false");
  return out.str();
}

std::string BuildPart7TaskGroupCancellationSourceClosureReplayKey(
    const Objc3FrontendPart7TaskGroupCancellationSourceClosureSummary &summary) {
  std::ostringstream out;
  out << summary.contract_id
      << ";async_callable_sites=" << summary.async_callable_sites
      << ";executor_sites=" << summary.executor_attribute_sites
      << ";task_creation_sites=" << summary.task_creation_sites
      << ";task_group_sites=" << summary.task_group_scope_sites << ":"
      << summary.task_group_add_task_sites << ":"
      << summary.task_group_wait_next_sites << ":"
      << summary.task_group_cancel_all_sites
      << ";cancellation_sites=" << summary.cancellation_check_sites << ":"
      << summary.cancellation_handler_sites
      << ";deterministic=" << (summary.deterministic_handoff ? "true" : "false");
  return out.str();
}

std::string BuildPart8SystemExtensionSourceClosureReplayKey(
    const Objc3FrontendPart8SystemExtensionSourceClosureSummary &summary) {
  std::ostringstream out;
  out << summary.contract_id
      << ";resource_sites=" << summary.resource_attribute_sites << ":"
      << summary.resource_close_clause_sites << ":"
      << summary.resource_invalid_clause_sites
      << ";borrowed_sites=" << summary.borrowed_pointer_sites
      << ";returns_borrowed_sites=" << summary.returns_borrowed_attribute_sites
      << ";capture_sites=" << summary.explicit_capture_list_sites << ":"
      << summary.explicit_capture_item_sites << ":"
      << summary.explicit_capture_weak_sites << ":"
      << summary.explicit_capture_unowned_sites << ":"
      << summary.explicit_capture_move_sites << ":"
      << summary.explicit_capture_plain_sites
      << ";deterministic=" << (summary.deterministic_handoff ? "true" : "false");
  return out.str();
}

std::string BuildPart8CleanupResourceCaptureSourceCompletionReplayKey(
    const Objc3FrontendPart8CleanupResourceCaptureSourceCompletionSummary
        &summary) {
  std::ostringstream out;
  out << summary.contract_id
      << ";cleanup_sites=" << summary.cleanup_attribute_sites << ":"
      << summary.cleanup_sugar_sites
      << ";resource_sites=" << summary.resource_attribute_sites << ":"
      << summary.resource_sugar_sites << ":"
      << summary.resource_close_clause_sites << ":"
      << summary.resource_invalid_clause_sites
      << ";capture_sites=" << summary.explicit_capture_list_sites << ":"
      << summary.explicit_capture_item_sites << ":"
      << summary.explicit_capture_weak_sites << ":"
      << summary.explicit_capture_unowned_sites << ":"
      << summary.explicit_capture_move_sites << ":"
      << summary.explicit_capture_plain_sites
      << ";deterministic=" << (summary.deterministic_handoff ? "true" : "false");
  return out.str();
}

std::string BuildPart8RetainableCFamilySourceCompletionReplayKey(
    const Objc3FrontendPart8RetainableCFamilySourceCompletionSummary &summary) {
  std::ostringstream out;
  out << summary.contract_id
      << ";family_sites=" << summary.family_retain_sites << ":"
      << summary.family_release_sites << ":"
      << summary.family_autorelease_sites
      << ";compat_sites=" << summary.compatibility_returns_retained_sites
      << ":" << summary.compatibility_returns_not_retained_sites
      << ":" << summary.compatibility_consumed_sites
      << ";deterministic=" << (summary.deterministic_handoff ? "true" : "false");
  return out.str();
}

std::string BuildPart9DispatchIntentSourceClosureReplayKey(
    const Objc3FrontendPart9DispatchIntentSourceClosureSummary &summary) {
  std::ostringstream out;
  out << summary.contract_id
      << ";callable_sites=" << summary.direct_callable_sites << ":"
      << summary.final_callable_sites << ":" << summary.dynamic_callable_sites
      << ";container_sites=" << summary.direct_members_container_sites << ":"
      << summary.final_container_sites << ":" << summary.sealed_container_sites
      << ":" << summary.actor_container_sites
      << ";deterministic="
      << (summary.deterministic_handoff ? "true" : "false");
  return out.str();
}

std::string BuildPart9DispatchIntentSourceCompletionReplayKey(
    const Objc3FrontendPart9DispatchIntentSourceCompletionSummary &summary) {
  std::ostringstream out;
  out << summary.contract_id
      << ";prefixed_container_sites="
      << summary.prefixed_container_attribute_sites
      << ";container_sites=" << summary.direct_members_container_sites << ":"
      << summary.final_container_sites << ":" << summary.sealed_container_sites
      << ";defaulting_sites=" << summary.effective_direct_member_sites << ":"
      << summary.direct_members_defaulted_method_sites << ":"
      << summary.direct_members_dynamic_opt_out_sites
      << ";deterministic="
      << (summary.deterministic_handoff ? "true" : "false");
  return out.str();
}

std::string BuildPart10MetaprogrammingSourceClosureReplayKey(
    const Objc3FrontendPart10MetaprogrammingSourceClosureSummary &summary) {
  std::ostringstream out;
  out << summary.contract_id
      << ";sites=" << summary.derive_marker_sites << ":"
      << summary.macro_marker_sites << ":"
      << summary.property_behavior_sites
      << ";deterministic="
      << (summary.deterministic_handoff ? "true" : "false");
  return out.str();
}

std::string BuildPart10MacroPackageProvenanceSourceCompletionReplayKey(
    const Objc3FrontendPart10MacroPackageProvenanceSourceCompletionSummary
        &summary) {
  std::ostringstream out;
  out << summary.contract_id
      << ";sites=" << summary.macro_marker_sites << ":"
      << summary.macro_package_sites << ":"
      << summary.macro_provenance_sites << ":"
      << summary.expansion_visible_macro_sites
      << ";deterministic="
      << (summary.deterministic_handoff ? "true" : "false");
  return out.str();
}

std::string BuildPart10PropertyBehaviorSourceCompletionReplayKey(
    const Objc3FrontendPart10PropertyBehaviorSourceCompletionSummary
        &summary) {
  std::ostringstream out;
  out << summary.contract_id
      << ";sites=" << summary.property_behavior_sites << ":"
      << summary.interface_property_behavior_sites << ":"
      << summary.implementation_property_behavior_sites << ":"
      << summary.protocol_property_behavior_sites << ":"
      << summary.synthesized_binding_visible_sites << ":"
      << summary.synthesized_getter_visible_sites << ":"
      << summary.synthesized_setter_visible_sites
      << ";deterministic="
      << (summary.deterministic_handoff ? "true" : "false");
  return out.str();
}

std::string BuildPart11ForeignImportSourceClosureReplayKey(
    const Objc3FrontendPart11ForeignImportSourceClosureSummary &summary) {
  std::ostringstream out;
  out << summary.contract_id
      << ";sites=" << summary.foreign_callable_sites << ":"
      << summary.extern_foreign_callable_sites << ":"
      << summary.import_module_annotation_sites << ":"
      << summary.imported_module_name_sites << ":"
      << summary.interop_annotation_sites
      << ";deterministic="
      << (summary.deterministic_handoff ? "true" : "false");
  return out.str();
}

std::string BuildPart11CppSwiftInteropAnnotationSourceCompletionReplayKey(
    const Objc3FrontendPart11CppSwiftInteropAnnotationSourceCompletionSummary
        &summary) {
  std::ostringstream out;
  out << summary.contract_id
      << ";sites=" << summary.swift_name_annotation_sites << ":"
      << summary.swift_private_annotation_sites << ":"
      << summary.cpp_name_annotation_sites << ":"
      << summary.header_name_annotation_sites << ":"
      << summary.interop_metadata_annotation_sites << ":"
      << summary.named_annotation_payload_sites
      << ";deterministic="
      << (summary.deterministic_handoff ? "true" : "false");
  return out.str();
}

std::string BuildPart12DiagnosticsMigratorSourceInventoryReplayKey(
    const Objc3FrontendPart12DiagnosticsMigratorSourceInventorySummary
        &summary) {
  std::ostringstream out;
  out << "families=" << summary.advanced_feature_family_count
      << ";dependencies=" << summary.dependency_surface_count
      << ";claims=" << summary.aggregated_source_only_claim_count
      << ";fail-closed=" << summary.fail_closed_construct_count
      << ";sites=" << summary.diagnostic_surface_sites << ":"
      << summary.fixit_surface_sites << ":" << summary.migrator_surface_sites
      << ":" << summary.canonicalization_hint_sites
      << ";parts=" << summary.error_surface_sites << ":"
      << summary.concurrency_surface_sites << ":"
      << summary.system_surface_sites << ":" << summary.dispatch_surface_sites
      << ":" << summary.metaprogramming_surface_sites << ":"
      << summary.interop_surface_sites
      << ";supported="
      << (summary.diagnostics_inventory_source_supported ? "true" : "false")
      << ":" << (summary.fixit_inventory_source_supported ? "true" : "false")
      << ":" << (summary.migrator_inventory_source_supported ? "true" : "false")
      << ";deterministic="
      << (summary.deterministic_handoff ? "true" : "false");
  return out.str();
}

std::string BuildPart12MigrationCanonicalizationSourceCompletionReplayKey(
    const Objc3FrontendPart12MigrationCanonicalizationSourceCompletionSummary
        &summary) {
  std::ostringstream out;
  out << "compat=" << summary.compatibility_mode
      << ";migration-assist="
      << (summary.migration_assist_enabled ? "true" : "false")
      << ";legacy=" << summary.legacy_yes_sites << ":" << summary.legacy_no_sites
      << ":" << summary.legacy_null_sites << ":" << summary.legacy_total_sites
      << ";canonical=" << summary.canonical_true_rewrite_sites << ":"
      << summary.canonical_false_rewrite_sites << ":"
      << summary.canonical_nil_rewrite_sites
      << ";candidates=" << summary.canonicalization_candidate_sites << ":"
      << summary.fixit_candidate_sites << ":"
      << summary.migrator_candidate_sites
      << ";ready="
      << (summary.ready_for_semantic_expansion ? "true" : "false")
      << ";deterministic="
      << (summary.deterministic_handoff ? "true" : "false");
  return out.str();
}

std::string BuildPart12DiagnosticTaxonomyPortabilityContractReplayKey(
    const Objc3Part12DiagnosticTaxonomyPortabilityContractSummary &summary) {
  std::ostringstream out;
  out << "portability-dependencies=" << summary.portability_dependency_count
      << ";diagnostics=" << summary.diagnostics_total << ":"
      << summary.diagnostics_after_pass_final << ":"
      << summary.diagnostics_emitted_total
      << ";arc-fixit=" << summary.ownership_arc_diagnostic_candidate_sites << ":"
      << summary.ownership_arc_fixit_available_sites << ":"
      << summary.ownership_arc_profiled_sites << ":"
      << summary.ownership_arc_weak_unowned_conflict_diagnostic_sites << ":"
      << summary.ownership_arc_empty_fixit_hint_sites << ":"
      << summary.ownership_arc_contract_violation_sites
      << ";migration-candidates="
      << summary.migration_canonicalization_candidate_sites
      << ";ready="
      << (summary.ready_for_lowering_and_runtime ? "true" : "false")
      << ";deterministic="
      << (summary.deterministic_handoff ? "true" : "false");
  return out.str();
}

std::string BuildPart12FeatureSpecificFixitSynthesisReplayKey(
    const Objc3Part12FeatureSpecificFixitSynthesisSummary &summary) {
  std::ostringstream out;
  out << "families=" << summary.fixit_family_count
      << ";migration=" << summary.migration_fixit_candidate_sites << ":"
      << summary.migrator_candidate_sites
      << ";ownership-arc=" << summary.ownership_arc_fixit_available_sites << ":"
      << summary.ownership_arc_empty_fixit_hint_sites
      << ";ready="
      << (summary.ready_for_lowering_and_runtime ? "true" : "false")
      << ";deterministic="
      << (summary.deterministic_handoff ? "true" : "false");
  return out.str();
}

std::string BuildPart7LowercaseProfileToken(std::string token) {
  std::transform(token.begin(), token.end(), token.begin(),
                 [](unsigned char value) {
                   return static_cast<char>(std::tolower(value));
                 });
  return token;
}

bool IsPart7TaskCreationSymbol(const std::string &symbol) {
  if (symbol.empty()) {
    return false;
  }
  const std::string lowered = BuildPart7LowercaseProfileToken(symbol);
  return lowered.find("task_spawn") != std::string::npos ||
         lowered.find("spawn_task") != std::string::npos ||
         lowered.find("detached_task") != std::string::npos ||
         lowered.find("task_detach") != std::string::npos;
}

bool IsPart7TaskGroupScopeSymbol(const std::string &symbol) {
  if (symbol.empty()) {
    return false;
  }
  const std::string lowered = BuildPart7LowercaseProfileToken(symbol);
  return lowered.find("with_task_group") != std::string::npos ||
         lowered.find("task_group_scope") != std::string::npos;
}

bool IsPart7TaskGroupAddTaskSymbol(const std::string &symbol) {
  if (symbol.empty()) {
    return false;
  }
  const std::string lowered = BuildPart7LowercaseProfileToken(symbol);
  return lowered.find("task_group_add_task") != std::string::npos ||
         lowered.find("group_add_task") != std::string::npos;
}

bool IsPart7TaskGroupWaitNextSymbol(const std::string &symbol) {
  if (symbol.empty()) {
    return false;
  }
  const std::string lowered = BuildPart7LowercaseProfileToken(symbol);
  return lowered.find("task_group_wait_next") != std::string::npos ||
         lowered.find("group_wait_next") != std::string::npos ||
         lowered.find("wait_next") != std::string::npos;
}

bool IsPart7TaskGroupCancelAllSymbol(const std::string &symbol) {
  if (symbol.empty()) {
    return false;
  }
  const std::string lowered = BuildPart7LowercaseProfileToken(symbol);
  return lowered.find("task_group_cancel_all") != std::string::npos ||
         lowered.find("group_cancel_all") != std::string::npos ||
         lowered.find("cancel_all") != std::string::npos;
}

bool IsPart7CancellationCheckSymbol(const std::string &symbol) {
  if (symbol.empty()) {
    return false;
  }
  const std::string lowered = BuildPart7LowercaseProfileToken(symbol);
  return lowered.find("cancelled") != std::string::npos ||
         lowered.find("is_cancelled") != std::string::npos ||
         lowered.find("cancellation") != std::string::npos;
}

bool IsPart7CancellationHandlerSymbol(const std::string &symbol) {
  if (symbol.empty()) {
    return false;
  }
  const std::string lowered = BuildPart7LowercaseProfileToken(symbol);
  return lowered.find("on_cancel") != std::string::npos ||
         lowered.find("cancel_handler") != std::string::npos ||
         lowered.find("with_cancellation_handler") != std::string::npos;
}

void CollectPart7AsyncSourceClosureExprSites(
    const Expr *expr, Objc3FrontendPart7AsyncSourceClosureSummary &summary) {
  if (expr == nullptr) {
    return;
  }
  if (expr->await_expression_enabled) {
    ++summary.await_expression_sites;
  }
  CollectPart7AsyncSourceClosureExprSites(expr->receiver.get(), summary);
  CollectPart7AsyncSourceClosureExprSites(expr->left.get(), summary);
  CollectPart7AsyncSourceClosureExprSites(expr->right.get(), summary);
  CollectPart7AsyncSourceClosureExprSites(expr->third.get(), summary);
  for (const auto &arg : expr->args) {
    CollectPart7AsyncSourceClosureExprSites(arg.get(), summary);
  }
}

void CollectPart7AsyncSourceClosureStmtSites(
    const Stmt *stmt, Objc3FrontendPart7AsyncSourceClosureSummary &summary) {
  if (stmt == nullptr) {
    return;
  }
  switch (stmt->kind) {
  case Stmt::Kind::Let:
    if (stmt->let_stmt != nullptr) {
      CollectPart7AsyncSourceClosureExprSites(stmt->let_stmt->value.get(), summary);
    }
    return;
  case Stmt::Kind::Return:
    if (stmt->return_stmt != nullptr) {
      CollectPart7AsyncSourceClosureExprSites(stmt->return_stmt->value.get(), summary);
    }
    return;
  case Stmt::Kind::If:
    if (stmt->if_stmt != nullptr) {
      CollectPart7AsyncSourceClosureExprSites(stmt->if_stmt->condition.get(), summary);
      for (const auto &then_stmt : stmt->if_stmt->then_body) {
        CollectPart7AsyncSourceClosureStmtSites(then_stmt.get(), summary);
      }
      for (const auto &else_stmt : stmt->if_stmt->else_body) {
        CollectPart7AsyncSourceClosureStmtSites(else_stmt.get(), summary);
      }
    }
    return;
  case Stmt::Kind::DoWhile:
    if (stmt->do_while_stmt != nullptr) {
      for (const auto &body_stmt : stmt->do_while_stmt->body) {
        CollectPart7AsyncSourceClosureStmtSites(body_stmt.get(), summary);
      }
      CollectPart7AsyncSourceClosureExprSites(stmt->do_while_stmt->condition.get(), summary);
    }
    return;
  case Stmt::Kind::For:
    if (stmt->for_stmt != nullptr) {
      CollectPart7AsyncSourceClosureExprSites(stmt->for_stmt->init.value.get(), summary);
      CollectPart7AsyncSourceClosureExprSites(stmt->for_stmt->condition.get(), summary);
      CollectPart7AsyncSourceClosureExprSites(stmt->for_stmt->step.value.get(), summary);
      for (const auto &body_stmt : stmt->for_stmt->body) {
        CollectPart7AsyncSourceClosureStmtSites(body_stmt.get(), summary);
      }
    }
    return;
  case Stmt::Kind::Switch:
    if (stmt->switch_stmt != nullptr) {
      CollectPart7AsyncSourceClosureExprSites(stmt->switch_stmt->condition.get(), summary);
      for (const auto &switch_case : stmt->switch_stmt->cases) {
        for (const auto &case_stmt : switch_case.body) {
          CollectPart7AsyncSourceClosureStmtSites(case_stmt.get(), summary);
        }
      }
    }
    return;
  case Stmt::Kind::While:
    if (stmt->while_stmt != nullptr) {
      CollectPart7AsyncSourceClosureExprSites(stmt->while_stmt->condition.get(), summary);
      for (const auto &body_stmt : stmt->while_stmt->body) {
        CollectPart7AsyncSourceClosureStmtSites(body_stmt.get(), summary);
      }
    }
    return;
  case Stmt::Kind::Block:
  case Stmt::Kind::Defer:
    if (stmt->block_stmt != nullptr) {
      for (const auto &body_stmt : stmt->block_stmt->body) {
        CollectPart7AsyncSourceClosureStmtSites(body_stmt.get(), summary);
      }
    }
    return;
  case Stmt::Kind::Expr:
    if (stmt->expr_stmt != nullptr) {
      CollectPart7AsyncSourceClosureExprSites(stmt->expr_stmt->value.get(), summary);
    }
    return;
  case Stmt::Kind::Break:
  case Stmt::Kind::Continue:
  case Stmt::Kind::Empty:
    return;
  }
}

void CollectPart7TaskGroupCancellationExprSites(
    const Expr *expr,
    Objc3FrontendPart7TaskGroupCancellationSourceClosureSummary &summary) {
  if (expr == nullptr) {
    return;
  }
  const auto collect_symbol = [&summary](const std::string &symbol) {
    if (IsPart7TaskCreationSymbol(symbol)) {
      ++summary.task_creation_sites;
    }
    if (IsPart7TaskGroupScopeSymbol(symbol)) {
      ++summary.task_group_scope_sites;
    }
    if (IsPart7TaskGroupAddTaskSymbol(symbol)) {
      ++summary.task_group_add_task_sites;
    }
    if (IsPart7TaskGroupWaitNextSymbol(symbol)) {
      ++summary.task_group_wait_next_sites;
    }
    if (IsPart7TaskGroupCancelAllSymbol(symbol)) {
      ++summary.task_group_cancel_all_sites;
    }
    if (IsPart7CancellationCheckSymbol(symbol)) {
      ++summary.cancellation_check_sites;
    }
    if (IsPart7CancellationHandlerSymbol(symbol)) {
      ++summary.cancellation_handler_sites;
    }
  };

  switch (expr->kind) {
  case Expr::Kind::Call:
    collect_symbol(expr->ident);
    break;
  case Expr::Kind::MessageSend:
    collect_symbol(expr->selector);
    break;
  default:
    break;
  }

  CollectPart7TaskGroupCancellationExprSites(expr->receiver.get(), summary);
  CollectPart7TaskGroupCancellationExprSites(expr->left.get(), summary);
  CollectPart7TaskGroupCancellationExprSites(expr->right.get(), summary);
  CollectPart7TaskGroupCancellationExprSites(expr->third.get(), summary);
  for (const auto &arg : expr->args) {
    CollectPart7TaskGroupCancellationExprSites(arg.get(), summary);
  }
}

void CollectPart7TaskGroupCancellationStmtSites(
    const Stmt *stmt,
    Objc3FrontendPart7TaskGroupCancellationSourceClosureSummary &summary) {
  if (stmt == nullptr) {
    return;
  }
  switch (stmt->kind) {
  case Stmt::Kind::Let:
    if (stmt->let_stmt != nullptr) {
      CollectPart7TaskGroupCancellationExprSites(
          stmt->let_stmt->value.get(), summary);
    }
    return;
  case Stmt::Kind::Return:
    if (stmt->return_stmt != nullptr) {
      CollectPart7TaskGroupCancellationExprSites(
          stmt->return_stmt->value.get(), summary);
    }
    return;
  case Stmt::Kind::If:
    if (stmt->if_stmt != nullptr) {
      CollectPart7TaskGroupCancellationExprSites(
          stmt->if_stmt->condition.get(), summary);
      for (const auto &then_stmt : stmt->if_stmt->then_body) {
        CollectPart7TaskGroupCancellationStmtSites(then_stmt.get(), summary);
      }
      for (const auto &else_stmt : stmt->if_stmt->else_body) {
        CollectPart7TaskGroupCancellationStmtSites(else_stmt.get(), summary);
      }
    }
    return;
  case Stmt::Kind::DoWhile:
    if (stmt->do_while_stmt != nullptr) {
      for (const auto &body_stmt : stmt->do_while_stmt->body) {
        CollectPart7TaskGroupCancellationStmtSites(body_stmt.get(), summary);
      }
      CollectPart7TaskGroupCancellationExprSites(
          stmt->do_while_stmt->condition.get(), summary);
    }
    return;
  case Stmt::Kind::For:
    if (stmt->for_stmt != nullptr) {
      CollectPart7TaskGroupCancellationExprSites(
          stmt->for_stmt->init.value.get(), summary);
      CollectPart7TaskGroupCancellationExprSites(
          stmt->for_stmt->condition.get(), summary);
      CollectPart7TaskGroupCancellationExprSites(
          stmt->for_stmt->step.value.get(), summary);
      for (const auto &body_stmt : stmt->for_stmt->body) {
        CollectPart7TaskGroupCancellationStmtSites(body_stmt.get(), summary);
      }
    }
    return;
  case Stmt::Kind::Switch:
    if (stmt->switch_stmt != nullptr) {
      CollectPart7TaskGroupCancellationExprSites(
          stmt->switch_stmt->condition.get(), summary);
      for (const auto &switch_case : stmt->switch_stmt->cases) {
        for (const auto &case_stmt : switch_case.body) {
          CollectPart7TaskGroupCancellationStmtSites(case_stmt.get(), summary);
        }
      }
    }
    return;
  case Stmt::Kind::While:
    if (stmt->while_stmt != nullptr) {
      CollectPart7TaskGroupCancellationExprSites(
          stmt->while_stmt->condition.get(), summary);
      for (const auto &body_stmt : stmt->while_stmt->body) {
        CollectPart7TaskGroupCancellationStmtSites(body_stmt.get(), summary);
      }
    }
    return;
  case Stmt::Kind::Block:
  case Stmt::Kind::Defer:
    if (stmt->block_stmt != nullptr) {
      for (const auto &body_stmt : stmt->block_stmt->body) {
        CollectPart7TaskGroupCancellationStmtSites(body_stmt.get(), summary);
      }
    }
    return;
  case Stmt::Kind::Expr:
    if (stmt->expr_stmt != nullptr) {
      CollectPart7TaskGroupCancellationExprSites(
          stmt->expr_stmt->value.get(), summary);
    }
    return;
  case Stmt::Kind::Break:
  case Stmt::Kind::Continue:
  case Stmt::Kind::Empty:
    return;
  }
}

void CollectPart3TypeSourceClosureExprSites(
    const Expr *expr, Objc3FrontendPart3TypeSourceClosureSummary &summary) {
  if (expr == nullptr) {
    return;
  }
  if (expr->kind == Expr::Kind::Binary && expr->op == "??") {
    ++summary.nil_coalescing_sites;
  }
  if (expr->kind == Expr::Kind::MessageSend && expr->optional_send_enabled) {
    ++summary.optional_send_sites;
  }
  if (expr->kind == Expr::Kind::MessageSend &&
      expr->optional_member_access_enabled) {
    ++summary.optional_member_access_sites;
  }
  if (expr->typed_keypath_literal_enabled) {
    ++summary.typed_keypath_literal_sites;
  }
  CollectPart3TypeSourceClosureExprSites(expr->receiver.get(), summary);
  CollectPart3TypeSourceClosureExprSites(expr->left.get(), summary);
  CollectPart3TypeSourceClosureExprSites(expr->right.get(), summary);
  CollectPart3TypeSourceClosureExprSites(expr->third.get(), summary);
  for (const auto &arg : expr->args) {
    CollectPart3TypeSourceClosureExprSites(arg.get(), summary);
  }
  for (const auto &stmt : expr->block_body) {
    if (stmt != nullptr) {
      const Stmt *nested = stmt.get();
      switch (nested->kind) {
      case Stmt::Kind::Let:
        if (nested->let_stmt != nullptr) {
          CollectPart3TypeSourceClosureExprSites(nested->let_stmt->value.get(), summary);
        }
        break;
      case Stmt::Kind::Assign:
        if (nested->assign_stmt != nullptr) {
          CollectPart3TypeSourceClosureExprSites(nested->assign_stmt->value.get(), summary);
        }
        break;
      case Stmt::Kind::Return:
        if (nested->return_stmt != nullptr) {
          CollectPart3TypeSourceClosureExprSites(nested->return_stmt->value.get(), summary);
        }
        break;
      case Stmt::Kind::Expr:
        if (nested->expr_stmt != nullptr) {
          CollectPart3TypeSourceClosureExprSites(nested->expr_stmt->value.get(), summary);
        }
        break;
      case Stmt::Kind::If:
      case Stmt::Kind::DoWhile:
      case Stmt::Kind::For:
      case Stmt::Kind::Switch:
      case Stmt::Kind::While:
      case Stmt::Kind::Block:
      case Stmt::Kind::Defer:
      case Stmt::Kind::Break:
      case Stmt::Kind::Continue:
      case Stmt::Kind::Empty:
        break;
      }
    }
  }
}

void CollectPart3TypeSourceClosureStmtSites(
    const Stmt *stmt, Objc3FrontendPart3TypeSourceClosureSummary &summary) {
  if (stmt == nullptr) {
    return;
  }
  switch (stmt->kind) {
  case Stmt::Kind::Let:
    if (stmt->let_stmt != nullptr) {
      CollectPart3TypeSourceClosureExprSites(stmt->let_stmt->value.get(), summary);
    }
    break;
  case Stmt::Kind::Assign:
    if (stmt->assign_stmt != nullptr) {
      CollectPart3TypeSourceClosureExprSites(stmt->assign_stmt->value.get(), summary);
    }
    break;
  case Stmt::Kind::Return:
    if (stmt->return_stmt != nullptr) {
      CollectPart3TypeSourceClosureExprSites(stmt->return_stmt->value.get(), summary);
    }
    break;
  case Stmt::Kind::Expr:
    if (stmt->expr_stmt != nullptr) {
      CollectPart3TypeSourceClosureExprSites(stmt->expr_stmt->value.get(), summary);
    }
    break;
  case Stmt::Kind::If:
    if (stmt->if_stmt != nullptr) {
      CollectPart3TypeSourceClosureExprSites(stmt->if_stmt->condition.get(), summary);
      for (const auto &child : stmt->if_stmt->then_body) {
        CollectPart3TypeSourceClosureStmtSites(child.get(), summary);
      }
      for (const auto &child : stmt->if_stmt->else_body) {
        CollectPart3TypeSourceClosureStmtSites(child.get(), summary);
      }
    }
    break;
  case Stmt::Kind::DoWhile:
    if (stmt->do_while_stmt != nullptr) {
      CollectPart3TypeSourceClosureExprSites(stmt->do_while_stmt->condition.get(), summary);
      for (const auto &child : stmt->do_while_stmt->body) {
        CollectPart3TypeSourceClosureStmtSites(child.get(), summary);
      }
    }
    break;
  case Stmt::Kind::For:
    if (stmt->for_stmt != nullptr) {
      CollectPart3TypeSourceClosureExprSites(stmt->for_stmt->init.value.get(), summary);
      CollectPart3TypeSourceClosureExprSites(stmt->for_stmt->condition.get(), summary);
      CollectPart3TypeSourceClosureExprSites(stmt->for_stmt->step.value.get(), summary);
      for (const auto &child : stmt->for_stmt->body) {
        CollectPart3TypeSourceClosureStmtSites(child.get(), summary);
      }
    }
    break;
  case Stmt::Kind::Switch:
    if (stmt->switch_stmt != nullptr) {
      CollectPart3TypeSourceClosureExprSites(stmt->switch_stmt->condition.get(), summary);
      for (const auto &case_stmt : stmt->switch_stmt->cases) {
        for (const auto &child : case_stmt.body) {
          CollectPart3TypeSourceClosureStmtSites(child.get(), summary);
        }
      }
    }
    break;
  case Stmt::Kind::While:
    if (stmt->while_stmt != nullptr) {
      CollectPart3TypeSourceClosureExprSites(stmt->while_stmt->condition.get(), summary);
      for (const auto &child : stmt->while_stmt->body) {
        CollectPart3TypeSourceClosureStmtSites(child.get(), summary);
      }
    }
    break;
  case Stmt::Kind::Block:
      case Stmt::Kind::Defer:
    if (stmt->block_stmt != nullptr) {
      for (const auto &child : stmt->block_stmt->body) {
        CollectPart3TypeSourceClosureStmtSites(child.get(), summary);
      }
    }
    break;
  case Stmt::Kind::Break:
  case Stmt::Kind::Continue:
  case Stmt::Kind::Empty:
    break;
  }
}

Objc3FrontendPart3TypeSourceClosureSummary BuildPart3TypeSourceClosureSummary(
    const Objc3Program &program,
    const Objc3FrontendObjectPointerNullabilityGenericsSummary
        &object_pointer_summary) {
  Objc3FrontendPart3TypeSourceClosureSummary summary;
  for (const auto &protocol_decl : program.protocols) {
    for (const auto &method_decl : protocol_decl.methods) {
      if (method_decl.protocol_requirement_kind ==
          Objc3ProtocolRequirementKind::Optional) {
        ++summary.protocol_optional_method_count;
      } else {
        ++summary.protocol_required_method_count;
      }
    }
    for (const auto &property_decl : protocol_decl.properties) {
      if (property_decl.protocol_requirement_kind ==
          Objc3ProtocolRequirementKind::Optional) {
        ++summary.protocol_optional_property_count;
      } else {
        ++summary.protocol_required_property_count;
      }
    }
  }

  summary.object_pointer_type_spelling_sites =
      object_pointer_summary.object_pointer_type_spellings;
  summary.pointer_declarator_entries =
      object_pointer_summary.pointer_declarator_entries;
  summary.nullability_suffix_entries =
      object_pointer_summary.nullability_suffix_entries;
  summary.generic_suffix_entries =
      object_pointer_summary.generic_suffix_entries;
  summary.protocol_optional_partition_source_supported = true;
  summary.object_pointer_nullability_source_supported = true;
  summary.pragmatic_generic_suffix_source_supported = true;
  summary.optional_binding_source_supported = true;
  summary.optional_send_source_supported = true;
  summary.nil_coalescing_source_supported = true;
  summary.typed_keypath_literal_source_supported = true;
  summary.optional_member_access_fail_closed = false;
  for (const auto &fn : program.functions) {
    for (const auto &stmt : fn.body) {
      CollectPart3TypeSourceClosureStmtSites(stmt.get(), summary);
    }
  }
  for (const auto &implementation : program.implementations) {
    for (const auto &method : implementation.methods) {
      for (const auto &stmt : method.body) {
        CollectPart3TypeSourceClosureStmtSites(stmt.get(), summary);
      }
    }
  }
  summary.deterministic_handoff =
      object_pointer_summary
          .deterministic_object_pointer_nullability_generics_handoff &&
      summary.protocol_required_method_count +
              summary.protocol_optional_method_count >=
          summary.protocol_optional_method_count &&
      summary.protocol_required_property_count +
              summary.protocol_optional_property_count >=
          summary.protocol_optional_property_count;
  summary.ready_for_semantic_expansion = summary.deterministic_handoff;
  summary.replay_key = BuildPart3TypeSourceClosureReplayKey(summary);
  return summary;
}

void CollectPart5ControlFlowSourceClosureStmtSites(
    const Stmt *stmt,
    Objc3FrontendPart5ControlFlowSourceClosureSummary &summary) {
  if (stmt == nullptr) {
    return;
  }
  switch (stmt->kind) {
  case Stmt::Kind::Let:
    break;
  case Stmt::Kind::Assign:
  case Stmt::Kind::Return:
  case Stmt::Kind::Expr:
    break;
  case Stmt::Kind::If:
    if (stmt->if_stmt != nullptr) {
      if (stmt->if_stmt->guard_binding_surface_enabled) {
        ++summary.guard_binding_sites;
        summary.guard_binding_clause_sites +=
            stmt->if_stmt->optional_binding_clause_count;
      }
      if (stmt->if_stmt->guard_condition_list_surface_enabled) {
        summary.guard_boolean_condition_sites +=
            stmt->if_stmt->guard_boolean_condition_clause_count;
      }
      for (const auto &child : stmt->if_stmt->then_body) {
        CollectPart5ControlFlowSourceClosureStmtSites(child.get(), summary);
      }
      for (const auto &child : stmt->if_stmt->else_body) {
        CollectPart5ControlFlowSourceClosureStmtSites(child.get(), summary);
      }
    }
    break;
  case Stmt::Kind::DoWhile:
    if (stmt->do_while_stmt != nullptr) {
      for (const auto &child : stmt->do_while_stmt->body) {
        CollectPart5ControlFlowSourceClosureStmtSites(child.get(), summary);
      }
    }
    break;
  case Stmt::Kind::For:
    if (stmt->for_stmt != nullptr) {
      for (const auto &child : stmt->for_stmt->body) {
        CollectPart5ControlFlowSourceClosureStmtSites(child.get(), summary);
      }
    }
    break;
  case Stmt::Kind::Switch:
    if (stmt->switch_stmt != nullptr) {
      if (stmt->switch_stmt->match_surface_enabled) {
        ++summary.match_statement_sites;
      } else {
        for (const auto &case_stmt : stmt->switch_stmt->cases) {
          if (case_stmt.is_default) {
            ++summary.switch_default_pattern_sites;
          } else {
            ++summary.switch_case_pattern_sites;
          }
          for (const auto &child : case_stmt.body) {
            CollectPart5ControlFlowSourceClosureStmtSites(child.get(), summary);
          }
        }
        break;
      }
      for (const auto &case_stmt : stmt->switch_stmt->cases) {
        if (case_stmt.is_default) {
          ++summary.match_default_sites;
        } else {
          ++summary.match_case_pattern_sites;
          switch (case_stmt.match_pattern_kind) {
          case MatchPatternKind::Wildcard:
            ++summary.match_wildcard_pattern_sites;
            break;
          case MatchPatternKind::LiteralInteger:
          case MatchPatternKind::LiteralBool:
          case MatchPatternKind::LiteralNil:
            ++summary.match_literal_pattern_sites;
            break;
          case MatchPatternKind::Binding:
            ++summary.match_binding_pattern_sites;
            break;
          case MatchPatternKind::ResultCase:
            ++summary.match_result_case_pattern_sites;
            break;
          case MatchPatternKind::None:
            break;
          }
        }
        for (const auto &child : case_stmt.body) {
          CollectPart5ControlFlowSourceClosureStmtSites(child.get(), summary);
        }
      }
    }
    break;
  case Stmt::Kind::While:
    if (stmt->while_stmt != nullptr) {
      for (const auto &child : stmt->while_stmt->body) {
        CollectPart5ControlFlowSourceClosureStmtSites(child.get(), summary);
      }
    }
    break;
  case Stmt::Kind::Block:
      case Stmt::Kind::Defer:
    if (stmt->block_stmt != nullptr) {
      for (const auto &child : stmt->block_stmt->body) {
        CollectPart5ControlFlowSourceClosureStmtSites(child.get(), summary);
      }
    }
    break;
  case Stmt::Kind::Break:
  case Stmt::Kind::Continue:
  case Stmt::Kind::Empty:
    break;
  }
}

Objc3FrontendPart5ControlFlowSourceClosureSummary
BuildPart5ControlFlowSourceClosureSummary(
    const Objc3Program &program,
    const std::vector<Objc3LexToken> &tokens) {
  Objc3FrontendPart5ControlFlowSourceClosureSummary summary;
  for (const auto &token : tokens) {
    if (token.kind == Objc3LexTokenKind::KwDefer) {
      ++summary.defer_keyword_sites;
    }
  }
  for (const auto &fn : program.functions) {
    for (const auto &stmt : fn.body) {
      CollectPart5ControlFlowSourceClosureStmtSites(stmt.get(), summary);
    }
  }
  for (const auto &implementation : program.implementations) {
    for (const auto &method : implementation.methods) {
      for (const auto &stmt : method.body) {
        CollectPart5ControlFlowSourceClosureStmtSites(stmt.get(), summary);
      }
    }
  }
  summary.guard_binding_source_supported = true;
  summary.guard_condition_list_source_supported = true;
  summary.switch_case_pattern_source_supported = true;
  summary.defer_statement_source_supported = true;
  summary.match_statement_source_supported = true;
  summary.match_wildcard_pattern_source_supported = true;
  summary.match_literal_pattern_source_supported = true;
  summary.match_binding_pattern_source_supported = true;
  summary.match_result_case_pattern_source_supported = true;
  summary.defer_keyword_reserved = true;
  summary.defer_fail_closed = false;
  summary.match_expression_fail_closed = true;
  summary.guarded_pattern_fail_closed = true;
  summary.type_test_pattern_fail_closed = true;
  summary.deterministic_handoff =
      summary.guard_binding_clause_sites >= summary.guard_binding_sites &&
      summary.guard_boolean_condition_sites +
              summary.guard_binding_sites >=
          summary.guard_binding_sites &&
      summary.switch_default_pattern_sites <=
          summary.switch_case_pattern_sites + summary.switch_default_pattern_sites &&
      summary.match_default_sites <=
          summary.match_statement_sites + summary.match_default_sites;
  summary.ready_for_semantic_expansion = summary.deterministic_handoff;
  summary.replay_key = BuildPart5ControlFlowSourceClosureReplayKey(summary);
  return summary;
}

Objc3FrontendPart6ErrorSourceClosureSummary
BuildPart6ErrorSourceClosureSummary(const Objc3Program &program,
                                    const std::vector<Objc3LexToken> &tokens) {
  Objc3FrontendPart6ErrorSourceClosureSummary summary;
  for (const auto &token : tokens) {
    if (token.kind == Objc3LexTokenKind::KwTry) {
      ++summary.try_keyword_sites;
    } else if (token.kind == Objc3LexTokenKind::KwThrow) {
      ++summary.throw_keyword_sites;
    } else if (token.kind == Objc3LexTokenKind::KwCatch) {
      ++summary.catch_keyword_sites;
    }
  }

  bool throws_profiles_normalized = true;
  bool result_profiles_normalized = true;
  bool ns_error_profiles_normalized = true;
  for (const auto &fn : program.functions) {
    if (fn.throws_declared) {
      ++summary.function_throws_declaration_sites;
    }
    throws_profiles_normalized =
        throws_profiles_normalized && fn.throws_declaration_profile_is_normalized;
    result_profiles_normalized =
        result_profiles_normalized && fn.result_like_profile_is_normalized;
    ns_error_profiles_normalized =
        ns_error_profiles_normalized && fn.ns_error_bridging_profile_is_normalized;
    summary.result_like_sites += fn.result_like_sites;
    summary.result_success_sites += fn.result_success_sites;
    summary.result_failure_sites += fn.result_failure_sites;
    summary.result_branch_sites += fn.result_branch_sites;
    summary.result_payload_sites += fn.result_payload_sites;
    summary.ns_error_bridging_sites += fn.ns_error_bridging_sites;
    summary.ns_error_out_parameter_sites += fn.ns_error_out_parameter_sites;
    summary.ns_error_bridge_path_sites += fn.ns_error_bridge_path_sites;
    summary.objc_nserror_attribute_sites += fn.objc_nserror_attribute_sites;
    summary.objc_status_code_attribute_sites +=
        fn.objc_status_code_attribute_sites;
    summary.status_code_success_clause_sites +=
        fn.status_code_success_clause_sites;
    summary.status_code_error_type_clause_sites +=
        fn.status_code_error_type_clause_sites;
    summary.status_code_mapping_clause_sites +=
        fn.status_code_mapping_clause_sites;
    throws_profiles_normalized =
        throws_profiles_normalized && fn.error_bridge_marker_profile_is_normalized;
  }
  for (const auto &implementation : program.implementations) {
    for (const auto &method : implementation.methods) {
      if (method.throws_declared) {
        ++summary.method_throws_declaration_sites;
      }
      throws_profiles_normalized =
          throws_profiles_normalized &&
          method.throws_declaration_profile_is_normalized;
      result_profiles_normalized =
          result_profiles_normalized && method.result_like_profile_is_normalized;
      ns_error_profiles_normalized =
          ns_error_profiles_normalized &&
          method.ns_error_bridging_profile_is_normalized;
      summary.result_like_sites += method.result_like_sites;
      summary.result_success_sites += method.result_success_sites;
      summary.result_failure_sites += method.result_failure_sites;
      summary.result_branch_sites += method.result_branch_sites;
      summary.result_payload_sites += method.result_payload_sites;
      summary.ns_error_bridging_sites += method.ns_error_bridging_sites;
      summary.ns_error_out_parameter_sites +=
          method.ns_error_out_parameter_sites;
      summary.ns_error_bridge_path_sites += method.ns_error_bridge_path_sites;
      summary.objc_nserror_attribute_sites +=
          method.objc_nserror_attribute_sites;
      summary.objc_status_code_attribute_sites +=
          method.objc_status_code_attribute_sites;
      summary.status_code_success_clause_sites +=
          method.status_code_success_clause_sites;
      summary.status_code_error_type_clause_sites +=
          method.status_code_error_type_clause_sites;
      summary.status_code_mapping_clause_sites +=
          method.status_code_mapping_clause_sites;
      throws_profiles_normalized =
          throws_profiles_normalized &&
          method.error_bridge_marker_profile_is_normalized;
    }
  }

  summary.throws_declaration_source_supported = true;
  summary.result_carrier_source_supported = true;
  summary.ns_error_bridging_source_supported = true;
  summary.error_bridge_marker_source_supported = true;
  summary.try_keyword_reserved = true;
  summary.throw_keyword_reserved = true;
  summary.catch_keyword_reserved = true;
  summary.try_fail_closed = true;
  summary.throw_fail_closed = true;
  summary.do_catch_fail_closed = true;
  summary.deterministic_handoff =
      throws_profiles_normalized && result_profiles_normalized &&
      ns_error_profiles_normalized &&
      summary.status_code_success_clause_sites <=
          summary.objc_status_code_attribute_sites &&
      summary.status_code_error_type_clause_sites <=
          summary.objc_status_code_attribute_sites &&
      summary.status_code_mapping_clause_sites <=
          summary.objc_status_code_attribute_sites &&
      summary.result_success_sites + summary.result_failure_sites <=
          summary.result_like_sites &&
      summary.ns_error_bridge_path_sites <=
          summary.ns_error_out_parameter_sites;
  summary.ready_for_semantic_expansion = summary.deterministic_handoff;
  summary.replay_key = BuildPart6ErrorSourceClosureReplayKey(summary);
  return summary;
}

Objc3FrontendPart7AsyncSourceClosureSummary
BuildPart7AsyncSourceClosureSummary(const Objc3Program &program,
                                    const std::vector<Objc3LexToken> &tokens) {
  Objc3FrontendPart7AsyncSourceClosureSummary summary;
  bool async_profiles_normalized = true;
  bool await_profiles_normalized = true;

  for (const auto &token : tokens) {
    if (token.kind == Objc3LexTokenKind::KwAsync) {
      ++summary.async_keyword_sites;
    } else if (token.kind == Objc3LexTokenKind::KwAwait) {
      ++summary.await_keyword_sites;
    }
  }

  for (const auto &fn : program.functions) {
    if (fn.async_declared) {
      ++summary.async_function_sites;
    }
    if (fn.executor_affinity_declared) {
      ++summary.executor_attribute_sites;
      if (fn.executor_affinity_kind == "main") {
        ++summary.executor_main_sites;
      } else if (fn.executor_affinity_kind == "global") {
        ++summary.executor_global_sites;
      } else if (fn.executor_affinity_named) {
        ++summary.executor_named_sites;
      }
    }
    async_profiles_normalized =
        async_profiles_normalized && fn.async_continuation_profile_is_normalized;
    await_profiles_normalized =
        await_profiles_normalized && fn.await_suspension_profile_is_normalized;
    for (const auto &stmt : fn.body) {
      CollectPart7AsyncSourceClosureStmtSites(stmt.get(), summary);
    }
  }

  for (const auto &interface_decl : program.interfaces) {
    for (const auto &method : interface_decl.methods) {
      if (method.async_declared) {
        ++summary.async_method_sites;
      }
      if (method.executor_affinity_declared) {
        ++summary.executor_attribute_sites;
        if (method.executor_affinity_kind == "main") {
          ++summary.executor_main_sites;
        } else if (method.executor_affinity_kind == "global") {
          ++summary.executor_global_sites;
        } else if (method.executor_affinity_named) {
          ++summary.executor_named_sites;
        }
      }
      async_profiles_normalized =
          async_profiles_normalized &&
          method.async_continuation_profile_is_normalized;
      await_profiles_normalized =
          await_profiles_normalized &&
          method.await_suspension_profile_is_normalized;
    }
  }

  for (const auto &implementation : program.implementations) {
    for (const auto &method : implementation.methods) {
      if (method.async_declared) {
        ++summary.async_method_sites;
      }
      if (method.executor_affinity_declared) {
        ++summary.executor_attribute_sites;
        if (method.executor_affinity_kind == "main") {
          ++summary.executor_main_sites;
        } else if (method.executor_affinity_kind == "global") {
          ++summary.executor_global_sites;
        } else if (method.executor_affinity_named) {
          ++summary.executor_named_sites;
        }
      }
      async_profiles_normalized =
          async_profiles_normalized &&
          method.async_continuation_profile_is_normalized;
      await_profiles_normalized =
          await_profiles_normalized &&
          method.await_suspension_profile_is_normalized;
      for (const auto &stmt : method.body) {
        CollectPart7AsyncSourceClosureStmtSites(stmt.get(), summary);
      }
    }
  }

  summary.async_function_source_supported = true;
  summary.async_method_source_supported = true;
  summary.await_expression_source_supported = true;
  summary.executor_attribute_source_supported = true;
  summary.deterministic_handoff =
      async_profiles_normalized && await_profiles_normalized &&
      summary.async_function_sites <= summary.async_keyword_sites &&
      summary.await_expression_sites <= summary.await_keyword_sites &&
      summary.executor_main_sites + summary.executor_global_sites +
              summary.executor_named_sites <=
          summary.executor_attribute_sites;
  summary.ready_for_semantic_expansion = summary.deterministic_handoff;
  summary.replay_key = BuildPart7AsyncSourceClosureReplayKey(summary);
  return summary;
}

Objc3FrontendPart7ActorMemberIsolationSourceClosureSummary
BuildPart7ActorMemberIsolationSourceClosureSummary(const Objc3Program &program) {
  Objc3FrontendPart7ActorMemberIsolationSourceClosureSummary summary;

  for (const auto &interface_decl : program.interfaces) {
    if (!interface_decl.is_actor) {
      continue;
    }
    ++summary.actor_interface_sites;
    summary.actor_property_sites += interface_decl.properties.size();
    summary.actor_method_sites += interface_decl.methods.size();
    summary.actor_member_metadata_sites +=
        interface_decl.properties.size() + interface_decl.methods.size();
    for (const auto &method : interface_decl.methods) {
      if (method.objc_nonisolated_declared) {
        ++summary.objc_nonisolated_annotation_sites;
      }
      if (method.executor_affinity_declared) {
        ++summary.actor_member_executor_annotation_sites;
      }
      if (method.async_declared) {
        ++summary.actor_async_method_sites;
      }
    }
  }

  summary.actor_declaration_source_supported = true;
  summary.actor_member_source_supported = true;
  summary.isolation_annotation_source_supported = true;
  summary.actor_metadata_surface_supported = true;
  summary.deterministic_handoff =
      summary.objc_nonisolated_annotation_sites <= summary.actor_method_sites &&
      summary.actor_member_executor_annotation_sites <= summary.actor_method_sites &&
      summary.actor_async_method_sites <= summary.actor_method_sites &&
      summary.actor_member_metadata_sites ==
          summary.actor_method_sites + summary.actor_property_sites;
  summary.ready_for_semantic_expansion = summary.deterministic_handoff;
  summary.replay_key =
      BuildPart7ActorMemberIsolationSourceClosureReplayKey(summary);
  return summary;
}

Objc3FrontendPart7TaskGroupCancellationSourceClosureSummary
BuildPart7TaskGroupCancellationSourceClosureSummary(
    const Objc3Program &program) {
  Objc3FrontendPart7TaskGroupCancellationSourceClosureSummary summary;

  for (const auto &fn : program.functions) {
    if (fn.async_declared) {
      ++summary.async_callable_sites;
    }
    if (fn.executor_affinity_declared) {
      ++summary.executor_attribute_sites;
    }
    for (const auto &stmt : fn.body) {
      CollectPart7TaskGroupCancellationStmtSites(stmt.get(), summary);
    }
  }

  for (const auto &implementation : program.implementations) {
    for (const auto &method : implementation.methods) {
      if (method.async_declared) {
        ++summary.async_callable_sites;
      }
      if (method.executor_affinity_declared) {
        ++summary.executor_attribute_sites;
      }
      for (const auto &stmt : method.body) {
        CollectPart7TaskGroupCancellationStmtSites(stmt.get(), summary);
      }
    }
  }

  summary.task_creation_source_supported = true;
  summary.task_group_source_supported = true;
  summary.cancellation_source_supported = true;
  summary.deterministic_handoff =
      summary.task_group_add_task_sites <=
          summary.task_group_scope_sites + summary.task_group_add_task_sites +
              summary.task_group_wait_next_sites +
              summary.task_group_cancel_all_sites &&
      summary.task_group_wait_next_sites <=
          summary.task_group_scope_sites + summary.task_group_add_task_sites +
              summary.task_group_wait_next_sites +
              summary.task_group_cancel_all_sites &&
      summary.task_group_cancel_all_sites <=
          summary.task_group_scope_sites + summary.task_group_add_task_sites +
              summary.task_group_wait_next_sites +
              summary.task_group_cancel_all_sites &&
      summary.cancellation_handler_sites <= summary.cancellation_check_sites + 1u;
  summary.ready_for_semantic_expansion = summary.deterministic_handoff;
  summary.replay_key =
      BuildPart7TaskGroupCancellationSourceClosureReplayKey(summary);
  return summary;
}

static void CollectPart8SystemExtensionStmtSites(
    const Stmt *stmt,
    Objc3FrontendPart8SystemExtensionSourceClosureSummary &summary);
static void CollectPart8CleanupResourceCaptureStmtSites(
    const Stmt *stmt,
    Objc3FrontendPart8CleanupResourceCaptureSourceCompletionSummary &summary);

static void CollectPart8SystemExtensionExprSites(
    const Expr *expr,
    Objc3FrontendPart8SystemExtensionSourceClosureSummary &summary) {
  if (expr == nullptr) {
    return;
  }
  switch (expr->kind) {
  case Expr::Kind::BlockLiteral:
    if (expr->block_has_explicit_capture_list) {
      ++summary.explicit_capture_list_sites;
      summary.explicit_capture_item_sites += expr->block_explicit_capture_count;
      summary.explicit_capture_weak_sites += expr->block_explicit_capture_weak_count;
      summary.explicit_capture_unowned_sites += expr->block_explicit_capture_unowned_count;
      summary.explicit_capture_move_sites += expr->block_explicit_capture_move_count;
      summary.explicit_capture_plain_sites += expr->block_explicit_capture_plain_count;
    }
    for (const auto &stmt : expr->block_body) {
      if (stmt != nullptr) {
        CollectPart8SystemExtensionStmtSites(stmt.get(), summary);
      }
    }
    return;
  case Expr::Kind::Call:
  case Expr::Kind::MessageSend:
    CollectPart8SystemExtensionExprSites(expr->receiver.get(), summary);
    CollectPart8SystemExtensionExprSites(expr->left.get(), summary);
    CollectPart8SystemExtensionExprSites(expr->right.get(), summary);
    CollectPart8SystemExtensionExprSites(expr->third.get(), summary);
    for (const auto &arg : expr->args) {
      CollectPart8SystemExtensionExprSites(arg.get(), summary);
    }
    return;
  case Expr::Kind::Binary:
  case Expr::Kind::Conditional:
    CollectPart8SystemExtensionExprSites(expr->left.get(), summary);
    CollectPart8SystemExtensionExprSites(expr->right.get(), summary);
    CollectPart8SystemExtensionExprSites(expr->third.get(), summary);
    return;
  default:
    return;
  }
}

static void CollectPart8SystemExtensionStmtSites(
    const Stmt *stmt,
    Objc3FrontendPart8SystemExtensionSourceClosureSummary &summary) {
  if (stmt == nullptr) {
    return;
  }
  switch (stmt->kind) {
  case Stmt::Kind::Let:
    if (stmt->let_stmt != nullptr) {
      if (stmt->let_stmt->resource_attribute_declared) {
        ++summary.resource_attribute_sites;
        if (!stmt->let_stmt->resource_close_symbol.empty()) {
          ++summary.resource_close_clause_sites;
        }
        if (!stmt->let_stmt->resource_invalid_expression.empty()) {
          ++summary.resource_invalid_clause_sites;
        }
      }
      CollectPart8SystemExtensionExprSites(stmt->let_stmt->value.get(), summary);
    }
    return;
  case Stmt::Kind::Assign:
    if (stmt->assign_stmt != nullptr) {
      CollectPart8SystemExtensionExprSites(stmt->assign_stmt->value.get(), summary);
    }
    return;
  case Stmt::Kind::Return:
    if (stmt->return_stmt != nullptr) {
      CollectPart8SystemExtensionExprSites(stmt->return_stmt->value.get(), summary);
    }
    return;
  case Stmt::Kind::If:
    if (stmt->if_stmt != nullptr) {
      CollectPart8SystemExtensionExprSites(stmt->if_stmt->condition.get(), summary);
      for (const auto &body_stmt : stmt->if_stmt->then_body) {
        CollectPart8SystemExtensionStmtSites(body_stmt.get(), summary);
      }
      for (const auto &body_stmt : stmt->if_stmt->else_body) {
        CollectPart8SystemExtensionStmtSites(body_stmt.get(), summary);
      }
    }
    return;
  case Stmt::Kind::DoWhile:
    if (stmt->do_while_stmt != nullptr) {
      for (const auto &body_stmt : stmt->do_while_stmt->body) {
        CollectPart8SystemExtensionStmtSites(body_stmt.get(), summary);
      }
      CollectPart8SystemExtensionExprSites(stmt->do_while_stmt->condition.get(), summary);
    }
    return;
  case Stmt::Kind::For:
    if (stmt->for_stmt != nullptr) {
      CollectPart8SystemExtensionExprSites(stmt->for_stmt->init.value.get(), summary);
      CollectPart8SystemExtensionExprSites(stmt->for_stmt->condition.get(), summary);
      CollectPart8SystemExtensionExprSites(stmt->for_stmt->step.value.get(), summary);
      for (const auto &body_stmt : stmt->for_stmt->body) {
        CollectPart8SystemExtensionStmtSites(body_stmt.get(), summary);
      }
    }
    return;
  case Stmt::Kind::Switch:
    if (stmt->switch_stmt != nullptr) {
      CollectPart8SystemExtensionExprSites(stmt->switch_stmt->condition.get(), summary);
      for (const auto &switch_case : stmt->switch_stmt->cases) {
        for (const auto &body_stmt : switch_case.body) {
          CollectPart8SystemExtensionStmtSites(body_stmt.get(), summary);
        }
      }
    }
    return;
  case Stmt::Kind::While:
    if (stmt->while_stmt != nullptr) {
      CollectPart8SystemExtensionExprSites(stmt->while_stmt->condition.get(), summary);
      for (const auto &body_stmt : stmt->while_stmt->body) {
        CollectPart8SystemExtensionStmtSites(body_stmt.get(), summary);
      }
    }
    return;
  case Stmt::Kind::Block:
  case Stmt::Kind::Defer:
    if (stmt->block_stmt != nullptr) {
      for (const auto &body_stmt : stmt->block_stmt->body) {
        CollectPart8SystemExtensionStmtSites(body_stmt.get(), summary);
      }
    }
    return;
  case Stmt::Kind::Expr:
    if (stmt->expr_stmt != nullptr) {
      CollectPart8SystemExtensionExprSites(stmt->expr_stmt->value.get(), summary);
    }
    return;
  default:
    return;
  }
}

Objc3FrontendPart8SystemExtensionSourceClosureSummary
BuildPart8SystemExtensionSourceClosureSummary(const Objc3Program &program) {
  Objc3FrontendPart8SystemExtensionSourceClosureSummary summary;

  auto count_borrowed_in_params = [&summary](const std::vector<FuncParam> &params) {
    for (const auto &param : params) {
      if (param.borrowed_pointer_qualified) {
        ++summary.borrowed_pointer_sites;
      }
    }
  };

  for (const auto &fn : program.functions) {
    count_borrowed_in_params(fn.params);
    if (fn.return_borrowed_pointer_qualified) {
      ++summary.borrowed_pointer_sites;
    }
    if (fn.objc_returns_borrowed_declared) {
      ++summary.returns_borrowed_attribute_sites;
    }
    for (const auto &stmt : fn.body) {
      CollectPart8SystemExtensionStmtSites(stmt.get(), summary);
    }
  }

  auto count_method = [&summary, &count_borrowed_in_params](const Objc3MethodDecl &method) {
    count_borrowed_in_params(method.params);
    if (method.return_borrowed_pointer_qualified) {
      ++summary.borrowed_pointer_sites;
    }
    if (method.objc_returns_borrowed_declared) {
      ++summary.returns_borrowed_attribute_sites;
    }
  };

  for (const auto &interface_decl : program.interfaces) {
    for (const auto &method : interface_decl.methods) {
      count_method(method);
    }
  }
  for (const auto &implementation : program.implementations) {
    for (const auto &method : implementation.methods) {
      count_method(method);
      for (const auto &stmt : method.body) {
        CollectPart8SystemExtensionStmtSites(stmt.get(), summary);
      }
    }
  }

  summary.resource_attribute_source_supported = true;
  summary.borrowed_pointer_source_supported = true;
  summary.returns_borrowed_source_supported = true;
  summary.explicit_capture_list_source_supported = true;
  summary.deterministic_handoff =
      summary.resource_close_clause_sites <= summary.resource_attribute_sites &&
      summary.resource_invalid_clause_sites <= summary.resource_attribute_sites &&
      summary.explicit_capture_weak_sites + summary.explicit_capture_unowned_sites +
              summary.explicit_capture_move_sites + summary.explicit_capture_plain_sites <=
          summary.explicit_capture_item_sites;
  summary.ready_for_semantic_expansion = summary.deterministic_handoff;
  summary.replay_key = BuildPart8SystemExtensionSourceClosureReplayKey(summary);
  return summary;
}

static void CollectPart8CleanupResourceCaptureExprSites(
    const Expr *expr,
    Objc3FrontendPart8CleanupResourceCaptureSourceCompletionSummary &summary) {
  if (expr == nullptr) {
    return;
  }
  switch (expr->kind) {
  case Expr::Kind::BlockLiteral:
    if (expr->block_has_explicit_capture_list) {
      ++summary.explicit_capture_list_sites;
      summary.explicit_capture_item_sites += expr->block_explicit_capture_count;
      summary.explicit_capture_weak_sites += expr->block_explicit_capture_weak_count;
      summary.explicit_capture_unowned_sites += expr->block_explicit_capture_unowned_count;
      summary.explicit_capture_move_sites += expr->block_explicit_capture_move_count;
      summary.explicit_capture_plain_sites += expr->block_explicit_capture_plain_count;
    }
    for (const auto &stmt : expr->block_body) {
      if (stmt != nullptr) {
        CollectPart8CleanupResourceCaptureStmtSites(stmt.get(), summary);
      }
    }
    return;
  case Expr::Kind::Call:
  case Expr::Kind::MessageSend:
    CollectPart8CleanupResourceCaptureExprSites(expr->receiver.get(), summary);
    CollectPart8CleanupResourceCaptureExprSites(expr->left.get(), summary);
    CollectPart8CleanupResourceCaptureExprSites(expr->right.get(), summary);
    CollectPart8CleanupResourceCaptureExprSites(expr->third.get(), summary);
    for (const auto &arg : expr->args) {
      CollectPart8CleanupResourceCaptureExprSites(arg.get(), summary);
    }
    return;
  case Expr::Kind::Binary:
  case Expr::Kind::Conditional:
    CollectPart8CleanupResourceCaptureExprSites(expr->left.get(), summary);
    CollectPart8CleanupResourceCaptureExprSites(expr->right.get(), summary);
    CollectPart8CleanupResourceCaptureExprSites(expr->third.get(), summary);
    return;
  default:
    return;
  }
}

static void CollectPart8CleanupResourceCaptureStmtSites(
    const Stmt *stmt,
    Objc3FrontendPart8CleanupResourceCaptureSourceCompletionSummary &summary) {
  if (stmt == nullptr) {
    return;
  }
  switch (stmt->kind) {
  case Stmt::Kind::Let:
    if (stmt->let_stmt != nullptr) {
      if (stmt->let_stmt->cleanup_attribute_declared) {
        ++summary.cleanup_attribute_sites;
        if (stmt->let_stmt->cleanup_sugar_declared) {
          ++summary.cleanup_sugar_sites;
        }
      }
      if (stmt->let_stmt->resource_attribute_declared) {
        ++summary.resource_attribute_sites;
        if (stmt->let_stmt->resource_sugar_declared) {
          ++summary.resource_sugar_sites;
        }
        if (!stmt->let_stmt->resource_close_symbol.empty()) {
          ++summary.resource_close_clause_sites;
        }
        if (!stmt->let_stmt->resource_invalid_expression.empty()) {
          ++summary.resource_invalid_clause_sites;
        }
      }
      CollectPart8CleanupResourceCaptureExprSites(stmt->let_stmt->value.get(),
                                                  summary);
    }
    return;
  case Stmt::Kind::Assign:
    if (stmt->assign_stmt != nullptr) {
      CollectPart8CleanupResourceCaptureExprSites(stmt->assign_stmt->value.get(),
                                                  summary);
    }
    return;
  case Stmt::Kind::Return:
    if (stmt->return_stmt != nullptr) {
      CollectPart8CleanupResourceCaptureExprSites(stmt->return_stmt->value.get(),
                                                  summary);
    }
    return;
  case Stmt::Kind::If:
    if (stmt->if_stmt != nullptr) {
      CollectPart8CleanupResourceCaptureExprSites(stmt->if_stmt->condition.get(),
                                                  summary);
      for (const auto &body_stmt : stmt->if_stmt->then_body) {
        CollectPart8CleanupResourceCaptureStmtSites(body_stmt.get(), summary);
      }
      for (const auto &body_stmt : stmt->if_stmt->else_body) {
        CollectPart8CleanupResourceCaptureStmtSites(body_stmt.get(), summary);
      }
    }
    return;
  case Stmt::Kind::DoWhile:
    if (stmt->do_while_stmt != nullptr) {
      for (const auto &body_stmt : stmt->do_while_stmt->body) {
        CollectPart8CleanupResourceCaptureStmtSites(body_stmt.get(), summary);
      }
      CollectPart8CleanupResourceCaptureExprSites(
          stmt->do_while_stmt->condition.get(), summary);
    }
    return;
  case Stmt::Kind::For:
    if (stmt->for_stmt != nullptr) {
      CollectPart8CleanupResourceCaptureExprSites(stmt->for_stmt->init.value.get(),
                                                  summary);
      CollectPart8CleanupResourceCaptureExprSites(
          stmt->for_stmt->condition.get(), summary);
      CollectPart8CleanupResourceCaptureExprSites(stmt->for_stmt->step.value.get(),
                                                  summary);
      for (const auto &body_stmt : stmt->for_stmt->body) {
        CollectPart8CleanupResourceCaptureStmtSites(body_stmt.get(), summary);
      }
    }
    return;
  case Stmt::Kind::Switch:
    if (stmt->switch_stmt != nullptr) {
      CollectPart8CleanupResourceCaptureExprSites(
          stmt->switch_stmt->condition.get(), summary);
      for (const auto &switch_case : stmt->switch_stmt->cases) {
        for (const auto &body_stmt : switch_case.body) {
          CollectPart8CleanupResourceCaptureStmtSites(body_stmt.get(), summary);
        }
      }
    }
    return;
  case Stmt::Kind::While:
    if (stmt->while_stmt != nullptr) {
      CollectPart8CleanupResourceCaptureExprSites(
          stmt->while_stmt->condition.get(), summary);
      for (const auto &body_stmt : stmt->while_stmt->body) {
        CollectPart8CleanupResourceCaptureStmtSites(body_stmt.get(), summary);
      }
    }
    return;
  case Stmt::Kind::Block:
  case Stmt::Kind::Defer:
    if (stmt->block_stmt != nullptr) {
      for (const auto &body_stmt : stmt->block_stmt->body) {
        CollectPart8CleanupResourceCaptureStmtSites(body_stmt.get(), summary);
      }
    }
    return;
  case Stmt::Kind::Expr:
    if (stmt->expr_stmt != nullptr) {
      CollectPart8CleanupResourceCaptureExprSites(stmt->expr_stmt->value.get(),
                                                  summary);
    }
    return;
  default:
    return;
  }
}

Objc3FrontendPart8CleanupResourceCaptureSourceCompletionSummary
BuildPart8CleanupResourceCaptureSourceCompletionSummary(
    const Objc3Program &program) {
  Objc3FrontendPart8CleanupResourceCaptureSourceCompletionSummary summary;

  for (const auto &fn : program.functions) {
    for (const auto &stmt : fn.body) {
      CollectPart8CleanupResourceCaptureStmtSites(stmt.get(), summary);
    }
  }
  for (const auto &implementation : program.implementations) {
    for (const auto &method : implementation.methods) {
      for (const auto &stmt : method.body) {
        CollectPart8CleanupResourceCaptureStmtSites(stmt.get(), summary);
      }
    }
  }

  summary.cleanup_attribute_source_supported = true;
  summary.resource_sugar_source_supported = true;
  summary.explicit_capture_list_source_supported = true;
  summary.deterministic_handoff =
      summary.cleanup_sugar_sites <= summary.cleanup_attribute_sites &&
      summary.resource_sugar_sites <= summary.resource_attribute_sites &&
      summary.resource_close_clause_sites <= summary.resource_attribute_sites &&
      summary.resource_invalid_clause_sites <= summary.resource_attribute_sites &&
      summary.explicit_capture_weak_sites + summary.explicit_capture_unowned_sites +
              summary.explicit_capture_move_sites + summary.explicit_capture_plain_sites <=
          summary.explicit_capture_item_sites;
  summary.ready_for_semantic_expansion = summary.deterministic_handoff;
  summary.replay_key =
      BuildPart8CleanupResourceCaptureSourceCompletionReplayKey(summary);
  return summary;
}

Objc3FrontendPart8RetainableCFamilySourceCompletionSummary
BuildPart8RetainableCFamilySourceCompletionSummary(
    const Objc3Program &program) {
  Objc3FrontendPart8RetainableCFamilySourceCompletionSummary summary;

  const auto accumulate_callable =
      [&summary](const auto &decl) {
        for (const std::string &attribute_name :
             decl.retainable_c_family_callable_attributes) {
          if (attribute_name == "objc_family_retain") {
            ++summary.family_retain_sites;
          } else if (attribute_name == "objc_family_release") {
            ++summary.family_release_sites;
          } else if (attribute_name == "objc_family_autorelease") {
            ++summary.family_autorelease_sites;
          } else if (attribute_name == "os_returns_retained" ||
                     attribute_name == "cf_returns_retained" ||
                     attribute_name == "ns_returns_retained") {
            ++summary.compatibility_returns_retained_sites;
          } else if (attribute_name == "os_returns_not_retained" ||
                     attribute_name == "cf_returns_not_retained" ||
                     attribute_name == "ns_returns_not_retained") {
            ++summary.compatibility_returns_not_retained_sites;
          } else if (attribute_name == "os_consumed" ||
                     attribute_name == "cf_consumed" ||
                     attribute_name == "ns_consumed") {
            ++summary.compatibility_consumed_sites;
          }
        }
      };

  for (const auto &fn : program.functions) {
    accumulate_callable(fn);
  }
  for (const auto &interface_decl : program.interfaces) {
    for (const auto &method : interface_decl.methods) {
      accumulate_callable(method);
    }
  }
  for (const auto &implementation : program.implementations) {
    for (const auto &method : implementation.methods) {
      accumulate_callable(method);
    }
  }

  summary.callable_annotation_source_supported = true;
  summary.compatibility_alias_source_supported = true;
  summary.deterministic_handoff = true;
  summary.ready_for_semantic_expansion = true;
  summary.replay_key =
      BuildPart8RetainableCFamilySourceCompletionReplayKey(summary);
  return summary;
}

Objc3FrontendPart9DispatchIntentSourceClosureSummary
BuildPart9DispatchIntentSourceClosureSummary(const Objc3Program &program) {
  Objc3FrontendPart9DispatchIntentSourceClosureSummary summary;

  const auto accumulate_callable = [&summary](const auto &decl) {
    if (decl.objc_direct_declared) {
      ++summary.direct_callable_sites;
    }
    if (decl.objc_final_declared) {
      ++summary.final_callable_sites;
    }
    if (decl.objc_dynamic_declared) {
      ++summary.dynamic_callable_sites;
    }
  };

  for (const auto &fn : program.functions) {
    accumulate_callable(fn);
  }
  for (const auto &interface_decl : program.interfaces) {
    if (interface_decl.objc_direct_members_declared) {
      ++summary.direct_members_container_sites;
    }
    if (interface_decl.objc_final_declared) {
      ++summary.final_container_sites;
    }
    if (interface_decl.objc_sealed_declared) {
      ++summary.sealed_container_sites;
    }
    if (interface_decl.is_actor) {
      ++summary.actor_container_sites;
    }
    for (const auto &method : interface_decl.methods) {
      accumulate_callable(method);
    }
  }
  for (const auto &implementation : program.implementations) {
    for (const auto &method : implementation.methods) {
      accumulate_callable(method);
    }
  }

  summary.callable_annotation_source_supported = true;
  summary.container_annotation_source_supported = true;
  summary.deterministic_handoff = true;
  summary.ready_for_semantic_expansion = true;
  summary.replay_key = BuildPart9DispatchIntentSourceClosureReplayKey(summary);
  return summary;
}

Objc3FrontendPart9DispatchIntentSourceCompletionSummary
BuildPart9DispatchIntentSourceCompletionSummary(const Objc3Program &program) {
  Objc3FrontendPart9DispatchIntentSourceCompletionSummary summary;

  std::unordered_map<std::string, bool> direct_members_by_container;
  for (const auto &interface_decl : program.interfaces) {
    if (interface_decl.prefixed_dispatch_control_attributes_declared) {
      ++summary.prefixed_container_attribute_sites;
    }
    if (interface_decl.objc_direct_members_declared) {
      ++summary.direct_members_container_sites;
    }
    if (interface_decl.objc_final_declared) {
      ++summary.final_container_sites;
    }
    if (interface_decl.objc_sealed_declared) {
      ++summary.sealed_container_sites;
    }
    direct_members_by_container.emplace(interface_decl.name,
                                        interface_decl.objc_direct_members_declared);

    for (const auto &method : interface_decl.methods) {
      const bool effective_direct =
          method.objc_direct_declared ||
          (interface_decl.objc_direct_members_declared &&
           !method.objc_dynamic_declared);
      if (effective_direct) {
        ++summary.effective_direct_member_sites;
      }
      if (interface_decl.objc_direct_members_declared &&
          !method.objc_direct_declared && !method.objc_dynamic_declared) {
        ++summary.direct_members_defaulted_method_sites;
      }
      if (interface_decl.objc_direct_members_declared &&
          method.objc_dynamic_declared) {
        ++summary.direct_members_dynamic_opt_out_sites;
      }
    }
  }

  for (const auto &implementation : program.implementations) {
    if (implementation.has_category) {
      continue;
    }
    const auto found = direct_members_by_container.find(implementation.name);
    const bool direct_members_enabled =
        found != direct_members_by_container.end() && found->second;
    for (const auto &method : implementation.methods) {
      const bool effective_direct =
          method.objc_direct_declared ||
          (direct_members_enabled && !method.objc_dynamic_declared);
      if (effective_direct) {
        ++summary.effective_direct_member_sites;
      }
      if (direct_members_enabled && !method.objc_direct_declared &&
          !method.objc_dynamic_declared) {
        ++summary.direct_members_defaulted_method_sites;
      }
      if (direct_members_enabled && method.objc_dynamic_declared) {
        ++summary.direct_members_dynamic_opt_out_sites;
      }
    }
  }

  summary.prefixed_attribute_source_supported = true;
  summary.defaulting_source_supported = true;
  summary.deterministic_handoff =
      summary.direct_members_defaulted_method_sites +
              summary.direct_members_dynamic_opt_out_sites <=
          summary.effective_direct_member_sites +
              summary.direct_members_dynamic_opt_out_sites;
  summary.ready_for_semantic_expansion = summary.deterministic_handoff;
  summary.replay_key =
      BuildPart9DispatchIntentSourceCompletionReplayKey(summary);
  return summary;
}

Objc3FrontendPart10MetaprogrammingSourceClosureSummary
BuildPart10MetaprogrammingSourceClosureSummary(const Objc3Program &program) {
  Objc3FrontendPart10MetaprogrammingSourceClosureSummary summary;

  for (const auto &fn : program.functions) {
    if (fn.objc_macro_declared) {
      ++summary.macro_marker_sites;
    }
  }
  for (const auto &interface_decl : program.interfaces) {
    if (interface_decl.objc_derive_declared) {
      ++summary.derive_marker_sites;
    }
    for (const auto &property : interface_decl.properties) {
      if (property.property_behavior_declared) {
        ++summary.property_behavior_sites;
      }
    }
    for (const auto &method : interface_decl.methods) {
      if (method.objc_macro_declared) {
        ++summary.macro_marker_sites;
      }
    }
  }
  for (const auto &protocol_decl : program.protocols) {
    for (const auto &property : protocol_decl.properties) {
      if (property.property_behavior_declared) {
        ++summary.property_behavior_sites;
      }
    }
    for (const auto &method : protocol_decl.methods) {
      if (method.objc_macro_declared) {
        ++summary.macro_marker_sites;
      }
    }
  }
  for (const auto &implementation : program.implementations) {
    for (const auto &property : implementation.properties) {
      if (property.property_behavior_declared) {
        ++summary.property_behavior_sites;
      }
    }
    for (const auto &method : implementation.methods) {
      if (method.objc_macro_declared) {
        ++summary.macro_marker_sites;
      }
    }
  }

  summary.derive_marker_source_supported = true;
  summary.macro_marker_source_supported = true;
  summary.property_behavior_source_supported = true;
  summary.deterministic_handoff = true;
  summary.ready_for_semantic_expansion = true;
  summary.replay_key =
      BuildPart10MetaprogrammingSourceClosureReplayKey(summary);
  return summary;
}

Objc3FrontendPart10MacroPackageProvenanceSourceCompletionSummary
BuildPart10MacroPackageProvenanceSourceCompletionSummary(
    const Objc3Program &program) {
  Objc3FrontendPart10MacroPackageProvenanceSourceCompletionSummary summary;

  const auto accumulate_callable = [&summary](const auto &decl) {
    if (decl.objc_macro_declared) {
      ++summary.macro_marker_sites;
    }
    if (decl.objc_macro_package_declared) {
      ++summary.macro_package_sites;
    }
    if (decl.objc_macro_provenance_declared) {
      ++summary.macro_provenance_sites;
    }
    if (decl.objc_macro_declared && decl.objc_macro_package_declared &&
        decl.objc_macro_provenance_declared) {
      ++summary.expansion_visible_macro_sites;
    }
  };

  for (const auto &fn : program.functions) {
    accumulate_callable(fn);
  }
  for (const auto &interface_decl : program.interfaces) {
    for (const auto &method : interface_decl.methods) {
      accumulate_callable(method);
    }
  }
  for (const auto &protocol_decl : program.protocols) {
    for (const auto &method : protocol_decl.methods) {
      accumulate_callable(method);
    }
  }
  for (const auto &implementation : program.implementations) {
    for (const auto &method : implementation.methods) {
      accumulate_callable(method);
    }
  }

  summary.macro_package_source_supported = true;
  summary.macro_provenance_source_supported = true;
  summary.expansion_visible_source_supported = true;
  summary.deterministic_handoff =
      summary.expansion_visible_macro_sites <= summary.macro_marker_sites &&
      summary.expansion_visible_macro_sites <= summary.macro_package_sites &&
      summary.expansion_visible_macro_sites <= summary.macro_provenance_sites;
  summary.ready_for_semantic_expansion = summary.deterministic_handoff;
  summary.replay_key =
      BuildPart10MacroPackageProvenanceSourceCompletionReplayKey(summary);
  return summary;
}

Objc3FrontendPart10PropertyBehaviorSourceCompletionSummary
BuildPart10PropertyBehaviorSourceCompletionSummary(const Objc3Program &program) {
  Objc3FrontendPart10PropertyBehaviorSourceCompletionSummary summary;

  const auto accumulate_property = [&summary](const Objc3PropertyDecl &property,
                                              const char *owner_kind) {
    if (!property.property_behavior_declared) {
      return;
    }
    ++summary.property_behavior_sites;
    const std::string kind(owner_kind);
    if (kind == "interface") {
      ++summary.interface_property_behavior_sites;
    } else if (kind == "implementation") {
      ++summary.implementation_property_behavior_sites;
    } else if (kind == "protocol") {
      ++summary.protocol_property_behavior_sites;
    }
    if (property.executable_synthesized_binding_kind == "implicit-ivar" &&
        !property.executable_synthesized_binding_symbol.empty()) {
      ++summary.synthesized_binding_visible_sites;
    }
    if (!property.effective_getter_selector.empty()) {
      ++summary.synthesized_getter_visible_sites;
    }
    if (property.effective_setter_available &&
        !property.effective_setter_selector.empty()) {
      ++summary.synthesized_setter_visible_sites;
    }
  };

  for (const auto &interface_decl : program.interfaces) {
    for (const auto &property : interface_decl.properties) {
      accumulate_property(property, "interface");
    }
  }
  for (const auto &protocol_decl : program.protocols) {
    for (const auto &property : protocol_decl.properties) {
      accumulate_property(property, "protocol");
    }
  }
  for (const auto &implementation : program.implementations) {
    for (const auto &property : implementation.properties) {
      accumulate_property(property, "implementation");
    }
  }

  summary.property_behavior_source_supported = true;
  summary.synthesized_declaration_visibility_supported = true;
  summary.deterministic_handoff =
      summary.interface_property_behavior_sites +
              summary.implementation_property_behavior_sites +
              summary.protocol_property_behavior_sites ==
          summary.property_behavior_sites;
  summary.ready_for_semantic_expansion = summary.deterministic_handoff;
  summary.replay_key =
      BuildPart10PropertyBehaviorSourceCompletionReplayKey(summary);
  return summary;
}

Objc3FrontendPart11ForeignImportSourceClosureSummary
BuildPart11ForeignImportSourceClosureSummary(const Objc3Program &program) {
  Objc3FrontendPart11ForeignImportSourceClosureSummary summary;

  const auto accumulate_callable = [&summary](const auto &decl) {
    if (decl.objc_foreign_declared) {
      ++summary.foreign_callable_sites;
      ++summary.interop_annotation_sites;
      if constexpr (requires { decl.is_prototype; }) {
        if (decl.is_prototype) {
          ++summary.extern_foreign_callable_sites;
        }
      } else if constexpr (requires { decl.has_body; }) {
        if (!decl.has_body) {
          ++summary.extern_foreign_callable_sites;
        }
      }
    }
    if (decl.objc_import_module_declared) {
      ++summary.import_module_annotation_sites;
      ++summary.interop_annotation_sites;
      if (!decl.objc_import_module_name.empty()) {
        ++summary.imported_module_name_sites;
      }
    }
  };

  for (const auto &fn : program.functions) {
    accumulate_callable(fn);
  }
  for (const auto &interface_decl : program.interfaces) {
    for (const auto &method : interface_decl.methods) {
      accumulate_callable(method);
    }
  }
  for (const auto &protocol_decl : program.protocols) {
    for (const auto &method : protocol_decl.methods) {
      accumulate_callable(method);
    }
  }
  for (const auto &implementation : program.implementations) {
    for (const auto &method : implementation.methods) {
      accumulate_callable(method);
    }
  }

  summary.foreign_declaration_source_supported = true;
  summary.imported_surface_source_supported = true;
  summary.interop_annotation_source_supported = true;
  summary.deterministic_handoff =
      summary.extern_foreign_callable_sites <= summary.foreign_callable_sites &&
      summary.imported_module_name_sites <= summary.import_module_annotation_sites &&
      summary.interop_annotation_sites ==
          summary.foreign_callable_sites + summary.import_module_annotation_sites;
  summary.ready_for_semantic_expansion = summary.deterministic_handoff;
  summary.replay_key = BuildPart11ForeignImportSourceClosureReplayKey(summary);
  return summary;
}

Objc3FrontendPart11CppSwiftInteropAnnotationSourceCompletionSummary
BuildPart11CppSwiftInteropAnnotationSourceCompletionSummary(
    const Objc3Program &program) {
  Objc3FrontendPart11CppSwiftInteropAnnotationSourceCompletionSummary summary;

  const auto accumulate_callable = [&summary](const auto &decl) {
    if (decl.objc_swift_name_declared) {
      ++summary.swift_name_annotation_sites;
      ++summary.interop_metadata_annotation_sites;
      if (!decl.objc_swift_name.empty()) {
        ++summary.named_annotation_payload_sites;
      }
    }
    if (decl.objc_swift_private_declared) {
      ++summary.swift_private_annotation_sites;
      ++summary.interop_metadata_annotation_sites;
    }
    if (decl.objc_cxx_name_declared) {
      ++summary.cpp_name_annotation_sites;
      ++summary.interop_metadata_annotation_sites;
      if (!decl.objc_cxx_name.empty()) {
        ++summary.named_annotation_payload_sites;
      }
    }
    if (decl.objc_header_name_declared) {
      ++summary.header_name_annotation_sites;
      ++summary.interop_metadata_annotation_sites;
      if (!decl.objc_header_name.empty()) {
        ++summary.named_annotation_payload_sites;
      }
    }
  };

  for (const auto &fn : program.functions) {
    accumulate_callable(fn);
  }
  for (const auto &interface_decl : program.interfaces) {
    for (const auto &method : interface_decl.methods) {
      accumulate_callable(method);
    }
  }
  for (const auto &protocol_decl : program.protocols) {
    for (const auto &method : protocol_decl.methods) {
      accumulate_callable(method);
    }
  }
  for (const auto &implementation : program.implementations) {
    for (const auto &method : implementation.methods) {
      accumulate_callable(method);
    }
  }

  summary.swift_annotation_source_supported = true;
  summary.cpp_annotation_source_supported = true;
  summary.interop_metadata_source_supported = true;
  summary.deterministic_handoff =
      summary.named_annotation_payload_sites ==
          summary.swift_name_annotation_sites +
              summary.cpp_name_annotation_sites +
              summary.header_name_annotation_sites &&
      summary.interop_metadata_annotation_sites ==
          summary.swift_name_annotation_sites +
              summary.swift_private_annotation_sites +
              summary.cpp_name_annotation_sites +
              summary.header_name_annotation_sites;
  summary.ready_for_semantic_expansion = summary.deterministic_handoff;
  summary.replay_key =
      BuildPart11CppSwiftInteropAnnotationSourceCompletionReplayKey(summary);
  return summary;
}

Objc3FrontendPart12DiagnosticsMigratorSourceInventorySummary
BuildPart12DiagnosticsMigratorSourceInventorySummary(
    const Objc3FrontendMigrationHints &migration_hints,
    const Objc3FrontendPart6ErrorSourceClosureSummary &part6_summary,
    const Objc3FrontendPart7AsyncSourceClosureSummary &part7_async_summary,
    const Objc3FrontendPart7ActorMemberIsolationSourceClosureSummary
        &part7_actor_summary,
    const Objc3FrontendPart7TaskGroupCancellationSourceClosureSummary
        &part7_task_summary,
    const Objc3FrontendPart8SystemExtensionSourceClosureSummary &part8_summary,
    const Objc3FrontendPart8CleanupResourceCaptureSourceCompletionSummary
        &part8_cleanup_summary,
    const Objc3FrontendPart8RetainableCFamilySourceCompletionSummary
        &part8_family_summary,
    const Objc3FrontendPart9DispatchIntentSourceClosureSummary
        &part9_closure_summary,
    const Objc3FrontendPart9DispatchIntentSourceCompletionSummary
        &part9_completion_summary,
    const Objc3FrontendPart10MetaprogrammingSourceClosureSummary
        &part10_closure_summary,
    const Objc3FrontendPart10MacroPackageProvenanceSourceCompletionSummary
        &part10_macro_summary,
    const Objc3FrontendPart10PropertyBehaviorSourceCompletionSummary
        &part10_property_summary,
    const Objc3FrontendPart11ForeignImportSourceClosureSummary
        &part11_closure_summary,
    const Objc3FrontendPart11CppSwiftInteropAnnotationSourceCompletionSummary
        &part11_completion_summary) {
  Objc3FrontendPart12DiagnosticsMigratorSourceInventorySummary summary;

  summary.advanced_feature_family_count = 6;
  summary.dependency_surface_count = summary.dependency_contract_ids.size();
  summary.aggregated_source_only_claim_count =
      part6_summary.source_only_claim_ids.size() +
      part7_async_summary.source_only_claim_ids.size() +
      part7_actor_summary.source_only_claim_ids.size() +
      part7_task_summary.source_only_claim_ids.size() +
      part8_summary.source_only_claim_ids.size() +
      part8_cleanup_summary.source_only_claim_ids.size() +
      part8_family_summary.source_only_claim_ids.size() +
      part9_closure_summary.source_only_claim_ids.size() +
      part9_completion_summary.source_only_claim_ids.size() +
      part10_closure_summary.source_only_claim_ids.size() +
      part10_macro_summary.source_only_claim_ids.size() +
      part10_property_summary.source_only_claim_ids.size() +
      part11_closure_summary.source_only_claim_ids.size() +
      part11_completion_summary.source_only_claim_ids.size();
  summary.fail_closed_construct_count =
      part6_summary.fail_closed_construct_ids.size();
  summary.error_surface_sites =
      part6_summary.function_throws_declaration_sites +
      part6_summary.method_throws_declaration_sites +
      part6_summary.result_like_sites + part6_summary.result_payload_sites +
      part6_summary.ns_error_bridging_sites +
      part6_summary.ns_error_out_parameter_sites +
      part6_summary.ns_error_bridge_path_sites +
      part6_summary.objc_nserror_attribute_sites +
      part6_summary.objc_status_code_attribute_sites +
      part6_summary.try_keyword_sites + part6_summary.throw_keyword_sites +
      part6_summary.catch_keyword_sites;
  summary.concurrency_surface_sites =
      part7_async_summary.async_keyword_sites +
      part7_async_summary.await_keyword_sites +
      part7_async_summary.executor_attribute_sites +
      part7_actor_summary.actor_interface_sites +
      part7_actor_summary.actor_method_sites +
      part7_actor_summary.actor_property_sites +
      part7_actor_summary.objc_nonisolated_annotation_sites +
      part7_actor_summary.actor_member_executor_annotation_sites +
      part7_task_summary.task_creation_sites +
      part7_task_summary.task_group_scope_sites +
      part7_task_summary.task_group_add_task_sites +
      part7_task_summary.task_group_wait_next_sites +
      part7_task_summary.task_group_cancel_all_sites +
      part7_task_summary.cancellation_check_sites +
      part7_task_summary.cancellation_handler_sites;
  summary.system_surface_sites =
      part8_summary.resource_attribute_sites +
      part8_summary.resource_close_clause_sites +
      part8_summary.borrowed_pointer_sites +
      part8_summary.returns_borrowed_attribute_sites +
      part8_summary.explicit_capture_list_sites +
      part8_summary.explicit_capture_item_sites +
      part8_cleanup_summary.cleanup_attribute_sites +
      part8_cleanup_summary.cleanup_sugar_sites +
      part8_cleanup_summary.resource_sugar_sites +
      part8_family_summary.family_retain_sites +
      part8_family_summary.family_release_sites +
      part8_family_summary.family_autorelease_sites +
      part8_family_summary.compatibility_returns_retained_sites +
      part8_family_summary.compatibility_returns_not_retained_sites +
      part8_family_summary.compatibility_consumed_sites;
  summary.dispatch_surface_sites =
      part9_closure_summary.direct_callable_sites +
      part9_closure_summary.final_callable_sites +
      part9_closure_summary.dynamic_callable_sites +
      part9_closure_summary.direct_members_container_sites +
      part9_closure_summary.final_container_sites +
      part9_closure_summary.sealed_container_sites +
      part9_completion_summary.prefixed_container_attribute_sites +
      part9_completion_summary.effective_direct_member_sites +
      part9_completion_summary.direct_members_defaulted_method_sites +
      part9_completion_summary.direct_members_dynamic_opt_out_sites;
  summary.metaprogramming_surface_sites =
      part10_closure_summary.derive_marker_sites +
      part10_closure_summary.macro_marker_sites +
      part10_closure_summary.property_behavior_sites +
      part10_macro_summary.macro_package_sites +
      part10_macro_summary.macro_provenance_sites +
      part10_macro_summary.expansion_visible_macro_sites +
      part10_property_summary.property_behavior_sites +
      part10_property_summary.synthesized_binding_visible_sites +
      part10_property_summary.synthesized_getter_visible_sites +
      part10_property_summary.synthesized_setter_visible_sites;
  summary.interop_surface_sites =
      part11_closure_summary.foreign_callable_sites +
      part11_closure_summary.import_module_annotation_sites +
      part11_closure_summary.imported_module_name_sites +
      part11_completion_summary.swift_name_annotation_sites +
      part11_completion_summary.swift_private_annotation_sites +
      part11_completion_summary.cpp_name_annotation_sites +
      part11_completion_summary.header_name_annotation_sites +
      part11_completion_summary.named_annotation_payload_sites;
  summary.diagnostic_surface_sites =
      summary.error_surface_sites + summary.concurrency_surface_sites +
      summary.system_surface_sites + summary.dispatch_surface_sites +
      summary.metaprogramming_surface_sites + summary.interop_surface_sites;
  summary.fixit_surface_sites = summary.diagnostic_surface_sites;
  summary.migrator_surface_sites = summary.diagnostic_surface_sites;
  summary.canonicalization_hint_sites = migration_hints.legacy_total();

  const bool dependencies_ready =
      part6_summary.ready_for_semantic_expansion &&
      part7_async_summary.ready_for_semantic_expansion &&
      part7_actor_summary.ready_for_semantic_expansion &&
      part7_task_summary.ready_for_semantic_expansion &&
      part8_summary.ready_for_semantic_expansion &&
      part8_cleanup_summary.ready_for_semantic_expansion &&
      part8_family_summary.ready_for_semantic_expansion &&
      part9_closure_summary.ready_for_semantic_expansion &&
      part9_completion_summary.ready_for_semantic_expansion &&
      part10_closure_summary.ready_for_semantic_expansion &&
      part10_macro_summary.ready_for_semantic_expansion &&
      part10_property_summary.ready_for_semantic_expansion &&
      part11_closure_summary.ready_for_semantic_expansion &&
      part11_completion_summary.ready_for_semantic_expansion;
  summary.diagnostics_inventory_source_supported = dependencies_ready;
  summary.fixit_inventory_source_supported = dependencies_ready;
  summary.migrator_inventory_source_supported = dependencies_ready;
  summary.deterministic_handoff =
      dependencies_ready && summary.dependency_surface_count == 14 &&
      summary.aggregated_source_only_claim_count > 0 &&
      summary.advanced_feature_family_count == 6 &&
      summary.diagnostic_surface_sites >= summary.error_surface_sites &&
      summary.diagnostic_surface_sites >= summary.interop_surface_sites;
  summary.ready_for_semantic_expansion = summary.deterministic_handoff;
  summary.replay_key =
      BuildPart12DiagnosticsMigratorSourceInventoryReplayKey(summary);
  return summary;
}

Objc3FrontendPart12MigrationCanonicalizationSourceCompletionSummary
BuildPart12MigrationCanonicalizationSourceCompletionSummary(
    const Objc3FrontendOptions &options,
    const Objc3FrontendMigrationHints &migration_hints,
    const Objc3FrontendPart12DiagnosticsMigratorSourceInventorySummary
        &inventory_summary) {
  Objc3FrontendPart12MigrationCanonicalizationSourceCompletionSummary summary;
  summary.compatibility_mode =
      options.compatibility_mode == Objc3FrontendCompatibilityMode::kLegacy
          ? "legacy"
          : "canonical";
  summary.migration_assist_enabled = options.migration_assist;
  summary.legacy_yes_sites = migration_hints.legacy_yes_count;
  summary.legacy_no_sites = migration_hints.legacy_no_count;
  summary.legacy_null_sites = migration_hints.legacy_null_count;
  summary.legacy_total_sites = migration_hints.legacy_total();
  summary.canonical_true_rewrite_sites = summary.legacy_yes_sites;
  summary.canonical_false_rewrite_sites = summary.legacy_no_sites;
  summary.canonical_nil_rewrite_sites = summary.legacy_null_sites;
  summary.canonicalization_candidate_sites = summary.legacy_total_sites;
  summary.fixit_candidate_sites = summary.legacy_total_sites;
  summary.migrator_candidate_sites = summary.legacy_total_sites;
  summary.dependency_inventory_ready =
      inventory_summary.ready_for_semantic_expansion;
  summary.canonicalization_surface_supported = summary.dependency_inventory_ready;
  summary.fixit_migration_surface_supported =
      summary.dependency_inventory_ready && summary.migration_assist_enabled;
  const bool counts_consistent =
      summary.legacy_total_sites ==
          summary.legacy_yes_sites + summary.legacy_no_sites +
              summary.legacy_null_sites &&
      summary.canonicalization_candidate_sites ==
          summary.canonical_true_rewrite_sites +
              summary.canonical_false_rewrite_sites +
              summary.canonical_nil_rewrite_sites &&
      summary.fixit_candidate_sites == summary.canonicalization_candidate_sites &&
      summary.migrator_candidate_sites ==
          summary.canonicalization_candidate_sites;
  summary.deterministic_handoff =
      summary.dependency_inventory_ready && counts_consistent &&
      inventory_summary.canonicalization_hint_sites ==
          summary.canonicalization_candidate_sites;
  summary.ready_for_semantic_expansion = summary.deterministic_handoff;
  if (!summary.dependency_inventory_ready) {
    summary.failure_reason =
        "part12 diagnostics/fix-it/migrator inventory prerequisite is not ready";
  }
  summary.replay_key =
      BuildPart12MigrationCanonicalizationSourceCompletionReplayKey(summary);
  return summary;
}

Objc3Part12DiagnosticTaxonomyPortabilityContractSummary
BuildPart12DiagnosticTaxonomyPortabilityContractSummary(
    const Objc3FrontendPart12MigrationCanonicalizationSourceCompletionSummary
        &migration_summary,
    const Objc3SemanticDiagnosticTaxonomyAndFixitSynthesisCoreFeatureImplementationSurface
        &diagnostic_surface) {
  Objc3Part12DiagnosticTaxonomyPortabilityContractSummary summary;
  summary.portability_dependency_count =
      summary.portability_dependency_issue_ids.size();
  summary.diagnostics_total = diagnostic_surface.diagnostics_total;
  summary.diagnostics_after_pass_final =
      diagnostic_surface.diagnostics_after_pass_final;
  summary.diagnostics_emitted_total =
      diagnostic_surface.diagnostics_emitted_total;
  summary.ownership_arc_diagnostic_candidate_sites =
      diagnostic_surface.ownership_arc_diagnostic_candidate_sites;
  summary.ownership_arc_fixit_available_sites =
      diagnostic_surface.ownership_arc_fixit_available_sites;
  summary.ownership_arc_profiled_sites =
      diagnostic_surface.ownership_arc_profiled_sites;
  summary.ownership_arc_weak_unowned_conflict_diagnostic_sites =
      diagnostic_surface.ownership_arc_weak_unowned_conflict_diagnostic_sites;
  summary.ownership_arc_empty_fixit_hint_sites =
      diagnostic_surface.ownership_arc_empty_fixit_hint_sites;
  summary.ownership_arc_contract_violation_sites =
      diagnostic_surface.ownership_arc_contract_violation_sites;
  summary.migration_canonicalization_candidate_sites =
      migration_summary.canonicalization_candidate_sites;
  summary.migration_surface_ready =
      migration_summary.ready_for_semantic_expansion;
  summary.portability_dependencies_frozen =
      summary.portability_dependency_count == 11;
  summary.recovery_replay_key = diagnostic_surface.recovery_replay_key;
  summary.arc_diagnostics_fixit_replay_key =
      diagnostic_surface.arc_diagnostics_fixit_replay_key;
  summary.deterministic_handoff =
      summary.migration_surface_ready &&
      summary.portability_dependencies_frozen &&
      diagnostic_surface.core_feature_impl_ready &&
      diagnostic_surface.arc_diagnostics_fixit_summary_deterministic &&
      diagnostic_surface.arc_diagnostics_fixit_handoff_deterministic &&
      diagnostic_surface.arc_diagnostics_fixit_case_accounting_consistent &&
      diagnostic_surface.replay_keys_ready &&
      summary.ownership_arc_fixit_available_sites <=
          summary.ownership_arc_diagnostic_candidate_sites +
              summary.ownership_arc_contract_violation_sites &&
      summary.ownership_arc_profiled_sites <=
          summary.ownership_arc_diagnostic_candidate_sites +
              summary.ownership_arc_contract_violation_sites &&
      summary.ownership_arc_empty_fixit_hint_sites <=
          summary.ownership_arc_fixit_available_sites +
              summary.ownership_arc_contract_violation_sites;
  summary.ready_for_lowering_and_runtime = summary.deterministic_handoff;
  if (!summary.migration_surface_ready) {
    summary.failure_reason =
        "part12 migration/canonicalization source completion prerequisite is not ready";
  } else if (!diagnostic_surface.core_feature_impl_ready) {
    summary.failure_reason =
        "semantic diagnostic taxonomy/fix-it core feature implementation surface is not ready";
  }
  summary.replay_key =
      BuildPart12DiagnosticTaxonomyPortabilityContractReplayKey(summary);
  return summary;
}

Objc3Part12FeatureSpecificFixitSynthesisSummary
BuildPart12FeatureSpecificFixitSynthesisSummary(
    const Objc3Part12DiagnosticTaxonomyPortabilityContractSummary
        &taxonomy_summary) {
  Objc3Part12FeatureSpecificFixitSynthesisSummary summary;
  summary.fixit_family_count = summary.fixit_family_ids.size();
  summary.migration_fixit_candidate_sites =
      taxonomy_summary.migration_canonicalization_candidate_sites;
  summary.migrator_candidate_sites =
      taxonomy_summary.migration_canonicalization_candidate_sites;
  summary.ownership_arc_fixit_available_sites =
      taxonomy_summary.ownership_arc_fixit_available_sites;
  summary.ownership_arc_empty_fixit_hint_sites =
      taxonomy_summary.ownership_arc_empty_fixit_hint_sites;
  summary.diagnostic_taxonomy_ready =
      taxonomy_summary.ready_for_lowering_and_runtime;
  summary.deterministic_handoff =
      summary.diagnostic_taxonomy_ready && summary.fixit_family_count == 2 &&
      summary.migration_fixit_candidate_sites ==
          summary.migrator_candidate_sites &&
      summary.ownership_arc_empty_fixit_hint_sites <=
          summary.ownership_arc_fixit_available_sites;
  summary.ready_for_lowering_and_runtime = summary.deterministic_handoff;
  if (!summary.diagnostic_taxonomy_ready) {
    summary.failure_reason =
        "part12 diagnostic taxonomy and portability contract prerequisite is not ready";
  }
  summary.replay_key =
      BuildPart12FeatureSpecificFixitSynthesisReplayKey(summary);
  return summary;
}

std::string BuildSymbolGraphScopeResolutionHandoffKey(
    const Objc3FrontendSymbolGraphScopeResolutionSummary &summary) {
  std::ostringstream out;
  out << "symbol_graph_nodes="
      << summary.global_symbol_nodes << ":" << summary.function_symbol_nodes << ":" << summary.interface_symbol_nodes
      << ":" << summary.implementation_symbol_nodes << ":" << summary.interface_property_symbol_nodes << ":"
      << summary.implementation_property_symbol_nodes << ":" << summary.interface_method_symbol_nodes << ":"
      << summary.implementation_method_symbol_nodes
      << ";scope_surface="
      << summary.top_level_scope_symbols << ":" << summary.nested_scope_symbols << ":"
      << summary.scope_frames_total
      << ";resolution_surface="
      << summary.implementation_interface_resolution_sites << ":" << summary.implementation_interface_resolution_hits
      << ":" << summary.implementation_interface_resolution_misses << ":" << summary.method_resolution_sites << ":"
      << summary.method_resolution_hits << ":" << summary.method_resolution_misses
      << ";deterministic="
      << (summary.deterministic_symbol_graph_handoff ? "true" : "false") << ":"
      << (summary.deterministic_scope_resolution_handoff ? "true" : "false");
  return out.str();
}

Objc3FrontendSymbolGraphScopeResolutionSummary BuildSymbolGraphScopeResolutionSummary(
    const Objc3SemanticIntegrationSurface &integration_surface,
    const Objc3SemanticTypeMetadataHandoff &type_metadata_handoff) {
  Objc3FrontendSymbolGraphScopeResolutionSummary summary;
  const Objc3SymbolGraphScopeResolutionSummary &integration_summary =
      integration_surface.symbol_graph_scope_resolution_summary;
  const Objc3SymbolGraphScopeResolutionSummary &type_metadata_summary =
      type_metadata_handoff.symbol_graph_scope_resolution_summary;

  const auto select_value = [&](std::size_t integration_value, std::size_t type_metadata_value) {
    if (integration_surface.built) {
      return integration_value;
    }
    return type_metadata_value;
  };

  summary.global_symbol_nodes = select_value(integration_summary.global_symbol_nodes,
                                             type_metadata_summary.global_symbol_nodes);
  summary.function_symbol_nodes = select_value(integration_summary.function_symbol_nodes,
                                               type_metadata_summary.function_symbol_nodes);
  summary.interface_symbol_nodes = select_value(integration_summary.interface_symbol_nodes,
                                                type_metadata_summary.interface_symbol_nodes);
  summary.implementation_symbol_nodes = select_value(integration_summary.implementation_symbol_nodes,
                                                     type_metadata_summary.implementation_symbol_nodes);
  summary.interface_property_symbol_nodes = select_value(integration_summary.interface_property_symbol_nodes,
                                                         type_metadata_summary.interface_property_symbol_nodes);
  summary.implementation_property_symbol_nodes = select_value(integration_summary.implementation_property_symbol_nodes,
                                                              type_metadata_summary.implementation_property_symbol_nodes);
  summary.interface_method_symbol_nodes = select_value(integration_summary.interface_method_symbol_nodes,
                                                       type_metadata_summary.interface_method_symbol_nodes);
  summary.implementation_method_symbol_nodes = select_value(integration_summary.implementation_method_symbol_nodes,
                                                            type_metadata_summary.implementation_method_symbol_nodes);
  summary.top_level_scope_symbols = select_value(integration_summary.top_level_scope_symbols,
                                                 type_metadata_summary.top_level_scope_symbols);
  summary.nested_scope_symbols = select_value(integration_summary.nested_scope_symbols,
                                              type_metadata_summary.nested_scope_symbols);
  summary.scope_frames_total = select_value(integration_summary.scope_frames_total,
                                            type_metadata_summary.scope_frames_total);
  summary.implementation_interface_resolution_sites =
      select_value(integration_summary.implementation_interface_resolution_sites,
                   type_metadata_summary.implementation_interface_resolution_sites);
  summary.implementation_interface_resolution_hits =
      select_value(integration_summary.implementation_interface_resolution_hits,
                   type_metadata_summary.implementation_interface_resolution_hits);
  summary.implementation_interface_resolution_misses =
      select_value(integration_summary.implementation_interface_resolution_misses,
                   type_metadata_summary.implementation_interface_resolution_misses);
  summary.method_resolution_sites = select_value(integration_summary.method_resolution_sites,
                                                 type_metadata_summary.method_resolution_sites);
  summary.method_resolution_hits = select_value(integration_summary.method_resolution_hits,
                                                type_metadata_summary.method_resolution_hits);
  summary.method_resolution_misses = select_value(integration_summary.method_resolution_misses,
                                                  type_metadata_summary.method_resolution_misses);

  const bool symbol_graph_fields_match =
      integration_summary.global_symbol_nodes == type_metadata_summary.global_symbol_nodes &&
      integration_summary.function_symbol_nodes == type_metadata_summary.function_symbol_nodes &&
      integration_summary.interface_symbol_nodes == type_metadata_summary.interface_symbol_nodes &&
      integration_summary.implementation_symbol_nodes == type_metadata_summary.implementation_symbol_nodes &&
      integration_summary.interface_property_symbol_nodes == type_metadata_summary.interface_property_symbol_nodes &&
      integration_summary.implementation_property_symbol_nodes ==
          type_metadata_summary.implementation_property_symbol_nodes &&
      integration_summary.interface_method_symbol_nodes == type_metadata_summary.interface_method_symbol_nodes &&
      integration_summary.implementation_method_symbol_nodes ==
          type_metadata_summary.implementation_method_symbol_nodes;
  const bool scope_resolution_fields_match =
      integration_summary.top_level_scope_symbols == type_metadata_summary.top_level_scope_symbols &&
      integration_summary.nested_scope_symbols == type_metadata_summary.nested_scope_symbols &&
      integration_summary.scope_frames_total == type_metadata_summary.scope_frames_total &&
      integration_summary.implementation_interface_resolution_sites ==
          type_metadata_summary.implementation_interface_resolution_sites &&
      integration_summary.implementation_interface_resolution_hits ==
          type_metadata_summary.implementation_interface_resolution_hits &&
      integration_summary.implementation_interface_resolution_misses ==
          type_metadata_summary.implementation_interface_resolution_misses &&
      integration_summary.method_resolution_sites == type_metadata_summary.method_resolution_sites &&
      integration_summary.method_resolution_hits == type_metadata_summary.method_resolution_hits &&
      integration_summary.method_resolution_misses == type_metadata_summary.method_resolution_misses;

  summary.deterministic_symbol_graph_handoff =
      integration_summary.deterministic &&
      type_metadata_summary.deterministic &&
      symbol_graph_fields_match &&
      summary.symbol_nodes_total() == summary.top_level_scope_symbols + summary.nested_scope_symbols;
  summary.deterministic_scope_resolution_handoff =
      integration_summary.deterministic &&
      type_metadata_summary.deterministic &&
      scope_resolution_fields_match &&
      summary.resolution_hits_total() <= summary.resolution_sites_total() &&
      summary.resolution_hits_total() + summary.resolution_misses_total() == summary.resolution_sites_total();
  summary.deterministic_handoff_key = BuildSymbolGraphScopeResolutionHandoffKey(summary);
  return summary;
}

}  // namespace

Objc3FrontendPipelineResult RunObjc3FrontendPipeline(const std::string &source,
                                                     const Objc3FrontendOptions &options) {
  Objc3FrontendPipelineResult result;

  Objc3LexerOptions lexer_options;
  lexer_options.language_version = options.language_version;
  lexer_options.compatibility_mode = options.compatibility_mode == Objc3FrontendCompatibilityMode::kLegacy
                                         ? Objc3LexerCompatibilityMode::kLegacy
                                         : Objc3LexerCompatibilityMode::kCanonical;
  lexer_options.migration_assist = options.migration_assist;
  Objc3Lexer lexer(source, lexer_options);
  std::vector<Objc3LexToken> tokens = lexer.Run(result.stage_diagnostics.lexer);
  const Objc3LexerMigrationHints &lexer_hints = lexer.MigrationHints();
  const Objc3LexerLanguageVersionPragmaContract &pragma_contract = lexer.LanguageVersionPragmaContract();
  const Objc3LexerBootstrapRegistrationSourceContract
      &bootstrap_registration_pragma_contract =
          lexer.BootstrapRegistrationSourceContract();
  result.migration_hints.legacy_yes_count = lexer_hints.legacy_yes_count;
  result.migration_hints.legacy_no_count = lexer_hints.legacy_no_count;
  result.migration_hints.legacy_null_count = lexer_hints.legacy_null_count;
  result.language_version_pragma_contract.seen = pragma_contract.seen;
  result.language_version_pragma_contract.directive_count = pragma_contract.directive_count;
  result.language_version_pragma_contract.duplicate = pragma_contract.duplicate;
  result.language_version_pragma_contract.non_leading = pragma_contract.non_leading;
  result.language_version_pragma_contract.first_line = pragma_contract.first_line;
  result.language_version_pragma_contract.first_column = pragma_contract.first_column;
  result.language_version_pragma_contract.last_line = pragma_contract.last_line;
  result.language_version_pragma_contract.last_column = pragma_contract.last_column;
  result.bootstrap_registration_source_pragma_contract
      .registration_descriptor.seen =
      bootstrap_registration_pragma_contract.registration_descriptor.seen;
  result.bootstrap_registration_source_pragma_contract
      .registration_descriptor.directive_count =
      bootstrap_registration_pragma_contract.registration_descriptor.directive_count;
  result.bootstrap_registration_source_pragma_contract
      .registration_descriptor.duplicate =
      bootstrap_registration_pragma_contract.registration_descriptor.duplicate;
  result.bootstrap_registration_source_pragma_contract
      .registration_descriptor.non_leading =
      bootstrap_registration_pragma_contract.registration_descriptor.non_leading;
  result.bootstrap_registration_source_pragma_contract
      .registration_descriptor.first_line =
      bootstrap_registration_pragma_contract.registration_descriptor.first_line;
  result.bootstrap_registration_source_pragma_contract
      .registration_descriptor.first_column =
      bootstrap_registration_pragma_contract.registration_descriptor.first_column;
  result.bootstrap_registration_source_pragma_contract
      .registration_descriptor.last_line =
      bootstrap_registration_pragma_contract.registration_descriptor.last_line;
  result.bootstrap_registration_source_pragma_contract
      .registration_descriptor.last_column =
      bootstrap_registration_pragma_contract.registration_descriptor.last_column;
  result.bootstrap_registration_source_pragma_contract
      .registration_descriptor.identifier =
      bootstrap_registration_pragma_contract.registration_descriptor.identifier;
  result.bootstrap_registration_source_pragma_contract.image_root.seen =
      bootstrap_registration_pragma_contract.image_root.seen;
  result.bootstrap_registration_source_pragma_contract.image_root.directive_count =
      bootstrap_registration_pragma_contract.image_root.directive_count;
  result.bootstrap_registration_source_pragma_contract.image_root.duplicate =
      bootstrap_registration_pragma_contract.image_root.duplicate;
  result.bootstrap_registration_source_pragma_contract.image_root.non_leading =
      bootstrap_registration_pragma_contract.image_root.non_leading;
  result.bootstrap_registration_source_pragma_contract.image_root.first_line =
      bootstrap_registration_pragma_contract.image_root.first_line;
  result.bootstrap_registration_source_pragma_contract.image_root.first_column =
      bootstrap_registration_pragma_contract.image_root.first_column;
  result.bootstrap_registration_source_pragma_contract.image_root.last_line =
      bootstrap_registration_pragma_contract.image_root.last_line;
  result.bootstrap_registration_source_pragma_contract.image_root.last_column =
      bootstrap_registration_pragma_contract.image_root.last_column;
  result.bootstrap_registration_source_pragma_contract.image_root.identifier =
      bootstrap_registration_pragma_contract.image_root.identifier;

  if (result.stage_diagnostics.lexer.empty()) {
    Objc3AstBuilderResult parse_result = BuildObjc3AstFromTokens(tokens);
    result.program = std::move(parse_result.program);
    result.stage_diagnostics.parser = std::move(parse_result.diagnostics);
    result.parser_contract_snapshot = parse_result.contract_snapshot;
  }
  result.selector_normalization_summary =
      BuildSelectorNormalizationSummary(Objc3ParsedProgramAst(result.program));
  result.property_attribute_summary =
      BuildPropertyAttributeSummary(Objc3ParsedProgramAst(result.program));
  result.object_pointer_nullability_generics_summary =
      BuildObjectPointerNullabilityGenericsSummary(Objc3ParsedProgramAst(result.program));
  result.part3_type_source_closure_summary = BuildPart3TypeSourceClosureSummary(
      Objc3ParsedProgramAst(result.program),
      result.object_pointer_nullability_generics_summary);
  result.part5_control_flow_source_closure_summary =
      BuildPart5ControlFlowSourceClosureSummary(
          Objc3ParsedProgramAst(result.program), tokens);
  result.part6_error_source_closure_summary =
      BuildPart6ErrorSourceClosureSummary(
          Objc3ParsedProgramAst(result.program), tokens);
  result.part7_async_source_closure_summary =
      BuildPart7AsyncSourceClosureSummary(
          Objc3ParsedProgramAst(result.program), tokens);
  result.part8_system_extension_source_closure_summary =
      BuildPart8SystemExtensionSourceClosureSummary(
          Objc3ParsedProgramAst(result.program));
  result.part8_cleanup_resource_capture_source_completion_summary =
      BuildPart8CleanupResourceCaptureSourceCompletionSummary(
          Objc3ParsedProgramAst(result.program));
  result.part8_retainable_c_family_source_completion_summary =
      BuildPart8RetainableCFamilySourceCompletionSummary(
          Objc3ParsedProgramAst(result.program));
  result.part9_dispatch_intent_source_closure_summary =
      BuildPart9DispatchIntentSourceClosureSummary(
          Objc3ParsedProgramAst(result.program));
  result.part9_dispatch_intent_source_completion_summary =
      BuildPart9DispatchIntentSourceCompletionSummary(
          Objc3ParsedProgramAst(result.program));
  result.part10_metaprogramming_source_closure_summary =
      BuildPart10MetaprogrammingSourceClosureSummary(
          Objc3ParsedProgramAst(result.program));
  result.part10_macro_package_provenance_source_completion_summary =
      BuildPart10MacroPackageProvenanceSourceCompletionSummary(
          Objc3ParsedProgramAst(result.program));
  result.part10_property_behavior_source_completion_summary =
      BuildPart10PropertyBehaviorSourceCompletionSummary(
          Objc3ParsedProgramAst(result.program));
  result.part11_foreign_import_source_closure_summary =
      BuildPart11ForeignImportSourceClosureSummary(
          Objc3ParsedProgramAst(result.program));
  result.part11_cpp_swift_interop_annotation_source_completion_summary =
      BuildPart11CppSwiftInteropAnnotationSourceCompletionSummary(
          Objc3ParsedProgramAst(result.program));
  result.part7_actor_member_isolation_source_closure_summary =
      BuildPart7ActorMemberIsolationSourceClosureSummary(
          Objc3ParsedProgramAst(result.program));
  result.part7_task_group_cancellation_source_closure_summary =
      BuildPart7TaskGroupCancellationSourceClosureSummary(
          Objc3ParsedProgramAst(result.program));
  result.part12_diagnostics_migrator_source_inventory_summary =
      BuildPart12DiagnosticsMigratorSourceInventorySummary(
          result.migration_hints, result.part6_error_source_closure_summary,
          result.part7_async_source_closure_summary,
          result.part7_actor_member_isolation_source_closure_summary,
          result.part7_task_group_cancellation_source_closure_summary,
          result.part8_system_extension_source_closure_summary,
          result.part8_cleanup_resource_capture_source_completion_summary,
          result.part8_retainable_c_family_source_completion_summary,
          result.part9_dispatch_intent_source_closure_summary,
          result.part9_dispatch_intent_source_completion_summary,
          result.part10_metaprogramming_source_closure_summary,
          result.part10_macro_package_provenance_source_completion_summary,
          result.part10_property_behavior_source_completion_summary,
          result.part11_foreign_import_source_closure_summary,
          result.part11_cpp_swift_interop_annotation_source_completion_summary);
  result.part12_migration_canonicalization_source_completion_summary =
      BuildPart12MigrationCanonicalizationSourceCompletionSummary(
          options, result.migration_hints,
          result.part12_diagnostics_migrator_source_inventory_summary);
  result.protocol_category_summary =
      BuildProtocolCategorySummary(Objc3ParsedProgramAst(result.program),
                                   result.integration_surface,
                                   result.sema_type_metadata_handoff);
  result.class_protocol_category_linking_summary =
      BuildClassProtocolCategoryLinkingSummary(result.sema_type_metadata_handoff.interface_implementation_summary,
                                               result.protocol_category_summary,
                                               result.integration_surface,
                                               result.sema_type_metadata_handoff);
  result.symbol_graph_scope_resolution_summary =
      BuildSymbolGraphScopeResolutionSummary(result.integration_surface,
                                             result.sema_type_metadata_handoff);
  const bool allow_part6_error_runtime_surface = true;
  if (result.stage_diagnostics.lexer.empty() && result.stage_diagnostics.parser.empty()) {
    NormalizeProgramDispatchSurfaceClassification(
        MutableObjc3ParsedProgramAst(result.program));

    Objc3SemanticValidationOptions semantic_options;
    semantic_options.max_message_send_args = options.lowering.max_message_send_args;
    // block-source-model-completion anchor: source-only frontend
    // runs may admit block literals through semantic validation when both IR
    // and object emission are disabled, but native emit paths must continue to
    // fail closed until runnable block lowering lands.
    semantic_options.allow_source_only_block_literals =
        !options.emit_ir && !options.emit_object;
    // defer-lowering anchor: defer statements are now admitted for
    // native validation because lane C owns executable cleanup insertion in
    // the IR path rather than keeping defer as a source-only semantic claim.
    semantic_options.allow_source_only_defer_statements = true;
    semantic_options.allow_source_only_error_runtime_surface =
        allow_part6_error_runtime_surface;
    semantic_options.arc_mode_enabled =
        options.arc_mode == Objc3FrontendArcMode::kEnabled;

    Objc3SemaPassManagerInput sema_input;
    sema_input.program = &result.program;
    sema_input.validation_options = semantic_options;
    sema_input.compatibility_mode = options.compatibility_mode == Objc3FrontendCompatibilityMode::kLegacy
                                        ? Objc3SemaCompatibilityMode::Legacy
                                        : Objc3SemaCompatibilityMode::Canonical;
    sema_input.migration_assist = options.migration_assist;
    sema_input.migration_hints.legacy_yes_count = result.migration_hints.legacy_yes_count;
    sema_input.migration_hints.legacy_no_count = result.migration_hints.legacy_no_count;
    sema_input.migration_hints.legacy_null_count = result.migration_hints.legacy_null_count;
    sema_input.diagnostics_bus.diagnostics = &result.stage_diagnostics.semantic;

    Objc3SemaPassManagerResult sema_result = RunObjc3SemaPassManager(sema_input);
    result.integration_surface = std::move(sema_result.integration_surface);
    result.sema_type_metadata_handoff = std::move(sema_result.type_metadata_handoff);
    result.protocol_category_summary =
        BuildProtocolCategorySummary(Objc3ParsedProgramAst(result.program),
                                     result.integration_surface,
                                     result.sema_type_metadata_handoff);
    result.class_protocol_category_linking_summary =
        BuildClassProtocolCategoryLinkingSummary(result.sema_type_metadata_handoff.interface_implementation_summary,
                                                 result.protocol_category_summary,
                                                 result.integration_surface,
                                                 result.sema_type_metadata_handoff);
    result.symbol_graph_scope_resolution_summary =
        BuildSymbolGraphScopeResolutionSummary(result.integration_surface,
                                               result.sema_type_metadata_handoff);
    result.sema_diagnostics_after_pass = sema_result.diagnostics_after_pass;
    result.sema_pass_flow_summary = sema_result.sema_pass_flow_summary;
    result.sema_parity_surface = sema_result.parity_surface;
    if (result.stage_diagnostics.semantic.empty() && !sema_result.diagnostics.empty()) {
      result.stage_diagnostics.semantic = std::move(sema_result.diagnostics);
    }
  }
  result.part5_control_flow_semantic_model_summary =
      BuildPart5ControlFlowSemanticModelSummary(
          Objc3ParsedProgramAst(result.program));
  result.part6_error_semantic_model_summary =
      BuildPart6ErrorSemanticModelSummary(
          result.part6_error_source_closure_summary, result.integration_surface);
  result.part7_actor_isolation_sendable_semantic_model_summary =
      BuildPart7ActorIsolationSendableSemanticModelSummary(
          result.part7_actor_member_isolation_source_closure_summary,
          result.integration_surface);
  result.part7_actor_isolation_sendability_enforcement_summary =
      BuildPart7ActorIsolationSendabilityEnforcementSummary(
          Objc3ParsedProgramAst(result.program),
          result.part7_actor_isolation_sendable_semantic_model_summary,
          result.stage_diagnostics.semantic);
  result.part7_actor_race_hazard_escape_diagnostics_summary =
      BuildPart7ActorRaceHazardEscapeDiagnosticsSummary(
          Objc3ParsedProgramAst(result.program),
          result.part7_actor_isolation_sendability_enforcement_summary,
          result.stage_diagnostics.semantic);
  result.part7_task_executor_cancellation_semantic_model_summary =
      BuildPart7TaskExecutorCancellationSemanticModelSummary(
          result.part7_task_group_cancellation_source_closure_summary,
          result.integration_surface);
  result.part8_system_extension_semantic_model_summary =
      BuildPart8SystemExtensionSemanticModelSummary(
          result.part8_system_extension_source_closure_summary,
          result.part8_cleanup_resource_capture_source_completion_summary,
          result.part8_retainable_c_family_source_completion_summary);
  result.part10_expansion_behavior_semantic_model_summary =
      BuildPart10ExpansionBehaviorSemanticModelSummary(
          result.part10_metaprogramming_source_closure_summary,
          result.part10_macro_package_provenance_source_completion_summary,
          result.part10_property_behavior_source_completion_summary,
          result.stage_diagnostics.semantic);
  result.part10_derive_expansion_inventory_summary =
      BuildPart10DeriveExpansionInventorySummary(
          result.program.ast,
          result.part10_expansion_behavior_semantic_model_summary,
          result.stage_diagnostics.semantic);
  result.part10_macro_safety_sandbox_determinism_summary =
      BuildPart10MacroSafetySandboxDeterminismSummary(
          result.program.ast,
          result.part10_derive_expansion_inventory_summary,
          result.stage_diagnostics.semantic);
  result.part10_property_behavior_legality_compatibility_summary =
      BuildPart10PropertyBehaviorLegalityCompatibilitySummary(
          result.program.ast,
          result.part10_macro_safety_sandbox_determinism_summary,
          result.stage_diagnostics.semantic);
  result.part9_dispatch_intent_semantic_model_summary =
      BuildPart9DispatchIntentSemanticModelSummary(
          result.part9_dispatch_intent_source_completion_summary,
          result.integration_surface);
  result.part9_dispatch_intent_legality_summary =
      BuildPart9DispatchIntentLegalitySummary(
          Objc3ParsedProgramAst(result.program),
          result.part9_dispatch_intent_semantic_model_summary,
          result.stage_diagnostics.semantic);
  result.part9_dispatch_intent_compatibility_summary =
      BuildPart9DispatchIntentCompatibilitySummary(
          Objc3ParsedProgramAst(result.program),
          result.part9_dispatch_intent_legality_summary,
          result.stage_diagnostics.semantic);
  result.part8_resource_move_use_after_move_semantics_summary =
      BuildPart8ResourceMoveUseAfterMoveSemanticsSummary(
          Objc3ParsedProgramAst(result.program),
          result.part8_system_extension_semantic_model_summary,
          result.stage_diagnostics.semantic);
  result.part8_borrowed_pointer_escape_analysis_summary =
      BuildPart8BorrowedPointerEscapeAnalysisSummary(
          Objc3ParsedProgramAst(result.program),
          result.part8_resource_move_use_after_move_semantics_summary,
          result.stage_diagnostics.semantic);
  result.part8_capture_list_retainable_family_legality_completion_summary =
      BuildPart8CaptureListRetainableFamilyLegalityCompletionSummary(
          Objc3ParsedProgramAst(result.program),
          result.part8_borrowed_pointer_escape_analysis_summary,
          result.stage_diagnostics.semantic);
  result.part7_structured_task_cancellation_semantic_summary =
      BuildPart7StructuredTaskCancellationSemanticSummary(
          result.part7_task_executor_cancellation_semantic_model_summary,
          result.stage_diagnostics.semantic);
  result.part7_executor_hop_affinity_compatibility_summary =
      BuildPart7ExecutorHopAffinityCompatibilitySummary(
          result.part7_structured_task_cancellation_semantic_summary,
          result.part7_async_source_closure_summary,
          result.stage_diagnostics.semantic);
  result.part7_async_effect_suspension_semantic_model_summary =
      BuildPart7AsyncEffectSuspensionSemanticModelSummary(
          result.part7_async_source_closure_summary, result.integration_surface);
  result.part7_await_suspension_resume_semantic_summary =
      BuildPart7AwaitSuspensionResumeSemanticSummary(
          result.part7_async_effect_suspension_semantic_model_summary,
          result.stage_diagnostics.semantic);
  result.part7_async_diagnostics_compatibility_summary =
      BuildPart7AsyncDiagnosticsCompatibilitySummary(
          result.part7_await_suspension_resume_semantic_summary,
          result.part7_async_source_closure_summary,
          Objc3ParsedProgramAst(result.program),
          result.stage_diagnostics.semantic);
  result.part6_try_do_catch_semantic_summary =
      BuildPart6TryDoCatchSemanticSummary(
          Objc3ParsedProgramAst(result.program),
          result.integration_surface,
          allow_part6_error_runtime_surface,
          result.stage_diagnostics.semantic);
  result.part6_error_bridge_legality_summary =
      BuildPart6ErrorBridgeLegalitySummary(
          Objc3ParsedProgramAst(result.program),
          allow_part6_error_runtime_surface,
          result.stage_diagnostics.semantic);
  result.part11_interop_semantic_model_summary =
      BuildPart11InteropSemanticModelSummary(
          result.part11_foreign_import_source_closure_summary,
          result.part11_cpp_swift_interop_annotation_source_completion_summary,
          result.part8_capture_list_retainable_family_legality_completion_summary,
          result.part6_error_bridge_legality_summary,
          result.part7_async_diagnostics_compatibility_summary,
          result.part7_actor_race_hazard_escape_diagnostics_summary);
  result.part11_interop_runtime_parity_summary =
      BuildPart11InteropRuntimeParitySummary(
          Objc3ParsedProgramAst(result.program),
          result.part11_interop_semantic_model_summary,
          result.stage_diagnostics.semantic);
  result.part11_cpp_interop_interaction_summary =
      BuildPart11CppInteropInteractionSummary(
          Objc3ParsedProgramAst(result.program),
          result.part11_interop_runtime_parity_summary,
          result.stage_diagnostics.semantic);
  result.part11_swift_interop_isolation_summary =
      BuildPart11SwiftInteropIsolationSummary(
          Objc3ParsedProgramAst(result.program),
          result.part11_cpp_interop_interaction_summary,
          result.stage_diagnostics.semantic);
  result.runtime_metadata_source_records =
      BuildRuntimeMetadataSourceRecordSet(Objc3ParsedProgramAst(result.program));
  result.executable_metadata_source_graph = BuildExecutableMetadataSourceGraph(
      Objc3ParsedProgramAst(result.program),
      result.runtime_metadata_source_records);
  result.executable_metadata_semantic_consistency_boundary =
      BuildExecutableMetadataSemanticConsistencyBoundary(
          result.executable_metadata_source_graph,
          result.protocol_category_summary,
          result.class_protocol_category_linking_summary,
          result.selector_normalization_summary,
          result.property_attribute_summary,
          result.symbol_graph_scope_resolution_summary);
  result.executable_metadata_semantic_validation_surface =
      BuildExecutableMetadataSemanticValidationSurface(
          result.executable_metadata_source_graph,
          result.executable_metadata_semantic_consistency_boundary,
          result.sema_type_metadata_handoff,
          result.class_protocol_category_linking_summary);
  result.executable_metadata_lowering_handoff_surface =
      BuildExecutableMetadataLoweringHandoffSurface(
          result.executable_metadata_source_graph,
          result.executable_metadata_semantic_consistency_boundary,
          result.executable_metadata_semantic_validation_surface,
          result.sema_type_metadata_handoff,
          result.protocol_category_summary,
          result.class_protocol_category_linking_summary,
          result.selector_normalization_summary,
          result.property_attribute_summary,
          result.symbol_graph_scope_resolution_summary,
          result.sema_parity_surface);
  result.executable_metadata_typed_lowering_handoff =
      BuildExecutableMetadataTypedLoweringHandoff(
          result.executable_metadata_source_graph,
          result.executable_metadata_semantic_consistency_boundary,
          result.executable_metadata_semantic_validation_surface,
          result.executable_metadata_lowering_handoff_surface);
  result.runtime_metadata_source_ownership_boundary =
      BuildRuntimeMetadataSourceOwnershipBoundary(result.runtime_metadata_source_records,
                                                  result.sema_type_metadata_handoff);
  result.typed_sema_to_lowering_contract_surface =
      BuildObjc3TypedSemaToLoweringContractSurface(result, options);
  result.runtime_export_legality_boundary = BuildRuntimeExportLegalityBoundary(
      result.runtime_metadata_source_ownership_boundary,
      result.typed_sema_to_lowering_contract_surface,
      result.integration_surface,
      result.protocol_category_summary,
      result.class_protocol_category_linking_summary,
      result.selector_normalization_summary,
      result.property_attribute_summary,
      result.object_pointer_nullability_generics_summary,
      result.symbol_graph_scope_resolution_summary,
      result.sema_parity_surface);
  result.runtime_export_enforcement_summary =
      BuildRuntimeExportEnforcementSummary(
          result.runtime_metadata_source_records,
          result.runtime_export_legality_boundary);
  if (result.stage_diagnostics.semantic.empty() &&
      HasRuntimeMetadataSourceRecords(result.runtime_metadata_source_records) &&
      !IsReadyObjc3RuntimeExportEnforcementSummary(
          result.runtime_export_enforcement_summary)) {
    const std::vector<Objc3RuntimeExportBlockingDiagnostic>
        runtime_export_blocking_diagnostics =
            BuildRuntimeExportBlockingDiagnostics(
                result.runtime_metadata_source_records,
                result.runtime_export_enforcement_summary);
    if (!runtime_export_blocking_diagnostics.empty()) {
      for (const auto &diagnostic : runtime_export_blocking_diagnostics) {
        result.stage_diagnostics.semantic.push_back(
            MakeDiag(diagnostic.line, diagnostic.column, diagnostic.code,
                     diagnostic.message));
      }
    } else {
      std::string runtime_export_failure_reason =
          result.runtime_export_enforcement_summary.failure_reason;
      if (runtime_export_failure_reason ==
              "runtime metadata export shape drift detected before lowering" &&
          !result.runtime_export_legality_boundary.failure_reason.empty()) {
        runtime_export_failure_reason +=
            " (" + result.runtime_export_legality_boundary.failure_reason + ")";
      }
      result.stage_diagnostics.semantic.push_back(MakeDiag(
          result.runtime_export_enforcement_summary.first_failure_line,
          result.runtime_export_enforcement_summary.first_failure_column,
          "O3S260",
          "runtime metadata export blocked: " + runtime_export_failure_reason));
    }
  }
  result.semantic_diagnostic_taxonomy_and_fixit_synthesis_scaffold =
      BuildObjc3SemanticDiagnosticTaxonomyAndFixitSynthesisScaffold(
          result.sema_pass_flow_summary,
          result.sema_parity_surface,
          result.typed_sema_to_lowering_contract_surface);
  result.semantic_diagnostic_taxonomy_and_fixit_core_feature_implementation_surface =
      BuildObjc3SemanticDiagnosticTaxonomyAndFixitSynthesisCoreFeatureImplementationSurface(
          result.sema_pass_flow_summary,
          result.sema_parity_surface,
          result.typed_sema_to_lowering_contract_surface,
          result.semantic_diagnostic_taxonomy_and_fixit_synthesis_scaffold);
  result.part12_diagnostic_taxonomy_portability_contract_summary =
      BuildPart12DiagnosticTaxonomyPortabilityContractSummary(
          result.part12_migration_canonicalization_source_completion_summary,
          result
              .semantic_diagnostic_taxonomy_and_fixit_core_feature_implementation_surface);
  result.part12_feature_specific_fixit_synthesis_summary =
      BuildPart12FeatureSpecificFixitSynthesisSummary(
          result.part12_diagnostic_taxonomy_portability_contract_summary);
  result.semantic_diagnostic_taxonomy_and_fixit_core_feature_expansion_surface =
      BuildObjc3SemanticDiagnosticTaxonomyAndFixitSynthesisCoreFeatureExpansionSurface(
          result.semantic_diagnostic_taxonomy_and_fixit_core_feature_implementation_surface,
          result.sema_parity_surface,
          result.typed_sema_to_lowering_contract_surface);
  result.parse_lowering_readiness_surface = BuildObjc3ParseLoweringReadinessSurface(result, options);
  result.semantic_diagnostic_taxonomy_and_fixit_edge_case_compatibility_surface =
      BuildObjc3SemanticDiagnosticTaxonomyAndFixitSynthesisEdgeCaseCompatibilitySurface(
          result.semantic_diagnostic_taxonomy_and_fixit_core_feature_expansion_surface,
          result.parse_lowering_readiness_surface,
          result.typed_sema_to_lowering_contract_surface);
  result.semantic_diagnostic_taxonomy_and_fixit_edge_case_expansion_and_robustness_surface =
      BuildObjc3SemanticDiagnosticTaxonomyAndFixitSynthesisEdgeCaseExpansionAndRobustnessSurface(
          result.semantic_diagnostic_taxonomy_and_fixit_edge_case_compatibility_surface,
          result.parse_lowering_readiness_surface);
  result.semantic_diagnostic_taxonomy_and_fixit_diagnostics_hardening_surface =
      BuildObjc3SemanticDiagnosticTaxonomyAndFixitSynthesisDiagnosticsHardeningSurface(
          result.semantic_diagnostic_taxonomy_and_fixit_edge_case_expansion_and_robustness_surface,
          result.parse_lowering_readiness_surface);
  result.semantic_diagnostic_taxonomy_and_fixit_recovery_determinism_hardening_surface =
      BuildObjc3SemanticDiagnosticTaxonomyAndFixitSynthesisRecoveryDeterminismHardeningSurface(
          result.semantic_diagnostic_taxonomy_and_fixit_diagnostics_hardening_surface,
          result.parse_lowering_readiness_surface);
  result.semantic_diagnostic_taxonomy_and_fixit_conformance_matrix_implementation_surface =
      BuildObjc3SemanticDiagnosticTaxonomyAndFixitSynthesisConformanceMatrixImplementationSurface(
          result.semantic_diagnostic_taxonomy_and_fixit_recovery_determinism_hardening_surface,
          result.parse_lowering_readiness_surface);
  result.semantic_diagnostic_taxonomy_and_fixit_conformance_corpus_expansion_surface =
      BuildObjc3SemanticDiagnosticTaxonomyAndFixitSynthesisConformanceCorpusExpansionSurface(
          result.semantic_diagnostic_taxonomy_and_fixit_conformance_matrix_implementation_surface,
          result.parse_lowering_readiness_surface);
  result.semantic_diagnostic_taxonomy_and_fixit_performance_quality_guardrails_surface =
      BuildObjc3SemanticDiagnosticTaxonomyAndFixitSynthesisPerformanceQualityGuardrailsSurface(
          result.semantic_diagnostic_taxonomy_and_fixit_conformance_corpus_expansion_surface,
          result.parse_lowering_readiness_surface);
  result.semantic_stability_spec_delta_closure_scaffold =
      BuildObjc3SemanticStabilitySpecDeltaClosureScaffold(
          result.typed_sema_to_lowering_contract_surface,
          result.parse_lowering_readiness_surface);
  result.semantic_stability_core_feature_implementation_surface =
      BuildObjc3SemanticStabilityCoreFeatureImplementationSurface(
          result.typed_sema_to_lowering_contract_surface,
          result.parse_lowering_readiness_surface,
          result.semantic_stability_spec_delta_closure_scaffold);
  result.lowering_runtime_stability_invariant_scaffold =
      BuildObjc3LoweringRuntimeStabilityInvariantScaffold(
          result.typed_sema_to_lowering_contract_surface,
          result.parse_lowering_readiness_surface);
  result.lowering_pipeline_pass_graph_scaffold =
      BuildObjc3LoweringPipelinePassGraphScaffold(result, options);
  result.lowering_pipeline_pass_graph_core_feature_surface =
      BuildObjc3LoweringPipelinePassGraphCoreFeatureSurface(result, options);
  result.ir_emission_completeness_scaffold =
      BuildObjc3IREmissionCompletenessScaffold(result);
  result.lowering_runtime_diagnostics_surfacing_scaffold =
      BuildObjc3LoweringRuntimeDiagnosticsSurfacingScaffold(result);
  result.lowering_runtime_diagnostics_surfacing_core_feature_implementation_surface =
      BuildObjc3LoweringRuntimeDiagnosticsSurfacingCoreFeatureImplementationSurface(
          result);
  result.lowering_runtime_diagnostics_surfacing_core_feature_expansion_surface =
      BuildObjc3LoweringRuntimeDiagnosticsSurfacingCoreFeatureExpansionSurface(
          result);
  result.lowering_runtime_diagnostics_surfacing_edge_case_compatibility_surface =
      BuildObjc3LoweringRuntimeDiagnosticsSurfacingEdgeCaseCompatibilitySurface(
          result);
  result
      .lowering_runtime_diagnostics_surfacing_edge_case_expansion_and_robustness_surface =
      BuildObjc3LoweringRuntimeDiagnosticsSurfacingEdgeCaseExpansionAndRobustnessSurface(
          result);
  result.lowering_runtime_diagnostics_surfacing_diagnostics_hardening_surface =
      BuildObjc3LoweringRuntimeDiagnosticsSurfacingDiagnosticsHardeningSurface(
          result);
  result
      .lowering_runtime_diagnostics_surfacing_recovery_determinism_hardening_surface =
      BuildObjc3LoweringRuntimeDiagnosticsSurfacingRecoveryDeterminismHardeningSurface(
          result);
  result
      .lowering_runtime_diagnostics_surfacing_conformance_matrix_implementation_surface =
      BuildObjc3LoweringRuntimeDiagnosticsSurfacingConformanceMatrixImplementationSurface(
          result);
  result.lowering_runtime_stability_core_feature_implementation_surface =
      BuildObjc3LoweringRuntimeStabilityCoreFeatureImplementationSurface(
          result.typed_sema_to_lowering_contract_surface,
          result.parse_lowering_readiness_surface,
          result.lowering_runtime_stability_invariant_scaffold);
  TransportObjc3DiagnosticsToParsedProgram(result.stage_diagnostics, result.program);
  return result;
}
