#include "ir/objc3_ir_frontend_metadata_publication_runtime_semantics_block_arc_arc_interaction_semantics.h"

#include <sstream>

#include "ast/objc3_ast.h"
#include "ir/objc3_ir_c_string.h"
#include "lower/objc3_lowering_contract.h"

void EmitObjc3IRArcInteractionSemanticsMetadataNode(std::ostringstream &out) {
  out << "!79 = !{!\""
      << EscapeCStringLiteral(Expr::kObjc3ArcInteractionSemanticsContractId)
      << "\", !\"" << EscapeCStringLiteral(Expr::kObjc3ArcInteractionSemanticsSourceModel)
      << "\", !\"" << EscapeCStringLiteral(Expr::kObjc3ArcInteractionSemanticsSemanticModel)
      << "\", !\"" << EscapeCStringLiteral(Expr::kObjc3ArcInferenceLifetimeContractId)
      << "\", !\"" << EscapeCStringLiteral(kObjc3WeakUnownedSemanticsLoweringLaneContract)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RetainReleaseOperationLoweringLaneContract)
      << "\", !\"" << EscapeCStringLiteral(kObjc3AutoreleasePoolScopeLoweringLaneContract)
      << "\", !\"" << EscapeCStringLiteral(kObjc3BlockStorageEscapeLoweringLaneContract)
      << "\", !\""
      << EscapeCStringLiteral(
             kObjc3ExecutableSynthesizedAccessorPropertyLoweringContractId)
      << "\", !\""
      << EscapeCStringLiteral(Expr::kObjc3ArcInteractionSemanticsFailClosedModel)
      << "\", !\""
      << EscapeCStringLiteral(Expr::kObjc3ArcInteractionSemanticsNonGoalModel)
      << "\"}\n";
}
