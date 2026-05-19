#include "ir/objc3_ir_frontend_metadata_publication_runtime_semantics_block_arc_arc_lowering_helpers_link_rows.h"

#include <sstream>

#include "ir/objc3_ir_frontend_metadata_publication_runtime_semantics_block_arc_arc_lowering_helpers_row_helpers.h"
#include "lower/objc3_lowering_contract.h"

void EmitObjc3IRArcAutomaticInsertionRuntimeLinkFields(
    std::ostringstream &out) {
  EmitObjc3IRArcLoweringHelperStringField(kObjc3RuntimeRetainI32Symbol, out);
  EmitObjc3IRArcLoweringHelperStringField(kObjc3RuntimeReleaseI32Symbol, out);
  EmitObjc3IRArcLoweringHelperStringField(kObjc3RuntimeAutoreleaseI32Symbol,
                                          out);
}

void EmitObjc3IRArcCleanupWeakLifetimeRuntimeLinkFields(
    std::ostringstream &out) {
  EmitObjc3IRArcLoweringHelperStringField(
      kObjc3RuntimeLoadWeakCurrentPropertyI32Symbol, out);
  EmitObjc3IRArcLoweringHelperStringField(
      kObjc3RuntimeStoreWeakCurrentPropertyI32Symbol, out);
  EmitObjc3IRArcLoweringHelperStringField(kObjc3RuntimeRetainI32Symbol, out);
  EmitObjc3IRArcLoweringHelperStringField(kObjc3RuntimeReleaseI32Symbol, out);
}

void EmitObjc3IRArcBlockAutoreleaseReturnRuntimeLinkFields(
    std::ostringstream &out) {
  EmitObjc3IRArcLoweringHelperStringField(kObjc3RuntimeRetainI32Symbol, out);
  EmitObjc3IRArcLoweringHelperStringField(kObjc3RuntimeReleaseI32Symbol, out);
  EmitObjc3IRArcLoweringHelperStringField(kObjc3RuntimeAutoreleaseI32Symbol,
                                          out);
  EmitObjc3IRArcLoweringHelperStringField(kObjc3RuntimePromoteBlockI32Symbol,
                                          out);
  EmitObjc3IRArcLoweringHelperStringField(kObjc3RuntimeInvokeBlockI32Symbol,
                                          out);
}
