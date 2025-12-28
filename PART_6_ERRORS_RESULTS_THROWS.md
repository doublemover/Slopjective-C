# Part 6 — Errors: Result, throws, try, and Propagation {#part-6}
_Working draft v0.10 — last updated 2025-12-28_

## 6.0 Overview {#part-6-0}

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

An *error value* is any Objective‑C object value that conforms to `Error`.

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
- diagnose effect mismatches on redeclaration/import ([C.2](#c-2)),
- and provide a stable calling convention for throwing functions/methods.

The recommended calling convention is the *trailing error-out parameter* described in [C.4](#c-4).

> Note: This draft intentionally chooses an error-out convention

**Reflection note (non-normative):** v1 does not require encoding `throws` in Objective‑C runtime type encodings. Toolchains may provide extended metadata as an extension.
 rather than stack unwinding, to keep interoperability with NSError/return-code APIs straightforward and to align with existing Objective‑C runtime practices.


Throwing is part of a function’s type.

- `R (^)(Args) throws` is a throwing block type.
- `R (^)(Args)` is non-throwing.

A throwing function value cannot be assigned to a non-throwing function type without an explicit adapter.


---

## 6.4 `throw` statement {#part-6-4}

### 6.4.1 Grammar {#part-6-4-1}
```text
throw-statement:
    'throw' expression ';'
```

### 6.4.2 Static semantics {#part-6-4-2}
A `throw` statement is permitted only within a `throws` function or within a `catch` block.

The thrown expression must be convertible to:
- `id<Error>` for untyped throws, or

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
Convert explicitly using `guard let … else { throw … }` or an explicit helper such as `ok_or(...)`.

### 6.6.5 Diagnostics {#part-6-6-5}
- Using `?` outside the follow-token restriction is ill-formed (fix-it: parenthesize).
- Using `?` without compatible early-exit carrier is ill-formed.

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
- Whether to introduce a standard attribute or syntax for “never throws” in generic contexts (mostly redundant with absence of `throws`).
- Whether to standardize a nil→error mapping helper (e.g., `orThrow(...)`) in the standard library rather than as language sugar.
- Whether to add typed throws in a future revision (future extension).


## 6.13 Future extensions (non-normative) {#part-6-13}

### 6.13.1 Typed throws {#part-6-13-1}
A future revision may introduce typed throws syntax (e.g., `throws(E)`) to restrict the set of throwable error types.
This is explicitly **not** part of Objective‑C 3.0 v1 to keep the core error model simple and interoperable with NSError and return-code APIs.
