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
- `objc3c-ir-emission-completeness-cross-lane-integration-sync/m228-c012-v1`
- `objc3c-ir-emission-completeness-docs-operator-runbook-sync/m228-c013-v1`
- `objc3c-ir-emission-completeness-release-candidate-and-replay-dry-run/m228-c014-v1`
- `objc3c-ir-emission-completeness-advanced-core-workpack-shard1/m228-c015-v1`
- `objc3c-ir-emission-completeness-advanced-edge-compatibility-workpack-shard1/m228-c016-v1`
- `objc3c-ir-emission-completeness-advanced-diagnostics-workpack-shard1/m228-c017-v1`
- `objc3c-ir-emission-completeness-advanced-conformance-workpack-shard1/m228-c018-v1`
- `objc3c-ir-emission-completeness-advanced-integration-workpack-shard1/m228-c019-v1`
- `objc3c-ownership-aware-lowering-behavior-release-candidate-and-replay-dry-run/m228-b014-v1`
- `objc3c-ownership-aware-lowering-behavior-advanced-core-workpack-shard1/m228-b015-v1`
- `objc3c-ownership-aware-lowering-behavior-advanced-edge-compatibility-workpack-shard1/m228-b016-v1`
- `objc3c-ownership-aware-lowering-behavior-advanced-diagnostics-workpack-shard1/m228-b017-v1`
- `objc3c-ownership-aware-lowering-behavior-advanced-conformance-workpack-shard1/m228-b018-v1`
- `objc3c-ownership-aware-lowering-behavior-advanced-integration-workpack-shard1/m228-b019-v1`
- `objc3c-ownership-aware-lowering-behavior-advanced-performance-workpack-shard1/m228-b020-v1`
- `objc3c-ownership-aware-lowering-behavior-advanced-core-workpack-shard2/m228-b021-v1`
- `objc3c-ownership-aware-lowering-behavior-integration-closeout-and-gate-signoff/m228-b022-v1`
- `objc3c-lowering-pipeline-pass-graph-release-replay-dry-run/m228-a014-v1`
- `objc3c-lowering-pipeline-pass-graph-advanced-core-workpack-shard1/m228-a015-v1`
- `objc3c-lowering-pipeline-pass-graph-integration-closeout-gate-signoff/m228-a016-v1`
- `objc3c-object-emission-link-path-reliability-performance-quality-guardrails/m228-d011-v1`
- `objc3c-ir-emission-completeness-advanced-performance-workpack-shard1/m228-c020-v1`
- `objc3c-ir-emission-completeness-advanced-core-workpack-shard2/m228-c021-v1`
- `objc3c-ir-emission-completeness-advanced-edge-compatibility-workpack-shard2/m228-c022-v1`
- `objc3c-ir-emission-completeness-advanced-diagnostics-workpack-shard2/m228-c023-v1`
- `objc3c-ir-emission-completeness-advanced-conformance-workpack-shard2/m228-c024-v1`
- `objc3c-ir-emission-completeness-advanced-integration-workpack-shard2/m228-c025-v1`
- `objc3c-ir-emission-completeness-advanced-performance-workpack-shard2/m228-c026-v1`
- `objc3c-ir-emission-completeness-advanced-core-workpack-shard3/m228-c027-v1`
- `objc3c-ir-emission-completeness-advanced-edge-compatibility-workpack-shard3/m228-c028-v1`
- `objc3c-ir-emission-completeness-advanced-diagnostics-workpack-shard3/m228-c029-v1`
- `objc3c-ir-emission-completeness-advanced-conformance-workpack-shard3/m228-c030-v1`
- `objc3c-ir-emission-completeness-advanced-integration-workpack-shard3/m228-c031-v1`
- `objc3c-ir-emission-completeness-advanced-performance-workpack-shard3/m228-c032-v1`
- `objc3c-ir-emission-completeness-advanced-core-workpack-shard4/m228-c033-v1`
- `objc3c-ir-emission-completeness-advanced-edge-compatibility-workpack-shard4/m228-c034-v1`
- `objc3c-ir-emission-completeness-advanced-diagnostics-workpack-shard4/m228-c035-v1`
- `objc3c-ir-emission-completeness-advanced-conformance-workpack-shard4/m228-c036-v1`
- `objc3c-ir-emission-completeness-advanced-integration-workpack-shard4/m228-c037-v1`
- `objc3c-ir-emission-completeness-integration-closeout-and-gate-signoff/m228-c038-v1`

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
17. Validate lane-C cross-lane integration sync gate:
   - `python scripts/check_m228_c012_ir_emission_completeness_cross_lane_integration_sync_contract.py`
