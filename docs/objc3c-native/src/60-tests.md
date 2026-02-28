<!-- markdownlint-disable-file MD041 -->

## Deterministic contract commands

From repo root:

```powershell
npm run test:objc3c
npm run test:objc3c:matrix
npm run test:objc3c:perf-budget
npm run test:objc3c:diagnostics-replay-proof
npm run test:objc3c:parser-replay-proof
npm run test:objc3c:parser-extraction-ast-builder-contract
npm run test:objc3c:parser-ast-extraction
npm run test:objc3c:sema-pass-manager-diagnostics-bus
npm run test:objc3c:lowering-replay-proof
npm run test:objc3c:execution-smoke
npm run test:objc3c:execution-replay-proof
npm run test:objc3c:driver-shell-split
npm run test:objc3c:lexer-extraction-token-contract
npm run test:objc3c:lexer-parity
npm run proof:objc3c
npm run test:objc3c:lane-e
npm run check:compiler-closeout:m137
npm run check:compiler-closeout:m138
npm run check:compiler-closeout:m139
npm run test:objc3c:m140-boundary-contract
npm run check:compiler-closeout:m140
npm run test:objc3c:m141-target-topology
npm run check:compiler-closeout:m141
npm run test:objc3c:m142-lowering-parity
npm run check:objc3c:library-cli-parity:source
npm run check:compiler-closeout:m142
npm run test:objc3c:m143-artifact-governance
npm run check:objc3c:library-cli-parity:source:m143
npm run check:compiler-closeout:m143
npm run test:objc3c:m144-llvm-capability-discovery
npm run check:objc3c:llvm-capabilities
npm run check:objc3c:library-cli-parity:source:m144
npm run check:compiler-closeout:m144
npm run test:objc3c:m145-direct-llvm-matrix
npm run check:compiler-closeout:m145
```

Driver shell split regression spot-check (M136-E001):

```powershell
npm run build:objc3c-native
npm run compile:objc3c -- tests/tooling/fixtures/native/hello.objc3 --out-dir tmp/artifacts/objc3c-native/m136-driver-shell/tests-objc3 --emit-prefix module_objc3
npm run compile:objc3c -- tests/tooling/fixtures/native/recovery/positive/lowering_dispatch/msgsend_lookup_basic.m --out-dir tmp/artifacts/objc3c-native/m136-driver-shell/tests-objectivec --emit-prefix module_objc
```

- Validates both shell branches (`.objc3` frontend path and non-`.objc3` Objective-C path) using deterministic fixture inputs.
- Keeps lane-E proof artifacts isolated under `tmp/artifacts/objc3c-native/m136-driver-shell/`.

- `npm run test:objc3c`
  - Runs `scripts/check_objc3c_native_recovery_contract.ps1`.
  - Verifies deterministic replay for baseline cases and all recovery fixtures.
  - Verifies `.objc3` manifest frontend pipeline/integration-surface contract fields.
  - Also exposed as `npm run check:compiler-impl-recovery`.
- `npm run test:objc3c:matrix`
  - Runs `scripts/run_objc3c_native_fixture_matrix.ps1`.
  - Executes all recovery fixtures and writes a per-run summary JSON under `tmp/artifacts/objc3c-native/fixture-matrix/<run_id>/summary.json`.
  - Current observed behavior on 2026-02-27 in this workspace: exits zero with `status: PASS`.
- `npm run test:objc3c:perf-budget`
  - Runs `scripts/check_objc3c_native_perf_budget.ps1`.
  - Enforces total elapsed compile budget across positive recovery fixtures (default `4000` ms, override via `OBJC3C_NATIVE_PERF_MAX_MS` or `-MaxElapsedMs`).
  - Runs fail-closed cache proof:
    - Run 1 (unique cache key): requires exactly one `cache_hit=false` marker.
    - Run 2 (same key): requires exactly one `cache_hit=true` marker.
    - Hashes for emitted artifacts (`.obj`, `.manifest.json`, `.diagnostics.txt`, `.ll` for `.objc3`) must match between miss/hit runs.
  - Writes per-run summary JSON under `tmp/artifacts/objc3c-native/perf-budget/<run_id>/summary.json` with `cache_proof` evidence.
- `npm run test:objc3c:driver-shell-split`
  - Runs `scripts/check_objc3c_driver_shell_split_contract.ps1`.
  - Verifies `main.cpp` shell boundary: parse + exit-code mapping + delegation-only contract into `driver/*`.
  - Runs a deterministic two-pass smoke compile over `tests/tooling/fixtures/native/driver_split/smoke_compile_driver_shell_split.objc3`.
  - Writes per-run summary JSON under `tmp/artifacts/objc3c-native/driver-shell-split/<run_id>/summary.json`.
- `npm run test:objc3c:lexer-extraction-token-contract`
  - Runs `scripts/check_objc3c_lexer_extraction_token_contract.ps1`.
  - Verifies lexer subsystem extraction surfaces (`lex/*`, pipeline wiring, `TokenKind` contract markers).
  - Replays positive/negative lexer fixtures and enforces deterministic diagnostics/artifact contracts.
  - Writes per-run summary JSON under `tmp/artifacts/objc3c-native/lexer-extraction-token-contract/<run_id>/summary.json`.
- `npm run test:objc3c:lexer-parity`
  - Runs `python -m pytest tests/tooling/test_objc3c_lexer_parity.py -q`.
  - Verifies lexer extraction parity contract surfaces:
    - lexer module files exist,
    - pipeline consumes lexer header boundary,
    - CMake registers lexer target/source wiring.
- `npm run check:compiler-closeout:m137`
  - Runs `python scripts/check_m137_lexer_contract.py`.
  - Runs `npm run test:objc3c:lexer-extraction-token-contract` and `npm run test:objc3c:lexer-parity`.
  - Enforces fail-closed M137 lexer/token contract wiring across build/docs/CI/release surfaces.
- `npm run check:compiler-closeout:m138`
  - Runs `python scripts/check_m138_parser_ast_contract.py`.
  - Runs `npm run test:objc3c:parser-extraction-ast-builder-contract` and `npm run test:objc3c:parser-ast-extraction`.
  - Enforces fail-closed M138 parser extraction + AST builder contract wiring across build/docs/CI/release surfaces.
- `npm run check:compiler-closeout:m139`
  - Runs `python scripts/check_m139_sema_pass_manager_contract.py`.
  - Runs `npm run test:objc3c:sema-pass-manager-diagnostics-bus`.
  - Enforces fail-closed M139 sema pass-manager + diagnostics-bus contract wiring across build/docs/CI/release surfaces.
- `npm run test:objc3c:m140-boundary-contract`
  - Runs `python -m pytest tests/tooling/test_objc3c_frontend_library_entrypoint_extraction.py tests/tooling/test_objc3c_m140_boundary_contract.py tests/tooling/test_objc3c_sema_extraction.py tests/tooling/test_objc3c_sema_pass_manager_extraction.py tests/tooling/test_objc3c_lowering_contract.py tests/tooling/test_objc3c_ir_emitter_extraction.py -q`.
  - Verifies extracted frontend library entrypoint wiring, sema type-metadata handoff determinism, and lowering-to-IR boundary replay markers.
- `npm run check:compiler-closeout:m140`
  - Runs `python scripts/check_m140_frontend_library_boundary_contract.py`.
  - Runs `npm run test:objc3c:m140-boundary-contract`.
  - Enforces fail-closed M140 frontend-library boundary contract wiring across source/docs/package surfaces.
- `npm run test:objc3c:m141-target-topology`
  - Runs `python -m pytest tests/tooling/test_objc3c_driver_cli_extraction.py tests/tooling/test_objc3c_cmake_target_topology.py tests/tooling/test_objc3c_process_io_extraction.py tests/tooling/test_objc3c_parser_contract_sema_integration.py tests/tooling/test_objc3c_sema_extraction.py tests/tooling/test_objc3c_sema_pass_manager_extraction.py tests/tooling/test_objc3c_lowering_contract.py tests/tooling/test_objc3c_ir_emitter_extraction.py -q`.
  - Verifies deterministic stage-target linkage topology across CMake driver, sema/type-system boundary, lower/IR/runtime-ABI targets, and aggregate executable wiring.
- `npm run check:compiler-closeout:m141`
  - Runs `python scripts/check_m141_cmake_target_topology_contract.py`.
  - Runs `npm run test:objc3c:m141-target-topology`.
  - Enforces fail-closed M141 CMake targetization/linkage-topology contract wiring across source/docs/package/workflow surfaces.
- `npm run test:objc3c:m142-lowering-parity`
  - Runs `python -m pytest tests/tooling/test_objc3c_library_cli_parity.py tests/tooling/test_objc3c_c_api_runner_extraction.py tests/tooling/test_objc3c_frontend_lowering_parity_contract.py tests/tooling/test_objc3c_sema_cli_c_api_parity_surface.py -q`.
  - Verifies source-mode CLI/C API parity harness execution surfaces, C API runner contract snippets, and M142 docs/package wiring.
- `npm run check:objc3c:library-cli-parity:source`
  - Runs `python scripts/check_objc3c_library_cli_parity.py --source ... --cli-bin artifacts/bin/objc3c-native.exe --c-api-bin artifacts/bin/objc3c-frontend-c-api-runner.exe --cli-ir-object-backend clang`.
  - Executes CLI and C API runner on one source input and compares deterministic diagnostics/manifest/IR/object digest surfaces.
  - Writes replay artifacts under `tmp/artifacts/objc3c-native/m142/library-cli-parity/`.
- `npm run check:compiler-closeout:m142`
  - Runs `python scripts/check_m142_frontend_lowering_parity_contract.py`.
  - Runs `npm run test:objc3c:m142-lowering-parity`.
  - Enforces fail-closed M142 parity harness wiring across source/docs/package/workflow surfaces.

## M142 validation/perf CLI and C API parity harness runbook

For deterministic M142 validation and replay evidence, execute the parity harness validation commands against source-mode CLI and C API runs, then enforce closeout contract drift checks.

- `npm run test:objc3c:m142-lowering-parity`
- `npm run check:objc3c:library-cli-parity:source`
- `npm run check:compiler-closeout:m142`

Replay artifact anchors:

- `tmp/artifacts/objc3c-native/m142/library-cli-parity/work/`
- `tmp/artifacts/objc3c-native/m142/library-cli-parity/summary.json`
- `artifacts/bin/objc3c-native.exe`
- `artifacts/bin/objc3c-frontend-c-api-runner.exe`

Recommended M142 validation contract check:

