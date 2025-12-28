# Part 12 — Diagnostics, Tooling, and Test Suites {#part-12}

_Working draft v0.10 — last updated 2025-12-28_

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

A conforming implementation shall provide diagnostics (at least warnings, and errors in strict-system profiles) for:

- **Resource cleanup correctness** ([Part 8](#part-8) [§8.3](#part-8-3)):
  - missing required cleanup for `objc_resource(...)` or `@resource(...)` declarations (if required by the selected profile),
  - double-close patterns detectable intra-procedurally,
  - use-after-move / use-after-discard for moved-from resources (where move checking is enabled).

- **Borrowed pointer escaping** ([Part 8](#part-8) [§8.7](#part-8-7)):
  - returning a `borrowed` pointer without the required `objc_returns_borrowed(owner_index=...)` marker,
  - storing a borrowed pointer into a longer-lived location (global, ivar, heap) in strict-system profiles,
  - passing a borrowed pointer to an `@escaping` block in strict-system profiles.

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

## 12.5 Conformance test suite (minimum expectations) {#part-12-5}

A conforming implementation shall ship or publish a test suite that covers at least:

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

## 12.6 Debuggability requirements (minimum) {#part-12-6}

For features like macros and async:

- stack traces must preserve source locations,
- debugging tools must be able to map generated code back to user code (macro expansion mapping),
- async tasks should be nameable/inspectable at least in debug builds.

## 12.7 Open issues {#part-12-7}

- Standard format for reporting conformance results across toolchains.
- Whether “strict concurrency checking” is a separate conformance level or a strictness sub-mode.
