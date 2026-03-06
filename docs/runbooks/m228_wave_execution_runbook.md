# M228 Wave Execution Runbook

## Scope

This runbook tracks current M228 lane sync coverage for:

- `objc3c-lowering-pipeline-pass-graph-performance-quality-guardrails/m228-a011-v1`
- `objc3c-ownership-aware-lowering-behavior-diagnostics-hardening/m228-b007-v1`
- `objc3c-ir-emission-completeness-edge-case-and-compatibility-completion/m228-c005-v1`
- `objc3c-object-emission-link-path-reliability-edge-case-expansion-and-robustness/m228-d006-v1`
- `objc3c-lane-e-replay-proof-performance-closeout-gate-edge-case-expansion-and-robustness-contract/m228-e006-v1`
- `objc3c-lane-e-replay-proof-performance-closeout-gate-diagnostics-hardening-contract/m228-e007-v1`
- `objc3c-lowering-pipeline-pass-graph-cross-lane-integration-sync/m228-a012-v1`
- `objc3c-lowering-pipeline-pass-graph-docs-operator-runbook-sync/m228-a013-v1`
- `objc3c-ownership-aware-lowering-behavior-cross-lane-integration-sync/m228-b012-v1`
- `objc3c-ownership-aware-lowering-behavior-docs-operator-runbook-sync/m228-b013-v1`
- `objc3c-ownership-aware-lowering-behavior-release-candidate-and-replay-dry-run/m228-b014-v1`
- `objc3c-ownership-aware-lowering-behavior-advanced-core-workpack-shard1/m228-b015-v1`
- `objc3c-ownership-aware-lowering-behavior-advanced-edge-compatibility-workpack-shard1/m228-b016-v1`
- `objc3c-ownership-aware-lowering-behavior-advanced-diagnostics-workpack-shard1/m228-b017-v1`
- `objc3c-ownership-aware-lowering-behavior-advanced-conformance-workpack-shard1/m228-b018-v1`
- `objc3c-ownership-aware-lowering-behavior-advanced-integration-workpack-shard1/m228-b019-v1`
- `objc3c-ownership-aware-lowering-behavior-advanced-performance-workpack-shard1/m228-b020-v1`
- `objc3c-ownership-aware-lowering-behavior-advanced-core-workpack-shard2/m228-b021-v1`
- `objc3c-lowering-pipeline-pass-graph-release-replay-dry-run/m228-a014-v1`
- `objc3c-lowering-pipeline-pass-graph-advanced-core-workpack-shard1/m228-a015-v1`
- `objc3c-lowering-pipeline-pass-graph-integration-closeout-gate-signoff/m228-a016-v1`
- `objc3c-object-emission-link-path-reliability-performance-quality-guardrails/m228-d011-v1`

## Canonical Validation Sequence

1. Build native binaries:
   - `npm run build:objc3c-native`
2. Run smoke compile through invocation wrapper:
   - `powershell -NoProfile -ExecutionPolicy Bypass -File scripts/objc3c_native_compile.ps1 tests/tooling/fixtures/native/hello.objc3 --out-dir tmp/artifacts/compilation/objc3c-native/m228-wave-smoke`
3. Validate cross-lane integration sync gate:
   - `python scripts/check_m228_a012_lowering_pipeline_decomposition_pass_graph_cross_lane_integration_sync_contract.py`
4. Validate docs/operator runbook synchronization gate:
   - `python scripts/check_m228_a013_lowering_pipeline_decomposition_pass_graph_docs_operator_runbook_sync_contract.py`
   - `python -m pytest tests/tooling/test_check_m228_a013_lowering_pipeline_decomposition_pass_graph_docs_operator_runbook_sync_contract.py -q`
5. Validate release-candidate replay dry-run gate:
   - `python scripts/check_m228_a014_lowering_pipeline_decomposition_pass_graph_release_candidate_replay_dry_run_contract.py`
   - `python -m pytest tests/tooling/test_check_m228_a014_lowering_pipeline_decomposition_pass_graph_release_candidate_replay_dry_run_contract.py -q`
   - `powershell -NoProfile -ExecutionPolicy Bypass -File scripts/run_m228_a014_lowering_pipeline_decomposition_pass_graph_release_replay_dry_run.ps1`
6. Run lane-A readiness chain:
   - `npm run check:objc3c:m228-a014-lane-a-readiness`
7. Validate advanced core workpack (shard 1) gate:
   - `python scripts/check_m228_a015_lowering_pipeline_decomposition_pass_graph_advanced_core_workpack_shard1_contract.py`
   - `python -m pytest tests/tooling/test_check_m228_a015_lowering_pipeline_decomposition_pass_graph_advanced_core_workpack_shard1_contract.py -q`
8. Run lane-A readiness chain:
   - `npm run check:objc3c:m228-a015-lane-a-readiness`
9. Validate integration closeout and gate sign-off gate:
   - `python scripts/check_m228_a016_lowering_pipeline_decomposition_pass_graph_integration_closeout_and_gate_signoff_contract.py`
   - `python -m pytest tests/tooling/test_check_m228_a016_lowering_pipeline_decomposition_pass_graph_integration_closeout_and_gate_signoff_contract.py -q`
10. Run lane-A readiness chain:
   - `npm run check:objc3c:m228-a016-lane-a-readiness`
11. Validate lane-D performance/quality guardrails gate:
   - `python scripts/check_m228_d011_object_emission_link_path_reliability_performance_quality_guardrails_contract.py`
   - `python -m pytest tests/tooling/test_check_m228_d011_object_emission_link_path_reliability_performance_quality_guardrails_contract.py -q`
