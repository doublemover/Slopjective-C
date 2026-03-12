# M265 Runnable Optionals, Generics, And Key-Path Matrix Cross-Lane Integration Sync Expectations (E002)

Contract ID: `objc3c-runnable-type-surface-closeout/m265-e002-v1`

Issue: `#7256`

## Objective

Close the current Part 3 milestone without widening the supported type surface.
`M265-E002` proves the supported closeout surface through:

- the upstream `A002/B003/C003/D003/E001` proof chain
- real native runtime rows for optional-send short-circuiting, optional binding/refinement, optional-member access, and validated typed key-path execution
- preserved generic metadata and replay evidence rather than a fabricated generic-runtime behavior claim

## Required implementation

1. Add a canonical expectations document and packet for the closeout issue.
2. Add:
   - `scripts/check_m265_e002_runnable_optionals_generics_and_key_path_matrix_cross_lane_integration_sync.py`
   - `tests/tooling/test_check_m265_e002_runnable_optionals_generics_and_key_path_matrix_cross_lane_integration_sync.py`
   - `scripts/run_m265_e002_lane_e_readiness.py`
3. Keep the closeout fail closed over:
   - `tmp/reports/m265/M265-A002/frontend_support_optional_binding_send_coalescing_keypath_summary.json`
   - `tmp/reports/m265/M265-B003/generic_erasure_keypath_legality_completion_summary.json`
   - `tmp/reports/m265/M265-C003/typed_keypath_artifact_emission_summary.json`
   - `tmp/reports/m265/M265-D003/cross_module_type_surface_preservation_summary.json`
   - `tmp/reports/m265/M265-E001/type_surface_executable_conformance_gate_summary.json`
4. The closeout checker must compile, link, and run real positive fixtures for:
   - optional-send argument short-circuiting
   - optional binding/refinement flow
   - optional-member access execution
   - validated typed key-path execution
5. The closeout must keep generic annotations truthful through preserved lowering/runtime-import evidence rather than claiming a standalone generic runtime row.
6. Code/spec anchors must remain explicit and deterministic.
7. `package.json` must wire:
   - `check:objc3c:m265-e002-runnable-optionals-generics-and-key-path-matrix`
   - `test:tooling:m265-e002-runnable-optionals-generics-and-key-path-matrix`
   - `check:objc3c:m265-e002-lane-e-readiness`
8. The closeout must explicitly hand off to `M266-A001`.

## Canonical models

- Matrix model:
  `closeout-matrix-consumes-a002-b003-c003-d003-and-e001-evidence-without-widening-the-supported-runnable-part3-slice`
- Runnable smoke model:
  `integrated-optional-send-binding-refinement-optional-member-access-and-validated-typed-keypath-runtime-rows-prove-the-supported-part3-slice-while-generics-stay-metadata-backed`
- Failure model:
  `fail-closed-on-runnable-part3-closeout-drift-or-doc-mismatch`

## Non-goals

- No new Part 3 source-surface expansion.
- No claim that pragmatic generics gain separate runtime behavior in this milestone.
- No multi-component typed key-path execution claim.
- No broader M266 control-flow/safety semantics.

## Evidence

- `tmp/reports/m265/M265-E002/runnable_type_surface_closeout_summary.json`
