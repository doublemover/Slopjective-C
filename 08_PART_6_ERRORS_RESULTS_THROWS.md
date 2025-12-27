# Part 6 — Errors: Result, throws, and Propagation

## 6.1 Purpose
This part defines a unified error-handling model that works with:
- NSError-based Cocoa APIs,
- return-code-based system APIs (kern_return_t / IOReturn style),
- optional-based failure patterns.

The goal is to allow new APIs to be written with modern ergonomics without breaking existing ABIs.

## 6.2 `Result<T, E>`
### 6.2.1 Definition
Objective‑C 3.0 defines a standard `Result<T, E>` type representing either:
- `Ok(T)` success
- `Err(E)` failure

The standard library shall provide:
- construction, inspection, and mapping operations
- pattern matching compatibility (Part 5)

### 6.2.2 Representation
The representation may be:
- a tagged value type (preferred for performance), or
- an object type (allowed when ABI constraints require).

The spec shall require:
- predictable layout within the compilation model,
- no hidden autoreleases that surprise callers.

> Open issue: decide standard library ABI vs header-only representation; ensure C/ObjC++ usability.

## 6.3 `throws` effect
### 6.3.1 Syntax
Objective‑C 3.0 introduces a `throws` effect on methods/functions.

Provisional syntax options:
- `- (T)foo:(A)a throws;`
- `- (T)foo:(A)a @throws;`

### 6.3.2 Semantics
A `throws` function either:
- returns normally with a value, or
- exits by throwing an error value of type `Error` (or a specific error type if constrained).

### 6.3.3 ABI lowering
A conforming implementation shall define lowering to existing ABIs:
- For Objective‑C methods, lowering may use:
  - an implicit out-parameter (NSError-like),
  - a hidden return aggregate,
  - or a runtime exception-like mechanism *only if explicitly enabled and safe*.

This draft recommends lowering to out-parameter / hidden return for compatibility, not to ObjC exceptions.

## 6.4 Propagation operator `?`
### 6.4.1 Purpose
`?` enables early-exit propagation for common error carriers:
- `Result`
- optionals
- (optionally) NSError/return-code wrappers

### 6.4.2 Syntax
Postfix:

```objc
value = expr?;
```

### 6.4.3 Semantics
If `expr` has type:
- `Result<T, E>`: if `Ok(t)` yield `t`; if `Err(e)` return/throw `e` according to the surrounding function’s error model.
- `T?`: if nonnull yield `T`; otherwise early-return `nil` or an error (depending on surrounding function’s declared effects).

The surrounding context must declare an appropriate early-exit carrier:
- function returns `Result<..., E>` or `throws E`, etc.

If no appropriate carrier exists, the code is ill-formed.

### 6.4.4 Diagnostics
In strict mode:
- propagation from ambiguous carriers is an error unless explicitly disambiguated.
- mixing `throws` and `Result` requires explicit bridging.

## 6.5 Bridging existing patterns
### 6.5.1 NSError out-parameter bridging
Objective‑C 3.0 standardizes annotations that declare an NSError-style method as throwable, enabling callers to use `try`/`?` style syntax (if `try` is included; otherwise `?` handles it).

For example:
- A method returning BOOL with trailing `NSError **` may be imported as `throws`.

### 6.5.2 Return-code bridging
Objective‑C 3.0 defines a standard wrapper for return-code APIs:

- `ReturnCode<E, SuccessPayload>` or an attribute that declares:
  - “zero means success; nonzero maps to E”
  - optional out-params become payload

This allows:
- `payload = call(...) ?;` style propagation.

> Open issue: decide whether the wrapper is a standard library type or purely attribute-driven lowering.

## 6.6 Error wrapping and chaining
Objective‑C 3.0 defines a standard facility for error chaining:
- an error may wrap an underlying cause and attach typed context.
- bridging to NSError uses `NSUnderlyingErrorKey` by default.

In strict mode, wrapping operations should preserve underlying error provenance.

## 6.7 Examples
### 6.7.1 Result + propagation
```objc
Result<Data*, MyError> fetch(...) {
  Data* d = readFile(path)?;
  return Ok(d);
}
```

### 6.7.2 throws + propagation
```objc
Data* fetch(...) throws {
  Data* d = readFile(path)?; // propagates thrown error
  return d;
}
```

## 6.8 Open issues
- Exact surface spelling: `try` keyword vs relying solely on `?`.
- Standard error protocol/type.
- How to interoperate with Objective‑C exceptions (likely: do not use them for `throws`).
