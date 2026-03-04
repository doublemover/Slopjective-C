# M248-B014 Semantic/Lowering Test Architecture Release-Candidate and Replay Dry-Run Packet

Packet: `M248-B014`
Milestone: `M248`
Lane: `B`
Freeze date: `2026-03-03`
Issue: `#6814`
Dependencies: `M248-B013`

## Purpose

Freeze lane-B semantic/lowering test architecture release-candidate and replay
dry-run prerequisites so B013 dependency continuity and release-replay closure
stay explicit, deterministic, and fail-closed, including code/spec anchors and
milestone optimization improvements as mandatory scope inputs.
This packet keeps code/spec anchors and milestone optimization improvements as mandatory scope inputs.

## Scope Anchors

- Contract:
  `docs/contracts/m248_semantic_lowering_test_architecture_release_candidate_and_replay_dry_run_b014_expectations.md`
- Checker:
  `scripts/check_m248_b014_semantic_lowering_test_architecture_release_candidate_and_replay_dry_run_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m248_b014_semantic_lowering_test_architecture_release_candidate_and_replay_dry_run_contract.py`
- Dry-run runner:
  `scripts/run_m248_b014_semantic_lowering_test_architecture_release_replay_dry_run.ps1`
- Dependency anchors from `M248-B013`:
  - `docs/contracts/m248_semantic_lowering_test_architecture_docs_and_operator_runbook_synchronization_b013_expectations.md`
  - `spec/planning/compiler/m248/m248_b013_semantic_lowering_test_architecture_docs_and_operator_runbook_synchronization_packet.md`
  - `scripts/check_m248_b013_semantic_lowering_test_architecture_docs_and_operator_runbook_synchronization_contract.py`
  - `tests/tooling/test_check_m248_b013_semantic_lowering_test_architecture_docs_and_operator_runbook_synchronization_contract.py`
  - `scripts/run_m248_b013_lane_b_readiness.py`
- Canonical readiness command names:
  - `check:objc3c:m248-b014-semantic-lowering-test-architecture-release-candidate-and-replay-dry-run-contract`
  - `test:tooling:m248-b014-semantic-lowering-test-architecture-release-candidate-and-replay-dry-run-contract`
  - `run:objc3c:m248-b014-semantic-lowering-test-architecture-release-replay-dry-run`
  - `check:objc3c:m248-b014-lane-b-readiness`
  - `check:objc3c:m248-b013-lane-b-readiness`
- Architecture/spec anchors:
  - `native/objc3c/src/ARCHITECTURE.md`
  - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - `spec/MODULE_METADATA_AND_ABI_TABLES.md`

## Deterministic Replay and Readiness Keys

- Deterministic replay artifacts:
  - `module.manifest.json`
  - `module.diagnostics.json`
  - `module.ll`
  - `module.object-backend.txt`
- Lane-B sema/lowering readiness keys:
  - `toolchain_runtime_ga_operations_docs_runbook_sync_consistent`
  - `toolchain_runtime_ga_operations_docs_runbook_sync_ready`
  - `long_tail_grammar_integration_closeout_consistent`
  - `long_tail_grammar_gate_signoff_ready`
  - `long_tail_grammar_integration_closeout_key`

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `test:objc3c:sema-pass-manager-diagnostics-bus`
- `test:objc3c:lowering-regression`

## Gate Commands

- `python scripts/check_m248_b013_semantic_lowering_test_architecture_docs_and_operator_runbook_synchronization_contract.py`
- `python scripts/check_m248_b014_semantic_lowering_test_architecture_release_candidate_and_replay_dry_run_contract.py --emit-json`
- `python -m pytest tests/tooling/test_check_m248_b014_semantic_lowering_test_architecture_release_candidate_and_replay_dry_run_contract.py -q`
- `powershell -NoProfile -ExecutionPolicy Bypass -File scripts/run_m248_b014_semantic_lowering_test_architecture_release_replay_dry_run.ps1`
- `npm run check:objc3c:m248-b014-lane-b-readiness`

## Evidence Output

- `tmp/reports/m248/M248-B014/replay_dry_run_summary.json`
- `tmp/reports/m248/M248-B014/semantic_lowering_test_architecture_release_candidate_and_replay_dry_run_contract_summary.json`