- `python -m pytest tests/tooling/test_objc3c_m142_validation_cli_c_api_parity_contract.py -q`
- `npm run test:objc3c:m143-artifact-governance`
  - Runs `python -m pytest tests/tooling/test_objc3c_library_cli_parity.py tests/tooling/test_objc3c_driver_cli_extraction.py tests/tooling/test_objc3c_c_api_runner_extraction.py tests/tooling/test_objc3c_parser_extraction.py tests/tooling/test_objc3c_parser_ast_builder_extraction.py tests/tooling/test_objc3c_sema_extraction.py tests/tooling/test_objc3c_sema_pass_manager_extraction.py tests/tooling/test_objc3c_frontend_types_extraction.py tests/tooling/test_objc3c_lowering_contract.py tests/tooling/test_objc3c_ir_emitter_extraction.py tests/tooling/test_objc3c_m143_artifact_tmp_governance_contract.py tests/tooling/test_objc3c_m143_sema_type_system_tmp_governance_contract.py tests/tooling/test_objc3c_m143_lowering_runtime_abi_tmp_governance_contract.py tests/tooling/test_check_m143_artifact_tmp_governance_contract.py -q`.
  - Verifies tmp-governed default output paths, parser/AST lane-A coverage wiring, sema/type-system lane-B governance, lowering/LLVM IR/runtime-ABI lane-C governance, source-mode work-root governance, and M143 docs/package wiring.
- `npm run check:objc3c:library-cli-parity:source:m143`
  - Runs `python scripts/check_objc3c_library_cli_parity.py --source ... --summary-out tmp/artifacts/compilation/objc3c-native/m143/library-cli-parity/summary.json ...`.
  - Enforces deterministic replay roots and fail-closed stale/missing artifact checks under tmp-governed paths.
- `npm run check:compiler-closeout:m143`
  - Runs `python scripts/check_m143_artifact_tmp_governance_contract.py`.
  - Runs `npm run test:objc3c:m143-artifact-governance`.
  - Enforces fail-closed M143 tmp-governance wiring across source/docs/package/workflow surfaces.
- `npm run test:objc3c:m144-llvm-capability-discovery`
  - Runs `python -m pytest tests/tooling/test_probe_objc3c_llvm_capabilities.py tests/tooling/test_objc3c_library_cli_parity.py::test_parity_source_mode_routes_backend_from_capabilities_when_enabled tests/tooling/test_objc3c_library_cli_parity.py::test_parity_source_mode_fail_closes_when_capability_parity_is_unavailable tests/tooling/test_objc3c_library_cli_parity.py::test_parity_source_mode_fail_closes_when_capability_routing_is_requested_without_summary tests/tooling/test_objc3c_driver_llvm_capability_routing_extraction.py tests/tooling/test_objc3c_driver_cli_extraction.py tests/tooling/test_objc3c_m144_llvm_capability_discovery_contract.py tests/tooling/test_check_m144_llvm_capability_discovery_contract.py -q`.
  - Verifies capability probe packet behavior, fail-closed backend routing extraction, and M144 docs/package wiring.
- `npm run check:objc3c:llvm-capabilities`
  - Runs `python scripts/probe_objc3c_llvm_capabilities.py --summary-out tmp/artifacts/objc3c-native/m144/llvm_capabilities/summary.json`.
  - Produces deterministic capability summary packet used by M144 routed parity workflows.
- `npm run check:compiler-closeout:m144`
  - Runs `python scripts/check_m144_llvm_capability_discovery_contract.py`.
  - Runs `npm run test:objc3c:m144-llvm-capability-discovery`.
  - Enforces fail-closed M144 capability discovery wiring across source/docs/package/workflow surfaces.
- `npm run test:objc3c:m145-direct-llvm-matrix`
  - Runs `powershell -NoProfile -ExecutionPolicy Bypass -File scripts/check_objc3c_sema_pass_manager_diagnostics_bus_contract.ps1`.
  - Runs `python -m pytest tests/tooling/test_check_m145_direct_llvm_matrix_contract.py -q`.
  - Verifies lane-B sema/type-system direct LLVM object-emission matrix coverage, including forced missing-llc fail-closed behavior.
  - Verifies matrix checks such as `runtime.positive.matrix.llvm_direct_forced_missing_llc.exit_codes` and backend-invariant negative diagnostics.
- `npm run test:objc3c:m145-direct-llvm-matrix:lane-d`
  - Runs `powershell -NoProfile -ExecutionPolicy Bypass -File scripts/check_conformance_suite.ps1`.
  - Runs `npm run test:objc3c:perf-budget`.
  - Verifies lane-D conformance/perf anchors for `M145-D001` and fail-closed matrix coverage.
- `npm run check:compiler-closeout:m145`
  - Runs `python scripts/check_m145_direct_llvm_matrix_contract.py`.
  - Runs `npm run test:objc3c:m145-direct-llvm-matrix`.
  - Runs `npm run test:objc3c:m145-direct-llvm-matrix:lane-d`.
  - Runs `python scripts/spec_lint.py --glob "docs/contracts/direct_llvm_emission_expectations.md"`.
  - Enforces fail-closed M145 direct LLVM object-emission matrix wiring across lane-B sema/type-system, lane-C runtime-ABI, and lane-D validation/conformance/perf surfaces.
- `npm run proof:objc3c`
  - Runs `scripts/run_objc3c_native_compile_proof.ps1`.
  - Replays `tests/tooling/fixtures/native/hello.objc3` twice and writes `artifacts/compilation/objc3c-native/proof_20260226/digest.json` on success.
- `npm run test:objc3c:lane-e`
  - Runs lane-E deterministic validation chain:
    - `npm run test:objc3c`
    - `npm run test:objc3c:diagnostics-replay-proof`
    - `npm run test:objc3c:parser-replay-proof`
    - `npm run test:objc3c:sema-pass-manager-diagnostics-bus`
    - `npm run test:objc3c:driver-shell-split`
    - `npm run test:objc3c:lexer-extraction-token-contract`
    - `npm run test:objc3c:lexer-parity`
    - `npm run test:objc3c:perf-budget`
    - `npm run test:objc3c:lowering-regression`
    - `npm run test:objc3c:lowering-replay-proof`
    - `npm run test:objc3c:typed-abi-replay-proof`
    - `npm run test:objc3c:execution-smoke`
    - `npm run test:objc3c:execution-replay-proof`
    - `npm run check:compiler-execution:m23`
    - `npm run check:compiler-execution:m24`
    - `npm run check:compiler-execution:m25:lane-e`
  - Fails closed if any stage exits non-zero.
- `powershell -NoProfile -ExecutionPolicy Bypass -File scripts/check_objc3c_diagnostics_replay_proof.ps1`
  - Runs diagnostics regression twice with distinct run IDs.
  - Canonicalizes deterministic per-fixture result surface and asserts SHA256 equality across replay.
  - Writes proof summary under `tmp/artifacts/objc3c-native/diagnostics-replay-proof/<proof_run_id>/summary.json`.
- `powershell -NoProfile -ExecutionPolicy Bypass -File scripts/check_objc3c_parser_replay_proof.ps1`
  - Replays dedicated malformed loop/control + assignment (including compound-assignment/update and `%=` parser negatives) + bitwise/shift + modulo + do-while + for + for-step + switch + conditional parser negatives (`negative_loop_control_parser_*.objc3`, `negative_assignment_parser_*.objc3`, `negative_bitwise_parser_*.objc3`, `negative_modulo_parser_*.objc3`, `negative_do_while_parser_*.objc3`, `negative_for_parser_*.objc3`, `negative_for_step_parser_*.objc3`, `negative_switch_parser_*.objc3`, `negative_conditional_parser_*.objc3`) twice per fixture.
  - Enforces deterministic non-zero exit codes and deterministic `module.diagnostics.txt` / `module.diagnostics.json` hashes and code sets across replay.
  - Enforces expected fixture diagnostic headers and parser-only expected codes (`O3P*`).
  - Writes proof summary under `tmp/artifacts/objc3c-native/parser-replay-proof/<proof_run_id>/summary.json`.
- `npm run test:objc3c:parser-extraction-ast-builder-contract`
  - Runs `scripts/check_objc3c_parser_extraction_ast_builder_contract.ps1`.
  - Verifies parser extraction boundaries and AST builder scaffold markers in `parse/*`, `ast/*`, pipeline wiring, and CMake parser target registration.
  - Replays one positive parser scaffold fixture and selected negative parser fixtures with deterministic diagnostics/artifact assertions.
  - Writes per-run summary JSON under `tmp/artifacts/objc3c-native/parser-extraction-ast-builder-contract/<run_id>/summary.json`.
- `npm run test:objc3c:sema-pass-manager-diagnostics-bus`
  - Runs `scripts/check_objc3c_sema_pass_manager_diagnostics_bus_contract.ps1`.
  - Runs `python -m pytest tests/tooling/test_objc3c_sema_extraction.py tests/tooling/test_objc3c_sema_pass_manager_extraction.py tests/tooling/test_objc3c_parser_contract_sema_integration.py tests/tooling/test_objc3c_pure_contract_extraction.py tests/tooling/test_objc3c_frontend_types_extraction.py -q`.
  - Verifies sema module extraction boundaries, parser-contract integration, pure-contract extraction boundary, and pipeline diagnostics-bus type surfaces.
  - Replays positive/negative sema fixtures and enforces deterministic diagnostics/artifact contracts for pass-manager + diagnostics-bus extraction.
  - Writes per-run summary JSON under `tmp/artifacts/compilation/objc3c-native/m143/sema-pass-manager-diagnostics-bus-contract/<run_id>/summary.json`.
  - Uses deterministic default run id `m143-sema-type-system-default` unless `OBJC3C_SEMA_PASS_MANAGER_DIAG_BUS_CONTRACT_RUN_ID` is explicitly provided.
- `powershell -NoProfile -ExecutionPolicy Bypass -File scripts/run_objc3c_lowering_regression_suite.ps1`
  - Replays all recovery fixtures (positive and negative) twice per fixture.
  - Includes optional Objective-C dispatch fixture roots when present (`recovery/positive/lowering_dispatch`, then `dispatch/positive`).
  - Enforces deterministic diagnostics for every fixture, deterministic manifests/IR for positive fixtures, and fail-closed artifact absence for negative fixtures.
  - For fixtures with `<fixture>.dispatch-ir.expect.txt`, emits replay IR via `clang -S -emit-llvm` and enforces deterministic dispatch IR marker matches across replay.
  - For fixtures with `<fixture>.objc3-ir.expect.txt`, enforces deterministic native-lowered IR marker matches across replay for emitted `module.ll`.
  - Optional clang override: `OBJC3C_NATIVE_LOWERING_CLANG_PATH=<clang executable>`.
  - Uses deterministic default run id `m143-lane-c-lowering-regression-default` unless `OBJC3C_NATIVE_LOWERING_RUN_ID` is explicitly provided.
  - Writes per-run summary under `tmp/artifacts/objc3c-native/lowering-regression/<run_id>/summary.json` plus stable latest summary at `tmp/artifacts/objc3c-native/lowering-regression/latest-summary.json`.
