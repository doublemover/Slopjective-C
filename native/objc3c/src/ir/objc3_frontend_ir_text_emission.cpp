#include "support/frontend_ir_text_emission.h"

#include "ir/objc3_ir_emitter.h"

bool EmitObjc3FrontendIRTextForArtifact(
    const Objc3Program &program,
    const Objc3LoweringContract &lowering_contract,
    const Objc3IRFrontendMetadata &frontend_metadata,
    std::string &ir,
    std::string &error) {
  return EmitObjc3IRText(program, lowering_contract, frontend_metadata, ir,
                         error);
}
