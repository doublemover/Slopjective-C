# objc3c Native Frontend Architecture Contract

Status: Accepted (M132-A001)
Scope: `native/objc3c/src/*`

This document defines dependency rules for the modular native frontend.

## Layer Model

The frontend is split into five layers plus integration adapters:

1. `lex/*`
2. `parse/*`
3. `sema/*`
4. `lower/*`
5. `ir/*`

Adapter and integration modules:

- `pipeline/*`: stage orchestration, contracts, stage result passing.
- `libobjc3c_frontend/*`: public embedding API over `pipeline/*`.
- `driver/*`: CLI argument parsing and exit-code mapping only.
- `io/*`: filesystem/process adapters for manifests, artifacts, and tool calls.

Current-state note:

- `main.cpp` is now driver-only and delegates to `driver/objc3_driver_main.h`.
- Parser/sema boundaries are enforced through explicit contracts in
  `parse/objc3_parser_contract.h`, `parse/objc3_ast_builder_contract.h`, and
  `sema/objc3_sema_contract.h`.
- M226 architecture-freeze work builds on this extracted layout and hardens
  parser completeness and parser-to-sema handoff determinism.
- M227 extends the sema boundary with pass-order and symbol-flow freeze rules
  in `sema/objc3_sema_pass_manager_contract.h` (`Objc3SemaPassFlowSummary`).
- M227 lane-B type-system freeze anchors canonical ObjC form sets in
  `sema/objc3_sema_contract.h` (`kObjc3CanonicalReferenceTypeForms`,
  `kObjc3CanonicalScalarMessageSendTypeForms`,
  `kObjc3CanonicalBridgeTopReferenceTypeForms`) to keep semantic checking
  deterministic across `id`/`Class`/`SEL`/`Protocol`/`instancetype` and
  object-pointer forms.
- M228 lane-A A001 lowering pipeline decomposition/pass-graph freeze anchors
  canonical stage-order and fail-closed lowering entrypoints in
  `pipeline/frontend_pipeline_contract.h`,
  `pipeline/objc3_frontend_pipeline.cpp`,
  `pipeline/objc3_frontend_artifacts.cpp`, and
  `lower/objc3_lowering_contract.cpp` so direct LLVM IR emission hardening can
  build on deterministic decomposition boundaries.
- M228 lane-A A002 modular split scaffolding extracts pass-graph readiness
  synthesis into `pipeline/objc3_lowering_pipeline_pass_graph_scaffold.cpp`
  and enforces fail-closed pass-graph gating in
  `pipeline/objc3_frontend_artifacts.cpp` before IR emission so lex/parse/sema,
  lowering-boundary normalization, and runtime dispatch declaration contracts
  remain deterministically synchronized.
- M228 lane-A A003 core feature implementation layers
  `pipeline/objc3_lowering_pipeline_pass_graph_core_feature_surface.cpp` over
  the A002 scaffold and propagates core-feature replay evidence into
  `ir/objc3_ir_emitter.h`/`ir/objc3_ir_emitter.cpp` metadata output so direct
  LLVM IR emission can fail closed on pass-graph core-feature drift.
- M228 lane-A A004 core feature expansion extends
  `pipeline/objc3_lowering_pipeline_pass_graph_core_feature_surface.cpp` with
  deterministic expansion readiness/key synthesis and enforces an additional
  fail-closed artifact gate in `pipeline/objc3_frontend_artifacts.cpp` so
  pass-graph expansion drift cannot bypass direct IR emission hardening.
- M228 lane-A A005 edge-case compatibility completion extends
  `pipeline/objc3_lowering_pipeline_pass_graph_core_feature_surface.cpp` with
  compatibility-handoff and language-version/pragma coordinate ordering gates,
  and enforces fail-closed compatibility gating in
  `pipeline/objc3_frontend_artifacts.cpp` before IR emission.
- M228 lane-A A006 edge-case expansion and robustness extends
  `pipeline/objc3_lowering_pipeline_pass_graph_core_feature_surface.cpp` with
  deterministic edge-case expansion/robustness guardrails
  (`edge_case_expansion_consistent`, `edge_case_robustness_*`) and enforces
  fail-closed robustness gating in `pipeline/objc3_frontend_artifacts.cpp`
  before IR emission.
- M228 lane-A A007 diagnostics hardening extends
  `pipeline/objc3_lowering_pipeline_pass_graph_core_feature_surface.cpp` with
  deterministic diagnostics hardening guardrails
  (`diagnostics_hardening_consistent`, `diagnostics_hardening_*`) and enforces
  fail-closed diagnostics-hardening gating in
  `pipeline/objc3_frontend_artifacts.cpp` before IR emission.
- M228 lane-A A008 recovery and determinism hardening extends
  `pipeline/objc3_lowering_pipeline_pass_graph_core_feature_surface.cpp` with
  deterministic recovery and determinism guardrails
  (`recovery_determinism_consistent`, `recovery_determinism_*`) and enforces
  fail-closed recovery-determinism gating in
  `pipeline/objc3_frontend_artifacts.cpp` before IR emission.
- M228 lane-A A009 conformance matrix implementation extends
  `pipeline/objc3_lowering_pipeline_pass_graph_core_feature_surface.cpp` with
  deterministic conformance-matrix guardrails
  (`conformance_matrix_consistent`, `conformance_matrix_*`) and enforces
  fail-closed conformance-matrix gating in
  `pipeline/objc3_frontend_artifacts.cpp` before IR emission.
- M228 lane-A A010 conformance corpus expansion extends
  `pipeline/objc3_lowering_pipeline_pass_graph_core_feature_surface.cpp` with
  deterministic conformance-corpus guardrails
  (`conformance_corpus_consistent`, `conformance_corpus_*`) and enforces
  fail-closed conformance-corpus gating in
  `pipeline/objc3_frontend_artifacts.cpp` before IR emission.
- M228 lane-A A011 performance and quality guardrails extends
  `pipeline/objc3_lowering_pipeline_pass_graph_core_feature_surface.cpp` with
  deterministic performance-quality guardrails
  (`performance_quality_guardrails_consistent`,
  `performance_quality_guardrails_*`) and enforces fail-closed
  performance-quality guardrails gating in
  `pipeline/objc3_frontend_artifacts.cpp` before IR emission.
- M228 lane-A A012 cross-lane integration sync anchors deterministic
  lane-contract continuity (`A011`, `B007`, `C005`, `D006`, `E006`) through
  contract/spec tooling so lane-A integration evidence fails closed when
  cross-lane contract anchors drift.
- M228 lane-B B001 ownership-aware lowering behavior freeze anchors
  ownership qualifier, retain/release, autoreleasepool, and ARC diagnostics
  replay surfaces in `lower/objc3_lowering_contract.h`,
  `lower/objc3_lowering_contract.cpp`, and
  `pipeline/objc3_frontend_artifacts.cpp` so ownership-lowering routing remains
  deterministic and fail-closed before modular split work.
- M228 lane-B B002 modular split scaffolding extracts ownership-aware lowering
  scaffold synthesis into
  `pipeline/objc3_ownership_aware_lowering_behavior_scaffold.h` and enforces a
  fail-closed scaffold gate in `pipeline/objc3_frontend_artifacts.cpp` so
  ownership qualifier, retain/release, autoreleasepool, and ARC diagnostics
  replay surfaces remain deterministic through lane-B modular split hardening.
