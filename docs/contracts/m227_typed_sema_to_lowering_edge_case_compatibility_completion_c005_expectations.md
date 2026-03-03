# M227 Typed Sema-to-Lowering Edge-Case and Compatibility Completion Expectations (C005)

Contract ID: `objc3c-typed-sema-to-lowering-edge-case-compatibility-completion/m227-c005-v1`
Status: Accepted
Scope: typed sema-to-lowering edge-case and compatibility completion on top of C004 core-feature expansion.

## Objective

Execute issue `#5125` by extending typed sema-to-lowering contract surfaces
with compatibility-handoff and parse-artifact edge-case robustness signals, and
require deterministic edge-case compatibility readiness before lowering
readiness can pass.
Code/spec anchors and milestone optimization improvements are mandatory scope inputs.

## Dependency Scope

- Dependencies: `M227-C004`
- `M227-C004` remains a mandatory prerequisite:
  - `docs/contracts/m227_typed_sema_to_lowering_core_feature_expansion_c004_expectations.md`
  - `scripts/check_m227_c004_typed_sema_to_lowering_core_feature_expansion_contract.py`
  - `tests/tooling/test_check_m227_c004_typed_sema_to_lowering_core_feature_expansion_contract.py`
  - `spec/planning/compiler/m227/m227_c004_typed_sema_to_lowering_core_feature_expansion_packet.md`

## Deterministic Invariants

1. `Objc3TypedSemaToLoweringContractSurface` carries edge-case compatibility fields:
   - `compatibility_handoff_consistent`
   - `language_version_pragma_coordinate_order_consistent`
   - `parse_artifact_replay_key_deterministic`
   - `parse_artifact_edge_case_robustness_consistent`
   - `typed_core_feature_edge_case_compatibility_ready`
   - `compatibility_handoff_key`
   - `parse_artifact_edge_robustness_key`
   - `typed_core_feature_edge_case_compatibility_key`
2. `Objc3ParseLoweringReadinessSurface` carries corresponding typed sema
   edge-case compatibility fields and fails closed when compatibility alignment
   drifts from typed handoff and parse/lowering readiness surfaces.
3. Typed contract and parse-readiness builders require non-empty compatibility
   keys plus deterministic replay/robustness continuity before
   `ready_for_lowering` can be true.
4. Failure reasons include explicit edge-case compatibility failure branches.

## Build and Readiness Integration

- `package.json` includes:
  - `check:objc3c:m227-c005-typed-sema-to-lowering-edge-case-compatibility-completion-contract`
  - `test:tooling:m227-c005-typed-sema-to-lowering-edge-case-compatibility-completion-contract`
  - `check:objc3c:m227-c005-lane-c-readiness`
- lane-C readiness chaining preserves C004 continuity:
  - `scripts/check_m227_c004_typed_sema_to_lowering_core_feature_expansion_contract.py`
  - `check:objc3c:m227-c005-lane-c-readiness`

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md` includes explicit M227 lane-C C005
  edge-case compatibility completion anchor text.
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md` includes lane-C C005 fail-closed
  edge-case compatibility completion governance text.
- `spec/MODULE_METADATA_AND_ABI_TABLES.md` includes deterministic lane-C C005
  edge-case compatibility metadata anchors.

## Milestone Optimization Inputs

- `package.json` includes `test:objc3c:sema-pass-manager-diagnostics-bus`.
- `package.json` includes `test:objc3c:lowering-regression`.

## Validation

- `python scripts/check_m227_c004_typed_sema_to_lowering_core_feature_expansion_contract.py`
- `python scripts/check_m227_c005_typed_sema_to_lowering_edge_case_compatibility_completion_contract.py`
- `python -m pytest tests/tooling/test_check_m227_c005_typed_sema_to_lowering_edge_case_compatibility_completion_contract.py -q`
- `npm run check:objc3c:m227-c005-lane-c-readiness`

## Evidence Path

- `tmp/reports/m227/M227-C005/typed_sema_to_lowering_edge_case_compatibility_completion_contract_summary.json`
