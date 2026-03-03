# M227 Typed Sema-to-Lowering Edge-Case Expansion and Robustness Expectations (C006)

Contract ID: `objc3c-typed-sema-to-lowering-edge-case-expansion-robustness/m227-c006-v1`
Status: Accepted
Scope: typed sema-to-lowering edge-case expansion and robustness on top of C005 compatibility completion.

## Objective

Execute issue `#5126` by extending typed sema-to-lowering contract surfaces
with explicit edge-case expansion consistency and edge-case robustness
readiness, then enforce parse/lowering alignment on those invariants with
deterministic fail-closed gating.
Code/spec anchors and milestone optimization improvements are mandatory scope inputs.

## Dependency Scope

- Dependencies: `M227-C005`
- `M227-C005` remains a mandatory prerequisite:
  - `docs/contracts/m227_typed_sema_to_lowering_edge_case_compatibility_completion_c005_expectations.md`
  - `scripts/check_m227_c005_typed_sema_to_lowering_edge_case_compatibility_completion_contract.py`
  - `tests/tooling/test_check_m227_c005_typed_sema_to_lowering_edge_case_compatibility_completion_contract.py`
  - `spec/planning/compiler/m227/m227_c005_typed_sema_to_lowering_edge_case_compatibility_completion_packet.md`

## Deterministic Invariants

1. `Objc3TypedSemaToLoweringContractSurface` carries edge-case expansion and
   robustness fields:
   - `typed_core_feature_edge_case_expansion_consistent`
   - `typed_core_feature_edge_case_robustness_ready`
   - `typed_core_feature_edge_case_robustness_key`
2. Typed surface readiness requires compatibility completion continuity and
   parse artifact replay/robustness continuity before edge-case robustness can
   be considered ready.
3. `Objc3ParseLoweringReadinessSurface` carries mapped typed sema edge-case
   expansion and robustness fields:
   - `typed_sema_edge_case_expansion_consistent`
   - `typed_sema_edge_case_robustness_ready`
   - `typed_sema_edge_case_robustness_key`
4. Parse/lowering readiness must fail closed when typed edge-case robustness
   alignment drifts from typed sema-to-lowering contract surfaces.

## Build and Readiness Integration

- `package.json` includes:
  - `check:objc3c:m227-c006-typed-sema-to-lowering-edge-case-expansion-and-robustness-contract`
  - `test:tooling:m227-c006-typed-sema-to-lowering-edge-case-expansion-and-robustness-contract`
  - `check:objc3c:m227-c006-lane-c-readiness`
- lane-C readiness chaining preserves C005 continuity:
  - `scripts/check_m227_c005_typed_sema_to_lowering_edge_case_compatibility_completion_contract.py`
  - `check:objc3c:m227-c006-lane-c-readiness`

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md` includes explicit M227 lane-C C006
  edge-case expansion and robustness anchor text.
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md` includes lane-C C006 fail-closed
  edge-case expansion and robustness governance text.
- `spec/MODULE_METADATA_AND_ABI_TABLES.md` includes deterministic lane-C C006
  edge-case expansion and robustness metadata anchors.

## Milestone Optimization Inputs

- `package.json` includes `test:objc3c:sema-pass-manager-diagnostics-bus`.
- `package.json` includes `test:objc3c:lowering-regression`.

## Validation

- `python scripts/check_m227_c005_typed_sema_to_lowering_edge_case_compatibility_completion_contract.py`
- `python scripts/check_m227_c006_typed_sema_to_lowering_edge_case_expansion_and_robustness_contract.py`
- `python -m pytest tests/tooling/test_check_m227_c006_typed_sema_to_lowering_edge_case_expansion_and_robustness_contract.py -q`
- `npm run check:objc3c:m227-c006-lane-c-readiness`

## Evidence Path

- `tmp/reports/m227/M227-C006/typed_sema_to_lowering_edge_case_expansion_and_robustness_contract_summary.json`