- `powershell -NoProfile -ExecutionPolicy Bypass -File scripts/check_objc3c_typed_abi_replay_proof.ps1`
  - Replays typed-signature `.objc3` fixtures twice and enforces deterministic `module.ll` plus required typed ABI marker token presence from adjacent `.objc3-ir.expect.txt` files.
  - Uses deterministic default run id `m143-lane-c-typed-abi-default` unless `OBJC3C_TYPED_ABI_REPLAY_PROOF_RUN_ID` is explicitly provided.
  - Writes proof summary under `tmp/artifacts/objc3c-native/typed-abi-replay-proof/<run_id>/summary.json`.
- `powershell -NoProfile -ExecutionPolicy Bypass -File scripts/check_objc3c_execution_replay_proof.ps1`
  - Runs execution smoke twice with distinct run IDs.
  - Canonicalizes summary surfaces (excluding run-path entropy) and enforces SHA256 equality across replay.
  - Writes proof summary under `tmp/artifacts/objc3c-native/execution-replay-proof/<proof_run_id>/summary.json`.
- `powershell -NoProfile -ExecutionPolicy Bypass -File scripts/check_objc3c_lowering_replay_proof.ps1`
  - Runs lowering regression twice with distinct run IDs.
  - Captures each run's deterministic summary and asserts SHA256 equality across replay.
  - Uses deterministic default proof run id `m143-lane-c-lowering-replay-proof-default` unless `OBJC3C_NATIVE_LOWERING_REPLAY_PROOF_RUN_ID` is explicitly provided.
  - Writes proof summary under `tmp/artifacts/objc3c-native/lowering-replay-proof/<proof_run_id>/summary.json`.

