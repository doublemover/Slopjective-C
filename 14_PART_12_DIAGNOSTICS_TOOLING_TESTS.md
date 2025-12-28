# Part 12 — Diagnostics, Tooling, and Test Suites
_Working draft v0.6 — last updated 2025-12-28_

## 12.1 Purpose
Objective‑C 3.0 treats tooling requirements as part of language quality and (eventually) conformance:

- diagnostics must be specific and actionable,
- fix‑its should exist for common mechanical migrations,
- and a test suite structure must be defined so implementations do not silently diverge.

This part does **not** define the full conformance profile checklist yet; it defines minimum expectations and the shape of the eventual test suite.

---

## 12.2 Diagnostic principles (normative intent)
A conforming implementation shall:
- emit at least one diagnostic for every ill‑formed construct defined by this spec,
- include the relevant source range and a short, human-readable explanation,
- where a mechanical fix exists, provide a fix‑it hint (or a structured replacement) in strictness modes that enable fix‑its.

Implementations should group diagnostics into named warning groups to support incremental adoption (e.g., `-Wobjc3-nullability`, `-Wobjc3-concurrency`, `-Wobjc3-system`).

---

## 12.3 Required diagnostics (minimum set)

### 12.3.1 Nullability and optionals (Parts 3, 5)
Toolchains shall diagnose:
- incomplete nullability in exported interfaces when the module’s policy requires completeness (strict error),
- passing nullable to nonnull without an unwrap/check (strict error),
- dereferencing a nullable value without unwrap (strict error),
- `guard let` / `if let` bindings where the bound expression is not optional (ill‑formed),
- a `guard` else-block that can fall through (ill‑formed in strict).

Toolchains should provide fix‑its:
- add `nullable` / `nonnull` annotations to parameters/returns,
- rewrite common nil-check idioms to `guard` or `if let`.

### 12.3.2 Optional chaining and optional message sends (Part 3; Decision D‑001)
Toolchains shall diagnose:
- optional chaining (`?.` or `[x? ...]`) applied to a member/method returning a non-reference type in v1 (ill‑formed),
- uses of optional message send where the callee returns a scalar/struct (ill‑formed),
- suspicious patterns where optional message sends drop results unintentionally (warning in permissive; error in strict where configured).

### 12.3.3 Errors, `throws`, and `Result` (Part 6)
Toolchains shall diagnose:
- `throw` outside a `throws` function (ill‑formed),
- `try`/`try?` misuse (ill‑formed),
- mismatch between `throws` and an API annotated as NSError-out-parameter without explicit bridging (diagnosable; strict error where configured).

#### 12.3.3.1 Optional propagation diagnostics (Decision D‑005)
Toolchains shall diagnose use of postfix propagation `?` on `T?` in contexts that cannot early-exit with `nil` (non-optional return types, `throws`, `Result`, etc.), with fix‑its suggesting `guard let` or explicit conversion helpers.

### 12.3.4 Concurrency and `async/await` (Part 7)
Toolchains shall diagnose:
- calling an `async` function without `await` (ill‑formed),
- awaiting in a non-async context (ill‑formed),
- using `try`/`await` ordering incorrectly when the grammar requires a canonical form (provide fix‑it to `try await`),
- violations of actor isolation rules in strict concurrency checking mode (at least warning; error in strict-system where configured).

#### 12.3.4.1 Task spawning diagnostics (Decision D‑003)
In strict concurrency checking configurations, toolchains should warn when a task-spawn operation’s handle/result is unused, to force “fire and forget” to be explicit.

### 12.3.5 System programming extensions (Part 8)
Toolchains shall diagnose (in strict-system at minimum):
- escaping a borrowed pointer beyond its valid lifetime region,
- using a moved-from resource value (if the move model is enabled for that type),
- missing cleanup when a resource type requires scope-exit cleanup and the compiler can prove it.

### 12.3.6 Performance and dynamism controls (Part 9)
Toolchains shall diagnose:
- overriding a `final`/`direct` method (strict error),
- subclassing a `final` class (strict error),
- subclassing a `sealed` class from outside its module (strict error),
- category declarations that conflict with `direct` constraints (at least warning).

### 12.3.7 Metaprogramming (Part 10)
Toolchains shall diagnose:
- non-deterministic or disallowed macro behavior according to the macro safety policy (Part 10),
- derive expansions that conflict with user-declared members without explicit resolution,
- macro/derive expansions that would generate ill‑formed code (with diagnostics at the macro call site and, where possible, at the generated site).

---

## 12.4 Fix‑its, migrators, and refactoring support (informative but intended)
Implementations should provide automated migrations for:
- inserting explicit nullability for exported interfaces (guided by annotations and heuristics),
- rewriting common NSError-out-parameter patterns into `throws`/`Result` where explicitly annotated,
- introducing `guard let` for common nil-check patterns,
- rewriting completion-handler patterns into `async` overlays where annotated.

---

## 12.5 Required tooling surfaces (informative but intended)
Toolchains should support:
- emitting extracted interfaces for modules (Part 2),
- showing macro expansions (Part 10),
- producing “explain why this is nullable/isolated” traces for nullability and concurrency diagnostics.

---

## 12.6 Test suite structure (informative but intended)
The eventual conformance suite should include at least:
- parser/grammar tests (new keywords, optional send syntax, `throws`, `async`),
- type-checking tests (nullability, optionals, propagation operator restrictions),
- module-boundary tests (nullability completeness and metadata preservation),
- runtime/semantic tests where behavior depends on runtime (actors/executors/cancellation).

This draft intentionally leaves the exact conformance checklist to a separate step.

---

## 12.7 Debuggability requirements (normative intent)
For features like macros and async:
- stack traces must preserve user source locations,
- debugging tools must be able to map generated code back to user code,
- macro expansion views must be available at least via a compiler option.

---

## 12.8 Open issues
- Standard format for reporting conformance results.
- Whether “strict concurrency checking” is a separate conformance level or a sub-mode within strictness.
