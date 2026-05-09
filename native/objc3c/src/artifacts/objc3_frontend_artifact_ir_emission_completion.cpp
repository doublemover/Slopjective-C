#include "artifacts/objc3_frontend_artifact_ir_emission_completion.h"

#include <string>

#include "artifacts/objc3_frontend_artifact_sanity.h"
#include "diag/objc3_diag_format.h"
#include "ir/objc3_ir_emitter.h"

namespace objc3::artifacts::frontend {

bool CompleteObjc3FrontendArtifactIREmission(
    Objc3FrontendArtifactBundle &bundle,
    const Objc3FrontendPipelineResult &pipeline_result,
    const Objc3FrontendOptions &options,
    const Objc3Program &program,
    const Objc3IRFrontendMetadata &ir_frontend_metadata,
    const Objc3RuntimeDispatchLoweringAbiContract
        &runtime_dispatch_lowering_abi_contract,
    const Objc3MessageSendSelectorLoweringContract
        &message_send_selector_lowering_contract) {
  std::string ir_error;
  // Historical extraction contract marker:
  // EmitObjc3IRText(pipeline_result.program, options.lowering,
  // ir_frontend_metadata, bundle.ir_text, ir_error)
  if (!EmitObjc3IRText(pipeline_result.program.ast, options.lowering,
                       ir_frontend_metadata, bundle.ir_text, ir_error)) {
    bundle.post_pipeline_diagnostics = {
        MakeDiag(1, 1, "O3L300", "LLVM IR emission failed: " + ir_error)};
    bundle.diagnostics = bundle.post_pipeline_diagnostics;
    bundle.manifest_json.clear();
    bundle.runtime_metadata_binary.clear();
    bundle.ir_text.clear();
    return false;
  }
  bundle.ir_text =
      std::string("; runtime_dispatch_lowering_abi_boundary = ") +
      Objc3RuntimeDispatchLoweringAbiBoundarySummary(
          runtime_dispatch_lowering_abi_contract) +
      "\n" + bundle.ir_text;

  if (objc3c::artifacts::IsSuspiciousObjc3NativeIRTruthGap(
          bundle.ir_text, program, message_send_selector_lowering_contract)) {
    bundle.post_pipeline_diagnostics = {
        MakeDiag(1, 1, "O3L330",
                 "LLVM IR emission failed: emitted native IR is suspiciously "
                 "trivial for runtime-bearing executable surface")};
    bundle.diagnostics = bundle.post_pipeline_diagnostics;
    bundle.manifest_json.clear();
    bundle.runtime_metadata_binary.clear();
    bundle.ir_text.clear();
    return false;
  }

  return true;
}

}  // namespace objc3::artifacts::frontend