- M228 lane-B B003 core feature implementation anchors ownership-aware lowering
  closure in `pipeline/objc3_ownership_aware_lowering_behavior_scaffold.h` and
  `pipeline/objc3_frontend_artifacts.cpp` so direct LLVM IR emission fails
  closed when ownership-lowering readiness, replay-key determinism, or
  milestone optimization improvements drift.
- M228 lane-B B004 core feature expansion extends
  `pipeline/objc3_ownership_aware_lowering_behavior_scaffold.h`,
  `pipeline/objc3_frontend_artifacts.cpp`, `ir/objc3_ir_emitter.h`, and
  `ir/objc3_ir_emitter.cpp` with deterministic expansion readiness/key
  synthesis so ownership-aware lowering fails closed when weak/unowned expansion
  accounting or replay-proof key transport drifts.
- M228 lane-B B005 edge-case and compatibility completion extends
  `pipeline/objc3_ownership_aware_lowering_behavior_scaffold.h` and
  `pipeline/objc3_frontend_artifacts.cpp` with deterministic edge-case
  compatibility consistency/readiness and compatibility-key transport so
  ownership-aware lowering fails closed on compatibility drift.
- M228 lane-B B006 edge-case expansion and robustness extends
  `pipeline/objc3_ownership_aware_lowering_behavior_scaffold.h` with
  deterministic robustness expansion/readiness and robustness-key transport so
  ownership-aware lowering compatibility gating remains fail-closed on
  robustness drift before IR emission.
- M228 lane-B B007 diagnostics hardening extends
  `pipeline/objc3_ownership_aware_lowering_behavior_scaffold.h` with
  deterministic diagnostics consistency/readiness and diagnostics-key transport
  so ownership-aware lowering compatibility gating remains fail-closed on
  diagnostics hardening drift before IR emission.
- M228 lane-C C001 IR emission completeness freeze anchors direct IR metadata
  and fail-closed emission routing in `ir/objc3_ir_emitter.h`,
  `ir/objc3_ir_emitter.cpp`, and `pipeline/objc3_frontend_artifacts.cpp` so
  ObjC pattern lowering evidence remains deterministic before lane-C modular
  split work.
- M228 lane-C C002 modular split scaffolding anchors IR emission completeness
  closure in `pipeline/objc3_ir_emission_completeness_scaffold.cpp` and
  `pipeline/objc3_ir_emission_completeness_scaffold.h` so pass-graph
  core/expansion/edge compatibility evidence can be transported through a
  deterministic fail-closed metadata scaffold.
- M228 lane-C C003 core feature implementation anchors IR emission
  core-feature readiness in
  `pipeline/objc3_ir_emission_core_feature_implementation_surface.h`,
  `pipeline/objc3_frontend_pipeline.cpp`, and
  `pipeline/objc3_frontend_artifacts.cpp` so direct LLVM IR emission fails
  closed when modular split transport, boundary handoff, or IR entrypoint
  readiness drifts.
- M228 lane-C C004 core feature expansion anchors IR emission
  expansion-readiness and expansion-key transport in
  `pipeline/objc3_ir_emission_core_feature_implementation_surface.h`,
  `pipeline/objc3_frontend_artifacts.cpp`, `ir/objc3_ir_emitter.h`, and
  `ir/objc3_ir_emitter.cpp` so direct LLVM IR emission fails closed when
  pass-graph expansion continuity or expansion metadata transport drifts.
- M228 lane-C C005 edge-case and compatibility completion anchors deterministic
  compatibility handoff consistency/readiness and compatibility-key transport
  in `pipeline/objc3_ir_emission_core_feature_implementation_surface.h`,
  `pipeline/objc3_frontend_artifacts.cpp`, `ir/objc3_ir_emitter.h`, and
  `ir/objc3_ir_emitter.cpp` so direct LLVM IR emission fails closed when
  pass-graph compatibility evidence or metadata continuity drifts.
- M228 lane-D D001 object emission/link-path reliability freeze anchors compile
  route APIs and backend-route scaffolds in `io/objc3_process.cpp`,
  `io/objc3_toolchain_runtime_ga_operations_scaffold.h`, and
  `io/objc3_toolchain_runtime_ga_operations_core_feature_surface.h` so clang
  and llvm-direct object emission remain deterministic and fail-closed.
- M228 lane-D D002 modular split scaffolding anchors toolchain/runtime
  backend selection/capability and artifact compile-route readiness in
  `io/objc3_toolchain_runtime_ga_operations_scaffold.h` and fail-closed
  pre-dispatch enforcement in `libobjc3c_frontend/frontend_anchor.cpp` so
  object emission cannot bypass deterministic scaffold gating.
- M228 lane-D D003 core feature implementation anchors toolchain/runtime
  backend-output marker path/payload and core-feature readiness in
  `io/objc3_toolchain_runtime_ga_operations_core_feature_surface.h` and
  fail-closed post-dispatch enforcement in
  `libobjc3c_frontend/frontend_anchor.cpp` so object emission/link-path
  reliability remains deterministic after backend dispatch.
- M228 lane-D D004 core feature expansion anchors explicit backend marker-path
  and marker-payload determinism guardrails in
  `io/objc3_toolchain_runtime_ga_operations_core_feature_surface.h` and
  fail-closed expansion evidence wiring in
  `libobjc3c_frontend/frontend_anchor.cpp` so object emission/link-path
  reliability remains deterministic when backend marker artifacts drift.
- M228 lane-D D005 edge-case and compatibility completion anchors deterministic
  compatibility consistency/readiness and compatibility-key transport in
  `io/objc3_toolchain_runtime_ga_operations_core_feature_surface.h` so
  toolchain/runtime object emission/link-path routing remains fail-closed when
  backend route/output compatibility evidence drifts.
- M228 lane-D D006 edge-case expansion and robustness anchors deterministic
  robustness consistency/readiness and robustness-key continuity in
  `io/objc3_toolchain_runtime_ga_operations_core_feature_surface.h` so
  toolchain/runtime object emission/link-path routing remains fail-closed when
  backend route/output robustness evidence drifts.
- M228 lane-E E001 replay-proof/performance closeout gate anchors dependency
  references (`M228-A001`, `M228-B001`, `M228-C002`, `M228-D001`) in
  `docs/contracts/m228_lane_e_replay_proof_and_performance_closeout_gate_e001_expectations.md`,
  `spec/planning/compiler/m228/m228_e001_replay_proof_and_performance_closeout_gate_contract_freeze_packet.md`,
  and `package.json` so closeout evidence remains deterministic and fail-closed
  while lane-C C002 modular split assets are pending.
- M228 lane-E E002 replay-proof/performance closeout modular split/scaffolding anchors dependency references
  (`M228-E001`, `M228-A002`, `M228-B002`,
  `M228-C004`, `M228-D002`) in
  `docs/contracts/m228_lane_e_replay_proof_and_performance_closeout_gate_modular_split_scaffolding_e002_expectations.md`,
  `spec/planning/compiler/m228/m228_e002_replay_proof_and_performance_closeout_gate_modular_split_scaffolding_packet.md`,
  and `package.json` so modular split/scaffolding closeout evidence remains
  deterministic and fail-closed while lane-C C004 modular split/scaffolding
  assets are pending GH seed.
- M228 lane-E E003 replay-proof/performance closeout core feature implementation anchors dependency references
  (`M228-E002`, `M228-A003`, `M228-B003`,
  `M228-C003`, and `M228-D003`) in
  `docs/contracts/m228_lane_e_replay_proof_and_performance_closeout_gate_core_feature_implementation_e003_expectations.md`,
  `spec/planning/compiler/m228/m228_e003_replay_proof_and_performance_closeout_gate_core_feature_implementation_packet.md`,
  and `package.json` so core-feature closeout evidence remains deterministic
  and fail-closed across lane-A through lane-D core feature workstreams.
