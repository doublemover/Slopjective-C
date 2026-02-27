# Part 1 — Versioning, Compatibility, and Conformance {#part-1}

_Working draft v0.11 — last updated 2026-02-27_

## 1.1 Purpose {#part-1-1}

This part defines:

- how Objective‑C 3.0 is enabled,
- how source and behavior changes are contained,
- conformance levels and the meaning of “strictness,”
- required feature-test mechanisms,
- migration tooling requirements.

## 1.2 Language mode selection {#part-1-2}

### 1.2.1 Compiler option {#part-1-2-1}

A conforming implementation shall provide a command-line mechanism equivalent to:

- `-fobjc-version=3`

Selecting ObjC 3.0 mode shall:

- enable the ObjC 3.0 grammar additions,
- enable ObjC 3.0 default rules (nonnull-by-default regions, etc.),
- enable ObjC 3.0 diagnostics (at least warnings in permissive mode).

### 1.2.2 Translation-unit granularity (v1 decision) {#part-1-2-2}

For Objective‑C 3.0 v1, language-version selection shall be **translation-unit-only**.

Mixing ObjC 3.0 and non‑ObjC3 language versions within a single translation unit is not conforming for v1.

A conforming implementation:

- shall treat the effective language version as fixed for the entire translation unit,
- shall reject per-region language-version switching forms (for example `push`/`pop`-style pragmas),
- shall diagnose any attempt to change language version after parsing has started.

### 1.2.3 Source directive (optional) {#part-1-2-3}

A conforming implementation may provide a source directive such as:

- `#pragma objc_language_version(3)`

If supported in v1, the directive:

- shall apply to the entire translation unit,
- shall appear only at file scope before the first non-preprocessor declaration/definition token,
- shall reject region-scoped variants (for example `#pragma objc_language_version(push, 3)` / `pop`),
- shall diagnose conflicting selections (for example command line selects `3` while source selects a different version value).

### 1.2.4 Required conformance tests for language-version selection (normative minimum) {#part-1-2-4}

A conforming implementation shall include tests that cover both accepted and rejected forms, including at minimum:

- TUV-01: accepted command-line translation-unit selection (`-fobjc-version=3`).
- TUV-02: accepted file-scope pragma form `#pragma objc_language_version(3)` before declarations.
- TUV-03: rejected region switching forms (`push`/`pop`, begin/end, or equivalent).
- TUV-04: rejected in-function or post-declaration version-selection pragma.
- TUV-05: rejected conflicting command-line/source version selections.

