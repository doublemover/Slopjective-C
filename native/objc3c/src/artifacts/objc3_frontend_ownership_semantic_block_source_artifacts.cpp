#include "artifacts/objc3_frontend_ownership_semantic_artifacts.h"

#include <algorithm>
#include <string>
#include <vector>

#include "ast/objc3_ast_core.h"
#include "ast/objc3_ast_declarations.h"
#include "pipeline/objc3_frontend_types.h"

namespace objc3::artifacts::frontend {
namespace {

#include "objc3_frontend_ownership_semantic_block_source_common.inc"
#include "objc3_frontend_ownership_semantic_block_source_model_completion.inc"
#include "objc3_frontend_ownership_semantic_block_source_storage_annotation.inc"

}  // namespace

Objc3BlockSourceModelCompletionContract BuildBlockSourceModelCompletionContract(
    const Objc3Program &program) {
  Objc3BlockSourceModelCompletionContract contract;

  for (const auto &global : program.globals) {
    AccumulateBlockSourceModelCompletionFromExpr(global.value.get(), contract);
  }
  for (const auto &function : program.functions) {
    for (const auto &stmt : function.body) {
      AccumulateBlockSourceModelCompletionFromStmt(stmt.get(), contract);
    }
  }
  for (const auto &protocol : program.protocols) {
    for (const auto &method : protocol.methods) {
      for (const auto &stmt : method.body) {
        AccumulateBlockSourceModelCompletionFromStmt(stmt.get(), contract);
      }
    }
  }
  for (const auto &interface_decl : program.interfaces) {
    for (const auto &method : interface_decl.methods) {
      for (const auto &stmt : method.body) {
        AccumulateBlockSourceModelCompletionFromStmt(stmt.get(), contract);
      }
    }
  }
  for (const auto &implementation : program.implementations) {
    for (const auto &method : implementation.methods) {
      for (const auto &stmt : method.body) {
        AccumulateBlockSourceModelCompletionFromStmt(stmt.get(), contract);
      }
    }
  }

  contract.deterministic = contract.non_normalized_sites == 0u &&
                           contract.contract_violation_sites == 0u;
  return contract;
}

Objc3BlockSourceStorageAnnotationContract
BuildBlockSourceStorageAnnotationContract(const Objc3Program &program) {
  Objc3BlockSourceStorageAnnotationContract contract;

  for (const auto &global : program.globals) {
    AccumulateBlockSourceStorageAnnotationFromExpr(global.value.get(), contract);
  }
  for (const auto &function : program.functions) {
    for (const auto &stmt : function.body) {
      AccumulateBlockSourceStorageAnnotationFromStmt(stmt.get(), contract);
    }
  }
  for (const auto &protocol : program.protocols) {
    for (const auto &method : protocol.methods) {
      for (const auto &stmt : method.body) {
        AccumulateBlockSourceStorageAnnotationFromStmt(stmt.get(), contract);
      }
    }
  }
  for (const auto &interface_decl : program.interfaces) {
    for (const auto &method : interface_decl.methods) {
      for (const auto &stmt : method.body) {
        AccumulateBlockSourceStorageAnnotationFromStmt(stmt.get(), contract);
      }
    }
  }
  for (const auto &implementation : program.implementations) {
    for (const auto &method : implementation.methods) {
      for (const auto &stmt : method.body) {
        AccumulateBlockSourceStorageAnnotationFromStmt(stmt.get(), contract);
      }
    }
  }

  contract.deterministic = contract.non_normalized_sites == 0u &&
                           contract.contract_violation_sites == 0u;
  return contract;
}

}  // namespace objc3::artifacts::frontend