- M228 lane-E E004 replay-proof/performance closeout core feature expansion anchors dependency references
  (`M228-E003`, `M228-A003`, `M228-B004`,
  `M228-C003`, `M228-D003`, and pending token `M228-C008`) in
  `docs/contracts/m228_lane_e_replay_proof_and_performance_closeout_gate_core_feature_expansion_e004_expectations.md`,
  `spec/planning/compiler/m228/m228_e004_replay_proof_and_performance_closeout_gate_core_feature_expansion_packet.md`,
  and `package.json` so core-feature expansion closeout evidence remains
  deterministic and fail-closed across lane-A through lane-D expansion
  dependencies.
- M228 lane-E E005 replay-proof/performance closeout edge-case and compatibility completion anchors
  dependency references (`M228-E004`, `M228-A004`, `M228-B006`, `M228-C004`,
  `M228-D005`, and pending token `M228-C010`) in
  `docs/contracts/m228_lane_e_replay_proof_and_performance_closeout_gate_edge_case_and_compatibility_completion_e005_expectations.md`,
  `spec/planning/compiler/m228/m228_e005_replay_proof_and_performance_closeout_gate_edge_case_and_compatibility_completion_packet.md`,
  and `package.json` so edge-case compatibility completion closeout evidence
  remains deterministic and fail-closed across lane-A through lane-D
  dependencies.
- M228 lane-E E006 edge-case expansion and robustness anchors dependency
  references (`M228-E005`, `M228-A006`, `M228-B006`, `M228-C006`, `M228-D006`)
  in
  `docs/contracts/m228_lane_e_replay_proof_and_performance_closeout_gate_edge_case_expansion_and_robustness_e006_expectations.md`,
  `spec/planning/compiler/m228/m228_e006_replay_proof_and_performance_closeout_gate_edge_case_expansion_and_robustness_packet.md`,
  and `package.json` so lane-E edge-case expansion/robustness closeout
  evidence remains deterministic and fail-closed across lane-A through lane-D
  dependencies.
- M228 lane-E E007 replay-proof/performance closeout diagnostics hardening anchors
  dependency references (`M228-E006`, `M228-A007`, `M228-B007`, `M228-D007`,
  and pending token `M228-C007`) in
  `docs/contracts/m228_lane_e_replay_proof_and_performance_closeout_gate_diagnostics_hardening_e007_expectations.md`,
  `spec/planning/compiler/m228/m228_e007_replay_proof_and_performance_closeout_gate_diagnostics_hardening_packet.md`,
  and `package.json` so diagnostics-hardening closeout evidence remains
  deterministic and fail-closed while lane-C diagnostics-hardening assets are
  pending.
- M247 lane-E E001 performance SLO gate/reporting anchors dependency references
  (`M247-A001`, `M247-B001`, `M247-C001`, `M247-D001`) in
  `docs/contracts/m247_lane_e_performance_slo_gate_and_reporting_e001_expectations.md`,
  `spec/planning/compiler/m247/m247_e001_performance_slo_gate_and_reporting_contract_freeze_packet.md`,
  and `package.json` so compile-time/perf budget governance evidence remains
  deterministic and fail-closed while lane A-D contract-freeze assets are
  pending GH seed.
- M247 lane-E E002 performance SLO modular split/scaffolding anchors
  dependency references (`M247-E001`, `M247-A002`, `M247-B002`, `M247-C002`,
  and `M247-D002`) in
  `docs/contracts/m247_lane_e_performance_slo_gate_and_reporting_modular_split_scaffolding_e002_expectations.md`,
  `spec/planning/compiler/m247/m247_e002_performance_slo_gate_and_reporting_modular_split_scaffolding_packet.md`,
  and `package.json` so modular split/scaffolding governance evidence remains
  deterministic and fail-closed while lane A-D modular split assets are pending
  GH seed.
- M248 lane-A A001 suite partitioning and fixture ownership anchors explicit
  lane-A contract freeze artifacts in
  `docs/contracts/m248_suite_partitioning_and_fixture_ownership_contract_freeze_a001_expectations.md`,
  `spec/planning/compiler/m248/m248_a001_suite_partitioning_and_fixture_ownership_contract_freeze_packet.md`,
  and `package.json` so suite partition boundaries and fixture ownership evidence
  remain deterministic and fail-closed for CI sharding governance.
- M248 lane-A A002 suite partitioning modular split/scaffolding anchors
  explicit lane-A scaffolding artifacts in
  `docs/contracts/m248_suite_partitioning_and_fixture_ownership_modular_split_scaffolding_a002_expectations.md`,
  `spec/planning/compiler/m248/m248_a002_suite_partitioning_and_fixture_ownership_modular_split_scaffolding_packet.md`,
  and `package.json` so modular split fixture continuity remains deterministic
  and fail-closed against `M248-A001` dependency drift.
- M248 lane-B B001 semantic/lowering test architecture anchors explicit lane-B
  contract freeze artifacts in
  `docs/contracts/m248_semantic_lowering_test_architecture_contract_freeze_b001_expectations.md`,
  `spec/planning/compiler/m248/m248_b001_semantic_lowering_test_architecture_contract_freeze_packet.md`,
  and `package.json` so semantic fixture boundaries and lowering replay evidence
  remain deterministic and fail-closed for CI sharding governance.
- M248 lane-B B002 semantic/lowering modular split/scaffolding anchors explicit
  lane-B scaffolding artifacts in
  `docs/contracts/m248_semantic_lowering_test_architecture_modular_split_scaffolding_b002_expectations.md`,
  `spec/planning/compiler/m248/m248_b002_semantic_lowering_test_architecture_modular_split_scaffolding_packet.md`,
  and `package.json` so modular split semantic continuity remains deterministic
  and fail-closed against `M248-B001` dependency drift.
- M249 lane-B B001 semantic compatibility and migration checks anchors explicit
  lane-B contract and architecture freeze artifacts in
  `docs/contracts/m249_semantic_compatibility_and_migration_checks_contract_freeze_b001_expectations.md`,
  `spec/planning/compiler/m249/m249_b001_semantic_compatibility_and_migration_checks_contract_and_architecture_freeze_packet.md`,
  and `package.json` so compatibility-mode and migration-assist sema/parse
  handoff evidence remains deterministic and fail-closed before lane-B modular
  split/scaffolding expansion.
- M249 lane-B B002 semantic compatibility/migration modular split/scaffolding
  anchors explicit lane-B scaffolding artifacts in
  `docs/contracts/m249_semantic_compatibility_and_migration_checks_modular_split_scaffolding_b002_expectations.md`,
  `spec/planning/compiler/m249/m249_b002_semantic_compatibility_and_migration_checks_modular_split_scaffolding_packet.md`,
  and `package.json` so modular split compatibility continuity remains
  deterministic and fail-closed against `M249-B001` dependency drift.
- M249 lane-B B003 semantic compatibility/migration core feature implementation anchors
  explicit lane-B core feature artifacts in
  `docs/contracts/m249_semantic_compatibility_and_migration_checks_core_feature_implementation_b003_expectations.md`,
  `spec/planning/compiler/m249/m249_b003_semantic_compatibility_and_migration_checks_core_feature_implementation_packet.md`,
  and `package.json` so core feature compatibility continuity remains
  deterministic and fail-closed against `M249-B002` dependency drift.
