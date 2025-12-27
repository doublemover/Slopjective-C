# Part 5 — Control Flow and Safety Constructs

## 5.1 Purpose
This part defines:
- `defer` as a core control-flow primitive,
- `guard` as a scope-exit conditional,
- `match` and pattern matching,
- rules that enable compilers to generate reliable cleanup and diagnostics.

## 5.2 `defer`
### 5.2.1 Syntax
```text
defer { statements }
```

### 5.2.2 Semantics (summary)
- Executing a `defer` registers a scope-exit action.
- Actions execute in LIFO order at scope exit.
- `defer` runs on normal exits and stack unwinding exits.

Full ordering rules, interaction with ARC releases, and restrictions are specified in Part 8 (because they are critical for system-library correctness). This part defines `defer` as a general language construct usable anywhere.

### 5.2.3 Restrictions
A deferred body shall not perform a non-local exit from the surrounding scope (diagnosed as error).

## 5.3 `guard`
### 5.3.1 Syntax
```objc
guard condition else { ... exit ... }
```

### 5.3.2 Semantics
- If `condition` is true, execution continues after the guard.
- If false, the `else` block executes and must exit the current scope.
- Conditions may include:
  - boolean expressions
  - optional binding (Part 3)
  - pattern matching binding (Part 5.4)

The compiler shall treat variables proven by the guard as refined after the guard.

## 5.4 `match` and patterns
### 5.4.1 Syntax (provisional)
```objc
match (expr) {
  case pattern1: { ... }
  case pattern2: { ... }
  default: { ... }
}
```

### 5.4.2 Pattern forms
ObjC 3.0 defines these initial pattern kinds:
- wildcard: `_`
- constant: `42`, `@"x"`, `nil`
- enum case: `.caseName` or `Type.caseName`
- tuple-like destructuring for sum types with payloads (Part 10 if ADTs are defined there)
- type test: `is Type` (optional)

### 5.4.3 Exhaustiveness
In strict mode:
- `match` over a closed set (closed enum / sum type) must be exhaustive or include `default`.
- Missing cases are errors.

### 5.4.4 Binding
Patterns may bind names:

```objc
case .ok(value): { ... }
```

Bound names are immutable by default.

### 5.4.5 Lowering
`match` shall lower to:
- a switch/if-chain that preserves evaluation order and side effects.
- no additional allocations are required by the construct itself.

## 5.5 Open issues
- Final syntax for patterns and case labels given Objective‑C’s existing `switch`.
- Whether to allow fallthrough (recommended: no).
- How pattern matching interacts with Objective‑C object type tests (`isKindOfClass:`) in a way that is safe and optimizable.
