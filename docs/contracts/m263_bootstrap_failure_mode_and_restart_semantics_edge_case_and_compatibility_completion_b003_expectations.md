# M263 Bootstrap Failure-Mode and Restart Semantics Expectations (B003)

Contract ID: `objc3c-runtime-bootstrap-failure-restart-semantics/m263-b003-v1`
Status: Accepted
Issue: `#7224`
Scope: `M263` lane-B edge-case and compatibility completion for fail-closed bootstrap restart, replay, and unsupported-topology semantics before lowering/runtime handoff.

## Objective

Land one live semantic/runtime bridge that publishes bootstrap failure-mode, restart, replay, and unsupported-topology behavior over the already-live reset/replay runtime hooks.

## Required Invariants

1. `native/objc3c/src/sema/objc3_sema_contract.h` remains the canonical declaration point for `Objc3BootstrapFailureRestartSemanticsSummary`.
2. `native/objc3c/src/sema/objc3_semantic_passes.cpp` remains the canonical builder point for the sema-owned failure/restart summary and its replay key.
3. `native/objc3c/src/pipeline/objc3_frontend_types.h` remains the canonical declaration point for `Objc3RuntimeBootstrapFailureRestartSemanticsSummary`.
4. `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp` remains the canonical manifest publication point for:
   - `frontend.pipeline.semantic_surface.objc_runtime_bootstrap_failure_restart_semantics`
   - flattened `runtime_bootstrap_failure_restart_semantics_*` summary keys
5. The live bridge stays explicitly tied to:
   - `objc3c-runtime-bootstrap-legality-duplicate-order-semantics/m263-b002-v1`
   - `objc3c-runtime-startup-bootstrap-semantics/m254-b002-v1`
   - `objc3c-runtime-bootstrap-reset-replay/m254-d003-v1`
6. The live failure/restart model preserves:
   - failure mode `abort-before-user-main-no-partial-registration-commit`
   - restart lifecycle `reset-clears-live-runtime-state-and-zeroes-image-local-init-cells`
   - replay order `replay-re-registers-retained-images-in-original-registration-order`
   - unsupported topology `replay-requires-empty-live-runtime-state-and-retained-bootstrap-catalog`
7. The semantic surface publishes the real `translation_unit_identity_key` together with the emitted registration-order ordinal and the runtime reset/replay symbol names.
8. Deterministic probe coverage proves:
   - replay without reset fails closed while live runtime state is still populated
   - reset clears live state but retains the bootstrap catalog for restart
   - reset plus replay restores the retained image in canonical order
   - the same restart cycle remains deterministic across explicit and default bootstrap identifier sources
9. `M263-B003` remains fail-closed and ready-for-lowering/runtime only when the upstream `M263-B002`, `M254-B002`, and `M254-D003` packets are all continuous.

## Non-Goals and Fail-Closed Rules

- `M263-B003` does not land multi-image bootstrap execution.
- `M263-B003` does not widen the public bootstrap runtime API.
- Unsupported restart topologies must fail closed rather than partially replaying live state.

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md`
- `docs/objc3c-native.md`
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
- `spec/MODULE_METADATA_AND_ABI_TABLES.md`

## Build and Readiness Integration

- `package.json` includes `check:objc3c:m263-b003-bootstrap-failure-mode-and-restart-semantics`.
- `package.json` includes `test:tooling:m263-b003-bootstrap-failure-mode-and-restart-semantics`.
- `package.json` includes `check:objc3c:m263-b003-lane-b-readiness`.

## Validation

- `python scripts/check_m263_b003_bootstrap_failure_mode_and_restart_semantics_edge_case_and_compatibility_completion.py`
- `python -m pytest tests/tooling/test_check_m263_b003_bootstrap_failure_mode_and_restart_semantics_edge_case_and_compatibility_completion.py -q`
- `npm run check:objc3c:m263-b003-lane-b-readiness`

## Evidence Path

- `tmp/reports/m263/M263-B003/bootstrap_failure_restart_semantics_summary.json`