- M248 lane-C C001 replay harness and artifact contract anchors explicit lane-C
  contract freeze artifacts in
  `docs/contracts/m248_replay_harness_and_artifact_contracts_contract_freeze_c001_expectations.md`,
  `spec/planning/compiler/m248/m248_c001_replay_harness_and_artifact_contracts_contract_freeze_packet.md`,
  and `package.json` so replay artifact boundaries and evidence routing remain
  deterministic and fail-closed for CI replay governance.
- M248 lane-C C002 replay harness and artifact modular split/scaffolding
  anchors explicit lane-C scaffolding artifacts in
  `docs/contracts/m248_replay_harness_and_artifact_contracts_modular_split_scaffolding_c002_expectations.md`,
  `spec/planning/compiler/m248/m248_c002_replay_harness_and_artifact_contracts_modular_split_scaffolding_packet.md`,
  and `package.json` so replay artifact scaffolding continuity remains
  deterministic and fail-closed against `M248-C001` dependency drift.
- M248 lane-D D001 runner reliability and platform operations anchors explicit
  lane-D contract freeze artifacts in
  `docs/contracts/m248_runner_reliability_and_platform_operations_contract_freeze_d001_expectations.md`,
  `spec/planning/compiler/m248/m248_d001_runner_reliability_and_platform_operations_contract_freeze_packet.md`,
  and `package.json` so runner/platform operation evidence remains deterministic
  and fail-closed for CI sharding and replay governance.
- M248 lane-D D002 runner modular split/scaffolding anchors explicit lane-D
  scaffolding artifacts in
  `docs/contracts/m248_runner_reliability_and_platform_operations_modular_split_scaffolding_d002_expectations.md`,
  `spec/planning/compiler/m248/m248_d002_runner_reliability_and_platform_operations_modular_split_scaffolding_packet.md`,
  and `package.json` so modular split runner continuity remains deterministic
  and fail-closed against `M248-D001` dependency drift.
- M249 lane-A A001 feature packaging surface and compatibility anchors explicit
  lane-A contract freeze artifacts in
  `docs/contracts/m249_feature_packaging_surface_and_compatibility_contracts_contract_freeze_a001_expectations.md`,
  `spec/planning/compiler/m249/m249_a001_feature_packaging_surface_and_compatibility_contracts_contract_freeze_packet.md`,
  and `package.json` so release packaging surface compatibility evidence remains
  deterministic and fail-closed for distribution boundary governance.
- M249 lane-A A002 feature packaging modular split/scaffolding anchors
  explicit lane-A scaffolding artifacts in
  `docs/contracts/m249_feature_packaging_surface_and_compatibility_contracts_modular_split_scaffolding_a002_expectations.md`,
  `spec/planning/compiler/m249/m249_a002_feature_packaging_surface_and_compatibility_contracts_modular_split_scaffolding_packet.md`,
  and `package.json` so modular split compatibility continuity remains
  deterministic and fail-closed against `M249-A001` dependency drift.
- M249 lane-A A003 feature packaging core feature implementation anchors
  explicit lane-A core-feature artifacts in
  `docs/contracts/m249_feature_packaging_surface_and_compatibility_contracts_core_feature_implementation_a003_expectations.md`,
  `spec/planning/compiler/m249/m249_a003_feature_packaging_surface_and_compatibility_contracts_core_feature_implementation_packet.md`,
  and `package.json` so core feature compatibility continuity remains
  deterministic and fail-closed against `M249-A002` dependency drift.
- M249 lane-C C001 IR/object packaging and symbol policy contract anchors
  explicit lane-C contract freeze artifacts in
  `docs/contracts/m249_ir_object_packaging_and_symbol_policy_contract_freeze_c001_expectations.md`,
  `spec/planning/compiler/m249/m249_c001_ir_object_packaging_and_symbol_policy_contract_freeze_packet.md`,
  and `package.json` so artifact packaging boundaries and symbol policy
  continuity remain deterministic and fail-closed for CI replay governance.
- M249 lane-C C002 IR/object packaging and symbol policy modular split/scaffolding anchors
  explicit lane-C scaffolding artifacts in
  `docs/contracts/m249_ir_object_packaging_and_symbol_policy_modular_split_scaffolding_c002_expectations.md`,
  `spec/planning/compiler/m249/m249_c002_ir_object_packaging_and_symbol_policy_modular_split_scaffolding_packet.md`,
  and `package.json` so modular split artifact packaging and symbol policy continuity remain
  deterministic and fail-closed against `M249-C001` dependency drift.
- M249 lane-C C003 IR/object packaging and symbol policy core feature implementation anchors
  explicit lane-C core-feature artifacts in
  `docs/contracts/m249_ir_object_packaging_and_symbol_policy_core_feature_implementation_c003_expectations.md`,
  `spec/planning/compiler/m249/m249_c003_ir_object_packaging_and_symbol_policy_core_feature_implementation_packet.md`,
  and `package.json` so core feature artifact packaging and symbol policy continuity remain
  deterministic and fail-closed against `M249-C001` and `M249-C002` dependency drift.
- M249 lane-D D001 installer/runtime operations and support tooling anchors
  explicit lane-D contract and architecture freeze artifacts in
  `docs/contracts/m249_installer_runtime_operations_and_support_tooling_contract_and_architecture_freeze_d001_expectations.md`,
  `spec/planning/compiler/m249/m249_d001_installer_runtime_operations_and_support_tooling_contract_and_architecture_freeze_packet.md`,
  and `package.json` so installer/runtime operations boundaries and support
  tooling continuity remain deterministic and fail-closed for release
  governance readiness.
- M249 lane-D D002 installer/runtime operations modular split/scaffolding
  anchors explicit lane-D scaffolding artifacts in
  `docs/contracts/m249_installer_runtime_operations_and_support_tooling_modular_split_scaffolding_d002_expectations.md`,
  `spec/planning/compiler/m249/m249_d002_installer_runtime_operations_and_support_tooling_modular_split_scaffolding_packet.md`,
  and `package.json` so modular split installer/runtime continuity remains
  deterministic and fail-closed against `M249-D001` dependency drift.
- M249 lane-D D003 installer/runtime operations core feature implementation anchors
  explicit lane-D core-feature artifacts in
  `docs/contracts/m249_installer_runtime_operations_and_support_tooling_core_feature_implementation_d003_expectations.md`,
  `spec/planning/compiler/m249/m249_d003_installer_runtime_operations_and_support_tooling_core_feature_implementation_packet.md`,
  and `package.json` so core feature installer/runtime continuity remains
  deterministic and fail-closed against `M249-D002` dependency drift.
- M249 lane-E E001 release gate/docs/runbooks contract and architecture freeze
  anchors dependency references (`M249-A001`, `M249-B001`, `M249-C001`, and
  `M249-D001`) in
  `docs/contracts/m249_lane_e_release_gate_docs_and_runbooks_contract_and_architecture_freeze_e001_expectations.md`,
  `spec/planning/compiler/m249/m249_e001_lane_e_release_gate_docs_and_runbooks_contract_and_architecture_freeze_packet.md`,
  and `package.json` so release-gate documentation/runbook governance evidence
  remains deterministic and fail-closed while lane A-D contract-freeze assets
  are pending GH seed.
- M249 lane-E E002 release gate/docs/runbooks modular split/scaffolding
  anchors dependency references (`M249-E001`, `M249-A002`, `M249-B002`,
  `M249-C002`, and `M249-D002`) in
  `docs/contracts/m249_lane_e_release_gate_docs_and_runbooks_modular_split_scaffolding_e002_expectations.md`,
  `spec/planning/compiler/m249/m249_e002_lane_e_release_gate_docs_and_runbooks_modular_split_scaffolding_packet.md`,
  and `package.json` so release-gate documentation/runbook modular split/scaffolding
  governance evidence remains deterministic and fail-closed while lane A-D modular
  split/scaffolding assets are pending GH seed.
