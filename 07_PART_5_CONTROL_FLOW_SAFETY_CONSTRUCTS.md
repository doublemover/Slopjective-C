# Part 5 — Control Flow and Safety Constructs
_Working draft v0.6 — last updated 2025-12-28_

## 5.1 Purpose
This part defines:
- `defer` as a core scope-exit primitive,
- `guard` as a scope-exit conditional with optional binding,
- `match` and pattern matching (including carrier patterns for `Result` and optionals),
- rules that enable compilers to generate reliable cleanup, lifetimes, and diagnostics.

The intent is to make common “safety patterns” **explicit in syntax** so compilers can diagnose mistakes (e.g., missing early returns, forgetting cleanup) and optimize more aggressively.

---

## 5.2 `defer`

### 5.2.1 Syntax
```text
defer-statement:
    'defer' compound-statement
```

Example:
```objc
defer { close(fd); }
```

### 5.2.2 Semantics (normative)
- Executing a `defer` statement registers its compound statement as a **scope-exit action** for the innermost enclosing scope.
- Scope-exit actions execute in **LIFO order** when that scope exits.

A scope exits when control leaves it by:
- reaching the end of the scope normally,
- `return`,
- `throw`/`rethrow`,
- `break`/`continue`,
- `goto` that jumps out of the scope,
- cancellation unwind for `async` functions (Part 7), if cancellation causes scope unwinding.

`defer` does **not** execute at `await` suspension points, because suspension is not scope exit.

### 5.2.3 Ordering relative to destructors/ARC (informative but intended)
Within a given scope, the compiler should order cleanup such that:
- explicit `defer` actions run before implicit end-of-scope releases of locals in the same scope, unless a specific construct requires otherwise.
- in `async` functions, `defer` actions follow the normal unwinding rules (i.e., they run on throw/cancellation unwind).

Where precise ordering matters for system resources and retainable C families, see Part 8.

### 5.2.4 Diagnostics
- A `defer` body that contains `return`, `throw`, `break`, `continue`, or `goto` out of the defer-body scope should be diagnosed (at least warning): it is usually a logic error.
- Toolchains should warn if a `defer` body is provably unreachable.

---

## 5.3 `guard`

### 5.3.1 Syntax (provisional)
```text
guard-statement:
    'guard' condition-list 'else' compound-statement

condition-list:
    condition (',' condition)*

condition:
    expression
  | 'let' identifier '=' expression
  | 'var' identifier '=' expression
```

Examples:
```objc
guard x != nil else { return; }
guard let s = maybeString else { throw SomeError(); }
```

### 5.3.2 Semantics (normative)
A `guard` statement evaluates its conditions in source order.

- If all conditions succeed, execution continues after the `guard`.
- If any condition fails, execution transfers into the `else` block.

**No-fallthrough rule:** the `else` block shall not “fall through” to the statement following the guard. It must exit the guarded scope by executing one of:
- `return`,
- `throw`,
- `break` / `continue` (when the guarded scope is a loop),
- `goto` to a label outside the guarded scope (discouraged; toolchains should warn).

### 5.3.3 Optional binding via `guard let` / `guard var`
For a condition `let x = e`:
- `e` must have an optional type `T?` (Part 3).
- If `e` is non-`nil`, `x` is bound to the unwrapped value of type `T` in the remainder of the guarded scope.
- If `e` is `nil`, the guard fails and the `else` block executes.

In strict nullability mode, bindings introduced by `guard let` are treated as **non-null** within the guarded scope.

### 5.3.4 Diagnostics
- If the `else` block can fall through, the program is ill‑formed (strict) or diagnosed (permissive with fix-it suggestions).
- Toolchains should provide fix-its to convert common patterns:
  - `if (!cond) { return; }` → `guard cond else { return; }`
  - `id x = maybe; if (!x) return;` → `guard let x = maybe else { return; }`

---

## 5.4 `match` and pattern matching

### 5.4.1 Syntax (provisional)
```text
match-statement:
    'match' '(' expression ')' '{' match-case+ '}'

match-case:
    'case' pattern ':' compound-statement
  | 'default' ':' compound-statement
```

Example:
```objc
match (result) {
  case Ok(let value): { use(value); }
  case Err(let err): { return Err(err); }
}
```

### 5.4.2 Semantics (normative)
- The match scrutinee expression is evaluated **exactly once**.
- Cases are tested in source order; the first matching case executes.
- Control does not fall through between cases (unlike C `switch`). Each case body is a compound statement.

### 5.4.3 Exhaustiveness (normative, limited v1)
If the scrutinee has one of the following types, then a `match` without a `default` shall be exhaustive (ill‑formed otherwise):
- `Result<T, E>`
- `T?` (optional)

Exhaustiveness means:
- `Result`: the cases cover both `Ok(...)` and `Err(...)`.
- `T?`: the cases cover both `nil` and the non-`nil` binding case.

For other types, exhaustiveness checking is implementation-defined in v1 (toolchains may provide it as a warning in strict modes).

### 5.4.4 Patterns (v1 set)
Objective‑C 3.0 v1 defines a minimal pattern set focused on carriers:

- **Wildcard**: `_` matches anything.
- **Binding**: `let name` binds the scrutinee (or subvalue) to a new immutable name.
- **Nil**: `nil` matches a nil optional/object pointer.
- **Result carriers**:
  - `Ok(p)` matches `Result::Ok` and binds its payload with subpattern `p`.
  - `Err(p)` matches `Result::Err` and binds its payload with subpattern `p`.

> Open issue: a richer pattern language (literals, ranges, tuple/struct destructuring, object type tests) is desirable but left for future drafts.

### 5.4.5 Diagnostics
- Non-exhaustive `match` over `Result`/optional without `default` is ill‑formed (strict) or diagnosed (permissive with fix‑its).
- Unreachable cases should be warned.
- Toolchains should offer fix-its to rewrite common `if/else` ladders on `Result` into `match`.

---

## 5.5 Interactions and ordering notes

### 5.5.1 `defer` and `throw` / `return`
`defer` actions execute during unwinding (return/throw), so they provide a structured alternative to `goto cleanup`.

### 5.5.2 `defer` and `async`
`await` suspension does not trigger defers. However, if an async function unwinds due to a thrown error or cancellation, defers run as usual (Part 7).

### 5.5.3 Evaluation order
The language shall specify evaluation order for `guard` conditions and `match` case testing as described above to preserve predictable side effects.

---

## 5.6 Open issues
- Final syntax for patterns and case labels given Objective‑C’s existing `switch`.
- Whether to support explicit `fallthrough` (recommended: no).
- Object type-test patterns (`is KindOfClass`) that are safe and optimizable.
