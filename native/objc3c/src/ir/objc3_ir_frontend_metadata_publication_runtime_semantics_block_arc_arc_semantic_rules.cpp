#include "ir/objc3_ir_frontend_metadata_publication_runtime_semantics_block_arc_arc_semantic_rules.h"

#include <sstream>

#include "ast/objc3_ast.h"
#include "ir/objc3_ir_c_string.h"
#include "lower/objc3_lowering_contract.h"

void EmitObjc3IRArcSemanticRulesMetadataNode(std::ostringstream &out) {
  out << "!77 = !{!\""
      << EscapeCStringLiteral(Expr::kObjc3ArcSemanticRulesContractId)
      << "\", !\"" << EscapeCStringLiteral(Expr::kObjc3ArcSemanticRulesSourceModel)
      << "\", !\"" << EscapeCStringLiteral(Expr::kObjc3ArcSemanticRulesSemanticModel)
      << "\", !\"" << EscapeCStringLiteral(kObjc3WeakUnownedSemanticsLoweringLaneContract)
      << "\", !\"" << EscapeCStringLiteral(kObjc3ArcDiagnosticsFixitLoweringLaneContract)
      << "\", !\"" << EscapeCStringLiteral(Expr::kObjc3ArcSemanticRulesFailClosedModel)
      << "\", !\"" << EscapeCStringLiteral(Expr::kObjc3ArcSemanticRulesNonGoalModel)
      << "\"}\n";
}
