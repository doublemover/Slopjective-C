#include "ir/objc3_ir_frontend_metadata_publication_runtime_semantics_block_arc_arc_source_mode.h"

#include <sstream>

#include "ast/objc3_ast.h"
#include "ir/objc3_ir_c_string.h"
#include "ir/objc3_ir_frontend_metadata.h"
#include "lower/objc3_lowering_contract.h"

void EmitObjc3IRArcSourceModeMetadataNodes(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  out << "!75 = !{!\""
      << EscapeCStringLiteral(Expr::kObjc3ArcSourceModeBoundaryContractId)
      << "\", !\"" << EscapeCStringLiteral(Expr::kObjc3ArcSourceModeBoundarySourceModel)
      << "\", !\"" << EscapeCStringLiteral(Expr::kObjc3ArcSourceModeBoundaryModeModel)
      << "\", !\"" << EscapeCStringLiteral(kObjc3OwnershipQualifierLoweringLaneContract)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RetainReleaseOperationLoweringLaneContract)
      << "\", !\"" << EscapeCStringLiteral(kObjc3AutoreleasePoolScopeLoweringLaneContract)
      << "\", !\"" << EscapeCStringLiteral(kObjc3WeakUnownedSemanticsLoweringLaneContract)
      << "\", !\"" << EscapeCStringLiteral(kObjc3ArcDiagnosticsFixitLoweringLaneContract)
      << "\", !\"" << EscapeCStringLiteral(Expr::kObjc3ArcSourceModeBoundaryNonGoalModel)
      << "\", !\"" << EscapeCStringLiteral(Expr::kObjc3ArcSourceModeBoundaryFailClosedModel)
      << "\"}\n";
  out << "!76 = !{!\""
      << EscapeCStringLiteral(Expr::kObjc3ArcModeHandlingContractId)
      << "\", !\"" << EscapeCStringLiteral(Expr::kObjc3ArcModeHandlingSourceModel)
      << "\", !\"" << EscapeCStringLiteral(Expr::kObjc3ArcModeHandlingModeModel)
      << "\", !\"" << EscapeCStringLiteral(metadata.arc_mode)
      << "\", !\"" << EscapeCStringLiteral(kObjc3OwnershipQualifierLoweringLaneContract)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RetainReleaseOperationLoweringLaneContract)
      << "\", !\"" << EscapeCStringLiteral(kObjc3AutoreleasePoolScopeLoweringLaneContract)
      << "\", !\"" << EscapeCStringLiteral(kObjc3WeakUnownedSemanticsLoweringLaneContract)
      << "\", !\"" << EscapeCStringLiteral(kObjc3ArcDiagnosticsFixitLoweringLaneContract)
      << "\", !\"" << EscapeCStringLiteral(Expr::kObjc3RunnableBlockRuntimeGateContractId)
      << "\", !\"" << EscapeCStringLiteral(Expr::kObjc3ArcModeHandlingFailClosedModel)
      << "\", !\"" << EscapeCStringLiteral(Expr::kObjc3ArcModeHandlingNonGoalModel)
      << "\"}\n";
}