18. Validate lane-C docs/operator runbook synchronization gate:
   - `python scripts/check_m228_c013_ir_emission_completeness_docs_operator_runbook_sync_contract.py`
   - `python -m pytest tests/tooling/test_check_m228_c013_ir_emission_completeness_docs_operator_runbook_sync_contract.py -q`
   - `npm run check:objc3c:m228-c013-lane-c-readiness`
19. Validate lane-C release-candidate and replay dry-run gate:
   - `python scripts/check_m228_c014_ir_emission_completeness_release_candidate_and_replay_dry_run_contract.py`
   - `python -m pytest tests/tooling/test_check_m228_c014_ir_emission_completeness_release_candidate_and_replay_dry_run_contract.py -q`
   - `npm run run:objc3c:m228-c014-ir-emission-completeness-release-replay-dry-run`
20. Run lane-C readiness chain:
   - `npm run check:objc3c:m228-c014-lane-c-readiness`
21. Validate lane-C advanced core workpack (shard 1) gate:
   - `python scripts/check_m228_c015_ir_emission_completeness_advanced_core_workpack_shard1_contract.py`
   - `python -m pytest tests/tooling/test_check_m228_c015_ir_emission_completeness_advanced_core_workpack_shard1_contract.py -q`
22. Run lane-C readiness chain:
   - `npm run check:objc3c:m228-c015-lane-c-readiness`
23. Validate lane-C advanced edge compatibility workpack (shard 1) gate:
   - `python scripts/check_m228_c016_ir_emission_completeness_advanced_edge_compatibility_workpack_shard1_contract.py`
   - `python -m pytest tests/tooling/test_check_m228_c016_ir_emission_completeness_advanced_edge_compatibility_workpack_shard1_contract.py -q`
24. Run lane-C readiness chain:
   - `npm run check:objc3c:m228-c016-lane-c-readiness`
25. Validate lane-C advanced diagnostics workpack (shard 1) gate:
   - `python scripts/check_m228_c017_ir_emission_completeness_advanced_diagnostics_workpack_shard1_contract.py`
   - `python -m pytest tests/tooling/test_check_m228_c017_ir_emission_completeness_advanced_diagnostics_workpack_shard1_contract.py -q`
26. Run lane-C readiness chain:
   - `npm run check:objc3c:m228-c017-lane-c-readiness`
27. Validate lane-C advanced conformance workpack (shard 1) gate:
   - `python scripts/check_m228_c018_ir_emission_completeness_advanced_conformance_workpack_shard1_contract.py`
   - `python -m pytest tests/tooling/test_check_m228_c018_ir_emission_completeness_advanced_conformance_workpack_shard1_contract.py -q`
28. Run lane-C readiness chain:
   - `npm run check:objc3c:m228-c018-lane-c-readiness`
29. Validate lane-C advanced integration workpack (shard 1) gate:
   - `python scripts/check_m228_c019_ir_emission_completeness_advanced_integration_workpack_shard1_contract.py`
   - `python -m pytest tests/tooling/test_check_m228_c019_ir_emission_completeness_advanced_integration_workpack_shard1_contract.py -q`
30. Run lane-C readiness chain:
   - `npm run check:objc3c:m228-c019-lane-c-readiness`
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
33. Validate lane-B integration closeout and gate sign-off gate:
   - `python scripts/check_m228_b022_ownership_aware_lowering_behavior_integration_closeout_and_gate_signoff_contract.py`
   - `python -m pytest tests/tooling/test_check_m228_b022_ownership_aware_lowering_behavior_integration_closeout_and_gate_signoff_contract.py -q`
