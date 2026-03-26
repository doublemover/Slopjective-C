# Objc3c Native Frontend (Current Surface)

The native frontend supports two input modes:

- `.objc3`: native lexer, parser, sema, lowering, IR emission, and object build
- non-`.objc3`: Objective-C parse/diagnostics and object build through the Objective-C path

## CLI Usage

```text
objc3c-native <input> [--out-dir <dir>] [--emit-prefix <name>] [--clang <path>] [--llc <path>] [-fobjc-version=<N>] [--objc3-language-version <N>] [--objc3-compat-mode <canonical|legacy>] [--objc3-migration-assist] [--objc3-ir-object-backend <clang|llvm-direct>] [--objc3-max-message-args <0-16>] [--objc3-runtime-dispatch-symbol <symbol>]
```

Defaults:

- output dir: `tmp/artifacts/compilation/objc3c-native`
- emit prefix: `module`
- clang: `clang`
- llc: `llc`
- language version: `3`
- runtime dispatch symbol: `objc3_msgsend_i32`

## C API Runner

```text
objc3c-frontend-c-api-runner <input> [--out-dir <dir>] [--emit-prefix <name>] [--clang <path>] [--llc <path>] [--summary-out <path>] [--no-emit-manifest] [--no-emit-ir] [--no-emit-object]
```

The native build publishes `artifacts/bin/objc3c-frontend-c-api-runner.exe`.
## Supported Grammar (Current)

The live `.objc3` frontend currently supports:

- module declarations
- global `let`
- functions and parameters
- statement blocks
- `if`, `while`, `do-while`, `for`, `switch`
- `break`, `continue`, `return`
- scalar expressions and conditional expressions
- arithmetic, logical, relational, bitwise, and shift operators
- function calls and bracket message sends
- selected Objective-C container and type forms admitted by the current parser

The grammar documentation here describes the admitted live surface only.
Historical milestone-by-milestone parser expansion notes are archived.
## Semantic Surface (Current)

The live frontend currently enforces:

- deterministic parser and semantic diagnostics
- lexical scope and symbol resolution
- scalar type compatibility across the admitted surface
- control-flow legality for loops, switches, and returns
- fail-closed handling for unsupported or incomplete language slices

## Lowering Surface (Current)

The live lowering path currently covers:

- scalar values and control flow
- function calls and admitted message-send lowering
- manifest generation
- LLVM IR emission
- object emission through the configured backend

## Runtime Boundary (Current)

The live compiler/runtime boundary is centered on emitted metadata plus the native runtime library under `native/objc3c/src/runtime` and `artifacts/lib/objc3_runtime.lib`.
## Runtime Execution Architecture (Current)

The live executable path is a single compile-to-runtime pipeline. Later runtime
closure work must extend this path, not bypass it.

## Working Boundary

- compiler-owned compile path:
  - `scripts/build_objc3c_native.ps1`
  - `scripts/objc3c_native_compile.ps1`
  - `native/objc3c/src/main.cpp`
  - `native/objc3c/src/driver/objc3_driver_main.cpp`
  - `native/objc3c/src/driver/objc3_compilation_driver.cpp`
  - `native/objc3c/src/driver/objc3_objc3_path.cpp`
  - `native/objc3c/src/lower/objc3_lowering_contract.h`
  - `native/objc3c/src/ir/objc3_ir_emitter.cpp`
  - `native/objc3c/src/io/objc3_process.cpp`
  - `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`
- runtime-owned installation and execution path:
  - `native/objc3c/src/runtime/objc3_runtime.h`
  - `native/objc3c/src/runtime/objc3_runtime.cpp`
  - `native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h`
- authoritative emitted artifacts:
  - `<prefix>.obj`
  - `<prefix>.ll`
  - `<prefix>.manifest.json`
  - `<prefix>.runtime-registration-manifest.json`
  - `<prefix>.compile-provenance.json`
- validation-owned proof path:
  - `scripts/check_objc3c_runtime_acceptance.py`
  - `scripts/check_objc3c_execution_replay_proof.ps1`
  - `scripts/check_objc3c_native_execution_smoke.ps1`
  - `scripts/objc3c_runtime_launch_contract.ps1`
  - `scripts/shared_compiler_runtime_acceptance_harness.py`

## Execution Flow

1. `scripts/build_objc3c_native.ps1` publishes `artifacts/bin/objc3c-native.exe`
   and `artifacts/lib/objc3_runtime.lib`.