- M249 lane-E E003 release gate/docs/runbooks core feature implementation
  anchors dependency references (`M249-E002`, `M249-A003`, `M249-B003`,
  `M249-C003`, and `M249-D003`) in
  `docs/contracts/m249_lane_e_release_gate_docs_and_runbooks_core_feature_implementation_e003_expectations.md`,
  `spec/planning/compiler/m249/m249_e003_lane_e_release_gate_docs_and_runbooks_core_feature_implementation_packet.md`,
  and `package.json` so release-gate documentation/runbook core-feature
  governance evidence remains deterministic and fail-closed while lane A-D core
  feature assets are pending GH seed.
- M243 lane-A A001 diagnostic grammar hooks/source precision anchors explicit
  parser diagnostic coordinate and fingerprint freeze in
  `parse/objc3_parse_support.cpp`, `parse/objc3_parser_contract.h`, and
  `pipeline/objc3_parse_lowering_readiness_surface.h` so diagnostic coordinate
  fidelity and deterministic parser diagnostic surfaces remain fail-closed.
- M243 lane-B B001 semantic diagnostic taxonomy/fix-it synthesis anchors explicit
  sema diagnostics bus and pass-flow freeze in
  `sema/objc3_sema_pass_manager_contract.h` and
  `sema/objc3_sema_pass_flow_scaffold.h` so semantic diagnostics accounting and
  fix-it deterministic handoff remain fail-closed.
- M250 lane-A frontend stability freeze anchors long-tail grammar closure to
  parser contract snapshots (`parse/objc3_parser_contract.h`) and parse/lowering
  readiness replay gates (`pipeline/objc3_parse_lowering_readiness_surface.h`)
  so parser determinism and recovery coverage remain fail-closed for GA
  readiness.
- M250 lane-A A003 core feature implementation anchors long-tail grammar
  closure identity (`long_tail_grammar_*`) in
  `parse/objc3_parser_contract.h` and wires fail-closed handoff enforcement in
  `pipeline/objc3_parse_lowering_readiness_surface.h`.
- M250 lane-A A004 core feature expansion anchors explicit long-tail grammar
  expansion accounting/replay gates (`long_tail_grammar_expansion_*`) in
  `pipeline/objc3_parse_lowering_readiness_surface.h` so parse/lowering
  readiness can fail closed on expansion drift with deterministic replay keys.
- M250 lane-A A005 edge compatibility completion anchors long-tail grammar
  compatibility handoff and edge-case readiness
  (`long_tail_grammar_edge_case_compatibility_*`) in
  `pipeline/objc3_parse_lowering_readiness_surface.h` so compatibility drift
  fails closed before lowering readiness is reported.
- M250 lane-A A006 edge-case expansion and robustness anchors explicit
  expansion/robustness guardrails (`long_tail_grammar_edge_case_*_robustness*`)
  in `pipeline/objc3_parse_lowering_readiness_surface.h` so edge-case expansion
  drift fails closed before conformance matrix readiness is reported.
- M250 lane-A A007 diagnostics hardening anchors explicit long-tail grammar
  diagnostics guardrails (`long_tail_grammar_diagnostics_hardening_*`) in
  `pipeline/objc3_parse_lowering_readiness_surface.h` so diagnostics drift fails
  closed before recovery/determinism and conformance-readiness gates are
  reported.
- M250 lane-A A008 recovery/determinism hardening anchors explicit long-tail
  grammar replay-hardening gates (`long_tail_grammar_recovery_determinism_*`)
  in `pipeline/objc3_parse_lowering_readiness_surface.h` so replay drift fails
  closed before conformance and guardrail readiness are reported.
- M250 lane-A A009 conformance matrix implementation anchors explicit long-tail
  grammar matrix gates (`long_tail_grammar_conformance_matrix_*`) in
  `pipeline/objc3_parse_lowering_readiness_surface.h` so matrix drift fails
  closed before corpus and guardrail readiness are reported.
- M250 lane-A A010 integration closeout and gate sign-off anchors explicit
  closeout/sign-off gates (`long_tail_grammar_integration_closeout_*`,
  `long_tail_grammar_gate_signoff_*`) in
  `pipeline/objc3_parse_lowering_readiness_surface.h` so lane-A cannot report
  ready-for-lowering before deterministic integration closeout is satisfied.
- M250 lane-B semantic stability freeze closes spec delta between
  `pipeline/objc3_typed_sema_to_lowering_contract_surface.h` and
  `pipeline/objc3_parse_lowering_readiness_surface.h` so semantic handoff
  determinism, conformance corpus closure, and performance guardrail gating are
  fail-closed under a single readiness boundary.
- M250 lane-B B002 modular split scaffolding anchors semantic-stability closure
  in `pipeline/objc3_semantic_stability_spec_delta_closure_scaffold.h` so
  typed sema handoff and parse/lowering readiness surfaces stay split while
  sharing a deterministic fail-closed scaffold key.
- M250 lane-B B003 core feature implementation anchors semantic stability
  readiness in
  `pipeline/objc3_semantic_stability_core_feature_implementation_surface.h`
  so typed semantic case accounting and parse conformance accounting remain
  deterministic and fail-closed behind the B002 scaffold.
- M250 lane-B B006 edge-case expansion and robustness anchors explicit
  semantic edge-case expansion/robustness guardrails
  (`edge_case_*_robustness*`) in
  `pipeline/objc3_semantic_stability_core_feature_implementation_surface.h`
  so edge-case expansion drift fails closed before semantic stability
  readiness is reported.
- M250 lane-B B007 diagnostics hardening anchors explicit semantic diagnostics
  hardening guardrails (`diagnostics_hardening_*`) in
  `pipeline/objc3_semantic_stability_core_feature_implementation_surface.h`
  so diagnostics drift fails closed before semantic stability readiness is
  reported.
- M250 lane-B B008 recovery/determinism hardening anchors explicit semantic
  recovery/determinism guardrails (`recovery_determinism_*`) in
  `pipeline/objc3_semantic_stability_core_feature_implementation_surface.h`
  so replay drift fails closed before semantic stability readiness is
  reported.
- M250 lane-B B009 conformance matrix anchors explicit semantic
  conformance-matrix guardrails (`conformance_matrix_*`) in
  `pipeline/objc3_semantic_stability_core_feature_implementation_surface.h`
  so matrix drift fails closed before semantic stability readiness is
  reported.
- M250 lane-B B010 conformance corpus expansion anchors explicit semantic
  conformance-corpus guardrails (`conformance_corpus_*`) in
  `pipeline/objc3_semantic_stability_core_feature_implementation_surface.h`
  so corpus drift fails closed before semantic stability readiness is
  reported.
- M250 lane-B B011 performance and quality guardrails anchors explicit semantic
  performance/quality guardrails (`performance_quality_guardrails_*`) in
  `pipeline/objc3_semantic_stability_core_feature_implementation_surface.h`
  so guardrail drift fails closed before semantic stability readiness is
  reported.
- M250 lane-B B012 integration closeout and gate sign-off anchors explicit semantic
  closeout/sign-off guardrails (`integration_closeout_*`, `gate_signoff_*`) in
  `pipeline/objc3_semantic_stability_core_feature_implementation_surface.h`
  so semantic stability cannot report final readiness before deterministic
  integration closeout is satisfied.
