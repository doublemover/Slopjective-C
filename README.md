# Objective-C 3.0

Slopjective-C is the native Objective-C 3.0 compiler, runtime, standard
library, package, tooling, and validation workspace. It takes `.objc3` source
through a checked compiler pipeline, emits LLVM/native artifacts, links against
an Objective-C 3 runtime surface, and ties public support claims to replayable
`npm run objc3c -- <action>` commands.

The project is built around one rule: if a feature is described as supported,
there is a checked source, fixture, schema, runtime probe, or public command
that owns that claim. Prose introduces the system; the capability matrix and
evidence map define exact support.

## Start With The Language

Objective-C 3.0 keeps Objective-C message syntax and runtime-oriented
programming, then adds a typed module language around it: module declarations,
typed functions, strict semantic flow, runtime-backed text and collection
values, package metadata, source/debug artifacts, and fail-closed diagnostics.

This checked fixture compiles and runs through the native execution smoke path:

```objc
module CollectionLiteralsMutationForIn;

fn main() -> i32 {
  var numbers = #array[1, 2, 3, 4];
  numbers += 5;
  numbers[1] = 7;
  delete numbers[0];

  var unique = #set[2, 2, 3, 4];
  unique += 5;
  delete unique[4];

  var lookup = #map[1: 10, 2: 20];
  lookup[2] = 25;
  delete lookup[1];

  var arraySum = 0;
  for (item in numbers) {
    arraySum += item;
  }

  var setSum = 0;
  for (item in unique) {
    setSum += item;
  }

  var mapSum = 0;
  for (key, value in lookup) {
    mapSum += key + value;
  }

  return numbers[0] + #array[8, 9][1] + arraySum + setSum + mapSum;
}
```

That example combines language and runtime features Objective-C 2.0 never put
behind one compiler contract: typed module functions, concrete runtime-owned
collection literals, indexed mutation, `for-in` lowering, strict runtime
handles, and deterministic failure modes.

The text runtime has a matching source-level path:

```objc
module SourceStringInterpolationTextI32;

extern fn objc3_runtime_stdlib_text_byte_count_i32(handle: Text) -> i32;
extern fn objc3_runtime_stdlib_text_equal_i32(left_handle: Text, right_handle: Text) -> i32;

fn main() -> i32 {
  let label = "count";
  let value = 42;
  let actual = "\(label)=\(value)";
  let expected = "count=42";
  if (objc3_runtime_stdlib_text_equal_i32(actual, expected) != 1) {
    return 70;
  }
  return objc3_runtime_stdlib_text_byte_count_i32(actual);
}
```

Compile either fixture from a fresh checkout after setup:

```powershell
npm run objc3c -- compile-objc3c tests/tooling/fixtures/native/execution/positive/collection_literals_mutation_for_in.objc3 --out-dir tmp/readme-collections --emit-prefix module
npm run objc3c -- compile-objc3c tests/tooling/fixtures/native/execution/positive/source_string_interpolation_text_i32.objc3 --out-dir tmp/readme-text --emit-prefix module
npm run objc3c -- compile-objc3c showcase/signalMesh/main.objc3 --out-dir tmp/readme-signalMesh --emit-prefix module
```

## Current Support Snapshot

The checked capability matrix currently contains 123 rows:

- 93 implemented rows
- 14 internal implementation or workflow-owner rows
- 13 reserved rows
- 3 rejected rows

Authoritative support data lives in:

- [`docs/support/capability_matrix.json`](docs/support/capability_matrix.json)
- [`docs/support/evidence_map.json`](docs/support/evidence_map.json)
- [`docs/support/capability_matrix.md`](docs/support/capability_matrix.md)
- [`docs/support/umbrella_readiness.md`](docs/support/umbrella_readiness.md)

Only `implemented` rows with manifest-backed `objc3c.behavior.*` claims are
public Objective-C 3.0 behavior. `reserved`, `rejected`, and `internal` rows
are deliberately not promoted by README text, PR bodies, issue comments, or
generated reports.

## What Works Now

### Compiler And Language

- Canonical `.objc3` module parsing.
- Typed module-level `fn`, `pure fn`, `let`, `var`, and `extern fn`
  declarations.
- Typed Objective-C interfaces, implementations, methods, categories,
  protocols, properties, and selector-bearing declarations.
- Parser and semantic diagnostics with structured recovery/fix-it records where
  the language surface defines them.
- Typed flow for current scalar, function, expression, statement, control-flow,
  protocol, generic, ownership, and effect surfaces.
