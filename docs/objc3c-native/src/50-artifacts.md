<!-- markdownlint-disable-file MD041 -->

## Artifacts and exit codes

For `.objc3` input:

- Always writes:
  - `<prefix>.diagnostics.txt`
  - `<prefix>.diagnostics.json`
- On success writes:
  - `<prefix>.manifest.json` (module/frontend/lowering/globals/functions)
  - `<prefix>.ll` (emitted IR)
  - `<prefix>.obj` (compiled from IR)

For non-`.objc3` input:

- Always writes:
  - `<prefix>.diagnostics.txt`
  - `<prefix>.diagnostics.json`
- On success writes:
  - `<prefix>.manifest.json` (libclang symbol rows)
  - `<prefix>.obj` (compiled Objective-C object)

Exit codes:

- `0`: success
- `1`: parse/semantic/diagnostic failure
- `2`: CLI usage / missing input / invalid arg / missing explicit clang path
- `3`: clang compile step failed

## M223 lowering/IR metadata envelope

Native `.objc3` IR emission now includes deterministic frontend-profile metadata in addition to lowering boundary replay data:

- Prologue comment:
  - `; frontend_profile = language_version=<N>, compatibility_mode=<mode>, migration_assist=<bool>, migration_legacy_total=<count>`
- Named LLVM metadata payload:
  - `!objc3.frontend = !{!0}`
  - `!0 = !{i32 <language_version>, !"compatibility_mode", i1 <migration_assist>, i64 <legacy_yes>, i64 <legacy_no>, i64 <legacy_null>, i64 <legacy_total>}`

Operator replay check (from repo root):

```powershell
npm run compile:objc3c -- tests/tooling/fixtures/native/hello.objc3 --out-dir tmp/artifacts/compilation/objc3c-native/m223/lowering-metadata --emit-prefix module
```

Then inspect:

- `tmp/artifacts/compilation/objc3c-native/m223/lowering-metadata/module.ll`
- `tmp/artifacts/compilation/objc3c-native/m223/lowering-metadata/module.manifest.json`

Both artifacts should present aligned compatibility/migration profile information for deterministic replay triage.

## M220 lowering/runtime public-beta triage profile

Public-beta lowering/runtime triage must ship as deterministic packet evidence rooted under `tmp/`:

- `packet root`:
  - `tmp/artifacts/compilation/objc3c-native/m220/lowering-runtime-public-beta-triage/`
- `packet artifacts`:
  - `tmp/artifacts/compilation/objc3c-native/m220/lowering-runtime-public-beta-triage/module.ll`
  - `tmp/artifacts/compilation/objc3c-native/m220/lowering-runtime-public-beta-triage/module.manifest.json`
  - `tmp/reports/objc3c-native/m220/lowering-runtime-public-beta-triage/replay-markers.txt`
- `ABI/IR anchors` (persist verbatim in each beta triage packet):
  - `; lowering_ir_boundary = runtime_dispatch_symbol=<symbol>;runtime_dispatch_arg_slots=<N>;selector_global_ordering=lexicographic`
  - `; frontend_profile = language_version=<N>, compatibility_mode=<mode>, migration_assist=<bool>, migration_legacy_total=<count>`
  - `!objc3.frontend = !{!0}`
  - `declare i32 @<symbol>(i32, ptr, i32, ..., i32)`
  - `"lowering":{"runtime_dispatch_symbol":"<symbol>","runtime_dispatch_arg_slots":<N>,"selector_global_ordering":"lexicographic"}`
- `replay markers` (source anchors to include in packet notes):
  - `Objc3LoweringIRBoundaryReplayKey(...)`
  - `invalid lowering contract runtime_dispatch_symbol`
- `patch-loop closure criteria`:
  - rerunning the same source + lowering options must produce byte-identical `module.ll` and `module.manifest.json`.
  - replay markers stay stable across reruns (no added/removed lines, no reordered anchors).
  - closure remains open if any ABI/IR anchor or replay marker is missing.

Public-beta triage capture commands (lowering/runtime lane):

1. `npm run compile:objc3c -- tests/tooling/fixtures/native/hello.objc3 --out-dir tmp/artifacts/compilation/objc3c-native/m220/lowering-runtime-public-beta-triage --emit-prefix module`
2. `rg -n "lowering_ir_boundary|frontend_profile|!objc3.frontend|declare i32 @" tmp/artifacts/compilation/objc3c-native/m220/lowering-runtime-public-beta-triage/module.ll > tmp/reports/objc3c-native/m220/lowering-runtime-public-beta-triage/replay-markers.txt`
3. `rg -n "\"lowering\":{\"runtime_dispatch_symbol\"" tmp/artifacts/compilation/objc3c-native/m220/lowering-runtime-public-beta-triage/module.manifest.json >> tmp/reports/objc3c-native/m220/lowering-runtime-public-beta-triage/replay-markers.txt`
4. `python -m pytest tests/tooling/test_objc3c_m220_lowering_public_beta_contract.py -q`