Test expectations and diagnostic portability requirements are defined in [§12.5.8](#part-12-5-8).

## 1.3 Reserved keywords and conflict handling {#part-1-3}

### 1.3.1 Reserved keywords {#part-1-3-1}

In ObjC 3.0 mode, at minimum the following tokens are reserved as keywords:

- Control flow: `defer`, `guard`, `match`, `case`
- Effects/concurrency: `async`, `await`, `actor`
- Errors: `try`, `throw`, `do`, `catch`, `throws`
- Bindings: `let`, `var`

### 1.3.2 Contextual keywords (non-normative guidance) {#part-1-3-1-2}

Some tokens are treated as **contextual keywords** only within specific grammar positions, to reduce breakage in existing codebases.

Examples include:

- `borrowed` (type qualifier; [Part 8](#part-8), also cataloged in [B.8.3](#b-8-3))
- `move`, `weak`, `unowned` (block capture lists; [Part 8](#part-8), also cataloged in [B.8.5](#b-8-5))

Conforming implementations should avoid reserving these tokens globally.

### 1.3.3 Raw identifiers {#part-1-3-2}

A conforming implementation shall provide a compatibility escape hatch for identifiers that collide with reserved keywords.

Two acceptable designs:

- a raw identifier syntax (e.g., `@identifier(defer)`), or
- a backtick escape (e.g., `` `defer` ``) in ObjC 3.0 mode only.

The toolchain shall also provide fix-its to mechanically rewrite collisions.

### 1.3.4 Backward compatibility note {#part-1-3-3}

This specification does not require keyword reservation to apply outside ObjC 3.0 mode.

## 1.4 Feature test macros {#part-1-4}

### 1.4.1 Required macros {#part-1-4-1}

A conforming implementation shall provide predefined macros to allow conditional compilation:

- `__OBJC_VERSION__` (integer; at least `3` in ObjC 3.0 mode)
- `__OBJC3__` (defined to `1` in ObjC 3.0 mode)

### 1.4.2 Per-feature macros {#part-1-4-2}

A conforming implementation shall provide per-feature macros:

- `__OBJC3_FEATURE_<NAME>__`

Examples:

- `__OBJC3_FEATURE_DEFER__`
- `__OBJC3_FEATURE_OPTIONALS__`
- `__OBJC3_FEATURE_THROWS__`
- `__OBJC3_FEATURE_ASYNC_AWAIT__`
- `__OBJC3_FEATURE_ACTORS__`

Feature macros shall be defined in ObjC 3.0 mode even if a feature is disabled by a profile, to allow feature probing (`0` meaning “recognized but disabled”).

### 1.4.3 Mode-selection macros (required) {#part-1-4-3}

A conforming implementation shall provide predefined mode-selection macros in ObjC 3.0 mode:

- `__OBJC3_STRICTNESS_LEVEL__` with values:
  - `0` for `permissive`,
  - `1` for `strict`,
  - `2` for `strict-system`.
- `__OBJC3_CONCURRENCY_MODE__` with values:
  - `0` for `off`,
  - `1` for `strict`.
- `__OBJC3_CONCURRENCY_STRICT__` as a boolean alias (`1` when `__OBJC3_CONCURRENCY_MODE__ == 1`, else `0`).

### 1.4.4 `__has_feature` integration (optional) {#part-1-4-4}

A conforming implementation may expose ObjC 3.0 features through a `__has_feature`-style mechanism. If present, it should align with the per-feature macros and mode-selection macros.

## 1.5 Conformance levels (strictness) {#part-1-5}

### 1.5.1 Levels {#part-1-5-1}

A translation unit in ObjC 3.0 mode may additionally declare a conformance level.

Objective‑C 3.0 v1 defines exactly three strictness levels:

- **Permissive**: language features enabled; safety checks default to warnings; legacy patterns allowed.
- **Strict**: key safety checks are errors; opt-outs require explicit unsafe spellings.
- **Strict-system**: strict + additional system-API safety checks (resource cleanup correctness, borrowed pointer escape analysis, etc.).

### 1.5.2 Selecting a level {#part-1-5-2}

A conforming implementation shall provide an option equivalent to:

- `-fobjc3-strictness=permissive|strict|strict-system`

### 1.5.3 Diagnostic escalation rule {#part-1-5-3}

If a construct is ill‑formed in strict mode, it may still be accepted in permissive mode with a warning _only_ if:

- the compiler can preserve baseline behavior, and
- the behavior is not undefined.

## 1.6 Orthogonal checking modes (submodes) {#part-1-6}

### 1.6.1 Strict concurrency checking {#part-1-6-1}

Concurrency checking is an orthogonal strictness sub-mode: a translation unit may enable additional checking beyond the selected strictness level.

For v1, strict concurrency checking is **not** a fourth strictness level.

A conforming implementation shall provide an option equivalent to:

- `-fobjc3-concurrency=strict|off`

Strict concurrency checking enables additional diagnostics defined in [Part 7](#part-7)/12 (Sendable, actor isolation misuse, executor affinity misuse, etc.).

### 1.6.2 Required strictness/sub-mode consistency rules (normative) {#part-1-6-2}

A conforming implementation shall apply strictness and strict concurrency as two axes:

- `-fobjc3-strictness=*` selects baseline strictness semantics.
- `-fobjc3-concurrency=*` enables or disables additional concurrency diagnostics/constraints from [Part 7](#part-7) and [Part 12](#part-12).

Consistency requirements:

- Enabling `-fobjc3-concurrency=strict` shall not weaken any diagnostic required by the selected strictness level.
- Disabling `-fobjc3-concurrency` shall not disable non-concurrency diagnostics required by the selected strictness level.
- Profile claims shall map to strictness/sub-mode combinations defined in [E.2](#e-2).

Conformance tests for this matrix are defined in [§12.5.10](#part-12-5-10).

### 1.6.3 Strict performance checking (optional) {#part-1-6-3}

Implementations may provide a “strict performance” mode enabling additional diagnostics and/or runtime assertions for [Part 9](#part-9) features (static regions, direct methods). If provided, the option shall be orthogonal to strictness levels.

> Note: This draft treats strict performance checking as an optional extension because runtime enforcement strategies differ by platform.

## 1.7 Source compatibility principles {#part-1-7}

### 1.7.1 No silent semantic changes in non‑ObjC3 code {#part-1-7-1}

Compiling code not in ObjC 3.0 mode shall not change meaning due to this specification.

### 1.7.2 Contained default changes {#part-1-7-2}

Default changes (e.g., nonnull-by-default) apply only inside ObjC 3.0 translation units and/or explicitly marked module boundaries ([Part 2](#part-2)/3).

### 1.7.3 Header compatibility {#part-1-7-3}

Headers intended to be consumed by non‑ObjC3 translation units shall:

- avoid ObjC 3.0‑only keywords in public API unless guarded by feature macros, or
- use canonical attribute spellings that are syntactically valid in baseline compilers ([Decision D-007](#decisions-d-007)).

## 1.8 Migration tooling requirements {#part-1-8}

A conforming implementation shall provide:

- diagnostics with fix-its for common migrations,
- a batch migrator capable of applying safe transformations.

Minimum migrator capabilities:

1. Insert nullability annotations based on static inference and usage ([Part 3](#part-3)).
2. Convert “weak‑strong dance” patterns to capture lists ([Part 8](#part-8)).
3. Convert common manual cleanup patterns to `defer` or `@resource` ([Part 8](#part-8)).
4. Suggest `withLifetime/keepAlive` when borrowed-pointer diagnostics trigger ([Part 8](#part-8)).
5. Provide stubs for `throws` and `Result` bridging from NSError patterns ([Part 6](#part-6)).
6. Where possible, annotate completion-handler C/ObjC APIs for `async` bridging overlays ([Part 11](#part-11)).

## 1.9 Profiles (hook point) {#part-1-9}

Objective‑C 3.0 defines optional **profiles**: named bundles of additional restrictions, defaults, and required library/runtime surfaces aimed at specific domains (e.g., “system”, “app”, “freestanding”).

Profiles are selected by **toolchain configuration**, not by source-level directives.
A conforming implementation shall provide a mechanism equivalent to:

- `-fobjc-profile=<name>`

Profiles may:

- enable additional diagnostics as errors ([Part 12](#part-12)),
- require particular standard modules (e.g., Concurrency; [Part 7](#part-7)),
- require additional module metadata preservation ([D](#d)),
- restrict unsafe constructs (e.g., borrowed pointer escaping; [Part 8](#part-8)).

This part defines the _hook point_ and selection mechanism; profile contents are specified in **[CONFORMANCE_PROFILE_CHECKLIST.md](#e)**.

When emitting a machine-readable conformance report, implementations shall report the selected profile set using the schema in [§12.4.5](#part-12-4-5).

