# M243-B001 Semantic Diagnostic Taxonomy and Fix-it Synthesis Contract and Architecture Freeze Packet

Packet: `M243-B001`
Milestone: `M243`
Lane: `B`
Dependencies: none

## Scope

Freeze lane-B semantic diagnostics taxonomy and fix-it synthesis contracts before
higher-order diagnostics expansion and cross-lane integration workpacks.

## Anchors

- Contract: `docs/contracts/m243_semantic_diagnostic_taxonomy_and_fix_it_synthesis_b001_expectations.md`
- Checker: `scripts/check_m243_b001_semantic_diagnostic_taxonomy_and_fix_it_synthesis_contract.py`
- Tooling tests: `tests/tooling/test_check_m243_b001_semantic_diagnostic_taxonomy_and_fix_it_synthesis_contract.py`
- Semantic pass manager contract: `native/objc3c/src/sema/objc3_sema_pass_manager_contract.h`
- Pass-flow scaffold API: `native/objc3c/src/sema/objc3_sema_pass_flow_scaffold.h`
- Typed sema/lowering projection: `native/objc3c/src/pipeline/objc3_typed_sema_to_lowering_contract_surface.h`
- Architecture anchor: `native/objc3c/src/ARCHITECTURE.md`

## Required Evidence

- `tmp/reports/m243/M243-B001/semantic_diagnostic_taxonomy_and_fix_it_synthesis_contract_summary.json`

## Determinism Criteria

- Semantic diagnostics accounting and pass-flow monotonicity remain fail-closed.
- Diagnostics bus publication remains deterministic and explicitly wired.
- ARC diagnostics/fix-it synthesis counters and deterministic handoff signals
  remain first-class semantic contract fields.
