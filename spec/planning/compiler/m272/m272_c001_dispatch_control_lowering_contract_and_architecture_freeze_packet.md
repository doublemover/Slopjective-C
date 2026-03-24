# M272-C001 Packet: Dispatch-Control Lowering Contract - Contract And Architecture Freeze

## Objective

Freeze one truthful Part 9 lowering contract for direct-call candidates, final/sealed dispatch boundaries, and metadata-preserved dynamism intent carriage.

## Implementation Requirements

1. Add a dedicated lowering contract in `native/objc3c/src/lower/objc3_lowering_contract.h` and validation/replay-key support in `native/objc3c/src/lower/objc3_lowering_contract.cpp`.
2. Build the contract in `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp` from the existing M272 B-lane semantic summaries.
3. Publish the contract in emitted manifest JSON under `frontend.pipeline.semantic_surface.objc_part9_dispatch_control_lowering_contract`.
4. Thread replay-stable Part 9 lowering metadata into emitted LLVM IR frontend metadata.
5. Add issue-local checker, readiness runner, pytest, expectations doc, and positive fixture.

## Truth Constraints

- Keep the issue fail-closed and lowering-focused.
- Do not claim live direct-call selector bypass, runtime dispatch-boundary realization, or runnable metadata-driven dispatch behavior.
- Preserve later M272 lane-C and lane-D work as the only place allowed to widen this boundary.