- Statement-form guarded match patterns using
  `case pattern where bool_condition: { ... }`; expression match, `=>` arms,
  type-test patterns, and strict-profile promotion remain outside that row.
- Protocol-qualified existentials, witness-model evidence, generic
  protocol-qualified arguments, callable type parameters, variance and
  specialization evidence, and collection generic identity semantics.
- Block capture and escape legality, ownership/memory-model checks, ARC cleanup
  integration, try/catch semantics, and error-unwind cleanup lowering.
- Source string literals and `Text` interpolation lowering through the checked
  stdlib text runtime.
- `#array`, `#map`, and `#set` literals for current concrete i32-backed
  collection rows, plus indexed mutation, append-style mutation, deletion, and
  syntax-level `for-in` lowering.
- Strict runtime-dispatch lowering, LLVM IR module emission, and native object
  emission.
- Semantic-preserving optimization governance, cache-aware dispatch proof, and
  exact-target devirtualization for checked supported cases.

### Runtime

- Strict dispatch errors and checked runtime dispatch entrypoints.
- Runtime method-cache snapshots, invalidation generations, stale-generation
  fallback, and bounded cache visibility.
- Class and metaclass realization, interface method tables, category/protocol
  registration, property and ivar reflection, selector metadata, registration
  replay, and bounded query snapshots.
- Public C reflection APIs for supported class, protocol, property, selector,
  and storage metadata.
- Runtime debug trace payloads for structured inspection, async task lanes, and
  error-unwind lanes.
- Runtime debug-anchor identity, value-inspection evidence, native debug-info
  evidence, and statement stepping for the integrated object-model production
  artifact path.
- Block copy/dispose/invoke helpers, byref forwarding, ownership transfer
  hooks, error bridge cleanup, task continuation lifecycle, async actors, actor
  mailbox isolation, and property behavior materialization.
- Mixed-image replay, imported runtime package replay, package loader bridge
  behavior, and cross-module generic metadata where their implemented rows
  define the support boundary.

### Standard Library

Checked modules live under [`stdlib/modules/`](stdlib/modules/).

- `objc3.text`: runtime-owned UTF-8 text storage, string-view and byte-span
  shapes, byte counts, equality, basic i32 formatting, source string literal
  lowering, source interpolation lowering, text builders, Unicode scalar
  counting/access/iteration, and stable failure status for invalid handles or
  mutation during iteration.
- `objc3.collections`: concrete i32 array, slice, map-entry, set-iteration, and
  descriptor runtime shapes; deterministic collection handles; indexed lookup
  and mutation; append-style insertion; deletion; invalid/stale handle
  rejection; and generic collection identity/type descriptor evidence.
- `objc3.concurrency`: task spawn, task-group cancellation, executor hop, actor
  mailbox helper surfaces, and runtime-backed v1 concurrency contracts.
- `stdlib.core`: shared runtime-backed storage substrate used by the checked
  standard-library rows.

### Packages, Modules, And Interop

- Public import lookup and visibility/reexport/rebuild contracts.
- Direct `@import` syntax with parser-owned module identity, locked package
  provenance, and fail-closed malformed, missing-provenance, and ambiguous
  module identity diagnostics.
- Dependency graph diagnostics, stale cache rejection, missing module
  diagnostics, import cycle diagnostics, duplicate export diagnostics, hidden
  declaration diagnostics, and ABI mismatch diagnostics.
- Package manager local registry model, deterministic package locks, offline
  mirror records, local trust envelopes, install/update/uninstall/rollback
  receipts, package signing, package verification, and clean distribution
  checks.
- Source-owned hosted-registry fixture resolution with a hermetic local service
  contract for fixture auth, trust, revocation, moderation, availability, and
  no-network fail-closed behavior; offline fixture-backed network dependency
  resolution, source-owned release-channel publication metadata, and package
  security hardening with pre-mutation extraction path checks.
- Runnable toolchain package channels, release manifests, SBOM/provenance
  publication, release operation policy, release channel lifecycle, ABI/API
  drift checks, and platform/toolchain support matrix checks.
- Runtime import/package metadata, interop package loader bridge evidence,
  mixed-image replay, C header import/export metadata, Swift/C++ annotation
  metadata, and Objective-C 2 adjacent metadata preservation where the checked
  interop rows define it.

### Developer Tooling

- One public command bridge: `npm run objc3c -- <action>`.
- Formatter, LSP/workspace, source graph, artifact inspector, runtime
  inspector, playground/repro, compile observability, and validation timing
  surfaces.
- Language-service replay for diagnostics, hover, definitions, document
  symbols, workspace symbols, lifecycle invalidation, and unsupported request
  failures.
