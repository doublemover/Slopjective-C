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
          property_record.executable_ivar_layout_offset_bytes =
              property.executable_ivar_layout_offset_bytes;
          property_record.executable_ivar_layout_padding_bytes =
              property.executable_ivar_layout_padding_bytes;
          property_record.executable_ivar_layout_inherited_slot_count =
              property.executable_ivar_layout_inherited_slot_count;
          property_record.executable_ivar_layout_inherited_size_bytes =
              property.executable_ivar_layout_inherited_size_bytes;
          property_record.executable_ivar_layout_owner_size_bytes =
              property.executable_ivar_layout_owner_size_bytes;
          property_record.executable_ivar_init_order_index =
              property.executable_ivar_init_order_index;
          property_record.executable_ivar_destroy_order_index =
              property.executable_ivar_destroy_order_index;
          property_record.executable_ivar_layout_valid =
              property.executable_ivar_layout_valid;
          property_record.executable_ivar_layout_replay_key =
              property.executable_ivar_layout_replay_key;
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
            ivar_record.executable_ivar_layout_offset_bytes =
                property.executable_ivar_layout_offset_bytes;
            ivar_record.executable_ivar_layout_padding_bytes =
                property.executable_ivar_layout_padding_bytes;
            ivar_record.executable_ivar_layout_inherited_slot_count =
                property.executable_ivar_layout_inherited_slot_count;
            ivar_record.executable_ivar_layout_inherited_size_bytes =
                property.executable_ivar_layout_inherited_size_bytes;
            ivar_record.executable_ivar_layout_owner_size_bytes =
                property.executable_ivar_layout_owner_size_bytes;
            ivar_record.executable_ivar_init_order_index =
                property.executable_ivar_init_order_index;
            ivar_record.executable_ivar_destroy_order_index =
                property.executable_ivar_destroy_order_index;
            ivar_record.executable_ivar_layout_valid =
                property.executable_ivar_layout_valid;
            ivar_record.executable_ivar_layout_replay_key =
                property.executable_ivar_layout_replay_key;
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
          node.executable_ivar_layout_offset_bytes =
              property.executable_ivar_layout_offset_bytes;
          node.executable_ivar_layout_padding_bytes =
              property.executable_ivar_layout_padding_bytes;
          node.executable_ivar_layout_inherited_slot_count =
              property.executable_ivar_layout_inherited_slot_count;
          node.executable_ivar_layout_inherited_size_bytes =
              property.executable_ivar_layout_inherited_size_bytes;
          node.executable_ivar_layout_owner_size_bytes =
              property.executable_ivar_layout_owner_size_bytes;
          node.executable_ivar_init_order_index =
              property.executable_ivar_init_order_index;
          node.executable_ivar_destroy_order_index =
              property.executable_ivar_destroy_order_index;
          node.executable_ivar_layout_valid =
              property.executable_ivar_layout_valid;
          node.executable_ivar_layout_replay_key =
              property.executable_ivar_layout_replay_key;
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
            ivar_node.executable_ivar_layout_offset_bytes =
                property.executable_ivar_layout_offset_bytes;
            ivar_node.executable_ivar_layout_padding_bytes =
                property.executable_ivar_layout_padding_bytes;
            ivar_node.executable_ivar_layout_inherited_slot_count =
                property.executable_ivar_layout_inherited_slot_count;
            ivar_node.executable_ivar_layout_inherited_size_bytes =
                property.executable_ivar_layout_inherited_size_bytes;
            ivar_node.executable_ivar_layout_owner_size_bytes =
                property.executable_ivar_layout_owner_size_bytes;
            ivar_node.executable_ivar_init_order_index =
                property.executable_ivar_init_order_index;
            ivar_node.executable_ivar_destroy_order_index =
                property.executable_ivar_destroy_order_index;
            ivar_node.executable_ivar_layout_valid =
                property.executable_ivar_layout_valid;
            ivar_node.executable_ivar_layout_replay_key =
                property.executable_ivar_layout_replay_key;
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
