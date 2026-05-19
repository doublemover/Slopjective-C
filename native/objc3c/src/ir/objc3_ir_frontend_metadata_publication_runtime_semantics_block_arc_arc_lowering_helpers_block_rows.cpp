#include "ir/objc3_ir_frontend_metadata_publication_runtime_semantics_block_arc_arc_lowering_helpers_block_rows.h"

#include <sstream>

#include "ast/objc3_ast.h"
#include "ir/objc3_ir_frontend_metadata_publication_runtime_semantics_block_arc_arc_lowering_helpers_link_rows.h"
#include "ir/objc3_ir_frontend_metadata_publication_runtime_semantics_block_arc_arc_lowering_helpers_row_helpers.h"
#include "lower/objc3_lowering_contract.h"

void EmitObjc3IRArcBlockAutoreleaseReturnLoweringMetadataNode(
    std::ostringstream &out) {
  BeginObjc3IRArcLoweringHelperMetadataNode(
      "!82", kObjc3ArcBlockAutoreleaseReturnLoweringContractId, out);
  EmitObjc3IRArcLoweringHelperStringField(
      kObjc3ArcBlockAutoreleaseReturnLoweringSourceModel, out);
  EmitObjc3IRArcLoweringHelperStringField(
      kObjc3ArcBlockAutoreleaseReturnLoweringModel, out);
  EmitObjc3IRArcLoweringHelperStringField(
      kObjc3ArcCleanupWeakLifetimeHooksContractId, out);
  EmitObjc3IRArcLoweringHelperStringField(kObjc3ArcAutomaticInsertionContractId,
                                          out);
  EmitObjc3IRArcLoweringHelperStringField(
      Expr::kObjc3ExecutableBlockEscapeRuntimeHookLoweringContractId, out);
  EmitObjc3IRArcBlockAutoreleaseReturnRuntimeLinkFields(out);
  EmitObjc3IRArcLoweringHelperStringField(
      kObjc3ArcBlockAutoreleaseReturnLoweringFailureModel, out);
  EmitObjc3IRArcLoweringHelperStringField(
      kObjc3ArcBlockAutoreleaseReturnLoweringNonGoalModel, out);
  EndObjc3IRArcLoweringHelperMetadataNode(out);
}
