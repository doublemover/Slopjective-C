#include "parse/objc3_parser_block_literal_source.h"

#include <algorithm>
#include <sstream>
#include <unordered_set>

namespace objc3c::parse {
namespace {

std::vector<std::string> BuildSortedUniqueStrings(std::vector<std::string> values) {
  std::sort(values.begin(), values.end());
  values.erase(std::unique(values.begin(), values.end()), values.end());
  return values;
}

#include "parse/objc3_parser_block_literal_capture_identifiers.inc"

void CollectBlockLiteralMutatedIdentifiersFromForClause(
    const ForClause &clause,
    std::vector<std::string> &mutated_identifiers) {
  if (clause.kind == ForClause::Kind::Assign && !clause.name.empty()) {
    mutated_identifiers.push_back(clause.name);
  }
}

void CollectBlockLiteralMutatedIdentifiersFromStatement(
    const Stmt *stmt,
    std::vector<std::string> &mutated_identifiers) {
  if (stmt == nullptr) {
    return;
  }
  switch (stmt->kind) {
  case Stmt::Kind::Let:
  case Stmt::Kind::Return:
  case Stmt::Kind::Expr:
  case Stmt::Kind::Break:
  case Stmt::Kind::Continue:
  case Stmt::Kind::Empty:
    return;
  case Stmt::Kind::Assign:
    if (stmt->assign_stmt != nullptr && !stmt->assign_stmt->name.empty()) {
      mutated_identifiers.push_back(stmt->assign_stmt->name);
    }
    return;
  case Stmt::Kind::If:
    if (stmt->if_stmt != nullptr) {
      for (const auto &then_stmt : stmt->if_stmt->then_body) {
        CollectBlockLiteralMutatedIdentifiersFromStatement(
            then_stmt.get(), mutated_identifiers);
      }
      for (const auto &else_stmt : stmt->if_stmt->else_body) {
        CollectBlockLiteralMutatedIdentifiersFromStatement(
            else_stmt.get(), mutated_identifiers);
      }
    }
    return;
  case Stmt::Kind::DoWhile:
    if (stmt->do_while_stmt != nullptr) {
      for (const auto &body_stmt : stmt->do_while_stmt->body) {
        CollectBlockLiteralMutatedIdentifiersFromStatement(
            body_stmt.get(), mutated_identifiers);
      }
    }
    return;
  case Stmt::Kind::For:
    if (stmt->for_stmt != nullptr) {
      CollectBlockLiteralMutatedIdentifiersFromForClause(
          stmt->for_stmt->init, mutated_identifiers);
      CollectBlockLiteralMutatedIdentifiersFromForClause(
          stmt->for_stmt->step, mutated_identifiers);
      for (const auto &body_stmt : stmt->for_stmt->body) {
        CollectBlockLiteralMutatedIdentifiersFromStatement(
            body_stmt.get(), mutated_identifiers);
      }
    }
    return;
  case Stmt::Kind::Switch:
    if (stmt->switch_stmt != nullptr) {
      for (const auto &switch_case : stmt->switch_stmt->cases) {
        for (const auto &case_stmt : switch_case.body) {
          CollectBlockLiteralMutatedIdentifiersFromStatement(
              case_stmt.get(), mutated_identifiers);
        }
      }
    }
    return;
  case Stmt::Kind::While:
    if (stmt->while_stmt != nullptr) {
      for (const auto &body_stmt : stmt->while_stmt->body) {
        CollectBlockLiteralMutatedIdentifiersFromStatement(
            body_stmt.get(), mutated_identifiers);
      }
    }
    return;
  case Stmt::Kind::Block:
  case Stmt::Kind::Defer:
    if (stmt->block_stmt != nullptr) {
      for (const auto &body_stmt : stmt->block_stmt->body) {
        CollectBlockLiteralMutatedIdentifiersFromStatement(
            body_stmt.get(), mutated_identifiers);
      }
    }
    return;
  }
}

}  // namespace

std::vector<std::string> BuildObjc3BlockLiteralCaptureSet(
    const std::vector<std::unique_ptr<Stmt>> &body,
    const std::vector<std::string> &parameter_names,
    bool &deterministic) {
  deterministic = true;
  std::unordered_set<std::string> parameter_name_set;
  for (const auto &name : parameter_names) {
    if (!parameter_name_set.insert(name).second) {
      deterministic = false;
    }
  }

  std::vector<std::string> used_identifiers;
  std::vector<std::string> declared_identifiers = parameter_names;
  for (const auto &stmt : body) {
    CollectBlockLiteralStmtIdentifiers(
        stmt.get(), used_identifiers, declared_identifiers);
  }

  std::unordered_set<std::string> declared_name_set;
  for (const auto &name : declared_identifiers) {
    if (!name.empty()) {
      declared_name_set.insert(name);
    }
  }

  std::vector<std::string> capture_names;
  capture_names.reserve(used_identifiers.size());
  for (const auto &used_name : used_identifiers) {
    if (used_name.empty() || declared_name_set.count(used_name) != 0u) {
      continue;
    }
    capture_names.push_back(used_name);
  }
  return BuildSortedUniqueStrings(std::move(capture_names));
}

std::vector<std::string> BuildObjc3BlockLiteralMutatedCaptureSet(
    const std::vector<std::unique_ptr<Stmt>> &body,
    const std::vector<std::string> &capture_names) {
  std::vector<std::string> mutated_identifiers;
  for (const auto &stmt : body) {
    CollectBlockLiteralMutatedIdentifiersFromStatement(
        stmt.get(), mutated_identifiers);
  }

  std::unordered_set<std::string> capture_name_set;
  for (const auto &capture_name : capture_names) {
    if (!capture_name.empty()) {
      capture_name_set.insert(capture_name);
    }
  }

  std::vector<std::string> mutated_capture_names;
  mutated_capture_names.reserve(mutated_identifiers.size());
  for (const auto &mutated_identifier : mutated_identifiers) {
    if (!mutated_identifier.empty() &&
        capture_name_set.count(mutated_identifier) != 0u) {
      mutated_capture_names.push_back(mutated_identifier);
    }
  }
  return BuildSortedUniqueStrings(std::move(mutated_capture_names));
}

std::string BuildObjc3BlockEscapeShapeSymbol(
    Objc3BlockLiteralSourceUseKind use_kind) {
  switch (use_kind) {
  case Objc3BlockLiteralSourceUseKind::ExpressionSite:
    return "expression-site";
  case Objc3BlockLiteralSourceUseKind::GlobalInitializer:
    return "global-initializer";
  case Objc3BlockLiteralSourceUseKind::LocalBindingInitializer:
    return "binding-initializer";
  case Objc3BlockLiteralSourceUseKind::AssignmentValue:
    return "assignment-value";
  case Objc3BlockLiteralSourceUseKind::ReturnValue:
    return "return-value";
  case Objc3BlockLiteralSourceUseKind::CallArgument:
    return "call-argument";
  case Objc3BlockLiteralSourceUseKind::MessageArgument:
    return "message-argument";
  }
  return "expression-site";
}

bool Objc3BlockEscapeShapePromotesToHeapCandidate(
    Objc3BlockLiteralSourceUseKind use_kind) {
  return use_kind != Objc3BlockLiteralSourceUseKind::ExpressionSite;
}

std::string BuildObjc3BlockHelperIntentProfile(
    std::size_t mutated_capture_count,
    std::size_t byref_capture_count,
    bool copy_helper_intent_required,
    bool dispose_helper_intent_required,
    Objc3BlockLiteralSourceUseKind use_kind) {
  std::ostringstream out;
  out << "block-helper-intent:mutable-captures=" << mutated_capture_count
      << ";byref-captures=" << byref_capture_count
      << ";copy-helper="
      << (copy_helper_intent_required ? "candidate" : "elided")
      << ";dispose-helper="
      << (dispose_helper_intent_required ? "candidate" : "elided")
      << ";escape-shape=" << BuildObjc3BlockEscapeShapeSymbol(use_kind);
  return out.str();
}

std::string BuildObjc3BlockEscapeShapeProfile(
    Objc3BlockLiteralSourceUseKind use_kind,
    bool promotes_to_heap_candidate,
    std::size_t capture_count,
    std::size_t byref_capture_count) {
  std::ostringstream out;
  out << "block-escape-shape:site=" << BuildObjc3BlockEscapeShapeSymbol(use_kind)
      << ";heap-candidate="
      << (promotes_to_heap_candidate ? "true" : "false")
      << ";captures=" << capture_count
      << ";byref-captures=" << byref_capture_count;
  return out.str();
}

}  // namespace objc3c::parse
