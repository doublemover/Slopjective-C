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
- `npm run proof:objc3c`
  - Runs `scripts/run_objc3c_native_compile_proof.ps1`.
  - Replays `tests/tooling/fixtures/native/hello.objc3` twice and writes `artifacts/compilation/objc3c-native/proof_20260226/digest.json` on success.
- `npm run test:objc3c:lane-e`
  - Runs lane-E deterministic validation chain:
    - `npm run test:objc3c`
    - `npm run test:objc3c:diagnostics-replay-proof`
    - `npm run test:objc3c:parser-replay-proof`
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
- `powershell -NoProfile -ExecutionPolicy Bypass -File scripts/run_objc3c_lowering_regression_suite.ps1`
  - Replays all recovery fixtures (positive and negative) twice per fixture.
  - Includes optional Objective-C dispatch fixture roots when present (`recovery/positive/lowering_dispatch`, then `dispatch/positive`).
  - Enforces deterministic diagnostics for every fixture, deterministic manifests/IR for positive fixtures, and fail-closed artifact absence for negative fixtures.
  - For fixtures with `<fixture>.dispatch-ir.expect.txt`, emits replay IR via `clang -S -emit-llvm` and enforces deterministic dispatch IR marker matches across replay.
  - For fixtures with `<fixture>.objc3-ir.expect.txt`, enforces deterministic native-lowered IR marker matches across replay for emitted `module.ll`.
  - Optional clang override: `OBJC3C_NATIVE_LOWERING_CLANG_PATH=<clang executable>`.
  - Writes per-run summary under `tmp/artifacts/objc3c-native/lowering-regression/<run_id>/summary.json` plus stable latest summary at `tmp/artifacts/objc3c-native/lowering-regression/latest-summary.json`.
- `powershell -NoProfile -ExecutionPolicy Bypass -File scripts/check_objc3c_typed_abi_replay_proof.ps1`
  - Replays typed-signature `.objc3` fixtures twice and enforces deterministic `module.ll` plus required typed ABI marker token presence from adjacent `.objc3-ir.expect.txt` files.
  - Writes proof summary under `tmp/artifacts/objc3c-native/typed-abi-replay-proof/<run_id>/summary.json`.
- `powershell -NoProfile -ExecutionPolicy Bypass -File scripts/check_objc3c_execution_replay_proof.ps1`
  - Runs execution smoke twice with distinct run IDs.
  - Canonicalizes summary surfaces (excluding run-path entropy) and enforces SHA256 equality across replay.
  - Writes proof summary under `tmp/artifacts/objc3c-native/execution-replay-proof/<proof_run_id>/summary.json`.
- `powershell -NoProfile -ExecutionPolicy Bypass -File scripts/check_objc3c_lowering_replay_proof.ps1`
  - Runs lowering regression twice with distinct run IDs.
  - Captures each run's deterministic summary and asserts SHA256 equality across replay.
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
python scripts/check_m137_lexer_contract.py
python -m pytest tests/tooling/test_objc3c_lexer_parity.py -q
python scripts/check_m23_execution_readiness.py
python scripts/check_m24_execution_readiness.py
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
