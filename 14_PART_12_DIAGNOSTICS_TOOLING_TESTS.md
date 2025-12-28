# Part 12 — Diagnostics, Tooling, and Test Suites

## 12.1 Purpose
Objective‑C 3.0 treats tooling requirements as part of conformance:
- diagnostics must be specific and actionable,
- fix-its must exist for common mechanical migrations,
- a conformance test suite must exist to prevent “whatever the compiler happens to do.”

## 12.2 Required diagnostics (minimum set)
A conforming implementation shall provide diagnostics for:

### 12.2.1 Nullability
- incomplete nullability in exported interfaces (strict mode error),
- nullable passed to nonnull without check/unwrap (strict error),
- dereferencing nullable without unwrap (strict error).

### 12.2.2 Optionals and bindings
- binding shadowing warnings,
- unwrap of optional without proof (strict error),
- IUO used in strict mode without boundary annotation (warning/error based on policy).

### 12.2.3 Resource and cleanup
- missing cleanup for types annotated `@requires_cleanup` (strict-system error),
- wrong cleanup function for resource family (strict-system error),
- double cleanup and use-after-take (strict-system error).

### 12.2.4 Borrowed pointers
- borrowed pointer escape to heap/global/escaping block (strict-system error),
- borrowed pointer used after owner lifetime ends (diagnose with suggested `withLifetime`).

### 12.2.5 Concurrency
- `await` outside an async context (error),
- calling an `async` function without `await` (error),
- capturing non-Sendable values into concurrent tasks (strict concurrency error),
- actor-isolated state accessed without required `await` (strict concurrency error),
- cross-executor calls into `@executor(main)` isolated code without required `await` hop (strict concurrency error),
- `@executor(main)` calls that would require a hop from non-async code (strict error; suggest explicit bridging API or making the caller async).


### 12.2.6 Errors and propagation
- calling a throwing function without `try` (error),
- `throw` used outside a throwing context (error),
- postfix propagation `?` used outside the follow-token restriction (error with fix-it),
- postfix propagation `?` applied to an optional in a non-optional-returning function (error in v1; suggest `guard let`/`if let`).

### 12.2.7 Performance controls
- direct method declared but used inconsistently (error),
- sealed/final violations (error),
- dynamic replacement in static regions (debug warning or strict perf error).

## 12.3 Fix-its and migrator behaviors
A conforming toolchain shall provide fix-its for:
- adding nullability annotations in headers (best-effort inference),
- rewriting weak-strong dance to capture list syntax,
- inserting `defer` for common cleanup patterns,
- inserting `withLifetime/keepAlive` for borrowed-pointer hazards,
- suggesting `Result`/`throws` wrappers for NSError patterns.

## 12.4 Static analyzer integration
The specification requires that:
- retainable families and resource annotations are visible to the analyzer,
- borrowed-pointer annotations are checkable (escape analysis),
- concurrency annotations feed race/cycle checkers where possible.

## 12.5 Conformance test suite structure
The official conformance suite shall include:
- parser tests (grammar acceptance and diagnostics)
- semantic tests (type rules, nullability refinement, borrow rules)
- codegen tests (ARC ordering with defer, coroutine lowering invariants)
- interop tests (C, Swift, ObjC++ compilation)

The suite should be executable in continuous integration for compilers implementing ObjC 3.0.

## 12.6 Debuggability requirements
For features like macros and async:
- stack traces must preserve source locations,
- debugging tools must be able to map generated code back to user code,
- macro expansion views must be available.

## 12.7 Open issues
- Standard format for reporting conformance results.
- Whether “strict concurrency checking” is a separate conformance level or a sub-mode within strictness.

### 12.2.5.1 Additional concurrency diagnostics (v0.5)
In strict concurrency checking mode, toolchains should warn when a task handle returned from a task-spawn API is unused, to make “fire and forget” explicit.