- M250 lane-C C002 modular split scaffolding anchors lowering/runtime stability
  and invariant-proof closure in
  `pipeline/objc3_lowering_runtime_stability_invariant_scaffold.h` so typed
  sema handoff and parse/lowering readiness surfaces share deterministic
  fail-closed runtime-proof gates.
- M250 lane-C C004 core feature expansion anchors explicit lowering/runtime
  expansion-accounting and replay-key guardrails in
  `pipeline/objc3_lowering_runtime_stability_core_feature_implementation_surface.h`
  so core feature readiness can fail closed on accounting drift.
- M250 lane-C C005 edge compatibility completion anchors parse/runtime
  compatibility handoff and edge-robustness gates in
  `pipeline/objc3_lowering_runtime_stability_core_feature_implementation_surface.h`
  so edge-case compatibility drift fails closed before runtime readiness.
- M250 lane-C C006 edge-case expansion and robustness anchors explicit
  lowering/runtime edge-case expansion and robustness guardrails
  (`edge_case_*_robustness*`) in
  `pipeline/objc3_lowering_runtime_stability_core_feature_implementation_surface.h`
  so edge-case expansion drift fails closed before runtime readiness is
  reported.
- M250 lane-C C007 diagnostics hardening anchors explicit lowering/runtime
  diagnostics guardrails (`diagnostics_hardening_*`) in
  `pipeline/objc3_lowering_runtime_stability_core_feature_implementation_surface.h`
  so diagnostics drift fails closed before runtime readiness is reported.
- M250 lane-C C008 recovery/determinism hardening anchors explicit lowering/runtime
  recovery guardrails (`recovery_determinism_*`) in
  `pipeline/objc3_lowering_runtime_stability_core_feature_implementation_surface.h`
  so replay drift fails closed before runtime readiness is reported.
- M250 lane-C C009 conformance matrix anchors explicit lowering/runtime
  conformance-matrix guardrails (`conformance_matrix_*`) in
  `pipeline/objc3_lowering_runtime_stability_core_feature_implementation_surface.h`
  so conformance-matrix drift fails closed before runtime readiness is
  reported.
- M250 lane-C C010 conformance corpus expansion anchors explicit lowering/runtime
  conformance-corpus guardrails (`conformance_corpus_*`) in
  `pipeline/objc3_lowering_runtime_stability_core_feature_implementation_surface.h`
  so conformance-corpus drift fails closed before runtime readiness is
  reported.
- M250 lane-C C011 performance and quality guardrails anchors explicit
  lowering/runtime performance guardrails (`performance_quality_guardrails_*`)
  in `pipeline/objc3_lowering_runtime_stability_core_feature_implementation_surface.h`
  so performance/quality drift fails closed before runtime readiness is
  reported.
- M250 lane-C C012 cross-lane integration sync anchors explicit lowering/runtime
  cross-lane synchronization guardrails (`cross_lane_integration_*`) in
  `pipeline/objc3_lowering_runtime_stability_core_feature_implementation_surface.h`
  so lane-C cannot report readiness before deterministic lane-A/lane-B/lane-C
  integration alignment is satisfied.
- M250 lane-C C013 integration closeout and gate sign-off anchors explicit
  lowering/runtime closeout/sign-off guardrails
  (`integration_closeout_*`, `gate_signoff_*`) in
  `pipeline/objc3_lowering_runtime_stability_core_feature_implementation_surface.h`
  so lane-C final readiness cannot report green before deterministic closeout
  evidence is satisfied.
- M250 lane-D D002 modular split scaffolding anchors toolchain/runtime GA
  operations readiness in
  `io/objc3_toolchain_runtime_ga_operations_scaffold.h` so backend routing and
  IR/object artifact compile gating stay deterministic and fail-closed before
  runtime object emission dispatch.
- M250 lane-D D003 core feature implementation anchors toolchain/runtime GA
  operations readiness closure in
  `io/objc3_toolchain_runtime_ga_operations_core_feature_surface.h` so scaffold
  readiness, compile dispatch outcomes, and backend marker recording stay
  deterministic and fail-closed before success exit status is returned.
- M250 lane-D D004 core feature expansion anchors explicit backend marker-path
  and marker-payload guardrails in
  `io/objc3_toolchain_runtime_ga_operations_core_feature_surface.h` plus
  `pipeline/objc3_frontend_types.h` so toolchain/runtime GA readiness fails
  closed on marker determinism drift before success exit status is returned.
- M250 lane-D D005 edge-case compatibility completion anchors explicit
  edge-case compatibility guardrails (`edge_case_compatibility_*`) in
  `io/objc3_toolchain_runtime_ga_operations_core_feature_surface.h` so
  toolchain/runtime GA readiness fails closed on backend route compatibility
  drift before success exit status is returned.
- M250 lane-D D006 edge-case expansion and robustness anchors explicit
  edge-case expansion/robustness guardrails (`edge_case_expansion_*`,
  `edge_case_robustness_*`) in
  `io/objc3_toolchain_runtime_ga_operations_core_feature_surface.h` so
  toolchain/runtime GA readiness fails closed on backend output robustness
  drift before success exit status is returned.
- M250 lane-D D007 diagnostics hardening anchors explicit diagnostics
  hardening guardrails (`diagnostics_hardening_*`) in
  `io/objc3_toolchain_runtime_ga_operations_core_feature_surface.h` so
  toolchain/runtime GA readiness fails closed on diagnostics drift before
  success exit status is returned.
- M250 lane-D D008 recovery/determinism hardening anchors explicit
  toolchain/runtime recovery guardrails in
  `pipeline/objc3_parse_lowering_readiness_surface.h` so lane-D recovery and
  replay determinism drift fails closed before conformance and sign-off gates.
- M250 lane-D D009 conformance matrix implementation anchors explicit
  toolchain/runtime conformance-matrix guardrails in
  `pipeline/objc3_parse_lowering_readiness_surface.h` so lane-D conformance
  matrix drift fails closed before corpus and sign-off gates.
- M250 lane-D D010 conformance corpus expansion anchors explicit
  toolchain/runtime conformance-corpus guardrails in
  `pipeline/objc3_parse_lowering_readiness_surface.h` so lane-D conformance
  corpus drift fails closed before performance guardrails and sign-off gates.
- M250 lane-D D011 performance and quality guardrails anchor explicit
  toolchain/runtime performance/quality guardrail checks in
  `pipeline/objc3_parse_lowering_readiness_surface.h` so lane-D guardrail
  drift fails closed before integration closeout and sign-off gates.
- M250 lane-D D012 cross-lane integration sync anchors explicit
  toolchain/runtime cross-lane integration guards in
  `pipeline/objc3_parse_lowering_readiness_surface.h` so lane-D integration
  drift fails closed before integration closeout and gate sign-off.
- M250 lane-D D013 docs and operator runbook synchronization anchors explicit
  toolchain/runtime docs/runbook synchronization guardrails in
  `pipeline/objc3_parse_lowering_readiness_surface.h` so lane-D docs-signoff
  drift fails closed before final gate sign-off.
- M250 lane-D D014 release-candidate replay dry-run anchors deterministic
  dry-run validation in
  `scripts/run_m250_d014_toolchain_runtime_ga_operations_readiness_release_replay_dry_run.ps1`
  so lane-D release-candidate replay drift fails closed before downstream
  advanced workpacks.