## M221 lowering/runtime GA blocker burn-down profile

GA-blocker burn-down evidence for lowering/runtime should be captured as a deterministic packet rooted under `tmp/`:

- `packet root`:
  - `tmp/artifacts/compilation/objc3c-native/m221/lowering-ga-blocker-burndown/`
- `packet artifacts`:
  - `tmp/artifacts/compilation/objc3c-native/m221/lowering-ga-blocker-burndown/module.ll`
  - `tmp/artifacts/compilation/objc3c-native/m221/lowering-ga-blocker-burndown/module.manifest.json`
  - `tmp/reports/objc3c-native/m221/lowering-ga-blocker-burndown/replay-markers.txt`
- `ABI/IR anchors` (persist verbatim in triage packet):
  - `; lowering_ir_boundary = runtime_dispatch_symbol=<symbol>;runtime_dispatch_arg_slots=<N>;selector_global_ordering=lexicographic`
  - `; frontend_profile = language_version=<N>, compatibility_mode=<mode>, migration_assist=<bool>, migration_legacy_total=<count>`
  - `!objc3.frontend = !{!0}`
  - `declare i32 @<symbol>(i32, ptr, i32, ..., i32)`
  - `"lowering":{"runtime_dispatch_symbol":"<symbol>","runtime_dispatch_arg_slots":<N>,"selector_global_ordering":"lexicographic"}`
- `replay markers` (source anchors to include in packet notes):
  - `Objc3LoweringIRBoundaryReplayKey(...)`
  - `invalid lowering contract runtime_dispatch_symbol`
- `GA blocker closure signal`:
  - identical source + lowering options produce byte-identical packet artifacts and stable replay markers.

Burn-down capture commands (lowering/runtime lane):

1. `npm run compile:objc3c -- tests/tooling/fixtures/native/hello.objc3 --out-dir tmp/artifacts/compilation/objc3c-native/m221/lowering-ga-blocker-burndown --emit-prefix module`
2. `rg -n "lowering_ir_boundary|frontend_profile|!objc3.frontend|declare i32 @" tmp/artifacts/compilation/objc3c-native/m221/lowering-ga-blocker-burndown/module.ll > tmp/reports/objc3c-native/m221/lowering-ga-blocker-burndown/replay-markers.txt`
3. `rg -n "\"lowering\":{\"runtime_dispatch_symbol\"" tmp/artifacts/compilation/objc3c-native/m221/lowering-ga-blocker-burndown/module.manifest.json >> tmp/reports/objc3c-native/m221/lowering-ga-blocker-burndown/replay-markers.txt`
4. `python -m pytest tests/tooling/test_objc3c_m221_lowering_ga_blocker_contract.py -q`

## M224 lowering/LLVM IR/runtime ABI release readiness

GA readiness evidence for native `.objc3` lowering remains deterministic and fail-closed:

- IR replay markers must remain present and aligned:
  - `; lowering_ir_boundary = runtime_dispatch_symbol=<symbol>;runtime_dispatch_arg_slots=<N>;selector_global_ordering=lexicographic`
  - `; frontend_profile = language_version=<N>, compatibility_mode=<mode>, migration_assist=<bool>, migration_legacy_total=<count>`
  - `!objc3.frontend = !{!0}`
- Manifest lowering packet must mirror the runtime boundary contract:
  - `"lowering":{"runtime_dispatch_symbol":"<symbol>","runtime_dispatch_arg_slots":<N>,"selector_global_ordering":"lexicographic"}`
- Runtime ABI declaration remains symbol-aligned with the lowering contract when dispatch calls are emitted:
  - `declare i32 @<symbol>(i32, ptr, i32, ..., i32)`
- Boundary normalization remains fail-closed for invalid symbols:
  - `invalid lowering contract runtime_dispatch_symbol`
- Determinism expectation for GA:
  - identical source + lowering options produce byte-identical `module.ll` and `module.manifest.json`.

Operator evidence sequence:

1. Generate artifacts in a deterministic tmp root:

```powershell
npm run compile:objc3c -- tests/tooling/fixtures/native/hello.objc3 --out-dir tmp/artifacts/compilation/objc3c-native/m224/lowering-release-readiness --emit-prefix module
```

2. Validate marker alignment in:
  - `tmp/artifacts/compilation/objc3c-native/m224/lowering-release-readiness/module.ll`
  - `tmp/artifacts/compilation/objc3c-native/m224/lowering-release-readiness/module.manifest.json`
3. Run contract guard:
  - `python -m pytest tests/tooling/test_objc3c_m224_lowering_release_contract.py -q`

## M225 lowering/runtime roadmap seeding profile

Post-1.0 backlog seeding for lowering/runtime 1.1/1.2 should record deterministic artifact evidence plus source-anchored ABI/IR signals:

- `1.1 artifact evidence capture`:
  - `tmp/artifacts/compilation/objc3c-native/m225/lowering-roadmap-seeding/module.ll`
  - `tmp/artifacts/compilation/objc3c-native/m225/lowering-roadmap-seeding/module.manifest.json`
  - extract and persist:
    - `; lowering_ir_boundary = runtime_dispatch_symbol=<symbol>;runtime_dispatch_arg_slots=<N>;selector_global_ordering=lexicographic`
    - `; frontend_profile = language_version=<N>, compatibility_mode=<mode>, migration_assist=<bool>, migration_legacy_total=<count>`
    - `declare i32 @<symbol>(i32, ptr, i32, ..., i32)`
    - `"lowering":{"runtime_dispatch_symbol":"<symbol>","runtime_dispatch_arg_slots":<N>,"selector_global_ordering":"lexicographic"}`
- `1.2 ABI/IR signal extraction`:
  - replay-key source marker: `Objc3LoweringIRBoundaryReplayKey(...)`
  - IR ABI declaration marker: `declare i32 @` + `runtime_dispatch_symbol`
  - lowering normalization marker: `invalid lowering contract runtime_dispatch_symbol`

Roadmap-seeding commands (lowering/runtime lane):

1. `npm run compile:objc3c -- tests/tooling/fixtures/native/hello.objc3 --out-dir tmp/artifacts/compilation/objc3c-native/m225/lowering-roadmap-seeding --emit-prefix module`
2. `python -m pytest tests/tooling/test_objc3c_m225_lowering_roadmap_seed_contract.py -q`

## Recovery fixture layout (`tests/tooling/fixtures/native/recovery`)

Current recovery fixtures are partitioned as:

```text
tests/tooling/fixtures/native/recovery/
  positive/
    bool_param_flow_relational.objc3
    comparison_logic.objc3
    function_return_annotation_bool.objc3
    function_return_annotation_bool.objc3-ir.expect.txt
    hello.objc3
    main_bool_entrypoint.objc3
    main_bool_entrypoint.objc3-ir.expect.txt
    lowering_dispatch/
      msgsend_lookup_basic.m
      msgsend_lookup_basic.dispatch-ir.expect.txt
      msgsend_lookup_two_args.m
      msgsend_lookup_two_args.dispatch-ir.expect.txt
    message_send_basic.objc3
    message_send_bool_compatible.objc3
    message_send_four_args.objc3
    message_send_keywords.objc3
    relational_logical_bool_literals.objc3
    return_paths_ok.objc3
    typed_bool_param_i32_expr_call.objc3
    typed_bool_param_i32_expr_call.objc3-ir.expect.txt
    typed_bool_return_literal_edges.objc3
    typed_bool_return_literal_edges.objc3-ir.expect.txt
    typed_i32_bool.objc3
    typed_i32_bool.objc3-ir.expect.txt
    typed_i32_return_from_bool_expr.objc3
    typed_i32_return_from_bool_expr.objc3-ir.expect.txt
  negative/
    neg_bool_fn_value.objc3
    neg_i32_param_from_bool_call.objc3
    neg_param_nullable.objc3
    neg_param_type_generic.objc3
    neg_rel_arg_mismatch.objc3
    neg_typed_bool_return_from_i32_call.objc3
    negative_arity_mismatch.objc3
    negative_message_arg_function.objc3
    negative_message_missing_keyword_colon.objc3
    negative_message_receiver_function.objc3
    negative_message_too_many_args.objc3
    negative_message_unterminated.objc3
    negative_missing_return.objc3
    negative_return_annotation_type_mismatch.objc3
    negative_return_annotation_unsupported_type.objc3
    negative_type_mismatch.objc3
    negative_undefined_symbol.objc3
```

`scripts/check_objc3c_native_recovery_contract.ps1` discovers fixtures recursively under `positive/` and `negative/` and enforces contract behavior per class (compile success/object+manifest artifacts for positive fixtures, deterministic failure diagnostics for negative fixtures).

`scripts/run_objc3c_lowering_regression_suite.ps1` also supports per-fixture dispatch IR assertions for Objective-C fixtures:

- Place `<fixture>.dispatch-ir.expect.txt` next to the `.m` file.
- One unique non-comment token per line (`#` and blank lines ignored; duplicate tokens fail validation).
- Optional dispatch fixture roots are discovered in deterministic order when present:
  - `tests/tooling/fixtures/native/recovery/positive/lowering_dispatch`
  - `tests/tooling/fixtures/native/dispatch/positive`
- The suite emits replay artifacts `module.dispatch.ll` for run1/run2 under the case directory and requires:
  - clang emit success for both runs,
  - deterministic `module.dispatch.ll` bytes across replay,
  - every token to appear in both emitted IR files.
- For `.objc3` fixtures, place `<fixture>.objc3-ir.expect.txt` next to the source file to assert deterministic native-lowered IR markers (`module.ll`) across replay runs.

