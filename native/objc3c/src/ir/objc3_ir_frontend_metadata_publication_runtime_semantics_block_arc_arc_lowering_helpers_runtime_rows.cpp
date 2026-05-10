#include "ir/objc3_ir_frontend_metadata_publication_runtime_semantics_block_arc_arc_lowering_helpers_runtime_rows.h"

#include <sstream>

#include "ast/objc3_ast.h"
#include "ir/objc3_ir_frontend_metadata_publication_runtime_semantics_block_arc_arc_lowering_helpers_link_rows.h"
#include "ir/objc3_ir_frontend_metadata_publication_runtime_semantics_block_arc_arc_lowering_helpers_row_helpers.h"
#include "lower/objc3_lowering_contract.h"

void EmitObjc3IRArcCleanupWeakLifetimeHooksMetadataNode(
    std::ostringstream &out) {
  BeginObjc3IRArcLoweringHelperMetadataNode(
      "!81", kObjc3ArcCleanupWeakLifetimeHooksContractId, out);
  EmitObjc3IRArcLoweringHelperStringField(
      kObjc3ArcCleanupWeakLifetimeHooksSourceModel, out);
  EmitObjc3IRArcLoweringHelperStringField(
      kObjc3ArcCleanupWeakLifetimeHooksLoweringModel, out);
  EmitObjc3IRArcLoweringHelperStringField(
      Expr::kObjc3ArcModeHandlingContractId, out);
  EmitObjc3IRArcLoweringHelperStringField(
      Expr::kObjc3ArcInteractionSemanticsContractId, out);
  EmitObjc3IRArcLoweringHelperStringField(
      kObjc3ArcLoweringAbiCleanupModelContractId, out);
  EmitObjc3IRArcLoweringHelperStringField(kObjc3ArcAutomaticInsertionContractId,
                                          out);
  EmitObjc3IRArcCleanupWeakLifetimeRuntimeLinkFields(out);
  EmitObjc3IRArcLoweringHelperStringField(
      kObjc3ArcCleanupWeakLifetimeHooksFailureModel, out);
  EmitObjc3IRArcLoweringHelperStringField(
      kObjc3ArcCleanupWeakLifetimeHooksNonGoalModel, out);
  EndObjc3IRArcLoweringHelperMetadataNode(out);
}
