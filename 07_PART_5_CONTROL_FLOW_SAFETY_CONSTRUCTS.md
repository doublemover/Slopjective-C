# Part 5 — Control Flow and Safety Constructs
_Working draft v0.6 — last updated 2025-12-28_

## 5.1 Purpose
This part defines:
- `defer` as a core control-flow primitive,
- `guard` as a scope-exit conditional with refinement,
- `match` and pattern matching,
- rules that enable compilers to generate reliable cleanup and diagnostics.

This part specifies syntax and high-level semantics. Precise cleanup ordering (especially for system resources) is specified in Part 8 and is normative.

## 5.2 `defer`

### 5.2.1 Syntax
```text
defer compound-statement
```

Example:
```objc
FILE *f = fopen(path, "r");
defer { if (f) fclose(f); }

// ... use f ...
```

### 5.2.2 Semantics (normative)
Executing a `defer` statement registers its body as a *scope-exit action* in the innermost enclosing lexical scope.

- Scope-exit actions execute in **LIFO** order at scope exit.
- Scope exit includes normal exit and stack unwinding (where the ABI runs cleanups).
- `defer` bodies execute **before** implicit ARC releases of strong locals in the same scope (Part 8).

### 5.2.3 Restrictions (normative)
A deferred body shall not perform a non-local exit from the enclosing scope. The following are ill‑formed inside a `defer` body:
- `return`, `break`, `continue`, `goto` that exits the enclosing scope,
- `throw` that exits the enclosing scope,
- C++ `throw` that exits the scope.

Rationale: defers must be reliably executed as cleanups; permitting non-local exits leads to un-auditable control flow.

### 5.2.4 Interaction with `async`
A `defer` body in an `async` function executes when the scope exits, even if the function suspends/resumes multiple times (Part 7 §7.9.2).

## 5.3 `guard`

### 5.3.1 Syntax
```text
guard-statement:
    'guard' guard-condition-list 'else' compound-statement
```

A `guard-condition-list` is a comma-separated list of:
- boolean expressions, and/or
- optional bindings (`let`/`var`) as defined in Part 3.

Example:
```objc
guard let user = maybeUser, user.isActive else {
  return nil;
}
```

### 5.3.2 Semantics (normative)
- Conditions are evaluated left-to-right.
- If all conditions succeed, execution continues after the `guard`.
- If any condition fails, the `else` block executes.

### 5.3.3 Mandatory exit (normative)
The `else` block of a `guard` statement shall perform a control-flow exit from the current scope. Conforming exits include:
- `return` (from the current function),
- `throw` (from the current `throws` function),
- `break` / `continue` (from the current loop),
- `goto` to a label outside the current scope.

If the compiler cannot prove that all control-flow paths in the `else` block exit, the program is ill‑formed.

### 5.3.4 Refinement
Names proven by guard conditions are *refined* after the guard:
- Optional bindings introduce nonnull values in the following scope.
- Null checks refine nullable variables to nonnull within the post-guard region (Part 3).

## 5.4 `match`

### 5.4.1 Motivation
Objective‑C has `switch`, but it is limited:
- it does not compose well with `Result`/error carriers,
- it does not bind values safely,
- it does not support exhaustiveness checking.

`match` is designed to be:
- explicit,
- analyzable,
- and lowerable to a switch/if-chain without allocations.

### 5.4.2 Syntax (normative)
```text
match-statement:
    'match' '(' expression ')' ' match-case+ match-default? '

match-case:
    'case' pattern ':' compound-statement

match-default:
    'default' ':' compound-statement
```

Notes:
- `match` is a statement (not an expression) in v1.
- Fallthrough is not permitted.

### 5.4.3 Evaluation order (normative)
- The scrutinee expression in `match (expr)` shall be evaluated exactly once.
- Case patterns are tested top-to-bottom.
- The first matching case is selected and its body executes.

### 5.4.4 Scoping (normative)
Bindings introduced by a case pattern are scoped to that case body only.

### 5.4.5 Exhaustiveness (normative)
In strict mode, `match` must be exhaustive when the compiler can prove the matched type is a *closed set*.

The following are treated as closed sets in v1:
- `Result<T, E>` (cases `Ok` and `Err`),
- other library-defined closed sum types explicitly marked as closed (future extension point).

For closed sets:
- Either all cases must be present, or a `default` must be present.
- Missing cases are an error in strict mode (warning in permissive mode).

### 5.4.6 Lowering (normative)
`match` shall lower to:
- a switch/if-chain that preserves evaluation order and side effects,
- with no heap allocation required by `match` itself.

## 5.5 Patterns (v1)

### 5.5.1 Wildcard
`_` matches anything and binds nothing.

### 5.5.2 Literal constants
Literal constants match values of compatible type:
- integers, characters,
- `@"string"` for Objective‑C string objects (pointer equality is not required; matching is defined for `Result` and closed types only in v1),
- `nil` for nullable pointer types.

> Note: Rich structural matching for arbitrary objects is intentionally not part of v1.

### 5.5.3 Binding pattern
A binding pattern introduces a name:

- `let name`
- `var name`

In v1:
- `let` bindings are immutable within the case body.
- `var` bindings are mutable within the case body.

### 5.5.4 `Result` case patterns
`Result<T, E>` participates in pattern matching (Part 6).

The following case patterns are defined:
- `.Ok(let value)`
- `.Err(let error)`

Example:
```objc
match (r) {
  case .Ok(let v): { use(v); }
  case .Err(let e): { log(e); }
}
```

### 5.5.5 Type-test patterns (optional in v1)
Implementations may provide a type-test pattern form:

- `is Type`
- `is Type let name` (bind as refined type)

If provided:
- For class types, it matches when the dynamic type is `Type` or a subclass.
- For protocol types, it matches when the value conforms to the protocol.

> This feature is optional in v1 because it intersects with Objective‑C runtime reflection and optimization.

## 5.6 Required diagnostics
Minimum diagnostics include:
- non-exiting `guard else` blocks (error),
- `defer` bodies containing non-local exits (error),
- unreachable `match` cases (warning),
- non-exhaustive `match` over `Result` in strict mode (error; fix-it: add missing case or `default`).

## 5.7 Open issues
- Whether `match` should also exist as an expression form (returning a value) in a future revision.
- Whether to standardize type-test patterns and how they should lower (runtime checks vs static reasoning).
- Whether to add guarded patterns (e.g., `case pat where condition:`) once keyword reservation is settled.
