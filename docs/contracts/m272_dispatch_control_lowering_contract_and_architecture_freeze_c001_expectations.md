# M272 Dispatch-Control Lowering Contract And Architecture Freeze Expectations (C001)

Contract ID: `objc3c-part9-dispatch-control-lowering-contract/m272-c001-v1`

## Required Surface

- Publish `frontend.pipeline.semantic_surface.objc_part9_dispatch_control_lowering_contract` in emitted frontend manifests.
- Derive the lowering packet from the already-landed Part 9 lane-B semantic summaries:
  - `objc_part9_dynamism_and_dispatch_control_semantic_model`
  - `objc_part9_override_finality_and_sealing_legality`
  - `objc_part9_dynamism_control_compatibility_diagnostics`
- Emit replay-stable Part 9 lowering metadata into LLVM IR frontend metadata.

## Truth Boundary

- This issue freezes lowering carriage and emitted metadata only.
- This issue does not claim:
  - live direct-call selector bypass
  - runtime dispatch-boundary realization
  - runnable metadata-driven dispatch behavior

## Positive Proof

- A positive Part 9 fixture must compile through `objc3c-frontend-c-api-runner` and emit:
  - `module.manifest.json`
  - `module.ll`
- The manifest packet and IR metadata must agree on:
  - direct-call candidate counts
  - direct-members defaulting and dynamic opt-out counts
  - final/sealed container counts
  - metadata-preserved callable/container counts
  - zero guard-blocked and zero contract-violation counts on the happy path
