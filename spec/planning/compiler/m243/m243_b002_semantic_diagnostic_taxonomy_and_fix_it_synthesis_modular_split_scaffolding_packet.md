# M243-B002 Semantic Diagnostic Taxonomy and Fix-it Synthesis Modular Split and Scaffolding Packet

Packet: `M243-B002`
Milestone: `M243`
Lane: `B`
Dependencies: `M243-B001`

## Scope

Enforce modular split/scaffolding continuity for semantic diagnostic taxonomy
and ARC fix-it synthesis handoff so lane-B diagnostics readiness remains
deterministic and fail-closed through typed sema handoff boundaries.

## Anchors

- Contract: `docs/contracts/m243_semantic_diagnostic_taxonomy_and_fix_it_synthesis_modular_split_scaffolding_b002_expectations.md`
- Checker: `scripts/check_m243_b002_semantic_diagnostic_taxonomy_and_fix_it_synthesis_modular_split_scaffolding_contract.py`
- Tooling tests: `tests/tooling/test_check_m243_b002_semantic_diagnostic_taxonomy_and_fix_it_synthesis_modular_split_scaffolding_contract.py`
- Scaffold header: `native/objc3c/src/pipeline/objc3_semantic_diagnostic_taxonomy_and_fix_it_synthesis_scaffold.h`
- Pipeline surface types: `native/objc3c/src/pipeline/objc3_frontend_types.h`
- Pipeline wiring: `native/objc3c/src/pipeline/objc3_frontend_pipeline.cpp`
- Architecture anchor: `native/objc3c/src/ARCHITECTURE.md`

## Required Evidence

- `tmp/reports/m243/M243-B002/semantic_diagnostic_taxonomy_and_fix_it_synthesis_modular_split_scaffolding_contract_summary.json`

## Determinism Criteria

- B002 scaffold closure is derived from sema pass-flow/parity and typed
  sema-to-lowering surfaces and does not bypass fail-closed gates.
- Diagnostics taxonomy accounting and ARC fix-it handoff remain computed from
  deterministic sema surface invariants.
- Frontend pipeline wiring exports scaffold state in
  `Objc3FrontendPipelineResult`.
