# M227 Wave Execution Runbook

## Contract Anchors

- `objc3c-semantic-pass-performance-quality-guardrails/m227-a011-v1`
- `objc3c-type-system-objc3-forms-diagnostics-hardening/m227-b007-v1`
- `objc3c-typed-sema-to-lowering-modular-split-scaffold/m227-c002-v1`
- `objc3c-runtime-facing-type-metadata-semantics-contract/m227-d001-v1`
- `objc3c-lane-e-semantic-conformance-quality-gate-contract/m227-e001-v1`
- `objc3c-semantic-pass-cross-lane-integration-sync/m227-a012-v1`
- `objc3c-semantic-pass-docs-operator-runbook-sync/m227-a013-v1`
- `objc3c-semantic-pass-release-replay-dry-run/m227-a014-v1`
- `objc3c-semantic-pass-advanced-core-workpack-shard1/m227-a015-v1`
- `objc3c-semantic-pass-advanced-edge-compatibility-workpack-shard1/m227-a016-v1`
- `objc3c-semantic-pass-advanced-diagnostics-workpack-shard1/m227-a017-v1`
- `objc3c-semantic-pass-advanced-conformance-workpack-shard1/m227-a018-v1`
- `objc3c-semantic-pass-advanced-integration-workpack-shard1/m227-a019-v1`
- `objc3c-semantic-pass-advanced-performance-workpack-shard1/m227-a020-v1`
- `objc3c-semantic-pass-integration-closeout-and-gate-signoff/m227-a021-v1`

## Operator Command Sequence

1. `npm run build:objc3c-native`
2. `npm run test:objc3c:execution-smoke`
3. `npm run test:objc3c:execution-replay-proof`
4. `python scripts/check_m227_a012_semantic_pass_cross_lane_integration_sync_contract.py`
5. `python scripts/check_m227_a013_semantic_pass_docs_operator_runbook_sync_contract.py`
6. `python -m pytest tests/tooling/test_check_m227_a013_semantic_pass_docs_operator_runbook_sync_contract.py -q`
7. `npm run check:objc3c:m227-a013-lane-a-readiness`
8. `python scripts/check_m227_a014_semantic_pass_release_candidate_replay_dry_run_contract.py`
9. `python -m pytest tests/tooling/test_check_m227_a014_semantic_pass_release_candidate_replay_dry_run_contract.py -q`
10. `powershell -NoProfile -ExecutionPolicy Bypass -File scripts/run_m227_a014_semantic_pass_release_replay_dry_run.ps1`
11. `npm run check:objc3c:m227-a014-lane-a-readiness`
12. `npm run check:objc3c:m227-a014-milestone-optimization-replay-proof`
13. `python scripts/check_m227_a015_semantic_pass_advanced_core_workpack_shard1_contract.py`
14. `python -m pytest tests/tooling/test_check_m227_a015_semantic_pass_advanced_core_workpack_shard1_contract.py -q`
15. `npm run check:objc3c:m227-a015-lane-a-readiness`
16. `python scripts/check_m227_a016_semantic_pass_advanced_edge_compatibility_workpack_shard1_contract.py`
17. `python -m pytest tests/tooling/test_check_m227_a016_semantic_pass_advanced_edge_compatibility_workpack_shard1_contract.py -q`
18. `npm run check:objc3c:m227-a016-lane-a-readiness`
19. `python scripts/check_m227_a017_semantic_pass_advanced_diagnostics_workpack_shard1_contract.py`
20. `python -m pytest tests/tooling/test_check_m227_a017_semantic_pass_advanced_diagnostics_workpack_shard1_contract.py -q`
21. `npm run check:objc3c:m227-a017-lane-a-readiness`
22. `python scripts/check_m227_a018_semantic_pass_advanced_conformance_workpack_shard1_contract.py`
23. `python -m pytest tests/tooling/test_check_m227_a018_semantic_pass_advanced_conformance_workpack_shard1_contract.py -q`
24. `npm run check:objc3c:m227-a018-lane-a-readiness`
25. `python scripts/check_m227_a019_semantic_pass_advanced_integration_workpack_shard1_contract.py`
26. `python -m pytest tests/tooling/test_check_m227_a019_semantic_pass_advanced_integration_workpack_shard1_contract.py -q`
27. `npm run check:objc3c:m227-a019-lane-a-readiness`
28. `python scripts/check_m227_a020_semantic_pass_advanced_performance_workpack_shard1_contract.py`
29. `python -m pytest tests/tooling/test_check_m227_a020_semantic_pass_advanced_performance_workpack_shard1_contract.py -q`
30. `npm run check:objc3c:m227-a020-lane-a-readiness`
31. `python scripts/check_m227_a021_semantic_pass_integration_closeout_and_gate_signoff_contract.py`
32. `python -m pytest tests/tooling/test_check_m227_a021_semantic_pass_integration_closeout_and_gate_signoff_contract.py -q`
33. `npm run check:objc3c:m227-a021-lane-a-readiness`

## Evidence

- `tmp/reports/m227/`
