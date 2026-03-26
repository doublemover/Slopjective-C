<!-- markdownlint-disable-file MD041 -->

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

## Shared Executable Acceptance Harness

- harness:
  - `scripts/shared_compiler_runtime_acceptance_harness.py`
- live executable suites:
  - `runtime-acceptance`
  - `public-test-fast`
  - `public-test-full`
- harness summary root:
  - `tmp/reports/runtime/shared-executable-acceptance-harness`

The shared executable acceptance harness is an integration runner over the live
runtime acceptance and public workflow entrypoints. It does not create a new
proof surface. Instead, it executes those suites, requires their emitted
reports to publish the runtime claim boundary, runtime state publication
surface, and acceptance suite surface contracts, and then writes a shared
summary that points back to those child executable reports.

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