34. Run lane-B readiness chain:
   - `npm run check:objc3c:m228-b022-lane-b-readiness`
35. Optional milestone optimization replay/compile sweeps:
   - `npm run compile:objc3c -- tests/tooling/fixtures/native/hello.objc3 -- --out-dir tmp/artifacts/compilation/objc3c-native/m228-b014 --emit-prefix module`
   - `npm run proof:objc3c`
   - `npm run test:objc3c:execution-smoke`
   - `npm run test:objc3c:lowering-replay-proof`

## Evidence

Persist wave evidence under:

- `tmp/reports/m228/`
36. `python scripts/check_m228_c020_ir_emission_completeness_advanced_performance_workpack_shard1_contract.py`
37. `python -m pytest tests/tooling/test_check_m228_c020_ir_emission_completeness_advanced_performance_workpack_shard1_contract.py -q`
38. `npm run check:objc3c:m228-c020-lane-c-readiness`
39. `python scripts/check_m228_c021_ir_emission_completeness_advanced_core_workpack_shard2_contract.py`
40. `python -m pytest tests/tooling/test_check_m228_c021_ir_emission_completeness_advanced_core_workpack_shard2_contract.py -q`
41. `npm run check:objc3c:m228-c021-lane-c-readiness`
42. `python scripts/check_m228_c022_ir_emission_completeness_advanced_edge_compatibility_workpack_shard2_contract.py`
43. `python -m pytest tests/tooling/test_check_m228_c022_ir_emission_completeness_advanced_edge_compatibility_workpack_shard2_contract.py -q`
44. `npm run check:objc3c:m228-c022-lane-c-readiness`
45. `python scripts/check_m228_c023_ir_emission_completeness_advanced_diagnostics_workpack_shard2_contract.py`
46. `python -m pytest tests/tooling/test_check_m228_c023_ir_emission_completeness_advanced_diagnostics_workpack_shard2_contract.py -q`
47. `npm run check:objc3c:m228-c023-lane-c-readiness`
48. `python scripts/check_m228_c024_ir_emission_completeness_advanced_conformance_workpack_shard2_contract.py`
49. `python -m pytest tests/tooling/test_check_m228_c024_ir_emission_completeness_advanced_conformance_workpack_shard2_contract.py -q`
50. `npm run check:objc3c:m228-c024-lane-c-readiness`
51. `python scripts/check_m228_c025_ir_emission_completeness_advanced_integration_workpack_shard2_contract.py`
52. `python -m pytest tests/tooling/test_check_m228_c025_ir_emission_completeness_advanced_integration_workpack_shard2_contract.py -q`
53. `npm run check:objc3c:m228-c025-lane-c-readiness`
54. `python scripts/check_m228_c026_ir_emission_completeness_advanced_performance_workpack_shard2_contract.py`
55. `python -m pytest tests/tooling/test_check_m228_c026_ir_emission_completeness_advanced_performance_workpack_shard2_contract.py -q`
56. `npm run check:objc3c:m228-c026-lane-c-readiness`
57. `python scripts/check_m228_c027_ir_emission_completeness_advanced_core_workpack_shard3_contract.py`
58. `python -m pytest tests/tooling/test_check_m228_c027_ir_emission_completeness_advanced_core_workpack_shard3_contract.py -q`
59. `npm run check:objc3c:m228-c027-lane-c-readiness`
60. `python scripts/check_m228_c028_ir_emission_completeness_advanced_edge_compatibility_workpack_shard3_contract.py`
61. `python -m pytest tests/tooling/test_check_m228_c028_ir_emission_completeness_advanced_edge_compatibility_workpack_shard3_contract.py -q`
62. `npm run check:objc3c:m228-c028-lane-c-readiness`
63. `python scripts/check_m228_c029_ir_emission_completeness_advanced_diagnostics_workpack_shard3_contract.py`
64. `python -m pytest tests/tooling/test_check_m228_c029_ir_emission_completeness_advanced_diagnostics_workpack_shard3_contract.py -q`
65. `npm run check:objc3c:m228-c029-lane-c-readiness`
66. `python scripts/check_m228_c030_ir_emission_completeness_advanced_conformance_workpack_shard3_contract.py`
67. `python -m pytest tests/tooling/test_check_m228_c030_ir_emission_completeness_advanced_conformance_workpack_shard3_contract.py -q`
68. `npm run check:objc3c:m228-c030-lane-c-readiness`
69. `python scripts/check_m228_c031_ir_emission_completeness_advanced_integration_workpack_shard3_contract.py`
70. `python -m pytest tests/tooling/test_check_m228_c031_ir_emission_completeness_advanced_integration_workpack_shard3_contract.py -q`
71. `npm run check:objc3c:m228-c031-lane-c-readiness`
72. `python scripts/check_m228_c032_ir_emission_completeness_advanced_performance_workpack_shard3_contract.py`
73. `python -m pytest tests/tooling/test_check_m228_c032_ir_emission_completeness_advanced_performance_workpack_shard3_contract.py -q`
74. `npm run check:objc3c:m228-c032-lane-c-readiness`
75. `python scripts/check_m228_c033_ir_emission_completeness_advanced_core_workpack_shard4_contract.py`
76. `python -m pytest tests/tooling/test_check_m228_c033_ir_emission_completeness_advanced_core_workpack_shard4_contract.py -q`
77. `npm run check:objc3c:m228-c033-lane-c-readiness`
78. `python scripts/check_m228_c034_ir_emission_completeness_advanced_edge_compatibility_workpack_shard4_contract.py`
79. `python -m pytest tests/tooling/test_check_m228_c034_ir_emission_completeness_advanced_edge_compatibility_workpack_shard4_contract.py -q`
80. `npm run check:objc3c:m228-c034-lane-c-readiness`
81. `python scripts/check_m228_c035_ir_emission_completeness_advanced_diagnostics_workpack_shard4_contract.py`
82. `python -m pytest tests/tooling/test_check_m228_c035_ir_emission_completeness_advanced_diagnostics_workpack_shard4_contract.py -q`
83. `npm run check:objc3c:m228-c035-lane-c-readiness`
84. `python scripts/check_m228_c036_ir_emission_completeness_advanced_conformance_workpack_shard4_contract.py`
85. `python -m pytest tests/tooling/test_check_m228_c036_ir_emission_completeness_advanced_conformance_workpack_shard4_contract.py -q`
86. `npm run check:objc3c:m228-c036-lane-c-readiness`
87. `python scripts/check_m228_c037_ir_emission_completeness_advanced_integration_workpack_shard4_contract.py`
88. `python -m pytest tests/tooling/test_check_m228_c037_ir_emission_completeness_advanced_integration_workpack_shard4_contract.py -q`
89. `npm run check:objc3c:m228-c037-lane-c-readiness`
90. `python scripts/check_m228_c038_ir_emission_completeness_integration_closeout_and_gate_signoff_contract.py`
91. `python -m pytest tests/tooling/test_check_m228_c038_ir_emission_completeness_integration_closeout_and_gate_signoff_contract.py -q`
92. `npm run check:objc3c:m228-c038-lane-c-readiness`





objc3c-object-emission-link-path-reliability-cross-lane-integration-sync/m228-d012-v1

npm run check:objc3c:m228-d012-lane-d-readiness


objc3c-object-emission-link-path-reliability-docs-operator-runbook-sync/m228-d013-v1

npm run check:objc3c:m228-d013-lane-d-readiness


objc3c-object-emission-link-path-reliability-release-candidate-and-replay-dry-run/m228-d014-v1

npm run check:objc3c:m228-d014-lane-d-readiness


objc3c-object-emission-link-path-reliability-advanced-core-workpack-shard1/m228-d015-v1

npm run check:objc3c:m228-d015-lane-d-readiness


objc3c-object-emission-link-path-reliability-integration-closeout-and-gate-signoff/m228-d016-v1

npm run check:objc3c:m228-d016-lane-d-readiness