## M26 Lane-E execution smoke harness contract (`scripts/check_objc3c_native_execution_smoke.ps1`)

Execution smoke validates a native `.objc3` compile-link-run path and fails closed on the first contract breach.

- Harness preconditions:
  - Native compiler executable must exist at `artifacts/bin/objc3c-native.exe` (or be buildable via `scripts/build_objc3c_native.ps1`).
  - Runtime shim source must exist at `tests/tooling/runtime/objc3_msgsend_i32_shim.c`.
  - `clang` must be invokable (override with `OBJC3C_NATIVE_EXECUTION_CLANG_PATH`).
- Positive fixture flow:
  - Discovers `*.objc3` recursively under `tests/tooling/fixtures/native/execution/positive` in deterministic sorted order.
  - Requires `<fixture>.exitcode.txt` sidecar with one integer expected process exit code.
  - Compiles via `objc3c-native.exe` and requires `module.obj`.
  - When positive `.meta.json` explicitly sets `execution.requires_runtime_shim`, harness verifies `module.ll` parity:
    - `true`: lowered IR must include both runtime dispatch declaration and call for the configured symbol.
    - `false`: lowered IR must include neither runtime dispatch declaration nor call for the configured symbol.
  - Links with `clang <module.obj> tests/tooling/runtime/objc3_msgsend_i32_shim.c -o module.exe`.
  - Runs `module.exe` and requires `run_exit == expected_exit`.
- Negative fixture flow:
  - Discovers `*.objc3` recursively under `tests/tooling/fixtures/native/execution/negative` in deterministic sorted order.
  - Requires `<fixture>.meta.json` sidecar.
  - Sidecar contract currently consumed by the harness:
    - `fixture`: filename that must match the fixture.
    - `expect_failure.stage`: `compile`, `link`, or `run`.
    - `expect_failure.required_diagnostic_tokens`: required stage-diagnostic tokens (case-sensitive matching).
    - `execution.requires_runtime_shim`: informational flag documenting whether a successful execution path would require the shim.
    - `execution.runtime_dispatch_symbol`: optional symbol name (defaults to `objc3_msgsend_i32`).
  - Stage behavior:
    - `compile`: requires non-zero compile exit and expected tokens in compile diagnostics.
    - `link`: requires compile success + non-zero link exit + expected tokens in link diagnostics.
    - `run`: requires compile/link success + non-zero run exit + expected tokens in run diagnostics.
- Output contract:
  - Per-run artifacts: `tmp/artifacts/objc3c-native/execution-smoke/<run_id>/`.
  - Per-case logs: compile/link/run logs and compile out-dir artifacts.
  - Summary JSON: `summary.json` with `status`, `total`, `passed`, `failed`, and per-case result rows.

## Runtime shim contract (`objc3_msgsend_i32`)

- `.objc3` message-send lowering emits direct calls with configurable slot count:
  - `declare i32 @objc3_msgsend_i32(i32, ptr, i32, ..., i32)`
  - Slot count equals `--objc3-max-message-args` (default `4`).
- Native execution smoke expects a C-ABI compatible runtime shim symbol named exactly `objc3_msgsend_i32`, equivalent to:

```c
int objc3_msgsend_i32(int receiver, const char *selector, int a0, int a1, int a2, int a3);
```

