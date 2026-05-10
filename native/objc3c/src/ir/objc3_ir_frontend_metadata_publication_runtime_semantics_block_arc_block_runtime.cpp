#include "ir/objc3_ir_frontend_metadata_publication_runtime_semantics_block_arc_block_runtime.h"

#include <sstream>

#include "ast/objc3_ast.h"
#include "ir/objc3_ir_c_string.h"
#include "lower/objc3_lowering_contract.h"

void EmitObjc3IRBlockRuntimeGateExecutionMetadataNodes(std::ostringstream &out) {
  out << "!73 = !{!\""
      << EscapeCStringLiteral(Expr::kObjc3RunnableBlockRuntimeGateContractId)
      << "\", !\""
      << EscapeCStringLiteral(Expr::kObjc3RunnableBlockRuntimeGateEvidenceModel)
      << "\", !\""
      << EscapeCStringLiteral(Expr::kObjc3RunnableBlockRuntimeGateActiveModel)
      << "\", !\""
      << EscapeCStringLiteral(Expr::kObjc3RunnableBlockRuntimeGateNonGoalModel)
      << "\", !\""
      << EscapeCStringLiteral(Expr::kObjc3RunnableBlockRuntimeGateFailClosedModel)
      << "\", !\""
      << EscapeCStringLiteral(
             Expr::kObjc3ExecutableBlockSourceStorageAnnotationContractId)
      << "\", !\""
      << EscapeCStringLiteral(
             Expr::kObjc3ExecutableBlockOwnershipSemanticsImplementationContractId)
      << "\", !\""
      << EscapeCStringLiteral(
             Expr::kObjc3ExecutableBlockEscapeRuntimeHookLoweringContractId)
      << "\", !\""
      << EscapeCStringLiteral(
             kObjc3RuntimeBlockByrefForwardingHeapPromotionInteropContractId)
      << "\"}\n";
  out << "!74 = !{!\""
      << EscapeCStringLiteral(Expr::kObjc3RunnableBlockExecutionMatrixContractId)
      << "\", !\""
      << EscapeCStringLiteral(Expr::kObjc3RunnableBlockExecutionMatrixEvidenceModel)
      << "\", !\""
      << EscapeCStringLiteral(Expr::kObjc3RunnableBlockExecutionMatrixActiveModel)
      << "\", !\""
      << EscapeCStringLiteral(Expr::kObjc3RunnableBlockExecutionMatrixNonGoalModel)
      << "\", !\""
      << EscapeCStringLiteral(Expr::kObjc3RunnableBlockExecutionMatrixFailClosedModel)
      << "\", !\""
      << EscapeCStringLiteral(Expr::kObjc3RunnableBlockRuntimeGateContractId)
      << "\"}\n";
}