Direct script equivalents:

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File scripts/check_objc3c_native_recovery_contract.ps1
powershell -NoProfile -ExecutionPolicy Bypass -File scripts/run_objc3c_native_fixture_matrix.ps1
powershell -NoProfile -ExecutionPolicy Bypass -File scripts/check_objc3c_native_perf_budget.ps1
powershell -NoProfile -ExecutionPolicy Bypass -File scripts/run_objc3c_native_compile_proof.ps1
powershell -NoProfile -ExecutionPolicy Bypass -File scripts/check_objc3c_parser_replay_proof.ps1
powershell -NoProfile -ExecutionPolicy Bypass -File scripts/check_objc3c_parser_extraction_ast_builder_contract.ps1
powershell -NoProfile -ExecutionPolicy Bypass -File scripts/run_objc3c_lowering_regression_suite.ps1
powershell -NoProfile -ExecutionPolicy Bypass -File scripts/check_objc3c_typed_abi_replay_proof.ps1
powershell -NoProfile -ExecutionPolicy Bypass -File scripts/check_objc3c_native_execution_smoke.ps1
powershell -NoProfile -ExecutionPolicy Bypass -File scripts/check_objc3c_execution_replay_proof.ps1
powershell -NoProfile -ExecutionPolicy Bypass -File scripts/check_objc3c_driver_shell_split_contract.ps1
powershell -NoProfile -ExecutionPolicy Bypass -File scripts/check_objc3c_lexer_extraction_token_contract.ps1
powershell -NoProfile -ExecutionPolicy Bypass -File scripts/check_objc3c_sema_pass_manager_diagnostics_bus_contract.ps1
python scripts/check_m137_lexer_contract.py
python scripts/check_m139_sema_pass_manager_contract.py
python scripts/check_m142_frontend_lowering_parity_contract.py
python scripts/check_m143_artifact_tmp_governance_contract.py
python scripts/check_m144_llvm_capability_discovery_contract.py
python scripts/check_m145_direct_llvm_matrix_contract.py
python -m pytest tests/tooling/test_objc3c_lexer_parity.py -q
python scripts/check_m23_execution_readiness.py
python scripts/check_m24_execution_readiness.py
```

## M223 operator quickstart (docs+CI parity)

For deterministic day-to-day operator usage, run this minimal sequence from repo root:

```powershell
npm run build:objc3c-native
npm run compile:objc3c -- tests/tooling/fixtures/native/hello.objc3 --out-dir tmp/artifacts/compilation/objc3c-native/m223/quickstart --emit-prefix module
npm run test:objc3c:m222-compatibility-migration
python scripts/build_objc3c_native_docs.py --check
```

Operational intent:

- `build:objc3c-native` verifies native toolchain wiring and executable output.
- `compile:objc3c` verifies deterministic compile artifact generation under `tmp/`.
- `test:objc3c:m222-compatibility-migration` verifies compatibility/migration contract surfaces.
- `build_objc3c_native_docs.py --check` verifies generated docs are in sync with source fragments.

## M223 validation/perf triage sequence

When validating release-facing behavior after compiler/runtime changes, run this ordered triage:

```powershell
npm run test:objc3c:m145-direct-llvm-matrix
npm run test:objc3c:execution-smoke
npm run test:objc3c:execution-replay-proof
npm run test:objc3c:perf-budget
```

Expected evidence roots:

- `tmp/artifacts/objc3c-native/execution-smoke/<run_id>/summary.json`
- `tmp/artifacts/objc3c-native/execution-replay-proof/<proof_run_id>/summary.json`
- `tmp/artifacts/objc3c-native/perf-budget/<run_id>/summary.json`

Fail-closed operator guidance:

1. treat non-zero exit in the above sequence as a hard stop.
2. inspect failing summary JSON first, then per-case logs under the same run root.
3. do not interpret replay/perf regressions without comparing both run1/run2 evidence packets.

## M224 validation/conformance/perf release readiness

For M224 release-readiness operators, run this fail-closed order from repo root:

```powershell
npm run test:objc3c:m145-direct-llvm-matrix
npm run test:objc3c:m145-direct-llvm-matrix:lane-d
npm run test:objc3c:execution-smoke
npm run test:objc3c:execution-replay-proof
```

Fail-closed release criteria:

1. any non-zero exit code in sequence is a hard stop; do not execute later commands.
2. `test:objc3c:m145-direct-llvm-matrix:lane-d` must execute both `scripts/check_conformance_suite.ps1` and `npm run test:objc3c:perf-budget`; treat either missing as release-blocking.
3. release-ready evidence requires both `tmp/artifacts/objc3c-native/execution-smoke/<run_id>/summary.json` and `tmp/artifacts/objc3c-native/execution-replay-proof/<proof_run_id>/summary.json`; missing summary evidence is a failure.

Contract check:

```powershell
python -m pytest tests/tooling/test_objc3c_m224_validation_release_contract.py -q
```

## M225 validation/perf roadmap seeding runbook

From repo root, run this deterministic order and stop immediately on the first non-zero exit:

## M146 validation @interface/@implementation runbook

From repo root, execute deterministic M146 contract checks in lane order:

- `python -m pytest tests/tooling/test_objc3c_m146_frontend_interface_implementation_contract.py -q`
- `python -m pytest tests/tooling/test_objc3c_m146_sema_interface_implementation_contract.py -q`
- `python -m pytest tests/tooling/test_objc3c_m146_lowering_interface_implementation_contract.py -q`
- `python -m pytest tests/tooling/test_objc3c_m146_validation_interface_implementation_contract.py -q`
- `npm run check:objc3c:m146-interface-implementation`

## M147 validation @protocol/@category runbook

From repo root, execute deterministic M147 contract checks in lane order:

- `python -m pytest tests/tooling/test_objc3c_m147_frontend_protocol_category_contract.py -q`
- `python -m pytest tests/tooling/test_objc3c_m147_sema_protocol_category_contract.py -q`
- `python -m pytest tests/tooling/test_objc3c_m147_lowering_protocol_category_contract.py -q`
- `python -m pytest tests/tooling/test_objc3c_m147_validation_protocol_category_contract.py -q`
- `npm run check:objc3c:m147-protocol-category`

## M148 validation selector-normalized method declaration runbook

From repo root, execute deterministic M148 contract checks in lane order:

- `python -m pytest tests/tooling/test_objc3c_m148_frontend_selector_normalization_contract.py -q`
- `python -m pytest tests/tooling/test_objc3c_m148_sema_selector_normalization_contract.py -q`
- `python -m pytest tests/tooling/test_objc3c_m148_lowering_selector_normalization_contract.py -q`
- `python -m pytest tests/tooling/test_objc3c_m148_validation_selector_normalization_contract.py -q`
- `npm run check:objc3c:m148-selector-normalization`

## M149 validation @property grammar and attribute parsing runbook

From repo root, execute deterministic M149 contract checks in lane order:

- `python -m pytest tests/tooling/test_objc3c_m149_frontend_property_attribute_contract.py -q`
- `python -m pytest tests/tooling/test_objc3c_m149_sema_property_attribute_contract.py -q`
- `python -m pytest tests/tooling/test_objc3c_m149_lowering_property_attribute_contract.py -q`
- `python -m pytest tests/tooling/test_objc3c_m149_validation_property_attribute_contract.py -q`
- `npm run check:objc3c:m149-property-attributes`

## M150 validation object-pointer declarators, nullability, lightweight generics parse runbook

From repo root, execute deterministic M150 contract checks in lane order:

- `python -m pytest tests/tooling/test_objc3c_m150_frontend_object_pointer_nullability_generics_contract.py -q`
- `python -m pytest tests/tooling/test_objc3c_m150_sema_object_pointer_nullability_generics_contract.py -q`
- `python -m pytest tests/tooling/test_objc3c_m150_lowering_object_pointer_nullability_generics_contract.py -q`
- `python -m pytest tests/tooling/test_objc3c_m150_validation_object_pointer_nullability_generics_contract.py -q`
- `npm run check:objc3c:m150-object-pointer-nullability-generics`

## M151 validation symbol graph and scope resolution overhaul runbook

From repo root, execute deterministic M151 contract checks in lane order:

- `python -m pytest tests/tooling/test_objc3c_m151_frontend_symbol_graph_scope_resolution_contract.py -q`
- `python -m pytest tests/tooling/test_objc3c_m151_sema_symbol_graph_scope_resolution_contract.py -q`
- `python -m pytest tests/tooling/test_objc3c_m151_lowering_symbol_graph_scope_resolution_contract.py -q`
- `python -m pytest tests/tooling/test_objc3c_m151_validation_symbol_graph_scope_resolution_contract.py -q`
- `npm run check:objc3c:m151-symbol-graph-scope-resolution`

## M152 validation class-protocol-category semantic linking runbook

From repo root, execute deterministic M152 contract checks in lane order:

- `python -m pytest tests/tooling/test_objc3c_m152_frontend_class_protocol_category_linking_contract.py -q`
- `python -m pytest tests/tooling/test_objc3c_m152_sema_class_protocol_category_linking_contract.py -q`
- `python -m pytest tests/tooling/test_objc3c_m152_lowering_class_protocol_category_linking_contract.py -q`
- `python -m pytest tests/tooling/test_objc3c_m152_validation_class_protocol_category_linking_contract.py -q`
- `npm run check:objc3c:m152-class-protocol-category-linking`

## M153 validation method lookup, override, and conflict semantics runbook

From repo root, execute deterministic M153 contract checks in lane order:

- `python -m pytest tests/tooling/test_objc3c_m153_frontend_method_lookup_override_conflict_contract.py -q`
- `python -m pytest tests/tooling/test_objc3c_m153_sema_method_lookup_override_conflict_contract.py -q`
- `python -m pytest tests/tooling/test_objc3c_m153_lowering_method_lookup_override_conflict_contract.py -q`
- `python -m pytest tests/tooling/test_objc3c_m153_validation_method_lookup_override_conflict_contract.py -q`
- `python -m pytest tests/tooling/test_objc3c_m153_integration_method_lookup_override_conflict_contract.py -q`
- `npm run check:objc3c:m153-method-lookup-override-conflicts`

```powershell
npm run test:objc3c:m145-direct-llvm-matrix
npm run test:objc3c:m145-direct-llvm-matrix:lane-d
npm run test:objc3c:execution-smoke
npm run test:objc3c:execution-replay-proof
```

Evidence packet fields for next-cycle milestone seeding:

- `tmp/artifacts/objc3c-native/perf-budget/<run_id>/summary.json`
  - `status`
  - `total_elapsed_ms`
  - `budget_margin_ms`
  - `cache_proof.status`
  - `cache_proof.run1.cache_hit`
  - `cache_proof.run2.cache_hit`
- `tmp/artifacts/objc3c-native/execution-smoke/<run_id>/summary.json`
  - `status`
  - `total`
  - `passed`
  - `failed`
  - `results[*].runtime_dispatch_symbol`
- `tmp/artifacts/objc3c-native/execution-replay-proof/<proof_run_id>/summary.json`
  - `status`
  - `run1_sha256`
  - `run2_sha256`
  - `run1_summary`
  - `run2_summary`

Contract check:

```powershell
python -m pytest tests/tooling/test_objc3c_m225_validation_roadmap_seed_contract.py -q
```

## M154 validation property synthesis and ivar binding semantics runbook

From repo root, execute deterministic M154 contract checks in lane order:

- `python -m pytest tests/tooling/test_objc3c_m154_frontend_property_synthesis_ivar_binding_contract.py -q`
- `python -m pytest tests/tooling/test_objc3c_m154_sema_property_synthesis_ivar_binding_contract.py -q`
- `python -m pytest tests/tooling/test_objc3c_m154_lowering_property_synthesis_ivar_binding_contract.py -q`
- `python -m pytest tests/tooling/test_objc3c_m154_validation_property_synthesis_ivar_binding_contract.py -q`
- `python -m pytest tests/tooling/test_objc3c_m154_integration_property_synthesis_ivar_binding_contract.py -q`
- `npm run check:objc3c:m154-property-synthesis-ivar-bindings`

## M155 validation id/class/SEL/object-pointer typecheck semantics runbook

From repo root, execute deterministic M155 contract checks in lane order:

- `python -m pytest tests/tooling/test_objc3c_m155_frontend_id_class_sel_object_pointer_typecheck_contract.py -q`
- `python -m pytest tests/tooling/test_objc3c_m155_sema_id_class_sel_object_pointer_typecheck_contract.py -q`
- `python -m pytest tests/tooling/test_objc3c_m155_lowering_id_class_sel_object_pointer_typecheck_contract.py -q`
- `npm run check:objc3c:m155-id-class-sel-object-pointer-typecheck-contracts`
- `npm run check:compiler-closeout:m155`

## M221 validation/perf GA blocker burn-down runbook

From repo root, run this deterministic blocker-burn sequence and fail closed on first non-zero exit:

```powershell
npm run test:objc3c:m145-direct-llvm-matrix
npm run test:objc3c:m145-direct-llvm-matrix:lane-d
npm run test:objc3c:execution-smoke
npm run test:objc3c:execution-replay-proof
```

GA blocker evidence packet fields:

- `tmp/artifacts/objc3c-native/perf-budget/<run_id>/summary.json`
  - `status`
  - `total_elapsed_ms`
  - `budget_margin_ms`
  - `cache_proof.status`
  - `cache_proof.run1.cache_hit`
  - `cache_proof.run2.cache_hit`
- `tmp/artifacts/conformance-suite/<target>/summary.json`
  - `suite.status`
  - `suite.failures`
  - `matrix.total_cases`
  - `matrix.failed_cases`
- `tmp/artifacts/objc3c-native/execution-smoke/<run_id>/summary.json`
  - `status`
  - `total`
  - `passed`
  - `failed`
  - `results[*].runtime_dispatch_symbol`
- `tmp/artifacts/objc3c-native/execution-replay-proof/<proof_run_id>/summary.json`
  - `status`
  - `run1_sha256`
  - `run2_sha256`
  - `run1_summary`
  - `run2_summary`

Contract check:

```powershell
python -m pytest tests/tooling/test_objc3c_m221_validation_ga_blocker_contract.py -q
```

## M220 validation/perf public-beta triage runbook

Public-beta triage loop requires deterministic validation/perf replay packets and strict command ordering.

```powershell
npm run test:objc3c:m145-direct-llvm-matrix
npm run test:objc3c:m145-direct-llvm-matrix:lane-d
npm run test:objc3c:execution-smoke
npm run test:objc3c:execution-replay-proof
```

Public-beta evidence packet fields:

- `tmp/artifacts/objc3c-native/perf-budget/<run_id>/summary.json`
  - `status`
  - `total_elapsed_ms`
  - `budget_margin_ms`
  - `cache_proof.status`
  - `cache_proof.run1.cache_hit`
  - `cache_proof.run2.cache_hit`
- `tmp/artifacts/conformance-suite/<target>/summary.json`
  - `suite.status`
  - `suite.failures`
  - `matrix.total_cases`
  - `matrix.failed_cases`
- `tmp/artifacts/objc3c-native/execution-smoke/<run_id>/summary.json`
  - `status`
  - `total`
  - `passed`
  - `failed`
  - `results[*].runtime_dispatch_symbol`
- `tmp/artifacts/objc3c-native/execution-replay-proof/<proof_run_id>/summary.json`
  - `status`
  - `run1_sha256`
  - `run2_sha256`
  - `run1_summary`
  - `run2_summary`

Contract check:

```powershell
python -m pytest tests/tooling/test_objc3c_m220_validation_public_beta_contract.py -q
```

## M219 validation/perf cross-platform parity runbook

Cross-platform parity validation runs the same deterministic command sequence on Windows, Linux, and macOS.

```powershell
npm run test:objc3c:m145-direct-llvm-matrix
npm run test:objc3c:m145-direct-llvm-matrix:lane-d
npm run test:objc3c:execution-smoke
npm run test:objc3c:execution-replay-proof
```

Cross-platform parity evidence packet fields:

- `tmp/artifacts/objc3c-native/perf-budget/<run_id>/summary.json`
  - `platform`
  - `status`
  - `total_elapsed_ms`
  - `budget_margin_ms`
  - `cache_proof.status`
- `tmp/artifacts/conformance-suite/<target>/summary.json`
  - `platform`
  - `suite.status`
  - `suite.failures`
  - `matrix.total_cases`
  - `matrix.failed_cases`
- `tmp/artifacts/objc3c-native/execution-smoke/<run_id>/summary.json`
  - `platform`
  - `status`
  - `total`
  - `passed`
  - `failed`
  - `results[*].runtime_dispatch_symbol`
- `tmp/artifacts/objc3c-native/execution-replay-proof/<proof_run_id>/summary.json`
  - `platform`
  - `status`
  - `run1_sha256`
  - `run2_sha256`
  - `run1_summary`
  - `run2_summary`

Contract check:

```powershell
python -m pytest tests/tooling/test_objc3c_m219_validation_cross_platform_contract.py -q
```

## M218 validation/perf RC provenance runbook

RC provenance validation runs deterministic test commands and captures attestable evidence packets.

```powershell
npm run test:objc3c:m145-direct-llvm-matrix
npm run test:objc3c:m145-direct-llvm-matrix:lane-d
npm run test:objc3c:execution-smoke
npm run test:objc3c:execution-replay-proof
```

RC provenance evidence packet fields:

- `tmp/artifacts/objc3c-native/perf-budget/<run_id>/summary.json`
  - `status`
  - `total_elapsed_ms`
  - `budget_margin_ms`
  - `attestation_sha256`
  - `provenance.bundle_id`
- `tmp/artifacts/conformance-suite/<target>/summary.json`
  - `suite.status`
  - `suite.failures`
  - `matrix.total_cases`
  - `matrix.failed_cases`
  - `attestation_sha256`
- `tmp/artifacts/objc3c-native/execution-smoke/<run_id>/summary.json`
  - `status`
  - `total`
  - `passed`
  - `failed`
  - `results[*].runtime_dispatch_symbol`
  - `attestation_sha256`
- `tmp/artifacts/objc3c-native/execution-replay-proof/<proof_run_id>/summary.json`
  - `status`
  - `run1_sha256`
  - `run2_sha256`
  - `run1_summary`
  - `run2_summary`
  - `attestation_sha256`

Contract check:

```powershell
python -m pytest tests/tooling/test_objc3c_m218_validation_rc_provenance_contract.py -q
```

## M217 validation/perf differential runbook

Differential testing runbook compares deterministic execution evidence against baseline toolchains.

```powershell
npm run test:objc3c:m145-direct-llvm-matrix
npm run test:objc3c:m145-direct-llvm-matrix:lane-d
npm run test:objc3c:execution-smoke
npm run test:objc3c:execution-replay-proof
```

Differential evidence packet fields:

- `tmp/artifacts/objc3c-native/perf-budget/<run_id>/summary.json`
  - `status`
  - `total_elapsed_ms`
  - `budget_margin_ms`
  - `baseline_delta_ms`
- `tmp/artifacts/conformance-suite/<target>/summary.json`
  - `suite.status`
  - `suite.failures`
  - `matrix.total_cases`
  - `matrix.failed_cases`
  - `baseline_diff_count`
- `tmp/artifacts/objc3c-native/execution-smoke/<run_id>/summary.json`
  - `status`
  - `total`
  - `passed`
  - `failed`
  - `results[*].runtime_dispatch_symbol`
  - `baseline_diff_count`
- `tmp/artifacts/objc3c-native/execution-replay-proof/<proof_run_id>/summary.json`
  - `status`
  - `run1_sha256`
  - `run2_sha256`
  - `run1_summary`
  - `run2_summary`
  - `baseline_diff_count`

Contract check:

```powershell
python -m pytest tests/tooling/test_objc3c_m217_validation_differential_contract.py -q
```

## M216 validation/perf conformance suite runbook

Conformance suite v1 validation runs deterministic command order and records spec-mapped evidence packets.

```powershell
npm run test:objc3c:m145-direct-llvm-matrix
npm run test:objc3c:m145-direct-llvm-matrix:lane-d
npm run test:objc3c:execution-smoke
npm run test:objc3c:execution-replay-proof
```

Conformance suite evidence packet fields:

- `tmp/artifacts/objc3c-native/perf-budget/<run_id>/summary.json`
  - `status`
  - `total_elapsed_ms`
  - `budget_margin_ms`
  - `spec_section_map`
- `tmp/artifacts/conformance-suite/<target>/summary.json`
  - `suite.status`
  - `suite.failures`
  - `matrix.total_cases`
  - `matrix.failed_cases`
  - `spec_section_map`
- `tmp/artifacts/objc3c-native/execution-smoke/<run_id>/summary.json`
  - `status`
  - `total`
  - `passed`
  - `failed`
  - `results[*].runtime_dispatch_symbol`
  - `spec_section_map`
- `tmp/artifacts/objc3c-native/execution-replay-proof/<proof_run_id>/summary.json`
  - `status`
  - `run1_sha256`
  - `run2_sha256`
  - `run1_summary`
  - `run2_summary`
  - `spec_section_map`

Contract check:

```powershell
python -m pytest tests/tooling/test_objc3c_m216_validation_conformance_contract.py -q
```

## M215 validation/perf SDK packaging runbook

SDK packaging validation runbook ensures deterministic evidence for IDE-consumable toolchain artifacts.

```powershell
npm run test:objc3c:m145-direct-llvm-matrix
npm run test:objc3c:m145-direct-llvm-matrix:lane-d
npm run test:objc3c:execution-smoke
npm run test:objc3c:execution-replay-proof
```

SDK packaging evidence packet fields:

- `tmp/artifacts/objc3c-native/perf-budget/<run_id>/summary.json`
  - `status`
  - `total_elapsed_ms`
  - `budget_margin_ms`
  - `sdk_bundle_id`
- `tmp/artifacts/conformance-suite/<target>/summary.json`
  - `suite.status`
  - `suite.failures`
  - `matrix.total_cases`
  - `matrix.failed_cases`
  - `sdk_bundle_id`
- `tmp/artifacts/objc3c-native/execution-smoke/<run_id>/summary.json`
  - `status`
  - `total`
  - `passed`
  - `failed`
  - `results[*].runtime_dispatch_symbol`
  - `sdk_bundle_id`
- `tmp/artifacts/objc3c-native/execution-replay-proof/<proof_run_id>/summary.json`
  - `status`
  - `run1_sha256`
  - `run2_sha256`
  - `run1_summary`
  - `run2_summary`
  - `sdk_bundle_id`

Contract check:

```powershell
python -m pytest tests/tooling/test_objc3c_m215_validation_sdk_packaging_contract.py -q
```

## M214 validation/perf daemonized compiler runbook

Daemon/watch validation runbook verifies deterministic incremental behavior and replay evidence.

```powershell
npm run test:objc3c:m145-direct-llvm-matrix
npm run test:objc3c:m145-direct-llvm-matrix:lane-d
npm run test:objc3c:execution-smoke
npm run test:objc3c:execution-replay-proof
```

Daemonized evidence packet fields:

- `tmp/artifacts/objc3c-native/perf-budget/<run_id>/summary.json`
  - `status`
  - `total_elapsed_ms`
  - `budget_margin_ms`
  - `incremental_cycle_id`
- `tmp/artifacts/conformance-suite/<target>/summary.json`
  - `suite.status`
  - `suite.failures`
  - `matrix.total_cases`
  - `matrix.failed_cases`
  - `incremental_cycle_id`
- `tmp/artifacts/objc3c-native/execution-smoke/<run_id>/summary.json`
  - `status`
  - `total`
  - `passed`
  - `failed`
  - `results[*].runtime_dispatch_symbol`
  - `incremental_cycle_id`
- `tmp/artifacts/objc3c-native/execution-replay-proof/<proof_run_id>/summary.json`
  - `status`
  - `run1_sha256`
  - `run2_sha256`
  - `run1_summary`
  - `run2_summary`
  - `incremental_cycle_id`

Contract check:

```powershell
python -m pytest tests/tooling/test_objc3c_m214_validation_daemonized_contract.py -q
```

## M213 validation/perf debug-info fidelity runbook

Debug-fidelity validation runbook verifies deterministic evidence for DWARF/PDB emission and source-level stepping.

```powershell
npm run test:objc3c:m145-direct-llvm-matrix
npm run test:objc3c:m145-direct-llvm-matrix:lane-d
npm run test:objc3c:execution-smoke
npm run test:objc3c:execution-replay-proof
```

Debug-fidelity evidence packet fields:

- `tmp/artifacts/objc3c-native/perf-budget/<run_id>/summary.json`
  - `status`
  - `total_elapsed_ms`
  - `budget_margin_ms`
  - `debug_symbol_map`
- `tmp/artifacts/conformance-suite/<target>/summary.json`
  - `suite.status`
  - `suite.failures`
  - `matrix.total_cases`
  - `matrix.failed_cases`
  - `debug_symbol_map`
- `tmp/artifacts/objc3c-native/execution-smoke/<run_id>/summary.json`
  - `status`
  - `total`
  - `passed`
  - `failed`
  - `results[*].runtime_dispatch_symbol`
  - `debug_symbol_map`
- `tmp/artifacts/objc3c-native/execution-replay-proof/<proof_run_id>/summary.json`
  - `status`
  - `run1_sha256`
  - `run2_sha256`
  - `run1_summary`
  - `run2_summary`
  - `debug_symbol_map`

Contract check:

```powershell
python -m pytest tests/tooling/test_objc3c_m213_validation_debug_fidelity_contract.py -q
```

## M212 validation/perf code-action runbook

Code-action/refactor validation runbook verifies deterministic rewrite-safety evidence.

```powershell
npm run test:objc3c:m145-direct-llvm-matrix
npm run test:objc3c:m145-direct-llvm-matrix:lane-d
npm run test:objc3c:execution-smoke
npm run test:objc3c:execution-replay-proof
```

Code-action evidence packet fields:

- `tmp/artifacts/objc3c-native/perf-budget/<run_id>/summary.json`
  - `status`
  - `total_elapsed_ms`
  - `budget_margin_ms`
  - `rewrite_safety_map`
- `tmp/artifacts/conformance-suite/<target>/summary.json`
  - `suite.status`
  - `suite.failures`
  - `matrix.total_cases`
  - `matrix.failed_cases`
  - `rewrite_safety_map`
- `tmp/artifacts/objc3c-native/execution-smoke/<run_id>/summary.json`
  - `status`
  - `total`
  - `passed`
  - `failed`
  - `results[*].runtime_dispatch_symbol`
  - `rewrite_safety_map`
- `tmp/artifacts/objc3c-native/execution-replay-proof/<proof_run_id>/summary.json`
  - `status`
  - `run1_sha256`
  - `run2_sha256`
  - `run1_summary`
  - `run2_summary`
  - `rewrite_safety_map`

Contract check:

```powershell
python -m pytest tests/tooling/test_objc3c_m212_validation_code_action_contract.py -q
```

## M211 validation/perf LSP semantic runbook

LSP semantic-token/navigation validation runbook verifies deterministic editor-facing evidence.

```powershell
npm run test:objc3c:m145-direct-llvm-matrix
npm run test:objc3c:m145-direct-llvm-matrix:lane-d
npm run test:objc3c:execution-smoke
npm run test:objc3c:execution-replay-proof
```

LSP semantic evidence packet fields:

- `tmp/artifacts/objc3c-native/perf-budget/<run_id>/summary.json`
  - `status`
  - `total_elapsed_ms`
  - `budget_margin_ms`
  - `semantic_token_map`
- `tmp/artifacts/conformance-suite/<target>/summary.json`
  - `suite.status`
  - `suite.failures`
  - `matrix.total_cases`
  - `matrix.failed_cases`
  - `semantic_token_map`
- `tmp/artifacts/objc3c-native/execution-smoke/<run_id>/summary.json`
  - `status`
  - `total`
  - `passed`
  - `failed`
  - `results[*].runtime_dispatch_symbol`
  - `semantic_token_map`
- `tmp/artifacts/objc3c-native/execution-replay-proof/<proof_run_id>/summary.json`
  - `status`
  - `run1_sha256`
  - `run2_sha256`
  - `run1_summary`
  - `run2_summary`
  - `semantic_token_map`

Contract check:

```powershell
python -m pytest tests/tooling/test_objc3c_m211_validation_lsp_contract.py -q
```

## M210 validation/perf performance-budget regression runbook

Performance-budget regression gating runbook verifies deterministic budget and replay surfaces before integration promotion.

```powershell
npm run test:objc3c:m145-direct-llvm-matrix
npm run test:objc3c:m145-direct-llvm-matrix:lane-d
npm run test:objc3c:execution-smoke
npm run test:objc3c:execution-replay-proof
npm run test:objc3c:perf-budget
```

Performance-regression evidence packet fields:

- `tmp/artifacts/objc3c-native/perf-budget/<run_id>/summary.json`
  - `status`
  - `total_elapsed_ms`
  - `budget_margin_ms`
  - `cache_proof.status`
  - `cache_proof.run1.cache_hit`
  - `cache_proof.run2.cache_hit`
- `tmp/artifacts/conformance-suite/<target>/summary.json`
  - `suite.status`
  - `suite.failures`
  - `matrix.total_cases`
  - `matrix.failed_cases`
  - `selector_global_ordering`
- `tmp/artifacts/objc3c-native/execution-smoke/<run_id>/summary.json`
  - `status`
  - `total`
  - `passed`
  - `failed`
  - `results[*].runtime_dispatch_symbol`
  - `results[*].selector_global_ordering`
- `tmp/artifacts/objc3c-native/execution-replay-proof/<proof_run_id>/summary.json`
  - `status`
  - `run1_sha256`
  - `run2_sha256`
  - `run1_summary`
  - `run2_summary`
  - `budget_margin_ms`

Contract check:

```powershell
python -m pytest tests/tooling/test_objc3c_m210_validation_perf_regression_contract.py -q
```

## M209 validation/perf profile-guided optimization runbook

Profile-guided optimization (PGO) validation runbook verifies deterministic profile evidence surfaces before enabling optimization-driven policy changes.

```powershell
npm run test:objc3c:m145-direct-llvm-matrix
npm run test:objc3c:m145-direct-llvm-matrix:lane-d
npm run test:objc3c:execution-smoke
npm run test:objc3c:execution-replay-proof
npm run test:objc3c:perf-budget
```

PGO evidence packet fields:

- `tmp/artifacts/objc3c-native/perf-budget/<run_id>/summary.json`
  - `status`
  - `total_elapsed_ms`
  - `budget_margin_ms`
  - `cache_proof.status`
  - `cache_proof.run1.cache_hit`
  - `cache_proof.run2.cache_hit`
- `tmp/artifacts/conformance-suite/<target>/summary.json`
  - `suite.status`
  - `suite.failures`
  - `matrix.total_cases`
  - `matrix.failed_cases`
  - `selector_global_ordering`
- `tmp/artifacts/objc3c-native/execution-smoke/<run_id>/summary.json`
  - `status`
  - `results[*].runtime_dispatch_symbol`
  - `results[*].selector_global_ordering`
- `tmp/artifacts/objc3c-native/execution-replay-proof/<proof_run_id>/summary.json`
  - `status`
  - `run1_sha256`
  - `run2_sha256`
  - `run1_summary`
  - `run2_summary`
  - `budget_margin_ms`

Contract check:

```powershell
python -m pytest tests/tooling/test_objc3c_m209_validation_pgo_contract.py -q
```

## M208 validation/perf whole-module optimization runbook

Whole-module optimization (WMO) validation runbook verifies deterministic module-shape and optimization-control evidence before promotion.

```powershell
npm run test:objc3c:m145-direct-llvm-matrix
npm run test:objc3c:m145-direct-llvm-matrix:lane-d
npm run test:objc3c:execution-smoke
npm run test:objc3c:execution-replay-proof
npm run test:objc3c:perf-budget
```

Whole-module optimization evidence packet fields:

- `tmp/artifacts/objc3c-native/perf-budget/<run_id>/summary.json`
  - `status`
  - `total_elapsed_ms`
  - `budget_margin_ms`
  - `cache_proof.status`
  - `cache_proof.run1.cache_hit`
  - `cache_proof.run2.cache_hit`
- `tmp/artifacts/conformance-suite/<target>/summary.json`
  - `suite.status`
  - `suite.failures`
  - `matrix.total_cases`
  - `matrix.failed_cases`
  - `selector_global_ordering`
- `tmp/artifacts/objc3c-native/execution-smoke/<run_id>/summary.json`
  - `status`
  - `results[*].runtime_dispatch_symbol`
  - `results[*].selector_global_ordering`
- `tmp/artifacts/objc3c-native/execution-replay-proof/<proof_run_id>/summary.json`
  - `status`
  - `run1_sha256`
  - `run2_sha256`
  - `run1_summary`
  - `run2_summary`
  - `budget_margin_ms`

Contract check:

```powershell
python -m pytest tests/tooling/test_objc3c_m208_validation_wmo_contract.py -q
```

## M207 validation/perf dispatch-specific optimization runbook

Dispatch-specific optimization validation runbook verifies deterministic dispatch-surface evidence before optimization pass promotion.

```powershell
npm run test:objc3c:m145-direct-llvm-matrix
npm run test:objc3c:m145-direct-llvm-matrix:lane-d
npm run test:objc3c:execution-smoke
npm run test:objc3c:execution-replay-proof
npm run test:objc3c:perf-budget
```

Dispatch-optimization evidence packet fields:

- `tmp/artifacts/objc3c-native/perf-budget/<run_id>/summary.json`
  - `status`
  - `total_elapsed_ms`
  - `budget_margin_ms`
  - `cache_proof.status`
  - `cache_proof.run1.cache_hit`
  - `cache_proof.run2.cache_hit`
- `tmp/artifacts/conformance-suite/<target>/summary.json`
  - `suite.status`
  - `suite.failures`
  - `matrix.total_cases`
  - `matrix.failed_cases`
  - `selector_global_ordering`
- `tmp/artifacts/objc3c-native/execution-smoke/<run_id>/summary.json`
  - `status`
  - `results[*].runtime_dispatch_symbol`
  - `results[*].selector_global_ordering`
- `tmp/artifacts/objc3c-native/execution-replay-proof/<proof_run_id>/summary.json`
  - `status`
  - `run1_sha256`
  - `run2_sha256`
  - `run1_summary`
  - `run2_summary`
  - `budget_margin_ms`

Contract check:

```powershell
python -m pytest tests/tooling/test_objc3c_m207_validation_dispatch_optimizations_contract.py -q
```

## M206 validation/perf canonical optimization pipeline stage-1 runbook

Canonical optimization stage-1 validation runbook verifies deterministic optimization-surface evidence across matrix, smoke, replay, and budget gates.

```powershell
npm run test:objc3c:m145-direct-llvm-matrix
npm run test:objc3c:m145-direct-llvm-matrix:lane-d
npm run test:objc3c:execution-smoke
npm run test:objc3c:execution-replay-proof
npm run test:objc3c:perf-budget
```

Canonical optimization stage-1 evidence packet fields:

- `tmp/artifacts/objc3c-native/perf-budget/<run_id>/summary.json`
  - `status`
  - `total_elapsed_ms`
  - `budget_margin_ms`
  - `cache_proof.status`
  - `cache_proof.run1.cache_hit`
  - `cache_proof.run2.cache_hit`
- `tmp/artifacts/conformance-suite/<target>/summary.json`
  - `suite.status`
  - `suite.failures`
  - `matrix.total_cases`
  - `matrix.failed_cases`
  - `selector_global_ordering`
- `tmp/artifacts/objc3c-native/execution-smoke/<run_id>/summary.json`
  - `status`
  - `results[*].runtime_dispatch_symbol`
  - `results[*].selector_global_ordering`
- `tmp/artifacts/objc3c-native/execution-replay-proof/<proof_run_id>/summary.json`
  - `status`
  - `run1_sha256`
  - `run2_sha256`
  - `run1_summary`
  - `run2_summary`
  - `budget_margin_ms`

Contract check:

```powershell
python -m pytest tests/tooling/test_objc3c_m206_validation_canonical_optimization_contract.py -q
```

## M205 validation/perf macro security policy runbook

Macro security policy validation runbook verifies deterministic fail-closed directive enforcement evidence across matrix, smoke, replay, and budget gates.

```powershell
npm run test:objc3c:m145-direct-llvm-matrix
npm run test:objc3c:m145-direct-llvm-matrix:lane-d
npm run test:objc3c:execution-smoke
npm run test:objc3c:execution-replay-proof
npm run test:objc3c:perf-budget
```

Macro-security evidence packet fields:

- `tmp/artifacts/objc3c-native/perf-budget/<run_id>/summary.json`
  - `status`
  - `total_elapsed_ms`
  - `budget_margin_ms`
  - `cache_proof.status`
  - `cache_proof.run1.cache_hit`
  - `cache_proof.run2.cache_hit`
- `tmp/artifacts/conformance-suite/<target>/summary.json`
  - `suite.status`
  - `suite.failures`
  - `matrix.total_cases`
  - `matrix.failed_cases`
  - `selector_global_ordering`
- `tmp/artifacts/objc3c-native/execution-smoke/<run_id>/summary.json`
  - `status`
  - `results[*].runtime_dispatch_symbol`
  - `results[*].selector_global_ordering`
- `tmp/artifacts/objc3c-native/execution-replay-proof/<proof_run_id>/summary.json`
  - `status`
  - `run1_sha256`
  - `run2_sha256`
  - `run1_summary`
  - `run2_summary`
  - `budget_margin_ms`

Contract check:

```powershell
python -m pytest tests/tooling/test_objc3c_m205_validation_macro_security_contract.py -q
```

## M204 validation/perf macro diagnostics and provenance runbook

Macro diagnostics/provenance validation runbook verifies deterministic diagnostic packet and source-location evidence across matrix, smoke, replay, and budget gates.

```powershell
npm run test:objc3c:m145-direct-llvm-matrix
npm run test:objc3c:m145-direct-llvm-matrix:lane-d
npm run test:objc3c:execution-smoke
npm run test:objc3c:execution-replay-proof
npm run test:objc3c:perf-budget
```

Macro-diagnostics evidence packet fields:

- `tmp/artifacts/objc3c-native/perf-budget/<run_id>/summary.json`
  - `status`
  - `total_elapsed_ms`
  - `budget_margin_ms`
  - `cache_proof.status`
  - `cache_proof.run1.cache_hit`
  - `cache_proof.run2.cache_hit`
- `tmp/artifacts/conformance-suite/<target>/summary.json`
  - `suite.status`
  - `suite.failures`
  - `matrix.total_cases`
  - `matrix.failed_cases`
  - `selector_global_ordering`
- `tmp/artifacts/objc3c-native/execution-smoke/<run_id>/summary.json`
  - `status`
  - `results[*].runtime_dispatch_symbol`
  - `results[*].selector_global_ordering`
- `tmp/artifacts/objc3c-native/execution-replay-proof/<proof_run_id>/summary.json`
  - `status`
  - `run1_sha256`
  - `run2_sha256`
  - `run1_summary`
  - `run2_summary`
  - `budget_margin_ms`

Contract check:

```powershell
python -m pytest tests/tooling/test_objc3c_m204_validation_macro_diagnostics_contract.py -q
```

## M203 validation/perf compile-time evaluation runbook

Compile-time evaluation validation runbook verifies deterministic constant-evaluation evidence across matrix, smoke, replay, and budget gates.

```powershell
npm run test:objc3c:m145-direct-llvm-matrix
npm run test:objc3c:m145-direct-llvm-matrix:lane-d
npm run test:objc3c:execution-smoke
npm run test:objc3c:execution-replay-proof
npm run test:objc3c:perf-budget
```

Compile-time-eval evidence packet fields:

- `tmp/artifacts/objc3c-native/perf-budget/<run_id>/summary.json`
  - `status`
  - `total_elapsed_ms`
  - `budget_margin_ms`
  - `cache_proof.status`
  - `cache_proof.run1.cache_hit`
  - `cache_proof.run2.cache_hit`
- `tmp/artifacts/conformance-suite/<target>/summary.json`
  - `suite.status`
  - `suite.failures`
  - `matrix.total_cases`
  - `matrix.failed_cases`
  - `selector_global_ordering`
- `tmp/artifacts/objc3c-native/execution-smoke/<run_id>/summary.json`
  - `status`
  - `results[*].runtime_dispatch_symbol`
  - `results[*].selector_global_ordering`
- `tmp/artifacts/objc3c-native/execution-replay-proof/<proof_run_id>/summary.json`
  - `status`
  - `run1_sha256`
  - `run2_sha256`
  - `run1_summary`
  - `run2_summary`
  - `budget_margin_ms`

Contract check:

```powershell
python -m pytest tests/tooling/test_objc3c_m203_validation_compile_time_eval_contract.py -q
```

## M202 validation/perf derive/synthesis runbook

Derive/synthesis validation runbook verifies deterministic synthesis-evidence surfaces across matrix, smoke, replay, and budget gates.

```powershell
npm run test:objc3c:m145-direct-llvm-matrix
npm run test:objc3c:m145-direct-llvm-matrix:lane-d
npm run test:objc3c:execution-smoke
npm run test:objc3c:execution-replay-proof
npm run test:objc3c:perf-budget
```

Derive/synthesis evidence packet fields:

- `tmp/artifacts/objc3c-native/perf-budget/<run_id>/summary.json`
  - `status`
  - `total_elapsed_ms`
  - `budget_margin_ms`
  - `cache_proof.status`
  - `cache_proof.run1.cache_hit`
  - `cache_proof.run2.cache_hit`
- `tmp/artifacts/conformance-suite/<target>/summary.json`
  - `suite.status`
  - `suite.failures`
  - `matrix.total_cases`
  - `matrix.failed_cases`
  - `selector_global_ordering`
- `tmp/artifacts/objc3c-native/execution-smoke/<run_id>/summary.json`
  - `status`
  - `results[*].runtime_dispatch_symbol`
  - `results[*].selector_global_ordering`
- `tmp/artifacts/objc3c-native/execution-replay-proof/<proof_run_id>/summary.json`
  - `status`
  - `run1_sha256`
  - `run2_sha256`
  - `run1_summary`
  - `run2_summary`
  - `budget_margin_ms`

Contract check:

```powershell
python -m pytest tests/tooling/test_objc3c_m202_validation_derive_synthesis_contract.py -q
```

## M201 validation/perf macro expansion architecture runbook

Macro-expansion architecture/isolation validation runbook verifies deterministic migration/provenance evidence across matrix, smoke, replay, and budget gates.

```powershell
npm run test:objc3c:m145-direct-llvm-matrix
npm run test:objc3c:m145-direct-llvm-matrix:lane-d
npm run test:objc3c:execution-smoke
npm run test:objc3c:execution-replay-proof
npm run test:objc3c:perf-budget
```

Macro-expansion architecture evidence packet fields:

- `tmp/artifacts/objc3c-native/perf-budget/<run_id>/summary.json`
  - `status`
  - `total_elapsed_ms`
  - `budget_margin_ms`
  - `cache_proof.status`
  - `cache_proof.run1.cache_hit`
  - `cache_proof.run2.cache_hit`
- `tmp/artifacts/conformance-suite/<target>/summary.json`
  - `suite.status`
  - `suite.failures`
  - `matrix.total_cases`
  - `matrix.failed_cases`
  - `selector_global_ordering`
- `tmp/artifacts/objc3c-native/execution-smoke/<run_id>/summary.json`
  - `status`
  - `results[*].runtime_dispatch_symbol`
  - `results[*].selector_global_ordering`
- `tmp/artifacts/objc3c-native/execution-replay-proof/<proof_run_id>/summary.json`
  - `status`
  - `run1_sha256`
  - `run2_sha256`
  - `run1_summary`
  - `run2_summary`
  - `budget_margin_ms`

Contract check:

```powershell
python -m pytest tests/tooling/test_objc3c_m201_validation_macro_expansion_contract.py -q
```

## M200 validation/perf interop integration suite and packaging

Interop integration suite/packaging validation runbook verifies deterministic fixture-packaging evidence across matrix, smoke, replay, and budget gates.

```powershell
npm run test:objc3c:m145-direct-llvm-matrix
npm run test:objc3c:m145-direct-llvm-matrix:lane-d
npm run test:objc3c:execution-smoke
npm run test:objc3c:execution-replay-proof
npm run test:objc3c:perf-budget
```

Interop integration suite packaging evidence packet fields:

- `tmp/artifacts/objc3c-native/perf-budget/<run_id>/summary.json`
  - `status`
  - `total_elapsed_ms`
  - `budget_margin_ms`
  - `cache_proof.status`
  - `cache_proof.run1.cache_hit`
  - `cache_proof.run2.cache_hit`
- `tmp/artifacts/conformance-suite/<target>/summary.json`
  - `suite.status`
  - `suite.failures`
  - `matrix.total_cases`
  - `matrix.failed_cases`
  - `selector_global_ordering`
- `tmp/artifacts/objc3c-native/execution-smoke/<run_id>/summary.json`
  - `status`
  - `results[*].runtime_dispatch_symbol`
  - `results[*].selector_global_ordering`
- `tmp/artifacts/objc3c-native/execution-replay-proof/<proof_run_id>/summary.json`
  - `status`
  - `run1_sha256`
  - `run2_sha256`
  - `run1_summary`
  - `run2_summary`
  - `budget_margin_ms`

Contract check:

```powershell
python -m pytest tests/tooling/test_objc3c_m200_validation_interop_packaging_contract.py -q
```

## M199 validation/perf foreign type import diagnostics runbook

Foreign type import diagnostics validation runbook verifies deterministic diagnostics evidence across matrix, smoke, replay, and budget gates.

```powershell
npm run test:objc3c:m145-direct-llvm-matrix
npm run test:objc3c:m145-direct-llvm-matrix:lane-d
npm run test:objc3c:execution-smoke
npm run test:objc3c:execution-replay-proof
npm run test:objc3c:perf-budget
```

Foreign type import diagnostics evidence packet fields:

- `tmp/artifacts/objc3c-native/perf-budget/<run_id>/summary.json`
  - `status`
  - `total_elapsed_ms`
  - `budget_margin_ms`
  - `cache_proof.status`
  - `cache_proof.run1.cache_hit`
  - `cache_proof.run2.cache_hit`
- `tmp/artifacts/conformance-suite/<target>/summary.json`
  - `suite.status`
  - `suite.failures`
  - `matrix.total_cases`
  - `matrix.failed_cases`
  - `selector_global_ordering`
- `tmp/artifacts/objc3c-native/execution-smoke/<run_id>/summary.json`
  - `status`
  - `results[*].runtime_dispatch_symbol`
  - `results[*].selector_global_ordering`
- `tmp/artifacts/objc3c-native/execution-replay-proof/<proof_run_id>/summary.json`
  - `status`
  - `run1_sha256`
  - `run2_sha256`
  - `run1_summary`
  - `run2_summary`
  - `budget_margin_ms`

Contract check:

```powershell
python -m pytest tests/tooling/test_objc3c_m199_validation_foreign_type_diagnostics_contract.py -q
```

## M198 validation/perf swift metadata bridge runbook

Swift metadata-bridge validation runbook verifies deterministic metadata evidence across matrix, smoke, replay, and budget gates.

```powershell
npm run test:objc3c:m145-direct-llvm-matrix
npm run test:objc3c:m145-direct-llvm-matrix:lane-d
npm run test:objc3c:execution-smoke
npm run test:objc3c:execution-replay-proof
npm run test:objc3c:perf-budget
```

Swift metadata-bridge evidence packet fields:

- `tmp/artifacts/objc3c-native/perf-budget/<run_id>/summary.json`
  - `status`
  - `total_elapsed_ms`
  - `budget_margin_ms`
  - `cache_proof.status`
  - `cache_proof.run1.cache_hit`
  - `cache_proof.run2.cache_hit`
- `tmp/artifacts/conformance-suite/<target>/summary.json`
  - `suite.status`
  - `suite.failures`
  - `matrix.total_cases`
  - `matrix.failed_cases`
  - `selector_global_ordering`
- `tmp/artifacts/objc3c-native/execution-smoke/<run_id>/summary.json`
  - `status`
  - `results[*].runtime_dispatch_symbol`
  - `results[*].selector_global_ordering`
- `tmp/artifacts/objc3c-native/execution-replay-proof/<proof_run_id>/summary.json`
  - `status`
  - `run1_sha256`
  - `run2_sha256`
  - `run1_summary`
  - `run2_summary`
  - `budget_margin_ms`

Contract check:

```powershell
python -m pytest tests/tooling/test_objc3c_m198_validation_swift_metadata_bridge_contract.py -q
```

## M197 validation/perf C++ interop shim strategy runbook

C++ interop-shim validation runbook verifies deterministic interop-shim evidence across matrix, smoke, replay, and budget gates.

```powershell
npm run test:objc3c:m145-direct-llvm-matrix
npm run test:objc3c:m145-direct-llvm-matrix:lane-d
npm run test:objc3c:execution-smoke
npm run test:objc3c:execution-replay-proof
npm run test:objc3c:perf-budget
```

C++ interop-shim evidence packet fields:

- `tmp/artifacts/objc3c-native/perf-budget/<run_id>/summary.json`
  - `status`
  - `total_elapsed_ms`
  - `budget_margin_ms`
  - `cache_proof.status`
  - `cache_proof.run1.cache_hit`
  - `cache_proof.run2.cache_hit`
- `tmp/artifacts/conformance-suite/<target>/summary.json`
  - `suite.status`
  - `suite.failures`
  - `matrix.total_cases`
  - `matrix.failed_cases`
  - `selector_global_ordering`
- `tmp/artifacts/objc3c-native/execution-smoke/<run_id>/summary.json`
  - `status`
  - `results[*].runtime_dispatch_symbol`
  - `results[*].selector_global_ordering`
- `tmp/artifacts/objc3c-native/execution-replay-proof/<proof_run_id>/summary.json`
  - `status`
  - `run1_sha256`
  - `run2_sha256`
  - `run1_summary`
  - `run2_summary`
  - `budget_margin_ms`

Contract check:

```powershell
python -m pytest tests/tooling/test_objc3c_m197_validation_cpp_interop_shim_contract.py -q
```

## M196 validation/perf C interop headers and ABI alignment runbook

C-interop header and ABI-alignment validation runbook verifies deterministic C wrapper/ABI evidence across matrix, smoke, replay, and budget gates.

```powershell
npm run test:objc3c:m145-direct-llvm-matrix
npm run test:objc3c:m145-direct-llvm-matrix:lane-d
npm run test:objc3c:execution-smoke
npm run test:objc3c:execution-replay-proof
npm run test:objc3c:perf-budget
```

C-interop header/ABI-alignment evidence packet fields:

- `tmp/artifacts/objc3c-native/perf-budget/<run_id>/summary.json`
  - `status`
  - `total_elapsed_ms`
  - `budget_margin_ms`
  - `cache_proof.status`
  - `cache_proof.run1.cache_hit`
  - `cache_proof.run2.cache_hit`
- `tmp/artifacts/conformance-suite/<target>/summary.json`
  - `suite.status`
  - `suite.failures`
  - `matrix.total_cases`
  - `matrix.failed_cases`
  - `selector_global_ordering`
- `tmp/artifacts/objc3c-native/execution-smoke/<run_id>/summary.json`
  - `status`
  - `results[*].runtime_dispatch_symbol`
  - `results[*].selector_global_ordering`
- `tmp/artifacts/objc3c-native/execution-replay-proof/<proof_run_id>/summary.json`
  - `status`
  - `run1_sha256`
  - `run2_sha256`
  - `run1_summary`
  - `run2_summary`
  - `budget_margin_ms`

Contract check:

```powershell
python -m pytest tests/tooling/test_objc3c_m196_validation_c_interop_headers_abi_contract.py -q
```

## M195 validation/perf system-extension conformance and policy runbook

System-extension conformance/policy validation runbook verifies deterministic privileged-surface policy evidence across matrix, smoke, replay, and budget gates.

```powershell
npm run test:objc3c:m145-direct-llvm-matrix
npm run test:objc3c:m145-direct-llvm-matrix:lane-d
npm run test:objc3c:execution-smoke
npm run test:objc3c:execution-replay-proof
npm run test:objc3c:perf-budget
```

System-extension conformance/policy evidence packet fields:

- `tmp/artifacts/objc3c-native/perf-budget/<run_id>/summary.json`
  - `status`
  - `total_elapsed_ms`
  - `budget_margin_ms`
  - `cache_proof.status`
  - `cache_proof.run1.cache_hit`
  - `cache_proof.run2.cache_hit`
- `tmp/artifacts/conformance-suite/<target>/summary.json`
  - `suite.status`
  - `suite.failures`
  - `matrix.total_cases`
  - `matrix.failed_cases`
  - `selector_global_ordering`
- `tmp/artifacts/objc3c-native/execution-smoke/<run_id>/summary.json`
  - `status`
  - `results[*].runtime_dispatch_symbol`
  - `results[*].selector_global_ordering`
- `tmp/artifacts/objc3c-native/execution-replay-proof/<proof_run_id>/summary.json`
  - `status`
  - `run1_sha256`
  - `run2_sha256`
  - `run1_summary`
  - `run2_summary`
  - `budget_margin_ms`

Contract check:

```powershell
python -m pytest tests/tooling/test_objc3c_m195_validation_system_extension_policy_contract.py -q
```

## M194 validation/perf atomics and memory-order mapping runbook

Atomics and memory-order mapping validation runbook verifies deterministic bitwise/shift assignment mapping evidence across matrix, smoke, replay, and budget gates.

```powershell
npm run test:objc3c:m145-direct-llvm-matrix
npm run test:objc3c:m145-direct-llvm-matrix:lane-d
npm run test:objc3c:execution-smoke
npm run test:objc3c:execution-replay-proof
npm run test:objc3c:perf-budget
```

Atomics and memory-order mapping evidence packet fields:

- `tmp/artifacts/objc3c-native/perf-budget/<run_id>/summary.json`
  - `status`
  - `total_elapsed_ms`
  - `budget_margin_ms`
  - `cache_proof.status`
  - `cache_proof.run1.cache_hit`
  - `cache_proof.run2.cache_hit`
- `tmp/artifacts/conformance-suite/<target>/summary.json`
  - `suite.status`
  - `suite.failures`
  - `matrix.total_cases`
  - `matrix.failed_cases`
  - `selector_global_ordering`
- `tmp/artifacts/objc3c-native/execution-smoke/<run_id>/summary.json`
  - `status`
  - `results[*].runtime_dispatch_symbol`
  - `results[*].selector_global_ordering`
- `tmp/artifacts/objc3c-native/execution-replay-proof/<proof_run_id>/summary.json`
  - `status`
  - `run1_sha256`
  - `run2_sha256`
  - `run1_summary`
  - `run2_summary`
  - `budget_margin_ms`

Contract check:

```powershell
python -m pytest tests/tooling/test_objc3c_m194_validation_atomics_memory_order_contract.py -q
```

## M193 validation/perf SIMD/vector type lowering runbook

SIMD/vector type lowering validation runbook verifies deterministic vector-friendly parser/sema/lowering evidence across matrix, smoke, replay, and budget gates.

```powershell
npm run test:objc3c:m145-direct-llvm-matrix
npm run test:objc3c:m145-direct-llvm-matrix:lane-d
npm run test:objc3c:execution-smoke
npm run test:objc3c:execution-replay-proof
npm run test:objc3c:perf-budget
```

SIMD/vector type lowering evidence packet fields:

- `tmp/artifacts/objc3c-native/perf-budget/<run_id>/summary.json`
  - `status`
  - `total_elapsed_ms`
  - `budget_margin_ms`
  - `cache_proof.status`
  - `cache_proof.run1.cache_hit`
  - `cache_proof.run2.cache_hit`
- `tmp/artifacts/conformance-suite/<target>/summary.json`
  - `suite.status`
  - `suite.failures`
  - `matrix.total_cases`
  - `matrix.failed_cases`
  - `selector_global_ordering`
- `tmp/artifacts/objc3c-native/execution-smoke/<run_id>/summary.json`
  - `status`
  - `results[*].runtime_dispatch_symbol`
  - `results[*].selector_global_ordering`
- `tmp/artifacts/objc3c-native/execution-replay-proof/<proof_run_id>/summary.json`
  - `status`
  - `run1_sha256`
  - `run2_sha256`
  - `run1_summary`
  - `run2_summary`
  - `budget_margin_ms`

Contract check:

```powershell
python -m pytest tests/tooling/test_objc3c_m193_validation_simd_vector_lowering_contract.py -q
```

## Current limitations (implemented behavior only)

- Top-level `.objc3` declarations currently include `module`, `let`, `fn`, `pure fn`, declaration-only `extern fn`, declaration-only `extern pure fn`, and declaration-only `pure extern fn`.
- Parameters and explicit function return annotations support `i32`, `bool`, `BOOL`, `NSInteger`, `NSUInteger`, `void` (return only), `id` (scalar alias), `Class` (scalar alias), `SEL` (scalar alias), `Protocol` (scalar alias), and `instancetype` (scalar alias).
- Function declarations can be full definitions (`{ ... }`) or semicolon-terminated prototypes, including explicit extern-qualified prototype declarations (`extern fn`, `extern pure fn`, `pure extern fn`).
- `id`/`Class`/`instancetype` parameter suffix forms (`id<...>`, `id?`, `id!`, `Class<...>`, `Class?`, `Class!`, `instancetype<...>`, `instancetype?`, `instancetype!`) are accepted and lowered as scalar aliases; non-`id`/`Class`/`instancetype` suffix usage remains deterministic `O3S206` diagnostics.
- `id`/`Class`/`instancetype` return suffix forms (`-> id<...>`, `-> id?`, `-> id!`, `-> Class<...>`, `-> Class?`, `-> Class!`, `-> instancetype<...>`, `-> instancetype?`, `-> instancetype!`) are accepted and lowered as scalar aliases; non-`id`/`Class`/`instancetype` return suffix usage remains deterministic `O3S206` diagnostics.
- Statements currently include `let`, assignment (`ident = expr;` plus `+=`, `-=`, `*=`, `/=`, `%=`, `&=`, `|=`, `^=`, `<<=`, `>>=` forms), update operators (`ident++`, `ident--`, `++ident`, `--ident`), `return`, `if`/`else`, `do-while`, `for`, `switch`, `while`, `break`, `continue`, and expression statements.
- Standalone empty statements (`;`) are accepted and treated as no-op statements.
- Standalone nested block statements (`{ ... }`) are accepted with lexical scope boundaries.
- Control-flow bodies for `if`/`else`, `while`, `for`, and `do-while` may be either braced blocks or single statements.
- Expressions are limited to numeric/bool literals, identifiers, identifier-target calls, bracketed message-send expressions, unary `!`/`+`/`-`/`~`, arithmetic (including `%`), bitwise/shift, relational/equality operators, logical `&&`/`||`, conditional (`?:`) expressions, and grouping.
- Calls are identifier-only targets (no chained calls or non-identifier call expressions).
- Lowering emits typed function ABI signatures (`bool` as `i1`, `i32` as `i32`) with boundary casts to preserve expression-level `i32` evaluation.
- Message-send lowering is emitted as direct native IR calls to runtime dispatch shim `@objc3_msgsend_i32`.
- Message-send lowering supports up to four explicit arguments by default and can be lowered via `--objc3-max-message-args`.
- Selector literal globals are canonically named in lexicographic selector order for deterministic replay.
- Global initializers must be compile-time constant expressions over literals/operators and may reference previously declared globals.
- `if` conditions are lowered as non-zero truthiness checks; there is no dedicated condition-type diagnostic.
- Lexer comment support is limited to `// ...` and non-nested `/* ... */`.
- Duplicate `module` declarations are rejected with deterministic `O3S200` diagnostics.