- Artifact inspection for emitted object bytes, symbol tables, sections,
  runtime metadata, package identities, receipts, trust rows, source graph
  records, and debug-map links.
- Public conformance suite manifest, public conformance scorecard/reporting,
  external validation replay, stress/fuzz/minimization surfaces, and nightly
  orchestration.

### Cross-Lane Programs

Objective-C 3.0 support is not only checked feature-by-feature. This branch
also carries integrated programs that force systems to interact:

- Advanced runtime closure: blocks, ownership, ARC cleanup, errors, async,
  actors, cancellation, property behaviors, macro provenance, package replay,
  source/debug records, ABI records, native object/IR/manifest evidence, and a
  17-case negative matrix.
- Object reflection and debugger artifacts: class/metaclass/category/protocol/
  property/ivar/selector/reflection/replay evidence, runtime debug anchors,
  value-inspection records, source graph links, object inventories, source-map
  and native line-table rows, emitted native debug-info evidence, and
  statement stepping on the integrated object-model artifact path.
- Text, collections, and packages: runtime text builders, collection literals,
  `for-in`, package import metadata, provider import surfaces, source graph
  package nodes, and declaration debug anchors.
- Optimization/runtime equivalence: cache-aware dispatch, exact-target
  devirtualization, and the bounded method-inlining safe subset with
  inline-frame/source-map, side-effect, and invalidation proof gates.
- Distribution package lifecycle: package manager behavior, package channels,
  release evidence, and tamper/fail-closed distribution checks.

## What Remains Reserved

The matrix is intentionally explicit about boundaries. Important reserved or
rejected rows include:

- Objective-C 2 runtime compatibility, Swift/C++ runtime mirroring, dynamic
  forwarding, and broad full-source-map publication for every production
  artifact path. Objective-C 3 object-model realization itself is implemented
  by the checked runtime/debugger identity graph.
- Broad advanced-runtime guarantees beyond the integrated Objective-C 3 runtime
  envelope. The combined fixture compiles, links, and runs with checked runtime
  evidence, while broad scheduler fairness, Swift ABI mirroring, distributed
  actor networking, and arbitrary macro-host execution remain reserved.
- Typed throws and value optionals. `throws(E)` is implemented as a bounded
  single-payload effect with exact parser/sema/interface identity, private
  error-out ABI lowering, exact typed catches, policy-backed `id<Error>` bridge
  catches, runtime-dispatch message-send coverage, and `try?` optionalization.
  Multi-payload, malformed, unsupported foreign-carrier, async propagation, and
  silent-erasure paths fail closed. `Optional<T>` is a semantic value-optional
  carrier with the bounded packed runtime ABI for `Optional<i32>`,
  `Optional<bool>`, and `Optional<id>` handles plus `Optional<i64>` language
  call/return lowering through the wide `{has_value,i64}` carrier. Lowercase
  `optional<T>` is rejected as `O3C004`; nil-to-scalar, nullable-pointer
  conversion, generalized nested/generic/property/ivar lowering, and broad
  runtime support remain unclaimed.
- Generic callable reification. Current support is the erased generic class
  receiver/free-function subset named by the generic callable row; explicit
  `@reify_generics`, Objective-C method type-parameter clauses, C/Objective-C
  style generic functions, and runtime reified metadata remain reserved.
- Strict-system language profile selection. `strict` and `strict-concurrency`
  are current claimable conformance selections with native profile validation,
  strict diagnostics, and strict-concurrency actor/sendability/task/scheduler
  checks. `strict-system` remains target-only release evidence and rejects
  fail-closed rather than aliasing strict-concurrency.
- The #8207 language-evolution umbrella. It is a readiness/truth row over
  typed throws, value optionals, generic callable reification, guarded match,
  and strict profiles, not a separate behavior claim.
- Live public hosted package registry services and arbitrary live network
  dependency resolution. Current package evidence is local/offline,
  fixture-backed, deterministic, and fail-closed.
- Broad heuristic method inlining and arbitrary dynamic-dispatch inlining.
  The bounded scalar safe subset is implemented only when every ownership,
  inline-frame source-map, callee-body identity, side-effect, and invalidation
  replay proof is present; missing proof paths still fail closed.
- Broad full source-map publication for every production artifact path.
  Bounded statement stepping and LLDB replay are implemented by narrower
  debugger rows; arbitrary host debugger sessions and every optimized binary
  remain outside the claim.