- Call-shape contract:
  - `receiver` is lowered as `i32`.
  - `selector` is a pointer to a lowered selector literal.
  - Dispatch argument slots are passed as `i32`; unused slots are zero-filled by lowering.
  - When lowered receiver value is `0` (`nil`), native IR short-circuits message send result to `0`.
  - Link-time runtime-dispatch symbol resolution still applies because emitted IR keeps a non-nil dispatch branch.
  - For direct syntactic `nil` receiver sends that lower to constant `0`, runtime-dispatch declaration/call emission is omitted.
  - For immutable local bindings initialized to `nil`, runtime-dispatch declaration/call emission is omitted when no other non-elided sends are present.
  - For flow-sensitive nil-bound send sites prior to reassignment, runtime-dispatch call emission is omitted.
  - Explicit `= nil` reassignment regains nil-bound elision eligibility for subsequent identifier receiver sends.
  - Nil-provenance elision eligibility propagates through `let`/`for let` alias bindings when their initializer expression is nil-proven.
  - Conditional receiver expressions are nil-elided when condition evaluation is compile-time known and selects a nil-proven branch.
  - Immutable global identifiers with nil-proven initializers are nil-elided under the same dispatch omission contract.
  - Mutable global identifiers are excluded from global nil elision and continue to lower through runtime nil-check branch scaffolding.
  - Runtime-dispatch declaration emission is now usage-driven: it is emitted only when lowering emits at least one non-elided runtime-dispatch call.
  - Compile-time non-nil receivers lower through a direct runtime-dispatch call path without `msg_nil/msg_dispatch` branch scaffolding.
  - Current non-nil fast-path eligibility includes non-zero integer literals, `YES`, unary constant-expression receivers, short-circuit logical constant-expression receivers, compile-time non-zero global constant identifiers, and flow-sensitive local bindings proven compile-time non-zero at the send site.
  - Global identifier proofing is mutation-gated: only receiver globals with no detected write forms in function bodies (`=`, compound assignment, `++`/`--`, and `for` init/step assignment/update clauses) are eligible for compile-time non-zero fast-path lowering.
  - Call boundaries invalidate global receiver proof state only when the callee is effect-summary impure; pure-call boundaries retain global nil/non-zero proofs and preserve post-call fast-path/elision eligibility.
  - `pure fn` annotations are supported for prototypes and definitions: pure-annotated external prototypes are treated as side-effect-free call boundaries, while impure pure-annotated definitions fail during semantic validation with deterministic diagnostic `O3S215` (`pure contract violation: function '<name>' declared 'pure' has side effects (cause: <cause-token>; cause-site:<line>:<column>; detail:<detail-token>@<line>:<column>)`) emitted at the definition location.
  - Deterministic pure-contract cause tokens currently include `global-write`, `message-send`, `impure-callee:<name>`, and `unannotated-extern-call:<name>`.
  - For call-derived causes (`impure-callee:*`, `unannotated-extern-call:*`), `cause-site` coordinates bind to the callee identifier token location.
  - `detail` carries the propagated leaf impurity token/site so transitive call-derived failures expose the underlying root cause deterministically.
  - Compile-time non-nil proofing uses local constant-expression tracking for literals, identifiers bound to compile-time constants, conditionals, unary canonical forms, and supported binary operators.
  - Logical `&&`/`||` constant evaluation follows short-circuit semantics so skipped branches are not required to be compile-time evaluable for receiver non-nil proofing.
  - Flow-sensitive non-zero proofs can be regained after explicit `=` writes to compile-time non-zero values and are invalidated by subsequent writes that are not compile-time non-zero proofs.
  - Numeric zero receiver sends keep runtime-dispatch declaration/call emission.
  - Reassignments to non-nil or runtime-unknown values keep runtime-dispatch declaration/call emission.
- Determinism assumption for smoke:
  - For identical inputs, shim behavior should be deterministic and stable.
  - Harness assertions are based on process exit code equality (positive fixtures) and deterministic link diagnostics token matching (negative fixtures).

## Execution fixture layout (`tests/tooling/fixtures/native/execution`)

Execution smoke fixture roots and sidecars:

```text
tests/tooling/fixtures/native/execution/
  positive/
    <name>.objc3
    <name>.exitcode.txt
    [optional] <name>.meta.json
  negative/
    <name>.objc3
    <name>.meta.json
```

Metadata conventions consumed by the current harness:

- Positive sidecar (`<name>.exitcode.txt`):
  - Single integer exit code (parsed as `int`).
- Optional positive sidecar (`<name>.meta.json`):
  - `fixture`: must match fixture filename.
  - `execution.native_compile_args`: optional string array appended to native compiler invocation.
  - `execution.requires_runtime_shim` (optional): when present, execution smoke enforces `module.ll` dispatch declaration/call parity for the configured dispatch symbol.
  - `execution.runtime_dispatch_symbol` (optional): runtime dispatch symbol for parity checks (defaults to `objc3_msgsend_i32`).
- Negative sidecar (`<name>.meta.json`):
  - `expect_failure.stage`: `compile`, `link`, or `run`.
  - `expect_failure.required_diagnostic_tokens`: deterministic stage-diagnostic token list.
  - `execution.requires_runtime_shim`: informational flag for runtime dependency.
  - `execution.runtime_dispatch_symbol` (optional): expected runtime symbol.

Current repository state on 2026-02-27:

- `tests/tooling/fixtures/native/execution/positive` includes baseline, assignment, do-while, and for-loop smoke fixtures with `.exitcode.txt` sidecars.
- `tests/tooling/fixtures/native/execution/negative` includes compile/link-stage fixtures with `.meta.json` sidecars.

## Build and compile commands

From repo root:

```powershell
npm run build:objc3c-native
npm run compile:objc3c -- tests/tooling/fixtures/native/hello.objc3
```

Native driver option for `.objc3` frontend behavior:
- `--objc3-max-message-args <0-16>`
  - Default: `4`
  - Tightens the semantic arg-cap check for bracketed message-send expressions.
- `--objc3-runtime-dispatch-symbol <symbol>`
  - Default: `objc3_msgsend_i32`
  - Rebinds the runtime dispatch call target used by message-send lowering (`[A-Za-z_.$][A-Za-z0-9_.$]*`).

