# Part 5 — Control Flow and Safety Constructs {#part-5}

_Working draft v0.11 — last updated 2026-02-27_

_Normative baseline references used in this part: [NR-C18](#part-0-2-1), [NR-LLVM-OBJC](#part-0-2-1), [NR-BLOCKS-ABI](#part-0-2-1), and where applicable [NR-OBJC-RUNTIME](#part-0-2-1)/[NR-ABI-PLATFORM](#part-0-2-1)._

## 5.1 Purpose {#part-5-1}

This part defines:

- `defer` as a core control-flow primitive,
- `guard` as a scope-exit conditional with refinement,
- `match` and pattern matching,
- rules that enable compilers to generate reliable cleanup and diagnostics.

This part specifies syntax and high-level semantics. Cross-cutting cleanup-scope ordering and transfer rules are centralized in [Part 8](#part-8) [§8.1](#part-8-1) and [§8.2.3](#part-8-2-3).

## 5.2 `defer` {#part-5-2}

### 5.2.1 Syntax {#part-5-2-1}

```text
defer compound-statement
```

Example:

```objc
FILE *f = fopen(path, "r");
defer { if (f) fclose(f); }

// ... use f ...
```

### 5.2.2 Semantics (normative) {#part-5-2-2}

Executing a `defer` statement registers its body as a _scope-exit action_ in the innermost enclosing lexical scope.

- Scope-exit actions execute in **LIFO** order at scope exit.
- Scope exit includes normal exit and stack unwinding (including ObjC/C++ exception unwinding where the ABI runs cleanups).
- Ordering relative to implicit ARC releases is defined in [Part 8](#part-8) [§8.2.3](#part-8-2-3).

### 5.2.3 Restrictions (normative) {#part-5-2-3}

A deferred body shall not perform a non-local exit from the enclosing scope. The following are ill‑formed inside a `defer` body:

- `return`, `break`, `continue`, `goto` that exits the enclosing scope,
- `throw` that exits the enclosing scope,
- C++ `throw` that exits the scope.
- calls to `longjmp`/`siglongjmp` that exit the enclosing scope or bypass cleanup scopes.

Rationale: defers must be reliably executed as cleanups; permitting non-local exits leads to un-auditable control flow.

### 5.2.4 Interaction with `async` {#part-5-2-4}

A `defer` body in an `async` function executes when the scope exits, even if the function suspends/resumes multiple times ([Part 7](#part-7) [§7.9.2](#part-7-9-2)).

### 5.2.5 Non-local transfer across cleanup scopes {#part-5-2-5}

`cleanup scope` is defined in [Part 8](#part-8) [§8.1](#part-8-1).

Normative non-local transfer behavior across cleanup scopes is centralized in [Part 8](#part-8) [§8.1](#part-8-1). Conforming implementations shall apply those rules unchanged to `defer` and other control-flow exits in this part.

## 5.3 `guard` {#part-5-3}

### 5.3.1 Syntax {#part-5-3-1}

```text
guard-statement:
    'guard' guard-condition-list 'else' compound-statement
```

A `guard-condition-list` is a comma-separated list of:

- boolean expressions, and/or
- optional bindings (`let`/`var`) as defined in [Part 3](#part-3).

Example:

```objc
guard let user = maybeUser, user.isActive else {
  return nil;
}
```

### 5.3.2 Semantics (normative) {#part-5-3-2}

- Conditions are evaluated left-to-right.
- If all conditions succeed, execution continues after the `guard`.
- If any condition fails, the `else` block executes.

### 5.3.3 Mandatory exit (normative) {#part-5-3-3}

The `else` block of a `guard` statement shall perform a control-flow exit from the current scope. Conforming exits include:

- `return` (from the current function),
- `throw` (from the current `throws` function),
- `break` / `continue` (from the current loop),
- `goto` to a label outside the current scope.

If the compiler cannot prove that all control-flow paths in the `else` block exit, the program is ill‑formed.

### 5.3.4 Refinement {#part-5-3-4}

Names proven by guard conditions are _refined_ after the guard:

- Optional bindings introduce nonnull values in the following scope.
- Null checks refine nullable variables to nonnull within the post-guard region ([Part 3](#part-3)).

## 5.4 `match` {#part-5-4}

### 5.4.1 Motivation {#part-5-4-1}

Objective‑C has `switch`, but it is limited:

- it does not compose well with `Result`/error carriers,
- it does not bind values safely,
- it does not support exhaustiveness checking.

`match` is designed to be:

- explicit,
- analyzable,
- and lowerable to a switch/if-chain without allocations.

### 5.4.2 Syntax (normative) {#part-5-4-2}

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
- `case pattern => expression` is reserved for a future expression form ([§5.4.7](#part-5-4-7)).
- Fallthrough is not permitted.

### 5.4.3 Evaluation order (normative) {#part-5-4-3}

- The scrutinee expression in `match (expr)` shall be evaluated exactly once.
- Case patterns are tested top-to-bottom.
- The first matching case is selected and its body executes.

### 5.4.4 Scoping (normative) {#part-5-4-4}

Bindings introduced by a case pattern are scoped to that case body only.

### 5.4.5 Exhaustiveness (normative) {#part-5-4-5}

In strict mode, `match` must be exhaustive when the compiler can prove the matched type is a _closed set_.

The following are treated as closed sets in v1:

- `Result<T, E>` (cases `Ok` and `Err`),
- other library-defined closed sum types explicitly marked as closed (future extension point).

For closed sets:

- Either all cases must be present, or a `default` must be present.
- Missing cases are an error in strict mode (warning in permissive mode).

### 5.4.6 Lowering (normative) {#part-5-4-6}

`match` shall lower to:

- a switch/if-chain that preserves evaluation order and side effects,
- with no heap allocation required by `match` itself.

### 5.4.7 Reserved expression form (v2+ hook; rejected in v1) {#part-5-4-7}

ObjC 3.0 v1 keeps `match` statement-only. A value-producing form is reserved for a future revision:

```text
match-expression (reserved):
    'match' '(' expression ')' '{' match-expression-case+ match-expression-default? '}'

match-expression-case (reserved):
    'case' pattern '=>' expression ';'

match-expression-default (reserved):
    'default' '=>' expression ';'
```

Reserved typing behavior (for v2+ design stability):

- each selected arm yields one value (non-`void`),
- all arm values must converge to a common result type under the same conversion lattice used for `?:`,
- expression form must be exhaustive for closed sets (or include `default`).

Required v1 behavior:

- using `match` in expression position is ill-formed,
- using `case ... => ...` is ill-formed and shall diagnose that this spelling is reserved for future expression-form `match`,
- parser/diagnostic conformance shall include [Appendix F](#f) rows `P-31`, `P-32`, and `P-33` ([Part 12](#part-12) [§12.5.1](#part-12-5-1), [§12.5.7](#part-12-5-7)).

## 5.5 Patterns (v1) {#part-5-5}

### 5.5.1 Wildcard {#part-5-5-1}

`_` matches anything and binds nothing.

### 5.5.2 Literal constants {#part-5-5-2}

Literal constants match values of compatible type:

- integers, characters,
- `@"string"` for Objective‑C string objects (pointer equality is not required; matching is defined for `Result` and closed types only in v1),
- `nil` for nullable pointer types.

> Note: Rich structural matching for arbitrary objects is intentionally not part of v1.

### 5.5.3 Binding pattern {#part-5-5-3}

A binding pattern introduces a name:

- `let name`
- `var name`

In v1:

- `let` bindings are immutable within the case body.
- `var` bindings are mutable within the case body.

### 5.5.4 `Result` case patterns {#part-5-5-4}

`Result<T, E>` participates in pattern matching ([Part 6](#part-6)).

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

### 5.5.5 Type-test patterns (optional feature set in v1) {#part-5-5-5}

Type-test patterns are **optional** in v1 (not core).

Feature-test contract:

- `__OBJC3_FEATURE_MATCH_TYPE_TEST_PATTERNS__` shall be predefined as `1` when supported and `0` when unsupported.
- If `__has_feature` integration exists ([Part 1](#part-1) [§1.4.3](#part-1-4-3)), `__has_feature(objc3_match_type_test_patterns)` shall report the same capability state.

Syntax (when enabled):

- `is Type`
- `is Type let name` (bind as refined type)
- `is Type var name` (mutable refined binding)

Semantics (when enabled):

- For class types, it matches when the dynamic type is `Type` or a subclass.
- For protocol types, it matches when the value conforms to the protocol.
- `let`/`var` bindings introduced by `is Type let/var name` are scoped to the selected case body.

Module metadata / interface requirements (when used in exported syntax payloads):

- A module payload that requires importer understanding of type-test pattern syntax shall include required capability `objc3.pattern.type_test.v1` in `required_capabilities` ([D.2.1](#d-2-1)).
- Importers that do not support this capability shall reject import as a hard error per [D.2.3](#d-2-3) and [Table E](#d-3-5).
- Textual interfaces exposing such syntax shall guard it with `#if __OBJC3_FEATURE_MATCH_TYPE_TEST_PATTERNS__` (or provide a portable fallback spelling).

Unsupported-mode requirements (`__OBJC3_FEATURE_MATCH_TYPE_TEST_PATTERNS__ == 0`):

- `case is ...` in a pattern position is ill-formed.
- Diagnostics shall explicitly state that type-test patterns are optional and disabled in the current mode, and suggest a mechanical rewrite (`default` + explicit `if`/cast chain) when feasible.

### 5.5.6 Guarded patterns are deferred in v1 (reserved syntax) {#part-5-5-6}

Guarded patterns are **deferred** from v1 core syntax. The following slot is reserved for a future revision:

```text
guarded-match-case (reserved):
    'case' pattern 'where' expression ':' compound-statement
```

Reservation and compatibility requirements:

- In v1, `case pattern where condition:` is ill-formed and shall be rejected with a targeted “guarded patterns are deferred” diagnostic.
- `where` is reserved as a contextual keyword only in the post-pattern guard slot.
- Outside that slot, `where` remains usable as an identifier for source compatibility.
- Parser conformance shall include [Appendix F](#f) rows `P-37`, `P-38`, and `P-39` ([Part 12](#part-12) [§12.5.1](#part-12-5-1), [§12.5.7](#part-12-5-7)).

## 5.6 Required diagnostics {#part-5-6}

Minimum diagnostics include:

- non-exiting `guard else` blocks (error),
- `defer` bodies containing non-local exits (error),
- unreachable `match` cases (warning),
- non-exhaustive `match` over `Result` in strict mode (error; fix-it: add missing case or `default`),
- `match` used where an expression is required (error; explain v1 statement-only rule),
- `case ... => ...` spelling in v1 (error; explain reserved future expression-form syntax),
- `case is ...` when `__OBJC3_FEATURE_MATCH_TYPE_TEST_PATTERNS__ == 0` (error; explain optional feature gate and suggest rewrite),
- importing module metadata that requires `objc3.pattern.type_test.v1` when unsupported locally (hard error),
- `case pattern where condition:` in v1 (error; explain guarded-pattern deferral and reservation).

## 5.7 Open issues {#part-5-7}

None currently tracked in this part.