- M250 lane-D D015 advanced core workpack (shard 1) anchors explicit
  toolchain/runtime advanced-core consistency/readiness guardrails
  (`toolchain_runtime_ga_operations_advanced_core_*`) in
  `pipeline/objc3_parse_lowering_readiness_surface.h` so lane-D
  docs/runbook synchronization closure is promoted into a hardened
  advanced-core gate before parse/lowering readiness can report ready.
- M250 lane-D D016 advanced edge compatibility workpack (shard 1) anchors
  explicit toolchain/runtime advanced edge-compatibility
  consistency/readiness guardrails
  (`toolchain_runtime_ga_operations_advanced_edge_compatibility_*`) in
  `pipeline/objc3_parse_lowering_readiness_surface.h` so lane-D advanced
  core closure is promoted into hardened edge-compatibility sign-off before
  parse/lowering readiness can report ready.
- M250 lane-D D017 advanced diagnostics workpack (shard 1) anchors explicit
  toolchain/runtime advanced diagnostics consistency/readiness guardrails
  (`toolchain_runtime_ga_operations_advanced_diagnostics_*`) in
  `pipeline/objc3_parse_lowering_readiness_surface.h` so lane-D advanced
  edge-compatibility closure is promoted into hardened diagnostics sign-off
  before parse/lowering readiness can report ready.
- M250 lane-D D018 advanced conformance workpack (shard 1) anchors explicit
  toolchain/runtime advanced conformance consistency/readiness guardrails
  (`toolchain_runtime_ga_operations_advanced_conformance_*`) in
  `pipeline/objc3_parse_lowering_readiness_surface.h` so lane-D advanced
  diagnostics closure is promoted into hardened conformance sign-off before
  parse/lowering readiness can report ready.
- M250 lane-D D019 advanced integration workpack (shard 1) anchors explicit
  toolchain/runtime advanced integration consistency/readiness guardrails
  (`toolchain_runtime_ga_operations_advanced_integration_*`) in
  `pipeline/objc3_parse_lowering_readiness_surface.h` so lane-D advanced
  conformance closure is promoted into hardened integration sign-off before
  parse/lowering readiness can report ready.
- M250 lane-D D020 advanced performance workpack (shard 1) anchors explicit
  toolchain/runtime advanced performance consistency/readiness guardrails
  (`toolchain_runtime_ga_operations_advanced_performance_*`) in
  `pipeline/objc3_parse_lowering_readiness_surface.h` so lane-D advanced
  integration closure is promoted into hardened performance sign-off before
  parse/lowering readiness can report ready.
- M250 lane-D D021 advanced core workpack (shard 2) anchors explicit
  toolchain/runtime advanced core shard-2 consistency/readiness guardrails
  (`toolchain_runtime_ga_operations_advanced_core_shard2_*`) in
  `pipeline/objc3_parse_lowering_readiness_surface.h` so lane-D advanced
  performance closure is promoted into hardened shard-2 core sign-off before
  parse/lowering readiness can report ready.
- M250 lane-D D022 integration closeout and gate sign-off anchors explicit
  toolchain/runtime integration-closeout sign-off consistency/readiness
  guardrails (`toolchain_runtime_ga_operations_integration_closeout_signoff_*`)
  in `pipeline/objc3_parse_lowering_readiness_surface.h` so lane-D advanced
  core shard-2 closure is promoted into final lane-D sign-off before
  parse/lowering readiness can report ready.
- M250 lane-E E003 core feature implementation anchors explicit final readiness
  core-feature dependency guardrails in
  `pipeline/objc3_final_readiness_gate_core_feature_implementation_surface.h`
  so lane-E sign-off fails closed when E002/A003/B003/C003/D003 dependency
  readiness drifts.
- M250 lane-E E004 core feature expansion anchors explicit final readiness
  expansion guardrails (`core_feature_expansion_*`) in
  `pipeline/objc3_final_readiness_gate_core_feature_implementation_surface.h`
  so lane-E fails closed when upstream lane expansion readiness drifts before
  final sign-off can remain green.
- M250 lane-E E005 edge-case compatibility completion anchors explicit final
  readiness edge-case compatibility guardrails
  (`edge_case_compatibility_*`) in
  `pipeline/objc3_final_readiness_gate_core_feature_implementation_surface.h`
  so lane-E fails closed when E004/A005/B005/C005/D005 compatibility readiness
  drifts before final sign-off can remain green.
- M250 lane-E E006 edge-case expansion and robustness anchors explicit final
  readiness edge-case expansion and robustness guardrails
  (`edge_case_expansion_*`, `edge_case_robustness_*`) in
  `pipeline/objc3_final_readiness_gate_core_feature_implementation_surface.h`
  so lane-E fails closed when E005/A002/B003/C003/D005 robustness prerequisites
  drift before final sign-off can remain green.
- M250 lane-E E007 diagnostics hardening anchors explicit final readiness
  diagnostics-hardening guardrails (`diagnostics_hardening_*`) in
  `pipeline/objc3_final_readiness_gate_core_feature_implementation_surface.h`
  so lane-E fails closed when E006/A003/B003/C003/D006 diagnostics hardening
  prerequisites drift before final sign-off can remain green.
- M250 lane-E E008 recovery/determinism hardening anchors explicit final
  readiness recovery and determinism guardrails (`recovery_determinism_*`) in
  `pipeline/objc3_final_readiness_gate_core_feature_implementation_surface.h`
  so lane-E fails closed when E007/A003/B004/C004/D007 recovery prerequisites
  drift before final sign-off can remain green.
- M250 lane-E E009 conformance-matrix implementation anchors explicit final
  readiness conformance-matrix guardrails (`conformance_matrix_*`) in
  `pipeline/objc3_final_readiness_gate_core_feature_implementation_surface.h`
  so lane-E fails closed when E008/A003/B004/C004/D007 conformance prerequisites
  drift before final sign-off can remain green.
- M250 lane-E E010 conformance-corpus expansion anchors explicit final
  readiness conformance-corpus guardrails (`conformance_corpus_*`) in
  `pipeline/objc3_final_readiness_gate_core_feature_implementation_surface.h`
  so lane-E fails closed when E009/A004/B004/C005/D008 conformance prerequisites
  drift before final sign-off can remain green.
- M250 lane-E E011 performance and quality guardrails anchors explicit final
  readiness performance/quality guardrails (`performance_quality_guardrails_*`) in
  `pipeline/objc3_final_readiness_gate_core_feature_implementation_surface.h`
  so lane-E fails closed when E010/A004/B005/C005/D009 quality prerequisites
  drift before final sign-off can remain green.
- M250 lane-E E012 cross-lane integration sync anchors explicit final
  readiness cross-lane integration guardrails (`cross_lane_integration_*`) in
  `pipeline/objc3_final_readiness_gate_core_feature_implementation_surface.h`
  so lane-E fails closed when E011/A004/B005/C006/D010 sync prerequisites
  drift before final sign-off can remain green.
- M250 lane-E E013 docs/runbook synchronization anchors explicit final
  readiness docs/runbook synchronization guardrails (`docs_runbook_sync_*`) in
  `pipeline/objc3_final_readiness_gate_core_feature_implementation_surface.h`
  so lane-E fails closed when E012/A005/B006/C006/D011 sync prerequisites
  drift before final sign-off can remain green.
- M250 lane-E E014 release-candidate replay dry-run anchors explicit final
  readiness release-candidate replay dry-run guardrails
  (`release_candidate_replay_dry_run_*`) in
  `pipeline/objc3_final_readiness_gate_core_feature_implementation_surface.h`
  so lane-E fails closed when E013/A005/B006/C007/D011 replay prerequisites
  drift before final sign-off can remain green.
