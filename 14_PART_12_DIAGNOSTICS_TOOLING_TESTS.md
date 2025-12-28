# Part 12 — Diagnostics, Tooling, and Test Suites
_Working draft v0.8 — last updated 2025-12-28_

## 12.1 Purpose
Objective‑C 3.0 treats tooling requirements as part of conformance:

- diagnostics must be specific and actionable,
- fix-its must exist for common mechanical migrations,
- and a conformance test suite must exist to prevent “whatever the compiler happens to do.”

This part is cross-cutting and references:
- **01B** for canonical spellings and interface emission requirements,
- **01C** for separate compilation and lowering contracts.

## 12.2 Diagnostic principles (normative)
A conforming implementation shall provide diagnostics that:
1. Identify the precise source range of the problem.
2. Explain the rule being violated in terms of Objective‑C 3.0 concepts (nonnull, optional send, executor hop, etc.).
3. Provide at least one actionable suggestion.
4. Provide a fix-it when the transformation is mechanical and semantics-preserving.

## 12.3 Required diagnostics (minimum set)

### 12.3.1 Nullability and optionals
- Incomplete nullability in exported interfaces (strict mode: error).
- Passing nullable to nonnull without check/unwrap (strict: error; permissive: warning).
- Dereferencing nullable without unwrap (strict: error).
- Optional message send or optional member access used on scalar/struct returns (error; v1 restriction).
- Ordinary message send on nullable receiver in strict mode (error; fix-it suggests optional send or `guard let`).

### 12.3.2 Errors and `throws`
- Calling a `throws` function/method without `try` (error).
- Redeclaring a `throws` declaration without `throws` (or vice versa) across module boundaries (error; see 01C.2).
- Using postfix propagation `e?` on `T?` in a non-optional-returning function (error; fix-it suggests `guard let` or explicit conditional).
- Using postfix propagation `e?` expecting it to map to `throws`/`Result` (error; explain “carrier preserving” rule).

### 12.3.3 Concurrency (`async/await`, executors, actors)
- Calling an `async` function without `await` where required by grammar/semantics (error).
- Calling into an `objc_executor(X)` declaration from a different executor without an `await` hop (strict concurrency: error; permissive: warning).
- Capturing non-Sendable-like values into a task-spawned async closure (strict concurrency: error; permissive: warning).
- Actor-isolated member access from outside the actor without an `await`ed hop (strict concurrency: error).

### 12.3.4 Modules and interface emission
- Using a module-qualified name with a non-imported module: error with fix-it to add `@import`.
- Importing an API through a mechanism that loses effects/attributes (e.g., textual header without metadata) when strictness requires them: warning with suggestion to enable modules/interface emission.
- Emitted interface does not use canonical spellings from 01B: tooling warning; in “interface verification” mode, error.

### 12.3.5 Performance/dynamism controls
- Overriding a `objc_final` method/class: error.
- Subclassing an `objc_final` class: error.
- Subclassing an `objc_sealed` class from another module: error.
- Declaring a category method that collides with a `objc_direct` selector: error.
- Calling a `objc_direct` method via dynamic facilities (e.g., `performSelector:`) when it can be statically diagnosed: warning (or error under strict performance profile).

## 12.4 Required tooling capabilities (minimum)

### 12.4.1 Migrator
A conforming toolchain shall provide (or ship) a migrator that can:
- insert nullability annotations based on inference and usage,
- rewrite common nullable-send patterns into optional sends or `guard let`,
- generate `throws` wrappers for NSError-out patterns (Part 6),
- suggest executor/actor hop fixes where statically determinable (Part 7).

### 12.4.2 Interface emission / verification
If the toolchain supports emitting an interface description for distribution, it shall also support a verification mode that checks:
- semantic equivalence between the original module and the emitted interface,
- and the use of canonical spellings from 01B.

### 12.4.3 Static analysis hooks
Implementations shall expose enough information for analyzers to:
- reason about nullability flow,
- reason about `throws` propagation,
- and enforce Sendable-like constraints under strict concurrency.

## 12.5 Conformance test suite (minimum expectations)
A conforming implementation shall ship or publish a test suite that covers at least:

### 12.5.1 Parsing/grammar
- `async`/`await`/`throws` grammar interactions (`try await`, `await try`, etc.).
- Module-qualified name parsing (`@A.B.C`).

### 12.5.2 Type system and diagnostics
- Strict vs permissive nullability behavior.
- Optional send restrictions (reference-only).
- Postfix propagation carrier-preserving rules.
- Effect mismatch diagnostics across module imports (01C.2).

### 12.5.3 Dynamic semantics
- Optional send argument evaluation does not occur when receiver is `nil`.
- `throws` propagation through nested `do/catch`.
- Cancellation propagation in structured tasks.

### 12.5.4 Runtime contracts
- Autorelease pool draining at suspension points (Objective‑C runtimes) (D‑006 / 01C.7).
- Executor hops preserve ordering and do not deadlock in basic scenarios.

## 12.6 Debuggability requirements (minimum)
For features like macros and async:
- stack traces must preserve source locations,
- debugging tools must be able to map generated code back to user code (macro expansion mapping),
- async tasks should be nameable/inspectable at least in debug builds.

## 12.7 Open issues
- Standard format for reporting conformance results across toolchains.
- Whether “strict concurrency checking” is a separate conformance level or a strictness sub-mode.