- Linux x64 and macOS arm64 host support. Windows x64 is the supported Tier 1
  host row; Linux and macOS rows are source-owned fail-closed contracts until
  build, package, install, installed-root execution, native execution, and
  reviewed checked-source promotion evidence exist. macOS promotion also
  requires Mach-O, dSYM, and runtime load-path proof.
- AddressSanitizer and UndefinedBehaviorSanitizer beyond the current Windows
  x64 runtime-package variants. ASan/UBSan package, install, and execution
  support is evidence-bound for Windows x64 through checked source promotion
  evidence; generated-only sanitizer reports and unsupported hosts do not
  promote support.
- Native object emission without `llc --filetype=obj` and a non-empty
  target-specific object from `llc`. Missing `llc` or target object emission is
  a fail-closed status, and clang must not be treated as a fallback object
  emitter for support, package, or execution claims.
- The #8206 platform expansion umbrella. It is an internal readiness/truth row
  over Windows x64 support, fail-closed Linux/macOS rows, Windows x64
  evidence-bound ASan/UBSan package variants, and #8232 native object emission,
  not a broad platform support claim.

## Fresh Setup

The supported host row is Windows x64 with PowerShell 7.

Install prerequisites:

- PowerShell 7 (`pwsh`)
- Node.js and `npm`
- Python 3 with `pip`
- A complete LLVM install with `clang++`, `llc`, `llvm-ar`, `llvm-config`,
  LLVM headers, and CMake package files. The repo-owned CI installer stages
  this under `C:\Users\<you>\Tools\LLVM\llvm-<version>-msvc`; alternatively,
  set `OBJC3C_LLVM_ROOT` or `LLVM_ROOT` to an equivalent full LLVM root.

LLVM tools used by the native path:

- `clang++.exe`
- `llc.exe`
- `llvm-lib.exe`
- `libclang.lib` or `clang.lib`
- LLVM headers under `include/`

From a fresh clone:

```powershell
git clone https://github.com/doublemover/Slopjective-C.git
cd .\Slopjective-C
npm ci
python -m pip install --upgrade pytest jsonschema
```

If LLVM is installed somewhere else:

```powershell
$env:OBJC3C_LLVM_ROOT = 'D:\path\to\LLVM'
$env:LLVM_ROOT = 'D:\path\to\LLVM'
$env:LLVM_DIR = 'D:\path\to\LLVM\lib\cmake\llvm'
```

Build the compiler, C API runner, and runtime archive:

```powershell
npm run objc3c -- build-native-binaries
```

The build publishes native outputs under `artifacts/` and writes machine-owned
intermediate/replay outputs under `tmp/`.

## First Working Session

Compile a checked fixture:

```powershell
npm run objc3c -- compile-objc3c tests/tooling/fixtures/native/execution/positive/basic_i32_return_main.objc3 --out-dir tmp/readme-basic --emit-prefix module
```

Run the default smoke path:

```powershell
npm run objc3c -- test-smoke
```

Run native execution smoke when `llc.exe` is available:

```powershell
$env:OBJC3C_NATIVE_EXECUTION_LLC_PATH = 'C:\Users\<you>\Tools\LLVM\llvm-22.1.6-msvc\bin\llc.exe'
npm run objc3c -- test-execution-smoke
```

Run the full nightly validation profile:

```powershell
npm run objc3c -- test-nightly
```

Nightly runs the broad compiler/runtime/conformance/stress/package/release
profile and writes its integrated report to
`tmp/reports/objc3c-public-workflow/test-nightly.json`.

## Showcase

The showcase sources are the fastest way to read Objective-C 3.0 as an
application language:

- [`showcase/auroraBoard/main.objc3`](showcase/auroraBoard/main.objc3):
  modules, protocols, categories, properties, reflection-shaped metadata, and
  object-model declarations.
- [`showcase/signalMesh/main.objc3`](showcase/signalMesh/main.objc3): typed
  status flow, executor annotations, async-shaped declarations, runtime
  messaging, and Objective-C message syntax.
- [`showcase/patchKit/main.objc3`](showcase/patchKit/main.objc3): derive
  annotations, macro provenance annotations, property behavior syntax, and
  interop-shaped declarations.

Compile one:

```powershell
npm run objc3c -- compile-objc3c showcase/auroraBoard/main.objc3 --out-dir tmp/readme-auroraBoard --emit-prefix module
```

Validate the portfolio:

```powershell
npm run objc3c -- check-showcase-surface
npm run objc3c -- validate-showcase
```

Package-aware sample libraries and apps live under
[`showcase/applicationFrameworkSamples/`](showcase/applicationFrameworkSamples/):