Direct script equivalents:

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File scripts/build_objc3c_native.ps1
powershell -NoProfile -ExecutionPolicy Bypass -File scripts/objc3c_native_compile.ps1 tests/tooling/fixtures/native/hello.objc3
```

## Driver shell split validation commands (M136-E001)

Use one `.objc3` input and one non-`.objc3` Objective-C input to validate both shell branches:

```powershell
npm run build:objc3c-native
npm run compile:objc3c -- tests/tooling/fixtures/native/hello.objc3 --out-dir tmp/artifacts/objc3c-native/m136-driver-shell/objc3 --emit-prefix module_objc3
npm run compile:objc3c -- tests/tooling/fixtures/native/recovery/positive/lowering_dispatch/msgsend_lookup_basic.m --out-dir tmp/artifacts/objc3c-native/m136-driver-shell/objectivec --emit-prefix module_objc
```

Expected success surface:

- `.objc3` compile writes diagnostics + manifest + `module_objc3.ll` + `module_objc3.obj`.
- Objective-C compile writes diagnostics + manifest + `module_objc.obj`.
- Both invocations exit `0` on success.

## Lexer extraction contract artifacts (M137-E001)

`scripts/check_objc3c_lexer_extraction_token_contract.ps1` emits deterministic validation artifacts under:

- `tmp/artifacts/objc3c-native/lexer-extraction-token-contract/<run_id>/`
  - `summary.json` (contract id, pass/fail counters, check rows, and per-case replay evidence)
  - run logs and replay output directories for positive/negative lexer fixtures

Commands:

```powershell
npm run test:objc3c:lexer-extraction-token-contract
npm run test:objc3c:lexer-parity
npm run check:compiler-closeout:m137
```

## Parser/AST extraction validation artifacts (M138-E001)

`npm run test:objc3c:parser-replay-proof` writes deterministic replay-proof outputs under:

- `tmp/artifacts/objc3c-native/parser-replay-proof/<proof_run_id>/summary.json`

Parser/AST extraction surface validation commands:

```powershell
npm run test:objc3c:parser-ast-extraction
npm run check:compiler-closeout:m138
```

`npm run check:compiler-closeout:m138` fail-closes on parser + AST builder + docs/CI/release wiring drift via:

- `python scripts/check_m138_parser_ast_contract.py`
- `python -m pytest tests/tooling/test_objc3c_parser_extraction.py tests/tooling/test_objc3c_parser_ast_builder_extraction.py -q`

## Sema pass-manager + diagnostics bus validation artifacts (M139-E001)

Sema extraction and diagnostics-bus validation commands:

```powershell
npm run test:objc3c:sema-pass-manager-diagnostics-bus
npm run check:compiler-closeout:m139
```

`npm run test:objc3c:sema-pass-manager-diagnostics-bus` writes deterministic pass-manager diagnostics-bus proof artifacts under:

- `tmp/artifacts/compilation/objc3c-native/m143/sema-pass-manager-diagnostics-bus-contract/<run_id>/summary.json`

Deterministic run-id behavior:

- Default run id is fixed to `m143-sema-type-system-default`.
- Optional override remains `OBJC3C_SEMA_PASS_MANAGER_DIAG_BUS_CONTRACT_RUN_ID` (validated token syntax).

`npm run check:compiler-closeout:m139` fail-closes on sema pass-manager and diagnostics-bus wiring drift via:

- `python scripts/check_m139_sema_pass_manager_contract.py`
- `powershell -NoProfile -ExecutionPolicy Bypass -File scripts/check_objc3c_sema_pass_manager_diagnostics_bus_contract.ps1`
- `python -m pytest tests/tooling/test_objc3c_sema_extraction.py tests/tooling/test_objc3c_sema_pass_manager_extraction.py tests/tooling/test_objc3c_parser_contract_sema_integration.py tests/tooling/test_objc3c_pure_contract_extraction.py tests/tooling/test_objc3c_frontend_types_extraction.py -q`

## Frontend library boundary validation artifacts (M140-E001)

M140 extraction and boundary closeout commands:

```powershell
npm run test:objc3c:m140-boundary-contract
npm run check:compiler-closeout:m140
```

`npm run check:compiler-closeout:m140` fail-closes on frontend library boundary drift via:

- `python scripts/check_m140_frontend_library_boundary_contract.py`
- `python -m pytest tests/tooling/test_objc3c_frontend_library_entrypoint_extraction.py tests/tooling/test_objc3c_m140_boundary_contract.py tests/tooling/test_objc3c_sema_extraction.py tests/tooling/test_objc3c_sema_pass_manager_extraction.py tests/tooling/test_objc3c_lowering_contract.py tests/tooling/test_objc3c_ir_emitter_extraction.py -q`

## CMake targetization and linkage topology validation artifacts (M141-E001)

M141 target-topology validation commands:

```powershell
npm run test:objc3c:m141-target-topology
npm run check:compiler-closeout:m141
```

`npm run check:compiler-closeout:m141` fail-closes on targetization/linkage-topology drift via:

- `python scripts/check_m141_cmake_target_topology_contract.py`
- `python -m pytest tests/tooling/test_objc3c_driver_cli_extraction.py tests/tooling/test_objc3c_cmake_target_topology.py tests/tooling/test_objc3c_process_io_extraction.py tests/tooling/test_objc3c_parser_contract_sema_integration.py tests/tooling/test_objc3c_sema_extraction.py tests/tooling/test_objc3c_sema_pass_manager_extraction.py tests/tooling/test_objc3c_lowering_contract.py tests/tooling/test_objc3c_ir_emitter_extraction.py -q`

## Frontend lowering parity harness artifacts (M142-E001)

Parity harness replay commands:

```powershell
npm run check:objc3c:library-cli-parity:source
npm run check:compiler-closeout:m142
```

`npm run check:objc3c:library-cli-parity:source` writes deterministic parity outputs under:

- `tmp/artifacts/objc3c-native/m142/library-cli-parity/work/library/`
- `tmp/artifacts/objc3c-native/m142/library-cli-parity/work/cli/`
- `tmp/artifacts/objc3c-native/m142/library-cli-parity/summary.json`

Where `<work_key>` is deterministic (derived from source + emit/lowering/runtime controls when not passed explicitly), effective replay roots are:

- `tmp/artifacts/objc3c-native/m142/library-cli-parity/work/<work_key>/library/`
- `tmp/artifacts/objc3c-native/m142/library-cli-parity/work/<work_key>/cli/`

Expected compared artifacts per side (`emit_prefix=module` default):

- `module.diagnostics.json`
- `module.manifest.json`
- `module.ll`
- `module.obj`

Object backend note for harness replay:

- CLI emits backend provenance sidecar `module.object-backend.txt` (`clang` or `llvm-direct`).
- M142 parity dimensions exclude `module.object-backend.txt`; it is a provenance note, not a compared artifact payload.
- Source-mode parity command pins `--cli-ir-object-backend clang` so CLI and C API object outputs are backend-aligned.

`npm run check:compiler-closeout:m142` fail-closes on parity harness source/docs/package drift via:

- `python scripts/check_m142_frontend_lowering_parity_contract.py`
- `python -m pytest tests/tooling/test_objc3c_library_cli_parity.py tests/tooling/test_objc3c_c_api_runner_extraction.py tests/tooling/test_objc3c_frontend_lowering_parity_contract.py tests/tooling/test_objc3c_sema_cli_c_api_parity_surface.py -q`

## Artifact tmp-path governance artifacts (M143-D001)

Tmp-governed parity governance commands:

```powershell
npm run check:objc3c:library-cli-parity:source:m143
npm run check:compiler-closeout:m143
```

`npm run check:objc3c:library-cli-parity:source:m143` writes replay-governed outputs under:

- `tmp/artifacts/compilation/objc3c-native/library-cli-parity/work/<work-key>/library/`
- `tmp/artifacts/compilation/objc3c-native/library-cli-parity/work/<work-key>/cli/`
- `tmp/artifacts/compilation/objc3c-native/m143/library-cli-parity/summary.json`

Governance contract notes:

- Work roots are deterministic via `--work-key` (or default-derived deterministic key when omitted).
- Source mode fail-closes when stale `<emit-prefix>` outputs are detected in target work roots.
- Source mode fail-closes when expected generated parity artifacts are missing after command execution.
- Tmp-path policy is default-enforced; non-tmp work roots require explicit opt-in.
- Lane-C lowering/runtime artifact roots remain under `tmp/artifacts/objc3c-native/`:
  - `tmp/artifacts/objc3c-native/lowering-regression/<run_id>/summary.json`
  - `tmp/artifacts/objc3c-native/typed-abi-replay-proof/<run_id>/summary.json`
  - `tmp/artifacts/objc3c-native/lowering-replay-proof/<proof_run_id>/summary.json`
- Lane-C deterministic default run ids:
  - `m143-lane-c-lowering-regression-default` (`OBJC3C_NATIVE_LOWERING_RUN_ID`)
  - `m143-lane-c-typed-abi-default` (`OBJC3C_TYPED_ABI_REPLAY_PROOF_RUN_ID`)
  - `m143-lane-c-lowering-replay-proof-default` (`OBJC3C_NATIVE_LOWERING_REPLAY_PROOF_RUN_ID`)

`npm run check:compiler-closeout:m143` fail-closes on tmp-governance source/docs/package drift via:

- `python scripts/check_m143_artifact_tmp_governance_contract.py`
- `python -m pytest tests/tooling/test_objc3c_library_cli_parity.py tests/tooling/test_objc3c_driver_cli_extraction.py tests/tooling/test_objc3c_c_api_runner_extraction.py tests/tooling/test_objc3c_parser_extraction.py tests/tooling/test_objc3c_parser_ast_builder_extraction.py tests/tooling/test_objc3c_sema_extraction.py tests/tooling/test_objc3c_sema_pass_manager_extraction.py tests/tooling/test_objc3c_frontend_types_extraction.py tests/tooling/test_objc3c_lowering_contract.py tests/tooling/test_objc3c_ir_emitter_extraction.py tests/tooling/test_objc3c_m143_artifact_tmp_governance_contract.py tests/tooling/test_objc3c_m143_sema_type_system_tmp_governance_contract.py tests/tooling/test_objc3c_m143_lowering_runtime_abi_tmp_governance_contract.py tests/tooling/test_check_m143_artifact_tmp_governance_contract.py -q`

## LLVM capability discovery artifacts (M144-E001)

Capability discovery and routing validation commands:

```powershell
npm run check:objc3c:llvm-capabilities
npm run check:objc3c:library-cli-parity:source:m144
npm run check:compiler-closeout:m144
```

Capability probe summary output:

- `tmp/artifacts/objc3c-native/m144/llvm_capabilities/summary.json`

Capability-routed source-mode parity output:

- `tmp/artifacts/compilation/objc3c-native/m144/library-cli-parity/work/<work-key>/library/`
- `tmp/artifacts/compilation/objc3c-native/m144/library-cli-parity/work/<work-key>/cli/`
- `tmp/artifacts/compilation/objc3c-native/m144/library-cli-parity/summary.json`

`npm run check:compiler-closeout:m144` fail-closes on capability discovery source/docs/package drift via:

- `python scripts/check_m144_llvm_capability_discovery_contract.py`
- `python -m pytest tests/tooling/test_probe_objc3c_llvm_capabilities.py tests/tooling/test_objc3c_library_cli_parity.py::test_parity_source_mode_routes_backend_from_capabilities_when_enabled tests/tooling/test_objc3c_library_cli_parity.py::test_parity_source_mode_fail_closes_when_capability_parity_is_unavailable tests/tooling/test_objc3c_library_cli_parity.py::test_parity_source_mode_fail_closes_when_capability_routing_is_requested_without_summary tests/tooling/test_objc3c_driver_llvm_capability_routing_extraction.py tests/tooling/test_objc3c_driver_cli_extraction.py tests/tooling/test_objc3c_m144_llvm_capability_discovery_contract.py tests/tooling/test_check_m144_llvm_capability_discovery_contract.py -q`

## Direct LLVM object-emission lane-B matrix artifacts (M145-B001)

Lane-B sema/type-system direct-LLVM matrix validation commands:

```powershell
npm run test:objc3c:m145-direct-llvm-matrix
npm run check:compiler-closeout:m145
```

`npm run test:objc3c:m145-direct-llvm-matrix` writes matrix artifacts under:

- `tmp/artifacts/compilation/objc3c-native/m143/sema-pass-manager-diagnostics-bus-contract/<run_id>/positive_smoke/`
- `tmp/artifacts/compilation/objc3c-native/m143/sema-pass-manager-diagnostics-bus-contract/<run_id>/positive_smoke_llvm_direct/`
- `tmp/artifacts/compilation/objc3c-native/m143/sema-pass-manager-diagnostics-bus-contract/<run_id>/positive_smoke_llvm_direct_forced_missing_llc/`
- `tmp/artifacts/compilation/objc3c-native/m143/sema-pass-manager-diagnostics-bus-contract/<run_id>/negative_backend_matrix/`
- `tmp/artifacts/compilation/objc3c-native/m143/sema-pass-manager-diagnostics-bus-contract/<run_id>/summary.json`

Fail-closed matrix markers captured in `summary.json` checks:

- `runtime.positive.matrix.llvm_direct_forced_missing_llc.exit_codes`
- `runtime.positive.matrix.llvm_direct_forced_missing_llc.fail_closed_marker`
- `runtime.negative.matrix.backend.exit_codes.<fixture>`

## Direct LLVM object-emission lane-D validation artifacts (M145-D001)

Lane-D conformance/perf validation commands:

```powershell
npm run test:objc3c:m145-direct-llvm-matrix:lane-d
npm run check:compiler-closeout:m145
```

Lane-D artifact roots:

- Conformance aggregation output:
  - `tmp/artifacts/conformance_suite/summary.json`
- Perf-budget summary output:
  - `tmp/artifacts/objc3c-native/perf-budget/<run_id>/summary.json`
- Coverage sources tied to lane-D fixture:
  - `tests/conformance/COVERAGE_MAP.md`
  - `tests/conformance/lowering_abi/manifest.json`
  - `tests/conformance/lowering_abi/M145-D001.json`

## Execution smoke commands (M26 lane-E)

```powershell
npm run test:objc3c:execution-smoke
npm run test:objc3c:execution-replay-proof
```

- Direct script equivalent path:

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File scripts/check_objc3c_native_execution_smoke.ps1
```

- Optional toolchain override:
  - `OBJC3C_NATIVE_EXECUTION_CLANG_PATH=<clang executable>`