12. Run lane-D readiness chain:
   - `npm run check:objc3c:m228-d011-lane-d-readiness`
13. Validate lane-E diagnostics hardening closeout gate:
   - `python scripts/check_m228_e007_replay_proof_and_performance_closeout_gate_diagnostics_hardening_contract.py`
   - `python -m pytest tests/tooling/test_check_m228_e007_replay_proof_and_performance_closeout_gate_diagnostics_hardening_contract.py -q`
14. Run lane-E readiness chain:
   - `npm run check:objc3c:m228-e007-lane-e-readiness`
15. Validate lane-B cross-lane integration sync gate:
   - `python scripts/check_m228_b012_ownership_aware_lowering_behavior_cross_lane_integration_sync_contract.py`
16. Validate lane-B docs/operator runbook synchronization gate:
   - `python scripts/check_m228_b013_ownership_aware_lowering_behavior_docs_operator_runbook_sync_contract.py`
   - `python -m pytest tests/tooling/test_check_m228_b013_ownership_aware_lowering_behavior_docs_operator_runbook_sync_contract.py -q`
17. Validate lane-B release-candidate and replay dry-run gate:
   - `npm run check:objc3c:m228-b013-lane-b-readiness`
   - `python scripts/check_m228_b014_ownership_aware_lowering_behavior_release_candidate_and_replay_dry_run_contract.py`
   - `python -m pytest tests/tooling/test_check_m228_b014_ownership_aware_lowering_behavior_release_candidate_and_replay_dry_run_contract.py -q`
   - `npm run run:objc3c:m228-b014-ownership-aware-lowering-behavior-release-replay-dry-run`
18. Run lane-B readiness chain:
   - `npm run check:objc3c:m228-b014-lane-b-readiness`
19. Validate lane-B advanced core workpack (shard 1) gate:
   - `python scripts/check_m228_b015_ownership_aware_lowering_behavior_advanced_core_workpack_shard1_contract.py`
   - `python -m pytest tests/tooling/test_check_m228_b015_ownership_aware_lowering_behavior_advanced_core_workpack_shard1_contract.py -q`
20. Run lane-B readiness chain:
   - `npm run check:objc3c:m228-b015-lane-b-readiness`
21. Validate lane-B advanced edge compatibility workpack (shard 1) gate:
   - `python scripts/check_m228_b016_ownership_aware_lowering_behavior_advanced_edge_compatibility_workpack_shard1_contract.py`
   - `python -m pytest tests/tooling/test_check_m228_b016_ownership_aware_lowering_behavior_advanced_edge_compatibility_workpack_shard1_contract.py -q`
22. Run lane-B readiness chain:
   - `npm run check:objc3c:m228-b016-lane-b-readiness`
23. Validate lane-B advanced diagnostics workpack (shard 1) gate:
   - `python scripts/check_m228_b017_ownership_aware_lowering_behavior_advanced_diagnostics_workpack_shard1_contract.py`
   - `python -m pytest tests/tooling/test_check_m228_b017_ownership_aware_lowering_behavior_advanced_diagnostics_workpack_shard1_contract.py -q`
24. Run lane-B readiness chain:
   - `npm run check:objc3c:m228-b017-lane-b-readiness`
25. Validate lane-B advanced conformance workpack (shard 1) gate:
   - `python scripts/check_m228_b018_ownership_aware_lowering_behavior_advanced_conformance_workpack_shard1_contract.py`
   - `python -m pytest tests/tooling/test_check_m228_b018_ownership_aware_lowering_behavior_advanced_conformance_workpack_shard1_contract.py -q`
26. Run lane-B readiness chain:
   - `npm run check:objc3c:m228-b018-lane-b-readiness`
27. Validate lane-B advanced integration workpack (shard 1) gate:
   - `python scripts/check_m228_b019_ownership_aware_lowering_behavior_advanced_integration_workpack_shard1_contract.py`
   - `python -m pytest tests/tooling/test_check_m228_b019_ownership_aware_lowering_behavior_advanced_integration_workpack_shard1_contract.py -q`
28. Run lane-B readiness chain:
   - `npm run check:objc3c:m228-b019-lane-b-readiness`
29. Validate lane-B advanced performance workpack (shard 1) gate:
   - `python scripts/check_m228_b020_ownership_aware_lowering_behavior_advanced_performance_workpack_shard1_contract.py`
   - `python -m pytest tests/tooling/test_check_m228_b020_ownership_aware_lowering_behavior_advanced_performance_workpack_shard1_contract.py -q`
30. Run lane-B readiness chain:
   - `npm run check:objc3c:m228-b020-lane-b-readiness`
31. Validate lane-B advanced core workpack (shard 2) gate:
   - `python scripts/check_m228_b021_ownership_aware_lowering_behavior_advanced_core_workpack_shard2_contract.py`
   - `python -m pytest tests/tooling/test_check_m228_b021_ownership_aware_lowering_behavior_advanced_core_workpack_shard2_contract.py -q`
32. Run lane-B readiness chain:
   - `npm run check:objc3c:m228-b021-lane-b-readiness`
33. Optional milestone optimization replay/compile sweeps:
   - `npm run compile:objc3c -- tests/tooling/fixtures/native/hello.objc3 -- --out-dir tmp/artifacts/compilation/objc3c-native/m228-b014 --emit-prefix module`
   - `npm run proof:objc3c`
   - `npm run test:objc3c:execution-smoke`
   - `npm run test:objc3c:lowering-replay-proof`

## Evidence

Persist wave evidence under:

- `tmp/reports/m228/`