2. `scripts/objc3c_native_compile.ps1` or the native executable drives the live
   `.objc3` compile path through driver, lowering, IR emission, and object
   emission.
3. The compile path emits the object, LLVM IR, registration manifest, and
   compile provenance as one coupled artifact set.
4. The runtime installs emitted image state through
   `objc3_runtime_register_image`, resolves selectors through
   `objc3_runtime_lookup_selector`, and executes calls through
   `objc3_runtime_dispatch_i32`.
5. Acceptance and replay proof only count when the emitted object, registration
   manifest, and linked probe all come from that same compile path.

## State Publication Surface

- front-door emitted surface:
  - `<prefix>.manifest.json`
  - key: `runtime_state_publication_surface`
- coupled runtime-owned surface:
  - `<prefix>.runtime-registration-manifest.json`
- required compile-owned artifacts:
  - `<prefix>.obj`
  - `<prefix>.ll`
- required public runtime ABI boundary:
  - `objc3_runtime_register_image`
  - `objc3_runtime_lookup_selector`
  - `objc3_runtime_dispatch_i32`
  - `objc3_runtime_reset_for_testing`

The compile manifest is the authoritative front-door runtime state publication
surface. It must point at the coupled registration manifest, emitted object and
IR artifacts, the runtime archive path, the registration entrypoint, the runtime
state snapshot symbol, and the published descriptor counts.

## Acceptance Suite Surface

- authoritative suite:
  - `scripts/check_objc3c_runtime_acceptance.py`
- authoritative report:
  - `tmp/reports/runtime/acceptance/summary.json`
- machine-readable key:
  - `acceptance_suite_surface`

The acceptance suite surface defines which cases may claim published runtime
state. It is authoritative only when the suite consumes the emitted compile
manifest, the coupled registration manifest, and the linked runtime probe path
together. The composite public workflow report carries this same surface
forward when a composite action runs runtime acceptance.

## Claim Boundary

- runnable:
  - behavior proven by real emitted objects linked against
    `artifacts/lib/objc3_runtime.lib`
  - runtime state derived from emitted registration tables and descriptors
  - probe observations produced through the public runtime entrypoints or
    compile-coupled manifests
- claim-only until later closure work lands:
  - any surface described only by comments, sidecars, or private placeholders
  - synthetic `.ll` or hand-authored artifacts with no matching compile output
  - proof that depends on compatibility shims without a coupled emitted object
  - future runtime capability that would require widening
    `native/objc3c/src/runtime/objc3_runtime.h`

## Explicit Non-Goals

- no milestone-specific compile wrappers, proof packets, or closeout sidecars
- no parallel dispatch or installation ABI outside the current runtime header
- no authoritative proof from replay text alone without emitted object and probe
- no widening of public runtime claims beyond what the live acceptance and probe
  path can execute today

The live runtime-acceptance, replay-proof, and composite public-workflow
reports publish this same claim boundary as machine-readable JSON so later work
cannot silently overclaim from sidecars or synthetic artifacts.

The runtime-owned subsystem dependency model is anchored in
`native/objc3c/src/runtime/ARCHITECTURE.md` and enforced by
`python scripts/check_objc3c_dependency_boundaries.py --strict`.
## Diagnostics

The live frontend writes deterministic diagnostics in two forms:

- `<prefix>.diagnostics.txt`
- `<prefix>.diagnostics.json`

Diagnostics are always emitted, including on failure.
The current native path is intentionally fail closed when it encounters unsupported constructs outside the admitted runnable surface.
## Artifacts and Exit Codes

For `.objc3` input:

- Always writes diagnostics text and JSON.
- On success writes:
  - manifest JSON
  - LLVM IR (`.ll`)
  - object file (`.obj`)

For non-`.objc3` input:

- Always writes diagnostics text and JSON.
- On success writes:
  - manifest JSON
  - compiled Objective-C object

Exit codes:

- `0`: success
- `1`: parse, semantic, or diagnostic failure
- `2`: CLI usage or invalid invocation
- `3`: toolchain compile step failure

## Build Artifacts

The live native build publishes:

- `artifacts/bin/objc3c-native.exe`
- `artifacts/bin/objc3c-frontend-c-api-runner.exe`
- `artifacts/lib/objc3_runtime.lib`

## Native Output Truth

