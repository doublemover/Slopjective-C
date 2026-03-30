# objc3c Compiler Throughput Boundary

## Working Boundary

This runbook defines the live compiler-throughput boundary for `M295`.

Use it when changing:

- parser, semantic, and lowering throughput on the live native compiler path
- incremental build cache reuse and invalidation behavior
- macro-host cache publication and compile-coupled docs generation cost
- heavyweight validation-tier ownership and duplicate compile removal
- compiler-throughput summaries, cache-proof artifacts, and packaged validation

Downstream `M295` work must stay on the existing native compiler executable,
compile wrapper, public workflow runner, native build wrapper, and validation
scripts listed here. Do not add a second benchmark harness, spreadsheet-only
measurement flow, or milestone-local validation packet.

## Throughput Taxonomy

The current truthful compiler-throughput workload families are:

- `compile-cold`
  - objective: measure one live wrapper-driven compile from source to emitted
    artifacts without cache reuse
- `compile-cache-hit`
  - objective: prove the existing wrapper cache restores a truthful output set
    without bypassing compile-output provenance or runtime-launch contracts
- `parser-sema-lowering`
  - objective: preserve the native compiler hot path from parse through semantic
    publication and lowering, not just final wall-clock timing
- `incremental-invalidation`
  - objective: measure and explain when compile reuse is invalidated instead of
    treating every build as cacheable
- `macro-host-runtime-boundary`
  - objective: keep metaprogramming host-cache publication compile-coupled and
    fail-closed while still timing the real artifact path
- `docs-generation`
  - objective: measure the checked-in native docs and command-surface generation
    paths that operators actually run
- `validation-tier-overlap`
  - objective: keep each heavyweight validation tier responsible for one
    guarantee so the public runner does not recompile the same corpus for
    duplicate signal

## Audit Inventory

The current audit inventory is:

- compiler/tooling throughput:
  - `scripts/objc3c_native_compile.ps1`
  - `artifacts/bin/objc3c-native.exe`
  - `scripts/build_objc3c_native.ps1`
  - `scripts/objc3c_public_workflow_runner.py`
- incremental build and invalidation:
  - wrapper `--use-cache`
  - `tmp/artifacts/objc3c-native/cache/`
  - incremental invalidation counters published through the live manifest and
    lowering/sema replay-key surfaces
- macro-host compile-coupled artifacts:
  - `tests/tooling/fixtures/native/macro_host_process_provider.objc3`
  - `tests/tooling/fixtures/native/macro_host_process_consumer.objc3`
  - `tests/tooling/runtime/macro_host_process_cache_integration_probe.cpp`
  - `module.metaprogramming-macro-host-cache.json`
- docs-generation paths:
  - `scripts/build_objc3c_native_docs.py`
  - `scripts/render_objc3c_public_command_surface.py`
  - `scripts/build_site_index.py`

## Workload Manifest

The authoritative workload inventory is checked in at
`tests/tooling/fixtures/compiler_throughput/workload_manifest.json`.

It currently divides the live surface into:

- `compile-cold-wrapper`
- `compile-cache-hit-wrapper`
- `incremental-cache-invalidation`
- `macro-host-cache-publication`
- `native-docs-generation`
- `command-surface-generation`
- `tier-overlap-audit`

## Validation Tier Map

The authoritative tier map is checked in at
`tests/tooling/fixtures/compiler_throughput/validation_tier_map.json`.

The live ownership split is:

- `test-fast`
  - bounded smoke slice
  - runtime acceptance and ABI/accessor proof
  - canonical replay/native-truth proof
- `test-smoke`
  - compile, link, and run execution behavior over the full runnable smoke
    corpus
- `test-recovery`
  - positive recovery compile success
  - negative recovery compile failure and deterministic diagnostics replay
- `test-full`
  - developer-facing default composite without the recovery fan-out
- `test-nightly`
  - non-default heavyweight coverage:
    - recovery
    - broad positive fixture matrix
    - static negative expectation header/token enforcement
    - conformance/stress/external/public/runtime-performance integrations

One authoritative owner per guarantee:

- compile/link/run execution behavior:
  - `test-execution-smoke`
- recovery compile success and deterministic diagnostics behavior:
  - `test-recovery`
- replay/native-truth proof:
  - `test-execution-replay`
- runtime acceptance and ABI/accessor proof:
  - `test-runtime-acceptance`
- negative expectation header enforcement:
  - `test-negative-expectations`
- broad positive recovery artifact sanity:
  - `test-fixture-matrix`

## Exact Live Implementation Paths

- compile/build wrappers:
  - `scripts/objc3c_native_compile.ps1`
  - `scripts/build_objc3c_native.ps1`
  - `scripts/objc3c_public_workflow_runner.py`
- heavyweight validation suites:
  - `scripts/check_objc3c_native_execution_smoke.ps1`
  - `scripts/check_objc3c_native_recovery_contract.ps1`
  - `scripts/check_objc3c_execution_replay_proof.ps1`
  - `scripts/run_objc3c_native_fixture_matrix.ps1`
  - `scripts/check_objc3c_negative_fixture_expectations.ps1`
  - `scripts/check_objc3c_runtime_acceptance.py`
- compile-coupled docs and command surfaces:
  - `scripts/build_objc3c_native_docs.py`
  - `scripts/render_objc3c_public_command_surface.py`
  - `docs/objc3c-native/src/60-tests.md`
  - `docs/runbooks/objc3c_public_command_surface.md`
- checked-in throughput metadata:
  - `tests/tooling/fixtures/compiler_throughput/source_surface.json`
  - `tests/tooling/fixtures/compiler_throughput/workload_manifest.json`
  - `tests/tooling/fixtures/compiler_throughput/validation_tier_map.json`

## Explicit Non-Goals

- no second compiler wrapper just for benchmarks
- no benchmark-only cache format or sidecar cache truth model
- no duplicate recovery-negative recompilation inside default validation tiers
- no docs-generation claim that is not tied to the checked-in docs builders
- no packaged-validation path that relies on repo-only benchmark helpers
