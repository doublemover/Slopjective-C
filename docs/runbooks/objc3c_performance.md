# objc3c Performance Benchmark Boundary

## Working Boundary

This runbook defines the live boundary for performance benchmark and comparison
work in the checked-in objc3c repo.

Use it when changing:

- compiler compile-latency benchmark flows
- showcase runtime timing flows
- comparative baseline workloads for ObjC2, Swift, and C++
- telemetry packets, normalization logic, and benchmark claim output

Downstream benchmark-surface work must stay on the existing compiler, showcase,
packaging, and public workflow surfaces listed here. Do not add a sidecar
benchmark app, synthetic spreadsheet workflow, or release-scope measurement
wrapper.

## Benchmark Taxonomy And Claim Classes

The current truthful benchmark taxonomy is:

- `compile-latency`
  - objective: measure end-to-end compile wall-clock behavior through the live
    objc3c public compile path
- `runtime-wall-clock`
  - objective: measure launch-and-exit wall-clock behavior for checked-in
    runnable objc3c showcase examples
- `comparative-compile-baseline`
  - objective: compare objc3c compile behavior against small reference
    workloads in ObjC2, Swift, and C++
- `comparative-runtime-baseline`
  - objective: compare executable runtime behavior only where the checked-in
    workload and toolchain make a real run possible
- `telemetry-raw-sample`
  - objective: preserve per-sample timings, command lines, toolchain versions,
    and hardware facts needed to replay or dispute a claim
- `normalized-summary`
  - objective: publish reproducible aggregates derived from raw sample packets
    instead of hand-written claims

The only current claim classes allowed from this surface are:

- `local-measurement`
  - means a result came from one explicitly described machine profile
- `toolchain-comparison`
  - means the same checked-in workload family was run through more than one
    compiler/runtime path with the exact commands captured
- `availability-limited`
  - means a baseline language was intentionally recorded as unavailable instead
    of silently omitted
- `non-portable`
  - means the result is meaningful for this machine/toolchain only and must not
    be presented as a universal claim

## Exact Live Implementation Paths

- public command and package surface:
  - package bridge: `npm run objc3c -- <action>`
  - `package.json`
  - `docs/runbooks/objc3c_public_command_surface.md`
- native compiler/runtime and build roots:
  - `native/objc3c/src/tools/objc3c_frontend_c_api_runner.cpp`
  - `npm run objc3c -- build-native-binaries`
  - `npm run objc3c -- compile-objc3c`
  - `npm run objc3c -- package-runnable-toolchain`
- checked-in objc3c workloads:
  - `showcase/portfolio.json`
  - `showcase/auroraBoard/main.objc3`
  - `showcase/signalMesh/main.objc3`
  - `showcase/patchKit/main.objc3`
  - `docs/tutorials/objc2_swift_cpp_comparison.md`
- benchmark workload and comparison corpus roots:
  - `tests/tooling/fixtures/performance/benchmark_portfolio.json`
  - `tests/tooling/fixtures/performance/objc3/`
  - `tests/tooling/fixtures/performance/baselines/`

## Live Artifact And Output Families

- native binaries and runtime library:
  - `artifacts/bin/objc3c-native.exe`
  - `artifacts/bin/objc3c-frontend-c-api-runner.exe`
  - `artifacts/lib/objc3_runtime.lib`
- machine-owned benchmark roots:
  - performance artifacts
  - performance reports
  - runnable-toolchain package staging
- build-owned owner artifact:
  - generated repo-superclean owner artifact selected by the checked-in build contract

## Exact Live Commands

- build the native toolchain before measuring:
  - `npm run objc3c -- build-native-binaries`
- compile one checked-in objc3c workload through the public compile path:
  - `npm run objc3c -- compile-objc3c showcase/auroraBoard/main.objc3`
- benchmark the live objc3 showcase workloads:
  - `npm run objc3c -- benchmark-performance`
- benchmark the checked-in ObjC2 Swift and C++ baselines:
  - `npm run objc3c -- benchmark-comparative-baselines`
- validate the staged runnable benchmark bundle:
  - `npm run objc3c -- validate-runnable-performance`
- run the integrated benchmark foundation validation flow:
  - `npm run objc3c -- validate-performance-foundation`
- stage the runnable toolchain before packaged benchmark validation:
  - `npm run objc3c -- package-runnable-toolchain`

## Exact Live Paths For Downstream Work

- benchmark boundary and operator guidance:
  - `docs/runbooks/objc3c_performance.md`
  - `docs/runbooks/objc3c_public_command_surface.md`
  - `docs/runbooks/objc3c_maintainer_workflows.md`
- benchmark corpus and comparison references:
  - `tests/tooling/fixtures/performance/benchmark_portfolio.json`
  - `showcase/portfolio.json`
  - `docs/tutorials/objc2_swift_cpp_comparison.md`
- executable build/package paths:
  - `npm run objc3c -- build-native-binaries`
  - `npm run objc3c -- compile-objc3c`
  - `npm run objc3c -- package-runnable-toolchain`
  - package bridge: `npm run objc3c -- <action>`

Helper implementations remain action-registry anchors for the public
commands above.

## Explicit Non-Goals

- no benchmark claims derived from screenshots or copied spreadsheet values
- no hidden hardware-normalization constants outside checked-in code
- no release-scope benchmark wrappers or duplicate package surfaces
- no cross-machine universal claims without raw sample packets
- no silent dropping of unavailable baseline toolchains