Treat these as authoritative only when they come from a real compiler invocation:

- emitted LLVM IR
- emitted object files
- manifests that point at those emitted outputs
- executable probes that link against `artifacts/lib/objc3_runtime.lib`

Do not treat these as authoritative proof:

- hand-written `.ll` files
- compatibility shims by themselves
- sidecars that are not tied to a reproducible compile and probe path

## Current Corrective Gaps

- unresolved sends still have one deterministic arithmetic fallback path in `native/objc3c/src/runtime/objc3_runtime.cpp`
- synthesized accessor IR still carries transitional lowering residue in `native/objc3c/src/ir/objc3_ir_emitter.cpp`
- native proof remains invalid unless the emitted object, manifest, and linked runtime probe all come from the same reproducible compile path
## Live Validation Commands

From repo root:

```powershell
python scripts/objc3c_public_workflow_runner.py test-fast
python scripts/objc3c_public_workflow_runner.py test-recovery
python scripts/objc3c_public_workflow_runner.py test-execution-smoke
python scripts/objc3c_public_workflow_runner.py test-execution-replay
python scripts/objc3c_public_workflow_runner.py test-runtime-acceptance
python scripts/objc3c_public_workflow_runner.py test-full
python scripts/objc3c_public_workflow_runner.py test-nightly
python scripts/objc3c_public_workflow_runner.py test-ci
python scripts/objc3c_public_workflow_runner.py proof-objc3c
python scripts/ci/check_task_hygiene.py
python scripts/check_objc3c_dependency_boundaries.py --strict
```

Targeted entrypoints accept bounded selectors when you need signal without the full corpus:

```powershell
python scripts/objc3c_public_workflow_runner.py test-execution-smoke -Limit 12
python scripts/objc3c_public_workflow_runner.py test-recovery -Limit 24
python scripts/objc3c_public_workflow_runner.py test-fixture-matrix -ShardIndex 0 -ShardCount 4
python scripts/objc3c_public_workflow_runner.py test-negative-expectations -FixtureGlob "tests/tooling/fixtures/native/recovery/negative/negative_assignment_*"
python scripts/objc3c_public_workflow_runner.py test-execution-replay -CaseId synthesized-accessor
```

Composite runner entrypoints also write one integrated report to `tmp/reports/objc3c-public-workflow/<action>.json`, with the exact child-suite summary paths captured from the live smoke, runtime-acceptance, replay, recovery, and matrix scripts.

## What The Live Test Surface Covers

- `test-fast`: bounded execution-smoke slice, runtime acceptance, and canonical replay/native-truth proof
- `test-smoke`: full runnable execution smoke corpus
- `test-recovery`: recovery compile success and deterministic diagnostics replay as a non-default heavy path
- `test-full`: smoke, runtime acceptance, and replay/native-truth proof without the recovery fan-out
- `test-nightly`: full validation plus recovery, positive fixture-matrix, and static negative-expectation sweeps
- dependency-boundary enforcement
- compact task-hygiene enforcement
- runtime dispatch over realized classes/categories/protocols
- synthesized property accessor execution over realized instance storage
- runtime-backed storage ownership reflection over emitted property descriptors
- native-output provenance through real compile and probe paths

## Current Corrective Gaps Under Test

- unresolved dispatch still has one deterministic fallback path after slow-path miss
- synthesized accessor IR still carries transitional lowering residue even though live getter/setter execution is already runtime-backed
- native-output truth requires the emitted object and linked probe to stay coupled end to end
# libobjc3c_frontend Library API

This document describes the live embedding surface exposed by `native/objc3c/src/libobjc3c_frontend/api.h`.

## Public Surface

- primary header: `native/objc3c/src/libobjc3c_frontend/api.h`
- version header: `native/objc3c/src/libobjc3c_frontend/version.h`
- optional C shim header: `native/objc3c/src/libobjc3c_frontend/c_api.h`

`api.h` exposes a C ABI with an opaque frontend context type.

## Stability

- exported symbols, enums, and struct layouts in `api.h` are the ABI boundary
- append-only growth for public structs
- zero-initialize option and result structs before use

## Compatibility and Versioning

- version macros live in `version.h`
- use `objc3c_frontend_is_abi_compatible(OBJC3C_FRONTEND_ABI_VERSION)` before invoking compile entrypoints
- `objc3c_frontend_version().abi_version` must match `objc3c_frontend_abi_version()`
