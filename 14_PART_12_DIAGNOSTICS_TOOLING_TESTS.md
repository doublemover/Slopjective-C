# Part 12 — Diagnostics, Tooling, and Test Suites
_Working draft v0.6 — last updated 2025-12-28_

## 12.1 Purpose
Objective‑C 3.0 treats tooling requirements as part of conformance:
- diagnostics must be specific and actionable,
- fix-its must exist for common mechanical migrations,
- a conformance test suite must exist to prevent “whatever the compiler happens to do.”

This part defines **minimum requirements** for:
- diagnostics,
- fix-its and migrators,
- analyzer integration,
- and test suite structure.

## 12.2 Diagnostic quality requirements (normative)
A conforming implementation shall ensure that diagnostics:
- point to the smallest relevant source span,
- include the reason and the violated rule,
- provide a fix-it when a mechanical fix exists,
- and are stable enough for CI (machine-consumable categories or codes).

## 12.3 Required diagnostics (minimum set)

### 12.3.1 Nullability (Part 3)
- incomplete nullability in exported interfaces (strict mode: error),
- nullable passed to nonnull without check/unwrap (strict: error),
- dereferencing nullable without unwrap (strict: error),
- unqualified name collision due to module imports (Part 2) (strict: error; fix-it: qualify name).

### 12.3.2 Optionals and bindings (Part 3/5)
- binding shadowing warnings,
- unwrap of optional without proof (strict: error),
- IUO used in strict mode without explicit boundary annotation (warning/error per policy),
- invalid use of optional chaining on scalar/struct returns (Part 3, D‑001) (error),
- optional propagation `e?` used in non-optional-returning function (Part 6, D‑005) (error with fix-it: `guard let` or explicit conversion).

### 12.3.3 Generics and key paths (Part 3)
- generic parameter constraint mismatch (error),
- key path that does not type-check against the base type (error),
- unsafe key path string conversion where not proven safe (strict: warning/error).

### 12.3.4 Errors and `Result` (Part 6)
- `throw` outside a `throws` function (error),
- `try` required but missing (error),
- unused error values in strict mode when intent is unclear (warning; fix-it: `try?` or explicit discard),
- `match` over `Result` not exhaustive in strict mode (Part 5) (error with fix-it: add missing case).

### 12.3.5 Concurrency (Part 7)
- `await` outside async context (error),
- calling async functions without `await` (error),
- calling executor-annotated declarations from the wrong executor without an async hop (strict concurrency: error),
- capturing non-Sendable values into concurrent tasks (strict concurrency: error),
- actor-isolated state accessed without await (error),
- unused task handles returned from task-spawn APIs (strict concurrency: warning; makes fire-and-forget explicit),
- unsafeSendable escape hatch used (strict concurrency: warning).

### 12.3.6 Resources and cleanup (Part 8)
- missing cleanup for types annotated as requiring cleanup (strict-system: error),
- wrong cleanup function for resource family (strict-system: error),
- double cleanup and use-after-take (strict-system: error),
- moved-from resource used (strict-system: error),
- capturing borrowed pointers into escaping blocks (strict-system: error).

### 12.3.7 `defer`/`guard`/`match` (Part 5)
- `defer` body containing non-local exit (error),
- `guard else` that does not exit (error),
- unreachable `match` cases (warning),
- fallthrough attempts in `match` (error; fallthrough is not supported).

### 12.3.8 Performance controls (Part 9)
- direct method declared but overridden/implemented illegally (error),
- sealed/final violations (error),
- dynamic replacement in static regions (debug warning or strict perf error if supported).

### 12.3.9 Macros and derives (Part 10)
- macro expansion failure with source location across expansion boundary (error),
- non-deterministic macro behavior detected (if toolchain can detect) (warning/error per policy),
- derive precondition failures (error).

## 12.4 Fix-its and migrator behaviors
A conforming toolchain shall provide fix-its for common migrations, at least:

- adding nullability annotations in headers (best-effort inference),
- rewriting weak-strong dance to capture list syntax,
- inserting `defer` for common cleanup patterns,
- inserting `withLifetime/keepAlive` for borrowed-pointer hazards,
- suggesting `Result`/`throws` wrappers for NSError/status-code APIs,
- rewriting keyword collisions using raw identifiers.

## 12.5 Analyzer integration
The specification requires that metadata be visible to analyzers:
- retainable families and transfer annotations (Part 8) feed retain/release modeling,
- borrowed-pointer annotations feed escape analysis,
- concurrency annotations feed race/cycle checkers where possible.

## 12.6 Warning groups and flags (recommended)
Toolchains should provide warning groups so teams can adopt ObjC 3.0 incrementally. Recommended groups:
- `-Wobjc3-nullability`
- `-Wobjc3-optionals`
- `-Wobjc3-errors`
- `-Wobjc3-concurrency`
- `-Wobjc3-system`
- `-Wobjc3-performance`
- `-Wobjc3-macros`

## 12.7 Conformance test suite structure
A conforming implementation should ship (or participate in) a conformance suite including:

1. **Parser tests** — grammar acceptance and keyword collision behavior.
2. **Semantic tests** — typing rules, nullability refinement, carrier propagation rules.
3. **Lowering tests** — `defer` ordering with ARC, coroutine lowering invariants, cancellation cleanup.
4. **Interop tests** — C, ObjC++, Swift import/export of key metadata.
5. **Diagnostics tests** — exact warning/error expectations under each conformance level.

The suite should be runnable in CI and provide machine-readable pass/fail reports.

## 12.8 Debuggability requirements
For features like macros and async:
- stack traces should preserve logical source locations,
- debuggers should be able to map generated code back to user code,
- macro expansion views must be available (IDE or compiler flag).

## 12.9 Open issues
- Standard format for reporting conformance results across toolchains.
- Whether strict concurrency checking is a separate conformance level or a sub-mode (this draft treats it as a sub-mode).
