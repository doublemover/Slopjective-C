# Part 12 — Diagnostics, Tooling, and Test Suites {#part-12}

_Working draft v0.11 — last updated 2026-02-27_

_Normative baseline references used in this part: [NR-C18](#part-0-2-1), [NR-LLVM-OBJC](#part-0-2-1), [NR-BLOCKS-ABI](#part-0-2-1), and where applicable [NR-OBJC-RUNTIME](#part-0-2-1)/[NR-ABI-PLATFORM](#part-0-2-1)._

## 12.1 Purpose {#part-12-1}

Objective‑C 3.0 treats tooling requirements as part of conformance:

- diagnostics must be specific and actionable,
- fix-its must exist for common mechanical migrations,
- and a conformance test suite must exist to prevent “whatever the compiler happens to do.”

This part is cross-cutting and references:

- **[B](#b)** for canonical spellings and interface emission requirements,
- **[C](#c)** for separate compilation and lowering contracts.
- **[D](#d)** for the normative checklist of required module metadata and ABI boundaries.

## 12.2 Diagnostic principles (normative) {#part-12-2}

A conforming implementation shall provide diagnostics that:

1. Identify the precise source range of the problem.
2. Explain the rule being violated in terms of Objective‑C 3.0 concepts (nonnull, optional send, executor hop, etc.).
3. Provide at least one actionable suggestion.
4. Provide a fix-it when the transformation is mechanical and semantics-preserving.

## 12.3 Required diagnostics (minimum set) {#part-12-3}

### 12.3.1 Nullability and optionals {#part-12-3-1}

- Incomplete nullability in exported interfaces (strict mode: error).
- Passing nullable to nonnull without check/unwrap (strict: error; permissive: warning).
- Dereferencing nullable without unwrap (strict: error).
- Optional message send or optional member access used on scalar/struct returns (error; v1 restriction).
- Ordinary message send on nullable receiver in strict mode (error; fix-it suggests optional send or `guard let`).

### 12.3.2 Errors and `throws` {#part-12-3-2}

- Calling a `throws` function/method without `try` (error).
- Redeclaring a `throws` declaration without `throws` (or vice versa) across module boundaries (error; see [C.2](#c-2)).
- Using postfix propagation `e?` on `T?` in a non-optional-returning function (error; fix-it suggests `guard let` or explicit conditional).
- Using postfix propagation `e?` expecting it to map to `throws`/`Result` (error; explain “carrier preserving” rule).

### 12.3.3 Concurrency (`async/await`, executors, actors) {#part-12-3-3}

For v1, strict concurrency checking is an orthogonal sub-mode selected independently from strictness level ([Part 1](#part-1) [§1.6](#part-1-6)).

- Any potentially suspending operation without `await` (error), including calling an `async` function and crossing executor/actor isolation boundaries ([Part 7](#part-7) / [D-011](#decisions-d-011)).
- Calling into an `objc_executor(X)` declaration from a different executor without an `await` hop (strict concurrency: error; permissive: warning).
- Capturing non-Sendable-like values into a task-spawned async closure (strict concurrency: error; permissive: warning).
- Actor-isolated member access from outside the actor without `await` when a hop may be required (strict concurrency: error).
- `await` applied to an expression that cannot suspend (strict: warning) (diagnostic: “unnecessary await”).

### 12.3.4 Modules and interface emission {#part-12-3-4}

- Missing required semantic metadata on imported declarations (effects/isolation/directness): error in strict modes; see [D.3.1](#d-3-1) [Table A](#d-3-1).
- Using a module-qualified name with a non-imported module: error with fix-it to add `@import`.
- Importing an API through a mechanism that loses effects/attributes (e.g., textual header without metadata) when strictness requires them: warning with suggestion to enable modules/interface emission.
- Emitted interface does not use canonical spellings from [B](#b): tooling warning; in “interface verification” mode, error.

### 12.3.5 Performance/dynamism controls {#part-12-3-5}

- Overriding a `objc_final` method/class: error.
- Subclassing an `objc_final` class: error.
- Subclassing an `objc_sealed` class from another module: error.
- Declaring a category method that collides with a `objc_direct` selector: error.
- Calling a `objc_direct` method via dynamic facilities (e.g., `performSelector:`) when it can be statically diagnosed: warning (or error under strict performance profile).

### 12.3.6 System programming extensions {#part-12-3-6}

A conforming implementation shall provide diagnostics (at least warnings by default; errors where required by selected strictness/profile) for:

- **Non-local control flow across cleanup scopes** ([Part 5](#part-5) [§5.2.3](#part-5-2-3), [§5.2.5](#part-5-2-5); [Part 8](#part-8) [§8.1](#part-8-1), [§8.2.4](#part-8-2-4)):
  - non-local exits inside `defer` bodies (`return`, `break`, `continue`, `goto`, ObjC/C++ `throw`, `longjmp`, `siglongjmp`) shall be diagnosed (these are ill-formed; in `strict` and `strict-system` profiles the diagnostic shall be an error),
  - a provable `longjmp`/`siglongjmp` that bypasses an active cleanup scope with pending actions shall be diagnosed as an error in `strict` and `strict-system` profiles.

- **Resource cleanup correctness** ([Part 8](#part-8) [§8.3](#part-8-3)):
  - missing required cleanup for `objc_resource(...)` or `@resource(...)` declarations (if required by the selected profile),
  - in v1 mode, use of `objc_resource(...)` / `@resource(...)` on struct/union fields is diagnosed as unsupported unless an aggregate-resource capability is explicitly enabled,
  - double-close patterns detectable intra-procedurally,
  - use-after-move / use-after-discard for moved-from resources (where move checking is enabled).

- **Borrowed pointer escaping** ([Part 8](#part-8) [§8.7](#part-8-7)):
  - returning a `borrowed` pointer without the required `objc_returns_borrowed(owner_index=...)` marker (or with an invalid owner index),
  - storing a borrowed pointer into longer-lived storage (global/static/thread-local, ivar/property, heap) in strict-system profiles,
  - passing a borrowed pointer to a parameter not proven non-escaping in strict-system profiles,
  - capturing a borrowed pointer in an escaping block in strict-system profiles,
  - converting a borrowed pointer to `void *`/integer when it can escape owner lifetime in strict-system profiles.

- **Capture list safety** ([Part 8](#part-8) [§8.8](#part-8-8)):
  - suspicious strong captures of `self` in `async` contexts (if enabled by profile),
  - `unowned` capture of potentially-nil objects without an explicit check,
  - mixed `weak` + `move` captures that create use-after-move hazards.

## 12.4 Required tooling capabilities (minimum) {#part-12-4}

### 12.4.1 Migrator {#part-12-4-1}

A conforming toolchain shall provide (or ship) a migrator that can:

- insert nullability annotations based on inference and usage,
- rewrite common nullable-send patterns into optional sends or `guard let`,
- generate `throws` wrappers for NSError-out patterns ([Part 6](#part-6)),
- suggest executor/actor hop fixes where statically determinable ([Part 7](#part-7)).

### 12.4.2 Interface emission / verification {#part-12-4-2}

If the toolchain supports emitting an interface description for distribution, it shall also support a verification mode that checks:

- semantic equivalence between the original module and the emitted interface,
- and the use of canonical spellings from [B](#b).

### 12.4.3 Static analysis hooks {#part-12-4-3}

Implementations shall expose enough information for analyzers to:

- reason about nullability flow,
- reason about `throws` propagation,
- and enforce Sendable-like constraints under strict concurrency.
- expose `borrowed` and `objc_returns_borrowed(owner_index=...)` annotations in AST dumps and module metadata for external analyzers ([Part 8](#part-8), [D](#d)).
- expose resource annotations (`objc_resource(...)`) and move-state tracking hooks where enabled by profile ([Part 8](#part-8)).

### 12.4.4 Conformance report emission and validation (normative) {#part-12-4-4}

A conforming implementation shall provide a machine-readable conformance report emission surface equivalent to:

- `--emit-objc3-conformance=<path>`
- `--emit-objc3-conformance-format=json|yaml`
- `--validate-objc3-conformance=<path>`

Compatibility notes:

- JSON output is required.
- YAML output is optional; if provided, it shall be semantically equivalent to the JSON model in [§12.4.5](#part-12-4-5).
- Implementations may provide different option spellings, but shall document the mapping to this canonical surface.
- Validation failure shall return a non-zero exit status and a stable diagnostic code.

### 12.4.5 Conformance report schema v1 (normative) {#part-12-4-5}

The canonical v1 report data model is:

```json
{
  "schema_id": "objc3-conformance-report/v1",
  "generated_at": "2026-01-31T15:04:05Z",
  "toolchain": {
    "name": "string",
    "vendor": "string",
    "version": "string",
    "target_triple": "string"
  },
  "language": {
    "language_family": "objective-c",
    "language_version": "3.0",
    "spec_revision": "v1"
  },
  "mode": {
    "strictness": "permissive|strict|strict-system",
    "concurrency": "off|strict"
  },
  "profiles": [
    {
      "id": "core|strict|strict-concurrency|strict-system",
      "status": "claimed|passed|failed|not-claimed",
      "evidence": ["string"]
    }
  ],
  "optional_features": [
    {
      "id": "string",
      "status": "claimed|not-claimed|experimental",
      "version": "string",
      "notes": "string"
    }
  ],
  "versions": {
    "frontend": "string",
    "runtime": "string",
    "stdlib": "string",
    "module_format": "string"
  },
  "known_deviations": [
    {
      "id": "string",
      "summary": "string",
      "status": "open|accepted|waived|fixed-pending-release",
      "affects": ["string"],
      "tracking": "string"
    }
  ]
}
```

Conformance requirements for this schema:

- All top-level keys shown above shall be present.
- `mode.strictness` and `mode.concurrency` shall match the effective command-line mode selection used for the conformance run.
- `profiles`, `optional_features`, and `known_deviations` shall be arrays (empty arrays are permitted).
- `versions` shall list concrete shipped component versions; `"unknown"` is not conforming for a released claim.
- Each known deviation shall have a stable `id` and a non-empty `summary`.

## 12.5 Conformance test suite (minimum expectations) {#part-12-5}

A conforming implementation shall ship or publish a test suite that covers at least:

### 12.5.0 Required repository structure (normative) {#part-12-5-0}

The conformance suite shall include at least these buckets (directory names may vary if mapping is documented):

- `tests/conformance/parser/`
- `tests/conformance/semantic/`
- `tests/conformance/lowering_abi/`
- `tests/conformance/module_roundtrip/`
- `tests/conformance/diagnostics/`

Bucket responsibilities:

- parser: grammar/disambiguation and syntax acceptance/rejection.
- semantic: typing/effects/isolation/nullability behavior.
- lowering_abi: ABI/lowering/runtime-contract behavior.
- module_roundtrip: emit/import/metadata preservation tests.
- diagnostics: required diagnostics and fix-it verification.

### 12.5.1 Parsing/grammar {#part-12-5-1}

- `async`/`await`/`throws` grammar interactions (`try await`, `await try`, etc.).
- Module-qualified name parsing (`@A.B.C`).

### 12.5.2 Type system and diagnostics {#part-12-5-2}

- Module metadata preservation tests for each [Table A](#d-3-1) item in [D](#d) (import module; verify semantics survive; verify mismatch diagnostics).
- Strict vs permissive nullability behavior.
- Optional send restrictions (reference-only).
- Postfix propagation carrier-preserving rules.
- Effect mismatch diagnostics across module imports ([C.2](#c-2)).

### 12.5.3 Dynamic semantics {#part-12-5-3}

- Optional send argument evaluation does not occur when receiver is `nil`.
- `throws` propagation through nested `do/catch`.
- Cancellation propagation in structured tasks.

### 12.5.4 Runtime contracts {#part-12-5-4}

- Autorelease pool draining at suspension points (Objective‑C runtimes) ([D-006](#decisions-d-006) / [C.7](#c-7)).
- Executor/actor hops preserve ordering and do not deadlock in basic scenarios.
- Calling executor-annotated or actor-isolated _synchronous_ members from outside requires `await` and schedules onto the correct executor ([D-011](#decisions-d-011)).

### 12.5.5 Non-local exits and cleanup scopes {#part-12-5-5}

- **Negative (strict/strict-system):** compile-fail tests where a `defer` body contains each forbidden non-local exit form (`return`, `break`, `continue`, `goto`, ObjC `@throw`, C++ `throw`, `longjmp`, `siglongjmp`).
- **Negative (strict/strict-system):** compile-fail tests with statically provable `longjmp`/`siglongjmp` transfers that bypass pending cleanup actions from `defer`, `@cleanup`, or `@resource`.
- **Conformance runtime (ObjC++):** C++ exception unwind across nested cleanup scopes executes scope-exit actions exactly once per registration, in LIFO order, before scope-final ARC releases.
- **Conformance runtime (ObjC EH, where supported):** Objective‑C exception unwind follows the same cleanup ordering/once semantics as C++ unwind.
- **Profile/ABI guard:** no test may require defined behavior for `longjmp`/`siglongjmp` bypass of cleanup scopes unless the implementation explicitly documents an ABI guarantee for cleanup execution on that transfer.

### 12.5.6 Profile coverage minima (normative minimum) {#part-12-5-6}

At minimum, the published suite shall include:

- **Core**
  - parser: 15 tests,
  - semantic: 25 tests,
  - lowering/ABI: 10 tests,
  - module round-trip: 12 tests,
  - diagnostics: 20 tests.
- **Strict**
  - at least 10 additional strict-only diagnostics/fix-it tests beyond Core.
- **Strict Concurrency**
  - at least 12 additional isolation/Sendable/executor-boundary tests.
- **Strict System**
  - at least 12 additional borrowed/resource/capture-list tests.

Counts are minimums; implementations may exceed them.

### 12.5.7 Portable diagnostic assertion format (normative) {#part-12-5-7}

Conformance diagnostics shall be assertable in a toolchain-portable structure with at least:

- stable diagnostic `code`,
- `severity` (`error`/`warning`/`note`),
- source `span` (line/column range),
- required `fixits` when the spec requires mechanical fix-it support.

Native harness syntax is permitted, but the implementation shall be able to emit this canonical structure for conformance reporting.

### 12.5.8 Language-version selection acceptance/rejection tests (normative minimum) {#part-12-5-8}

The conformance suite shall include explicit tests for Objective‑C 3.0 v1 translation-unit-only language-version selection:

- TUV-01 (accept): command line `-fobjc-version=3` applies to the entire translation unit.
- TUV-02 (accept): `#pragma objc_language_version(3)` at file scope before declarations applies to the entire translation unit.
- TUV-03 (reject): per-region language-version switching forms (for example `push`/`pop`, begin/end, or equivalent) are rejected.
- TUV-04 (reject): a version-selection pragma inside a function/body scope is rejected.
- TUV-05 (reject): a version-selection pragma that conflicts with command-line mode selection is rejected.

For reject cases, the suite shall assert stable diagnostic code/severity/span metadata per [§12.5.7](#part-12-5-7).

### 12.5.9 Conformance-report schema/content tests (normative minimum) {#part-12-5-9}

The conformance suite shall include machine-readable report tests:

- CRPT-01 (emit-json): `--emit-objc3-conformance=<path>` produces JSON that validates against [§12.4.5](#part-12-4-5).
- CRPT-02 (required-keys): emitted report includes `profiles`, `optional_features`, `versions`, and `known_deviations`.
- CRPT-03 (yaml-equivalence): when YAML emission is supported, JSON and YAML emissions for the same build parse to equivalent data.
- CRPT-04 (reject-missing-required): `--validate-objc3-conformance=<path>` rejects a report missing any required top-level key.
- CRPT-05 (reject-invalid-enum): validator rejects invalid `profiles[*].id` or `profiles[*].status` values.
- CRPT-06 (known-deviation-shape): each `known_deviations` entry is validated for required `id`, `summary`, and `status` fields.

### 12.5.10 Strictness/sub-mode consistency tests (normative minimum) {#part-12-5-10}

The conformance suite shall include matrix tests proving consistent behavior between strictness and strict concurrency sub-mode:

- SCM-01: `-fobjc3-strictness=strict -fobjc3-concurrency=off` keeps strict non-concurrency diagnostics as errors and allows concurrency-only diagnostics to follow non-strict-concurrency policy.
- SCM-02: `-fobjc3-strictness=strict -fobjc3-concurrency=strict` upgrades required concurrency diagnostics to strict-concurrency behavior.
- SCM-03: `-fobjc3-strictness=strict-system -fobjc3-concurrency=off` keeps strict-system diagnostics active.
- SCM-04: `-fobjc3-strictness=strict-system -fobjc3-concurrency=strict` enforces both strict-system and strict-concurrency requirements.
- SCM-05: preprocessing tests verify `__OBJC3_STRICTNESS_LEVEL__`, `__OBJC3_CONCURRENCY_MODE__`, and `__OBJC3_CONCURRENCY_STRICT__` values for each matrix point.
- SCM-06: conformance report `mode` fields and claimed `profiles[*].id` values are consistent with the mode-selection matrix.

### 12.5.11 Retainable-family ARC/non-ARC tests (normative minimum) {#part-12-5-11}

For [Part 8](#part-8) [§8.4](#part-8-4), the conformance suite shall include:

- RF-ARC-01 (negative): with ARC enabled, direct calls to functions annotated `objc_family_retain(...)`, `objc_family_release(...)`, or `objc_family_autorelease(...)` for an ObjC-integrated family are diagnosed as errors.
- RF-ARC-02 (positive): with ARC enabled, transfer-annotated family APIs type-check and lower without requiring explicit manual retain/release calls.
- RF-MRR-01 (positive): with ARC disabled, balanced explicit calls to family retain/release functions are accepted.
- RF-MRR-02 (negative/diagnostic): with ARC disabled and strict-system enabled, statically provable intra-procedural over-release or leak patterns on retainable-family values are diagnosed.
- RF-MOD-01 (round-trip): emitted/imported interfaces preserve `objc_retainable_family(...)`, `objc_family_*`, and ObjC-integration markers with semantics unchanged.

### 12.5.12 Borrowed-pointer strict-system tests (normative minimum) {#part-12-5-12}

For [Part 8](#part-8) [§8.7](#part-8-7), the conformance suite shall include:

- BRW-NEG-01: storing a borrowed pointer into global/static/thread-local storage is a strict-system error.
- BRW-NEG-02: storing a borrowed pointer into heap/ivar/property storage is a strict-system error.
- BRW-NEG-03: capturing a borrowed pointer into an escaping block is a strict-system error.
- BRW-NEG-04: passing a borrowed pointer to a parameter not proven non-escaping is a strict-system error.
- BRW-NEG-05: returning borrowed storage without valid `objc_returns_borrowed(owner_index=...)` is a strict-system error.
- BRW-POS-01: passing borrowed pointers to parameters explicitly marked non-escaping is accepted without escape diagnostics.
- BRW-POS-02: capturing borrowed pointers in blocks proven non-escaping and synchronously invoked is accepted without escape diagnostics.
- BRW-POS-03: copying from a borrowed region into owned storage (without pointer escape) is accepted.
- BRW-POS-04: temporary local stack assignments that do not outlive owner lifetime are accepted.

### 12.5.13 Aggregate-resource initialization/cleanup tests (normative minimum for capability claimants) {#part-12-5-13}

For implementations claiming [Part 8](#part-8) [§8.3.6](#part-8-3-6) capability `objc3.system.aggregate_resources`, the conformance suite shall include:

- AGR-NEG-01 (v1 gate): in Objective‑C 3.0 v1 mode without the capability claim, `objc_resource(...)` on struct/union fields is rejected with a stable diagnostic.
- AGR-RT-01 (partial init failure): when field `k` initialization fails, already-initialized annotated fields `[0..k-1]` are cleaned exactly once in reverse declaration order.
- AGR-RT-02 (multi-field failure order): for three or more annotated fields, injected failure at each position verifies reverse-order partial cleanup of the initialized prefix.
- AGR-RT-03 (normal destruction order): when all annotated fields initialize successfully, scope exit cleanup runs in reverse declaration order.

## 12.6 Debuggability requirements (minimum) {#part-12-6}

For features like macros and async:

- stack traces must preserve source locations,
- debugging tools must be able to map generated code back to user code (macro expansion mapping),
- async tasks should be nameable/inspectable at least in debug builds.

## 12.7 Open issues {#part-12-7}

No open issues are tracked in this part for v1.


