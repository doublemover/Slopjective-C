# M243-B004 Semantic Diagnostic Taxonomy and Fix-it Synthesis Core Feature Expansion Packet

Packet: `M243-B004`
Milestone: `M243`
Lane: `B`
Freeze date: `2026-03-03`
Dependencies: `M243-B003`

## Purpose

Freeze lane-B core feature expansion prerequisites for semantic diagnostic
taxonomy and ARC fix-it synthesis continuity so typed handoff and replay/payload
accounting stay deterministic and fail-closed, with code/spec anchors and milestone
optimization improvements as mandatory scope inputs.

## Scope Anchors

- Contract:
  `docs/contracts/m243_semantic_diagnostic_taxonomy_and_fix_it_synthesis_core_feature_expansion_b004_expectations.md`
- Checker:
  `scripts/check_m243_b004_semantic_diagnostic_taxonomy_and_fix_it_synthesis_core_feature_expansion_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m243_b004_semantic_diagnostic_taxonomy_and_fix_it_synthesis_core_feature_expansion_contract.py`
- Core feature expansion surfaces:
  - `native/objc3c/src/pipeline/objc3_semantic_diagnostic_taxonomy_and_fix_it_synthesis_core_feature_expansion_surface.h`
  - `native/objc3c/src/pipeline/objc3_frontend_types.h`
  - `native/objc3c/src/pipeline/objc3_frontend_pipeline.cpp`
- Build/readiness scripts (`package.json`):
  - `check:objc3c:m243-b004-lane-b-readiness`
- Dependency anchors from `M243-B003`:
  - `docs/contracts/m243_semantic_diagnostic_taxonomy_and_fix_it_synthesis_core_feature_implementation_b003_expectations.md`
  - `spec/planning/compiler/m243/m243_b003_semantic_diagnostic_taxonomy_and_fix_it_synthesis_core_feature_implementation_packet.md`
  - `scripts/check_m243_b003_semantic_diagnostic_taxonomy_and_fix_it_synthesis_core_feature_implementation_contract.py`
  - `tests/tooling/test_check_m243_b003_semantic_diagnostic_taxonomy_and_fix_it_synthesis_core_feature_implementation_contract.py`
- Architecture/spec anchors:
  - `native/objc3c/src/ARCHITECTURE.md`
  - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - `spec/MODULE_METADATA_AND_ABI_TABLES.md`

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `test:objc3c:sema-pass-manager-diagnostics-bus`
- `test:objc3c:lowering-regression`

## Gate Commands

- `python scripts/check_m243_b004_semantic_diagnostic_taxonomy_and_fix_it_synthesis_core_feature_expansion_contract.py`
- `python -m pytest tests/tooling/test_check_m243_b004_semantic_diagnostic_taxonomy_and_fix_it_synthesis_core_feature_expansion_contract.py -q`
- `npm run check:objc3c:m243-b004-lane-b-readiness`

## Evidence Output

- `tmp/reports/m243/M243-B004/semantic_diagnostic_taxonomy_and_fix_it_synthesis_core_feature_expansion_summary.json`