- object runtime sample library
- interop adapter sample library
- stdlib text and collections CLI sample
- async runtime application sample

Validate them with:

```powershell
npm run objc3c -- validate-application-framework-samples
```

## Public Command Surface

Normal user-facing work goes through one command shape:

```powershell
npm run objc3c -- <action>
```

Common actions:

- Build: `build-native-binaries`, `build-native-contracts`,
  `build-native-full`, `build-native-reconfigure`
- Compile/proof: `compile-objc3c`, `proof-objc3c`
- Inspect: `inspect-artifact`, `inspect-source-graph`,
  `inspect-language-service`, `inspect-debug-map`, `trace-runtime-debug`
- Test: `test-smoke`, `test-ci`, `test-full`, `test-nightly`,
  `test-execution-smoke`, `test-runtime-acceptance-fast`
- Conformance: `validate-conformance-corpus`, `check-conformance-minima`,
  `validate-public-conformance-reporting`
- Runtime: `validate-object-model-conformance`,
  `validate-public-runtime-reflection-api`, `validate-cache-aware-dispatch`,
  `validate-advanced-runtime-closure`
- Standard library: `validate-stdlib-foundation`
- Modules/interop: `validate-module-interop-contracts`,
  `validate-direct-import-module-syntax`, `validate-interop-conformance`,
  `validate-runnable-interop`
- Packages: `validate-package-manager-model`, `validate-package-registry-model`,
  `validate-package-network-publication`, `validate-package-security-hardening`,
  `validate-package-mirror`, `validate-package-install-distribution`,
  `package-install`, `package-verify`, `package-sign`
- Release: `validate-abi-governance`, `validate-release-foundation`,
  `validate-packaging-channels`, `validate-release-operations`,
  `validate-distribution-credibility`

The generated command appendix is
[`docs/runbooks/objc3c_public_command_surface.md`](docs/runbooks/objc3c_public_command_surface.md).

## Repository Map

- [`native/objc3c/`](native/objc3c/): compiler, parser, sema, lowering, IR,
  runtime, C API, driver, support, diagnostics, pipeline, artifacts, and tools.
- [`stdlib/`](stdlib/): checked standard-library modules and runtime contracts.
- [`showcase/`](showcase/): example programs and package-aware application
  framework samples.
- [`spec/`](spec/): language, ABI, runtime, interop, metadata, package, and
  release contracts.
- [`docs/tutorials/`](docs/tutorials/): setup, walkthrough, comparison, and
  sample guides.
- [`docs/support/`](docs/support/): capability matrix, evidence map, umbrella
  readiness, support truth, schema examples, and claim ownership.
- [`docs/runbooks/`](docs/runbooks/): operator and maintainer workflows.
- [`docs/objc3c-native/src/`](docs/objc3c-native/src/): native
  compiler/runtime architecture source.
- [`schemas/`](schemas/): artifact, report, package, runtime, tooling,
  conformance, and release schema contracts.
- [`scripts/`](scripts/): build, validation, package, release, report, and
  workflow tooling.
- [`tests/`](tests/): native behavior, conformance, runtime, stress, e2e, and
  tooling coverage.
- [`site/`](site/): public overview source and generated page.
- `tmp/`, `artifacts/`: machine-owned build, package, compiler, and replay
  outputs.

## Reading Path

1. Compile
   [`tests/tooling/fixtures/native/execution/positive/collection_literals_mutation_for_in.objc3`](tests/tooling/fixtures/native/execution/positive/collection_literals_mutation_for_in.objc3).
2. Read [`docs/tutorials/getting_started.md`](docs/tutorials/getting_started.md).
3. Read [`showcase/README.md`](showcase/README.md).
4. Check exact support rows in
   [`docs/support/capability_matrix.md`](docs/support/capability_matrix.md).
5. Read [`docs/objc3c-native.md`](docs/objc3c-native.md) for compiler/runtime
   architecture.

## Development Notes

- Keep user-facing workflow commands on `npm run objc3c -- <action>`.
- Treat `native/objc3c/` as the compiler/runtime implementation root.
- Treat `stdlib/` as the checked standard-library root.
- Treat `showcase/` as the checked example source root.
- Treat `docs/support/capability_matrix.json` and
  `docs/support/evidence_map.json` as support truth.
- Treat `tmp/` and `artifacts/` as machine-owned output roots.
- Update generated checked-in docs through their owner actions.

Useful validation commands:

```powershell
npm run objc3c -- test-smoke
npm run objc3c -- check-task-hygiene
npm run objc3c -- check-markdown
```

## License

No repository-wide license file is currently present.
