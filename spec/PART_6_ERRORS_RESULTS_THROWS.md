# Part 6 — Errors: Result, throws, try, and Propagation {#part-6}

_Working draft v0.11 — last updated 2026-02-23_

_Normative baseline references used in this part: [NR-C18](#part-0-2-1), [NR-LLVM-OBJC](#part-0-2-1), [NR-BLOCKS-ABI](#part-0-2-1), and where applicable [NR-OBJC-RUNTIME](#part-0-2-1)/[NR-ABI-PLATFORM](#part-0-2-1)._

## 6.0 Overview {#part-6-0}

### v0.10 resolved decisions {#part-6-v0-10-resolved-decisions}

- ObjC 3.0 v1 does not add a dedicated `never throws` marker; absence of `throws` is the canonical non-throwing form.
- Generic non-throwing requirements are expressed using non-throwing function/block types, not a new keyword or attribute.
- Nil-to-error mapping is explicit and library-defined via canonical `objc3.errors` helpers (`orThrow` and `okOr`), not language sugar.
- Typed throws remains a planned post-v1 feature; v1 reserves syntax/metadata slots and requires compatibility diagnostics for non-v1 typed-throws metadata.

### v0.9 resolved decisions {#part-6-v0-9-resolved-decisions}

- The required module metadata set for preserving `throws` across module boundaries is enumerated in D.
- `try`/`throw`/`throws` are explicitly specified to be compile-time effects; runtime reflection of `throws` is not required in v1.

### v0.8 resolved decisions {#part-6-v0-8-resolved-decisions}

- `throws` uses an untyped error value: `id<Error>`.
- Postfix optional propagation `e?` is carrier-preserving and is only valid in optional-returning functions (no implicit nil→error mapping).
- Conforming implementations provide a stable calling convention for `throws` across module boundaries ([C.4](#c-4)).

### v0.5 resolved decisions {#part-6-v0-5-resolved-decisions}

- Optional propagation `e?` for `e : T?` is valid **only** in optional-returning functions (carrier-preserving). It is ill‑formed in `throws` or `Result` contexts (no implicit nil→error mapping).

### v0.4 resolved decisions {#part-6-v0-4-resolved-decisions}

- `throws` is **untyped** in v1: thrown values are `id<Error>`.
- Typed throws syntax `throws(E)` is reserved for future extension and is not part of v1 grammar.

Objective‑C 3.0 standardizes a modern, explicit error model that can be used in new code while interoperating with existing Cocoa and system APIs.

This part defines:

- A standard error protocol `Error`.
- A `throws` effect for methods/functions (typed at least as `id<Error>`).
- `throw` statement and `try` expressions.
- A `do`/`catch` statement for handling thrown errors.
- A standard `Result<T, E>` carrier type.
- A postfix **propagation** operator `?` for `Result` and optionals (with disambiguation rules suitable for C/ObjC syntax).
- Interoperability rules for NSError-out-parameter patterns and return-code APIs.

This error system is designed to be implementable without relying on Objective‑C exceptions (`@throw/@try/@catch`), which remain separate.

---

## 6.1 Lexical and grammar additions {#part-6-1}

### 6.1.1 New keywords {#part-6-1-1}

In ObjC 3.0 mode, the following are reserved by this part:

- `throws`, `throw`, `try`, `do`, `catch`

### 6.1.2 Grammar summary (high level) {#part-6-1-2}

This part introduces:

- `throws` as a function/method specifier.
- `throw` as a statement.
- `try`, `try?`, `try!` as expressions.
- `do { ... } catch ...` as a statement.
- `?` as a postfix propagation operator in limited contexts (see [§6.6](#part-6-6)).

---

## 6.2 The `Error` protocol and error values {#part-6-2}

### 6.2.1 Definition {#part-6-2-1}

Objective‑C 3.0 defines a standard protocol:

```objc
@protocol Error
@end
```

An _error value_ is any Objective‑C object value that conforms to `Error`.

### 6.2.2 Bridging to NSError {#part-6-2-2}

For platforms with Foundation:

- `NSError` shall be treated as conforming to `Error`.

Implementations may provide:

- implicit bridging from common error representations (e.g., status codes) into error objects via library hooks.

> Note: The language does not require Foundation, but defines the protocol so multiple ecosystems can conform.

---

## 6.3 `throws` effect {#part-6-3}

### 6.3.1 Grammar {#part-6-3-1}

A function or method declaration may include a `throws` specifier.

Provisional grammar (illustrative):

```text
function-declaration:
    declaration-specifiers declarator throws-specifier? function-body

throws-specifier:
    'throws'
```

### 6.3.2 Semantics {#part-6-3-2}

A `throws` function may either:

- return normally with its declared return value, or
- exit by throwing an error value.

The thrown value type is `id<Error>`.

### 6.3.3 Call-site requirements {#part-6-3-3}

A call to a throwing function is ill-formed unless it appears:

- within a `try` expression, or
- within a context that explicitly handles the error (e.g., bridging to Result).

### 6.3.4 Function and block types {#part-6-3-4}

A throwing function’s type is distinct from a non-throwing function’s type.

Examples (illustrative):

- `R (^)(Args) throws` is a throwing block type.
- `R (^)(Args)` is a non-throwing block type.

**Conversion rules (normative intent):**

- A non-throwing function/block value may be implicitly converted to a throwing type (it never throws).
- A throwing function/block value shall not be implicitly converted to a non-throwing type.
  Toolchains may provide an explicit adapter helper that converts a throwing callable into a non-throwing callable by handling errors (e.g., by trapping, by mapping to `Result`, or by returning an optional), but such adapters must be explicit at the call site.

`async` and `throws` compose: `async throws` is a distinct combined effect set.

### 6.3.5 ABI and lowering (normative for implementations) {#part-6-3-5}

Source-level semantics for `throws` are defined in this part.
In addition, conforming implementations shall ensure `throws` is stable under separate compilation ([C.2](#c-2)).

A conforming implementation shall:

- record the `throws` effect in module metadata (see also [D.3.1](#d-3-1) [Table A](#d-3-1)),
- reserve typed-throws metadata slots with v1-default values as specified in [§6.3.7](#part-6-3-7),
- diagnose effect mismatches on redeclaration/import ([C.2](#c-2)),
- and provide a stable calling convention for throwing functions/methods.

The recommended calling convention is the _trailing error-out parameter_ described in [C.4](#c-4).

> Note: This draft intentionally chooses an error-out convention

**Reflection note (non-normative):** v1 does not require encoding `throws` in Objective‑C runtime type encodings. Toolchains may provide extended metadata as an extension.
rather than stack unwinding, to keep interoperability with NSError/return-code APIs straightforward and to align with existing Objective‑C runtime practices.

Throwing is part of a function’s type.

- `R (^)(Args) throws` is a throwing block type.
- `R (^)(Args)` is non-throwing.

A throwing function value cannot be assigned to a non-throwing function type without an explicit adapter.

### 6.3.6 Non-throwing declarations (v1 decision) {#part-6-3-6}

ObjC 3.0 v1 does not define a dedicated `nothrows`/`never throws` keyword or standard attribute.

- A function or method declaration without `throws` shall be non-throwing.
- Module/interface metadata shall preserve non-throwing status by encoding the absence of `throws` (or an equivalent canonical `throws=false` bit); no additional source marker is required for conformance.

Recommended patterns for generic and callable APIs:

- Require non-throwing callable types directly (for example, `R (^)(Args)` rather than `R (^)(Args) throws`).
- If an API should accept both throwing and non-throwing callables, declare the parameter as throwing and rely on the implicit non-throwing to throwing conversion in [§6.3.4](#part-6-3-4).
- When adapting a throwing callable to a non-throwing callable, use an explicit adapter that handles the error path.

### 6.3.7 Reserved typed-throws slots (planned, not in v1) {#part-6-3-7}

Typed throws is planned for a future revision, but it is not available in ObjC 3.0 v1.

Reserved source syntax slots:

- `throws(type-name)`
- `throws(type-name, ...)`

v1 parser and diagnostics requirements:

- A declaration using `throws(` ... `)` shall be rejected in v1 mode with a diagnostic that typed throws is unsupported in v1.
- A compiler shall not reinterpret `throws(...)` as bare `throws`, and shall not silently erase the parenthesized payload.
- A fix-it may suggest replacing `throws(...)` with bare `throws` when preserving behavior.

Reserved metadata slots for module/interface exchange:

- `throws_kind`: enum slot. v1 requires `untyped`; `typed` is reserved.
- `throws_type_arity`: unsigned slot. v1 requires `0`.
- `throws_type_refs`: sequence slot of canonical type references. v1 requires empty.

Compatibility constraints:

- A v1 consumer that imports non-v1 typed-throws metadata values (for example, `throws_kind=typed`, non-zero arity, or non-empty type refs) shall emit an incompatibility diagnostic and reject that declaration for v1 conformance.
- A producer targeting v1 shall not emit non-v1 typed-throws metadata values.
- If a future revision introduces typed throws, untyped and typed declarations are effect-signature-distinct across module boundaries unless that future revision explicitly defines a conversion rule.

---

## 6.4 `throw` statement {#part-6-4}

### 6.4.1 Grammar {#part-6-4-1}

```text
throw-statement:
    'throw' expression ';'
```

### 6.4.2 Static semantics {#part-6-4-2}

A `throw` statement is permitted only within a `throws` function or within a `catch` block.

In v1, `throws` is untyped. The thrown expression shall be convertible to `id<Error>`.

Typed throws forms (for example, `throws(E)`) are reserved for a future revision; their v1 reservation and compatibility rules are defined in [§6.3.7](#part-6-3-7).

### 6.4.3 Dynamic semantics {#part-6-4-3}

Executing `throw e;`:

- evaluates `e`,
- exits the current function by the error path,
- executes scope-exit actions (`defer`, resource cleanups) as specified in [Part 8](#part-8).

> Note: This is not Objective‑C exception throwing. It is a structured error return path.

---

## 6.5 `try` expressions {#part-6-5}

### 6.5.1 Grammar {#part-6-5-1}

```text
try-expression:
    'try' expression
  | 'try' '?' expression
  | 'try' '!' expression
```

### 6.5.2 `try e` {#part-6-5-2}

`try e` evaluates `e` in a context that may throw.

- If `e` completes normally, `try e` yields its value.
- If `e` throws, the error is propagated to the caller, and the enclosing function must be `throws`.

### 6.5.3 `try? e` {#part-6-5-3}

`try? e` converts a throwing evaluation into an optional result:

- If `e` succeeds, yields the value.
- If `e` throws, yields `nil` and discards the error.

In strict mode, discarding errors should produce a warning unless explicitly marked as intentional.

### 6.5.4 `try! e` {#part-6-5-4}

`try! e` forces a throwing evaluation:

- If `e` succeeds, yields the value.
- If `e` throws, the program traps.

### 6.5.5 Typing {#part-6-5-5}

- `try e` has the type of `e`.
- `try? e` has optional type of `e` (i.e., `T?`).
- `try! e` has the type of `e` but is unsafe in strict mode unless proven not to throw.

---

## 6.6 Postfix propagation operator `?` (Result / Optional) {#part-6-6}

### 6.6.1 Purpose {#part-6-6-1}

The postfix propagation operator provides Rust-like ergonomics for **carrier types** without relying on exceptions:

- `Result<T, E>`: unwrap or early-return error.
- `T?`: unwrap or early-return `nil` (or throw) depending on surrounding function’s declared return/effects.

This operator is intentionally limited to avoid conflict with C’s conditional operator `?:`.

### 6.6.2 Grammar and disambiguation {#part-6-6-2}

The propagation operator is a postfix `?` applied to a **postfix-expression**, with a syntactic restriction:

- It may only appear when the next token is one of:
  - `')'`, `']'`, `'}'`, `','`, `';'`

Grammar:

```text
propagate-expression:
    postfix-expression '?'
```

> Note: This means `foo()? + 1` must be written as `(foo()?) + 1`.

### 6.6.3 Semantics for `Result` {#part-6-6-3}

If `e` has type `Result<T, E>` then `e?` behaves as:

- if `Ok(t)`, yield `t`,
- if `Err(err)`, early-exit from the innermost function using that function’s error model:

Mapping:

- returns `Result<_, E>` → `return Err(err);`
- `throws` → `throw err;`
- otherwise ill-formed.

### 6.6.4 Semantics for optionals {#part-6-6-4}

If `e` has type `T?` then `e?` yields `T` if nonnull, else early-exits.

Carrier rule (normative):

- `e?` is permitted only if the innermost enclosing function’s return type is an optional type.
- In that case, if `e` is `nil`, execution performs `return nil;` from that function.
- Otherwise `e?` yields the unwrapped `T`.

If the enclosing function is `throws` or returns `Result<…>`, use of `e?` is **ill‑formed** in v1.
Convert explicitly using `guard let` or the canonical `objc3.errors` helper APIs in [§6.6.6](#part-6-6-6).

### 6.6.5 Diagnostics {#part-6-6-5}

- Using `?` outside the follow-token restriction is ill-formed (fix-it: parenthesize).
- Using `?` without compatible early-exit carrier is ill-formed.

### 6.6.6 Explicit nil→error helpers (`orThrow`, `okOr`) {#part-6-6-6}

The standard library defines canonical explicit adapters for optional-to-error mapping in `objc3.errors` ([S.2.2](#s-2-2)):

- `orThrow<T>(value: T?, makeError: () -> id<Error>) throws -> T`
- `okOr<T, E: Error>(value: T?, makeError: () -> E) -> Result<T, E>`

Interaction with `try` and postfix propagation is normative:

- `orThrow(...)` is throwing. A call shall follow normal `throws` call-site rules in [§6.3.3](#part-6-3-3), including `try` where required.
- `okOr(...)` is non-throwing and yields `Result<T, E>`.
- In a `throws` function, optional nil-to-error conversion shall be explicit (for example, `try orThrow(opt, makeError)` or `guard let ... else { throw ... }`).
- In a `Result<..., E>`-returning function, `okOr(opt, makeError)?` is the canonical propagation form.
- `try` does not perform optional carrier conversion by itself; it only handles throwing evaluation.

---

## 6.7 `do` / `catch` {#part-6-7}

### 6.7.1 Grammar {#part-6-7-1}

```text
do-statement:
    'do' compound-statement catch-clauses

catch-clauses:
    catch-clause+
catch-clause:
    'catch' catch-pattern? compound-statement

catch-pattern:
    '(' type-name identifier? ')'
```

### 6.7.2 Semantics {#part-6-7-2}

- The `do` block establishes a handler for thrown errors.
- On throw, transfer to first matching catch; otherwise propagate outward.

### 6.7.3 Catch matching {#part-6-7-3}

A catch pattern `(T e)` matches if the thrown value:

- is an instance of `T` (class),
- conforms to `T` (protocol),
- or bridges to `T` via defined conversions.

A bare `catch { ... }` matches any error.

---

## 6.8 `Result<T, E>` standard type {#part-6-8}

### 6.8.1 Definition {#part-6-8-1}

The standard library shall provide a closed two-case Result type:

```text
Result<T, E> = Ok(T) | Err(E)
```

### 6.8.2 Requirements {#part-6-8-2}

- Construction and inspection shall be possible without allocation where possible.
- `Result` participates in pattern matching ([Part 5](#part-5)) and propagation (`?`).

---

## 6.9 Interoperability: NSError-out-parameter conventions {#part-6-9}

### 6.9.1 Recognizing an NSError-throwing signature {#part-6-9-1}

Eligible if:

- final parameter is `NSError **` (or equivalent),
- annotated as error-out parameter (attribute or convention),
- return type is `BOOL` or object pointer.

### 6.9.2 Standard attribute {#part-6-9-2}

Canonical attribute: `__attribute__((objc_nserror))` (see [B.4.1](#b-4-1)).

### 6.9.3 `try` lowering for NSError-bridged calls {#part-6-9-3}

Compiler shall:

- synthesize temporary error out parameter,
- call,
- on failure, throw error or generic error if missing,
- on success, return value.

---

## 6.10 Interoperability: return-code APIs {#part-6-10}

### 6.10.1 Status attribute {#part-6-10-1}

Canonical attribute: `__attribute__((objc_status_code(success: constant, error_type: Type, mapping: Function)))` (see [B.4.2](#b-4-2)).

### 6.10.2 Bridging {#part-6-10-2}

- Under `try`: throw on non-success.
- Under Result overlay: return `Err(mapped)`.

---

## 6.11 Required diagnostics {#part-6-11}

Minimum diagnostics:

- calling throwing function without `try` (error),
- `throw` outside throws (error),
- misuse of postfix `?` (error with fix-it), including using `T?` propagation in `throws`/`Result` contexts,
- `try!` without proof (warning/error policy in strict).

---

## 6.12 Open issues {#part-6-12}

- None in this part as of v0.10.

## 6.13 Future extensions (non-normative) {#part-6-13}

### 6.13.1 Typed throws {#part-6-13-1}

A future revision may introduce typed throws syntax to restrict throwable error sets.
ObjC 3.0 v1 intentionally ships only untyped `throws` and uses the reservation rules in [§6.3.7](#part-6-3-7) to preserve forward compatibility with that future work.

