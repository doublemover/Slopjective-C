# M228 Ownership-Aware Lowering Behavior Conformance Corpus Expansion Expectations (B010)

Contract ID: `objc3c-ownership-aware-lowering-behavior-conformance-corpus-expansion/m228-b010-v1`
Status: Accepted
Scope: ownership-aware lowering conformance-corpus expansion on top of B009 conformance-matrix implementation.

## Objective

Expand lane-B ownership-aware lowering closure with explicit conformance-corpus
consistency/readiness and conformance-corpus-key continuity so ownership
lowering remains deterministic and fail-closed before LLVM IR emission.

## Dependency Scope

- Dependencies: `M228-B009`
- M228-B009 remains a mandatory prerequisite for B010 conformance-corpus
  expansion:
  - `docs/contracts/m228_ownership_aware_lowering_behavior_conformance_matrix_implementation_b009_expectations.md`
  - `scripts/check_m228_b009_ownership_aware_lowering_behavior_conformance_matrix_implementation_contract.py`
  - `tests/tooling/test_check_m228_b009_ownership_aware_lowering_behavior_conformance_matrix_implementation_contract.py`
  - `spec/planning/compiler/m228/m228_b009_ownership_aware_lowering_behavior_conformance_matrix_implementation_packet.md`

## Deterministic Invariants

1. `Objc3OwnershipAwareLoweringBehaviorScaffold` carries conformance-corpus
   expansion fields:
   - `conformance_corpus_consistent`
   - `conformance_corpus_ready`
   - `conformance_corpus_key`
2. `BuildObjc3OwnershipAwareLoweringBehaviorConformanceCorpusKey(...)`
   remains deterministic and keyed by B009 conformance-matrix continuity plus
   parse-lowering conformance-corpus evidence.
3. `BuildObjc3OwnershipAwareLoweringBehaviorScaffold(...)` computes
   conformance-corpus fail-closed from conformance-matrix readiness and parse
   conformance-corpus consistency/case-count/key continuity.
4. `IsObjc3OwnershipAwareLoweringBehaviorConformanceCorpusReady(...)` fails
   closed when conformance-corpus consistency/readiness or conformance-corpus
   key continuity drifts.
5. `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp` enforces explicit
   fail-closed lane-B conformance-corpus gating with deterministic diagnostic
   code `O3L320`.

## Build and Readiness Integration

- `package.json` includes:
  - `check:objc3c:m228-b010-ownership-aware-lowering-behavior-conformance-corpus-expansion-contract`
  - `test:tooling:m228-b010-ownership-aware-lowering-behavior-conformance-corpus-expansion-contract`
  - `check:objc3c:m228-b010-lane-b-readiness`
- lane-B readiness chaining remains deterministic and fail-closed:
  - `check:objc3c:m228-b009-lane-b-readiness`
  - `check:objc3c:m228-b010-lane-b-readiness`

## Architecture and Spec Anchors

Shared-file deltas required for full lane-B readiness.

- `native/objc3c/src/ARCHITECTURE.md` includes M228 lane-B B010 conformance
  corpus expansion anchor text.
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md` includes M228 lane-B B010
  conformance-corpus fail-closed wiring text.
- `spec/MODULE_METADATA_AND_ABI_TABLES.md` includes deterministic lane-B B010
  conformance-corpus metadata anchors.

## Validation

- `python scripts/check_m228_b009_ownership_aware_lowering_behavior_conformance_matrix_implementation_contract.py`
- `python scripts/check_m228_b010_ownership_aware_lowering_behavior_conformance_corpus_expansion_contract.py`
- `python -m pytest tests/tooling/test_check_m228_b010_ownership_aware_lowering_behavior_conformance_corpus_expansion_contract.py -q`
- `npm run check:objc3c:m228-b010-lane-b-readiness`

## Evidence Path

- `tmp/reports/m228/M228-B010/ownership_aware_lowering_behavior_conformance_corpus_expansion_contract_summary.json`
