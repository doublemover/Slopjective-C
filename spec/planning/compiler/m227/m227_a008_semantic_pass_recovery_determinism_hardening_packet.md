# M227-A008 Semantic Pass Recovery and Determinism Hardening Packet

Packet: `M227-A008`  
Milestone: `M227`  
Lane: `A`

## Scope

Harden parser-recovery replay determinism across sema pass flow summaries, parity surfaces, and artifact projection.

## Anchors

- Contract: `docs/contracts/m227_semantic_pass_recovery_determinism_hardening_expectations.md`
- Checker: `scripts/check_m227_a008_semantic_pass_recovery_determinism_hardening_contract.py`
- Tooling tests: `tests/tooling/test_check_m227_a008_semantic_pass_recovery_determinism_hardening_contract.py`
- Sema contract surface: `native/objc3c/src/sema/objc3_sema_pass_manager_contract.h`
- Sema manager implementation: `native/objc3c/src/sema/objc3_sema_pass_manager.cpp`
- Artifact projection: `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`

## Required Evidence

- `tmp/reports/m227/M227-A008/semantic_pass_recovery_determinism_hardening_contract_summary.json`

## Determinism Criteria

- Parser recovery replay readiness and case-pass booleans are carried in pass-flow summary.
- Recovery replay contract/key determinism are required for pass-flow and parity readiness.
- Recovery replay fields are projected into frontend artifact payloads for replay auditing.