- M250 lane-E E015 advanced core shard1 anchors explicit final
  readiness advanced-core shard1 guardrails (`advanced_core_shard1_*`) in
  `pipeline/objc3_final_readiness_gate_core_feature_implementation_surface.h`
  so lane-E fails closed when E014/A006/B007/C007/D012 core prerequisites
  drift before final sign-off can remain green.
- M250 lane-E E016 advanced edge compatibility shard1 anchors explicit final
  readiness advanced-edge-compatibility shard1 guardrails
  (`advanced_edge_compatibility_shard1_*`) in
  `pipeline/objc3_final_readiness_gate_core_feature_implementation_surface.h`
  so lane-E fails closed when E015/A006/B007/C008/D013 edge prerequisites
  drift before final sign-off can remain green.
- M250 lane-E E017 advanced diagnostics shard1 anchors explicit final
  readiness advanced-diagnostics shard1 guardrails
  (`advanced_diagnostics_shard1_*`) in
  `pipeline/objc3_final_readiness_gate_core_feature_implementation_surface.h`
  so lane-E fails closed when E016/A006/B008/C008/D014 diagnostics prerequisites
  drift before final sign-off can remain green.
- M250 lane-E E018 advanced conformance shard1 anchors explicit final
  readiness advanced-conformance shard1 guardrails
  (`advanced_conformance_shard1_*`) in
  `pipeline/objc3_final_readiness_gate_core_feature_implementation_surface.h`
  so lane-E fails closed when E017/A007/B008/C009/D015 conformance prerequisites
  drift before final sign-off can remain green.
- M250 lane-E E019 advanced integration shard1 anchors explicit final
  readiness advanced-integration shard1 guardrails
  (`advanced_integration_shard1_*`) in
  `pipeline/objc3_final_readiness_gate_core_feature_implementation_surface.h`
  so lane-E fails closed when E018/A007/B008/C009/D015 integration prerequisites
  drift before final sign-off can remain green.
- M250 lane-E E020 advanced performance shard1 anchors explicit final
  readiness advanced-performance shard1 guardrails
  (`advanced_performance_shard1_*`) in
  `pipeline/objc3_final_readiness_gate_core_feature_implementation_surface.h`
  so lane-E fails closed when E019/A007/B009/C010/D016 performance prerequisites
  drift before final sign-off can remain green.
- M250 lane-E E021 advanced core shard2 anchors explicit final
  readiness advanced-core shard2 guardrails (`advanced_core_shard2_*`) in
  `pipeline/objc3_final_readiness_gate_core_feature_implementation_surface.h`
  so lane-E fails closed when E020/A008/B009/C010/D017 core prerequisites
  drift before final sign-off can remain green.
- M250 lane-E E022 advanced edge compatibility shard2 anchors explicit final
  readiness advanced-edge-compatibility shard2 guardrails
  (`advanced_edge_compatibility_shard2_*`) in
  `pipeline/objc3_final_readiness_gate_core_feature_implementation_surface.h`
  so lane-E fails closed when E021/A008/B010/C011/D018 edge prerequisites
  drift before final sign-off can remain green.
- M250 lane-E E023 advanced diagnostics shard2 anchors explicit final
  readiness advanced-diagnostics shard2 guardrails
  (`advanced_diagnostics_shard2_*`) in
  `pipeline/objc3_final_readiness_gate_core_feature_implementation_surface.h`
  so lane-E fails closed when E022/A009/B010/C011/D019 diagnostics prerequisites
  drift before final sign-off can remain green.
- M250 lane-E E024 advanced conformance shard2 anchors explicit final
  readiness advanced-conformance shard2 guardrails
  (`advanced_conformance_shard2_*`) in
  `pipeline/objc3_final_readiness_gate_core_feature_implementation_surface.h`
  so lane-E fails closed when E023/A009/B011/C012/D020 conformance prerequisites
  drift before final sign-off can remain green.
- M250 lane-E E025 advanced integration shard2 anchors explicit final
  readiness advanced-integration shard2 guardrails
  (`advanced_integration_shard2_*`) in
  `pipeline/objc3_final_readiness_gate_core_feature_implementation_surface.h`
  so lane-E fails closed when E024/A009/B011/C012/D020 integration prerequisites
  drift before final sign-off can remain green.
- M250 lane-E E026 advanced performance shard2 anchors explicit final
  readiness advanced-performance shard2 guardrails
  (`advanced_performance_shard2_*`) in
  `pipeline/objc3_final_readiness_gate_core_feature_implementation_surface.h`
  so lane-E fails closed when E025/A010/B012/C013/D021 performance prerequisites
  drift before final sign-off can remain green.
- M250 lane-E E027 integration closeout sign-off anchors explicit final
  readiness integration-closeout sign-off guardrails
  (`integration_closeout_signoff_*`) in
  `pipeline/objc3_final_readiness_gate_core_feature_implementation_surface.h`
  so lane-E fails closed when E026/A010/B012/C013/D022 closeout prerequisites
  drift before final sign-off can remain green.
## Ownership Map

- Lane A: `lex/*`, `parse/*`, `lower/*`, `ir/*`, `driver/*`, `io/*`
- Lane B: `pipeline/*`, `sema/*`, `libobjc3c_frontend/*`
- Lane C: generated docs/site tooling and policies (outside `src` tree)
- Lane D: parity/perf/determinism/fuzz gates (tests + scripts)
- Lane E: workflow/governance docs and CI policy wiring

## Dependency Rules

Rules apply to both include direction and link direction.

Allowed dependencies:

- `driver -> libobjc3c_frontend, io`
- `libobjc3c_frontend -> pipeline`
- `pipeline -> lex, parse, sema, lower, ir`
- `lower -> sema`
- `ir -> lower`
- `io -> (no frontend layer dependencies)`
- `lex -> (none)`
- `parse -> lex`
- `sema -> parse`

Forbidden dependencies:

- No stage module may depend on `driver/*`.
- No stage module may depend on `io/*`.
- `lex/*` may not depend on `parse/*`, `sema/*`, `lower/*`, `ir/*`.
- `parse/*` may not depend on `sema/*`, `lower/*`, `ir/*`.
- `sema/*` may not depend on `lower/*`, `ir/*`.
- `lower/*` may not depend on `ir/*`.
- `pipeline/*` may not depend on `driver/*` or `io/*`.
- `libobjc3c_frontend/*` may not depend on `driver/*` or `io/*`.

## Contract Checks (Automation Targets)

The future boundary-check script (`scripts/check_objc3c_dependency_boundaries.py`)
shall enforce:

- Include graph constraints for `#include "..."` within `native/objc3c/src`.
- CMake target link-direction constraints aligned with this document.
- A denylist for reverse dependencies violating stage order.

## Examples

Allowed:

- `parse/objc3_parser.cpp` including `lex/objc3_lexer.h`
- `pipeline/frontend_pipeline.cpp` including `sema/semantic_passes.h`
- `driver/main_driver.cpp` including `libobjc3c_frontend/api.h`

Forbidden:

- `sema/semantic_passes.cpp` including `lower/lower_to_ir.h`
- `ir/ir_emitter.cpp` including `driver/cli_options.h`
- `lex/objc3_lexer.cpp` including `parse/objc3_parser.h`

## Refactor Safety Invariants

- Behavior parity is enforced outside this file by Lane D gates.
- New modules must keep deterministic diagnostics ordering.
- `main.cpp` remains orchestration-only and must not absorb parser/sema logic.
- Parser recovery behavior must remain replay-proof and deterministic.
