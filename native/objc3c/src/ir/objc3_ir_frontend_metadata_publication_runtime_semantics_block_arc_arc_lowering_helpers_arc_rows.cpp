#include "ir/objc3_ir_frontend_metadata_publication_runtime_semantics_block_arc_arc_lowering_helpers_arc_rows.h"

#include <sstream>

#include "ast/objc3_ast.h"
#include "ir/objc3_ir_frontend_metadata_publication_runtime_semantics_block_arc_arc_lowering_helpers_link_rows.h"
#include "ir/objc3_ir_frontend_metadata_publication_runtime_semantics_block_arc_arc_lowering_helpers_row_helpers.h"
#include "lower/objc3_lowering_contract.h"

void EmitObjc3IRArcAutomaticInsertionMetadataNode(std::ostringstream &out) {
  BeginObjc3IRArcLoweringHelperMetadataNode(
      "!80", kObjc3ArcAutomaticInsertionContractId, out);
  EmitObjc3IRArcLoweringHelperStringField(
      kObjc3ArcAutomaticInsertionSourceModel, out);
  EmitObjc3IRArcLoweringHelperStringField(
      kObjc3ArcAutomaticInsertionLoweringModel, out);
  EmitObjc3IRArcLoweringHelperStringField(
      Expr::kObjc3ArcModeHandlingContractId, out);
  EmitObjc3IRArcLoweringHelperStringField(
      Expr::kObjc3ArcInferenceLifetimeContractId, out);
  EmitObjc3IRArcLoweringHelperStringField(
      Expr::kObjc3ArcInteractionSemanticsContractId, out);
  EmitObjc3IRArcLoweringHelperStringField(
      kObjc3ArcLoweringAbiCleanupModelContractId, out);
  EmitObjc3IRArcAutomaticInsertionRuntimeLinkFields(out);
  EmitObjc3IRArcLoweringHelperStringField(
      kObjc3ArcAutomaticInsertionFailureModel, out);
  EmitObjc3IRArcLoweringHelperStringField(
      kObjc3ArcAutomaticInsertionNonGoalModel, out);
  EndObjc3IRArcLoweringHelperMetadataNode(out);
}
