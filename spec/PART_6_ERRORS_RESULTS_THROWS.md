# Part 6 — Errors: Result, throws, try, and Propagation {#part-6}

_Working draft v0.11 — last updated 2026-02-23_

_Normative baseline references used in this part: [NR-C18](#part-0-2-1), [NR-LLVM-OBJC](#part-0-2-1), [NR-BLOCKS-ABI](#part-0-2-1), and where applicable [NR-OBJC-RUNTIME](#part-0-2-1)/[NR-ABI-PLATFORM](#part-0-2-1)._

## 6.0 Overview {#part-6-0}

### v0.10 resolved decisions {#part-6-v0-10-resolved-decisions}

- ObjC 3.0 v1 does not add a dedicated `never throws` marker; absence of `throws` is the canonical non-throwing form.
- Generic non-throwing requirements are expressed using non-throwing function/block types, not a new keyword or attribute.
- Nil-to-error mapping is explicit and library-defined via canonical `objc3.errors` helpers (`orThrow` and `okOr`), not language sugar.
- Typed throws is an implemented bounded single-payload effect surface: a single `throws(E)` payload is preserved in source/interface metadata, lowered through the hidden error-out ABI, executed for the checked direct-call, runtime-dispatch message-send, catch/bridge, and `try?` paths, and invalid payload shapes, incompatible catches, unsupported foreign carriers, async propagation, and silent erasure remain canonical fail-closed diagnostics. A generalized typed-error ABI is not a v1 claim.

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

- Bare `throws` is **untyped** in v1: thrown values are `id<Error>`.
- Typed throws syntax `throws(E)` is admitted as a bounded single-payload effect with hidden error-out ABI lowering; exact typed catches and policy-backed `id<Error>` bridge catches are the only admitted catch compatibility records until a broader typed-error ABI is specified.

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

## M267 current implementation semantic boundary

Current implementation status (`M267-B001`):

- the compiler now publishes one truthful Part 6 semantic packet at
  `frontend.pipeline.semantic_surface.objc_part6_error_semantic_model`
- currently live semantic carriage:
  - `throws` declarations
  - result-like carrier profiles
  - `NSError` bridging profiles
  - canonical `objc_nserror` / `objc_status_code(...)` bridge markers
- currently deferred runnable behavior:
  - postfix propagation
  - status-to-error execution
  - native thrown-error ABI
- older throws/unwind summaries are still emitted only as placeholder sema packets;
  they are not yet the runnable Part 6 propagation model

Current implementation status (`M267-B002`):

- the compiler now also publishes one truthful Part 6 packet at
  `frontend.pipeline.semantic_surface.objc_part6_try_do_catch_semantics`
- currently live source-only semantics:
  - `try`, `try?`, and `try!`
  - `throw`
  - `do/catch`
  - propagating-try context legality
  - throwing/bridged operand legality
  - catch-order legality
- currently deferred runnable behavior:
  - native IR/object/executable lowering for `try`, `throw`, and `do/catch`
  - runnable catch transfer
  - postfix propagation
  - status-to-error execution
  - native thrown-error ABI

Current implementation status (`M267-B003`):

- the compiler now also publishes one truthful Part 6 packet at
  `frontend.pipeline.semantic_surface.objc_part6_error_bridge_legality`
- currently live source-only bridge legality:
  - canonical `objc_nserror` and `objc_status_code(...)` marker validation
  - deterministic out-parameter, return-shape, `error_type`, and mapping checks
  - only semantically valid bridge call surfaces qualify for `try`
  - unsupported bridge combinations fail closed semantically
- currently deferred runnable behavior:
  - native status-to-error execution
  - runnable bridge lowering/runtime support
  - native thrown-error ABI

Current implementation status (`M267-C001`):

- lane C first froze the lowering boundary packet at
  `frontend.pipeline.semantic_surface.objc_part6_throws_abi_propagation_lowering`
- emitted IR now carries:
  - `; part6_throws_abi_propagation_lowering = ...`
  - `!objc3.objc_part6_throws_abi_propagation_lowering = !{!87}`
- `M267-C002` is the next issue.

Current implementation status (`M267-C002`):

- the compiler now also publishes one truthful Part 6 lowering packet at
  `frontend.pipeline.semantic_surface.objc_part6_throws_abi_propagation_lowering`
- this packet now carries:
  - hidden error-out ABI lowering
  - native `throw` propagation
  - `try`, `try?`, and `try!` lowering
  - `do/catch` dispatch
  - status-to-`NSError` bridge propagation
- emitted IR now carries:
  - `; part6_throws_abi_propagation_lowering = ...`
  - `!objc3.objc_part6_throws_abi_propagation_lowering = !{!87}`
  - `ready_for_runtime_execution=true`
- currently deferred runnable behavior:
  - separate-compilation replay completion
  - broader cross-module preservation
  - generalized native thrown-error object ABI

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

### 6.3.7 Bounded single-payload typed throws {#part-6-3-7}

Typed throws is implemented for a single payload in ObjC 3.0 v1 and lowers
through the same private hidden error-out path used by untyped `throws`. The
#8233 compiler contract owns the parser payload shape, AST preservation,
textual-interface metadata, exact effect identity, direct-call and
runtime-dispatch error paths, catch/bridge policy, `try?` optionalization, and
fail-closed boundaries that prevent silent erasure into bare `throws`.

Source syntax:

- `throws(type-name)` is admitted as a typed effect payload.
- `throws(type-name, ...)`, `throws()`, malformed payloads, and non-type payloads
  are rejected.

v1 parser and diagnostics requirements:

- A declaration using exactly one source type payload in `throws(` ... `)` shall
  preserve the canonical payload spelling in the frontend source model.
- Parser-owned typed-throws rejections cover empty payloads (`throws()`),
  multi-payload spellings (`throws(E1, E2)`), malformed unclosed payloads, and
  non-type payloads as fail-closed `O3P182` cases.
- A compiler shall not reinterpret `throws(E)` as bare `throws`, and shall not
  silently erase the parenthesized payload.
- A fix-it may suggest replacing an invalid `throws(...)` shape with bare
  `throws` only when that explicitly drops the typed payload.

Metadata slots for module/interface exchange:

- `throws_kind`: enum slot. v1 producers may emit `none`, `untyped`, or `typed`.
- `typed_payload_arity`: unsigned slot. v1 typed throws requires `1`; untyped
  and non-throwing declarations require `0`.
- `declared_error_type`: canonical source payload spelling for typed throws,
  `id<Error>` for bare throws, and empty for non-throwing declarations.
- A declaration with bare `throws` has effect record `throws_kind=untyped`,
  `typed_payload_arity=0`, and the v1 declared error carrier `id<Error>`.
- A declaration with `throws(E)` has effect record `throws_kind=typed`,
  `typed_payload_arity=1`, and `declared_error_type=E`.
- A declaration without `throws` has effect record `throws_kind=none` and
  `typed_payload_arity=0`.
- Semantic callable metadata shall derive an effect signature key from those
  slots. The v1 keys are `throws:none`, `throws:untyped:id<Error>`, and
  `throws:typed:<declared_error_type>`.
- Protocol conformance, duplicate protocol requirement detection, and callable
  compatibility checks shall compare the typed throws effect key exactly. A
  `throws(E)` requirement is not satisfied by bare `throws`, and a `throws(E1)`
  requirement is not satisfied by `throws(E2)`, until a later version specifies
  typed error variance or bridge conversions.
- `do/catch` compatibility for a known single-payload typed throw shall admit an
  exact typed catch payload and the explicit `id<Error>` bridge catch. A typed
  catch with a different payload is rejected, and unsupported foreign carriers
  are rejected fail-closed rather than treated as catch-all or untyped bridge
  matches.

Version and lowering constraints:

- A v1 consumer that imports typed-throws metadata with erased payload spelling,
  zero typed payload arity, a mismatched `declared_error_type`, or a missing
  typed payload lowering claim shall emit an incompatibility diagnostic and
  reject that declaration for v1 conformance.
- A producer targeting v1 shall mark single-payload typed throws as
  `typed-error-out-abi`; this reuses the private error slot ABI while preserving
  the typed effect key in semantic/interface metadata.
- Untyped and typed declarations are effect-signature-distinct across module
  boundaries unless a later version explicitly defines a conversion rule.

---

## 6.4 `throw` statement {#part-6-4}

### 6.4.1 Grammar {#part-6-4-1}

```text
throw-statement:
    'throw' expression ';'
```

### 6.4.2 Static semantics {#part-6-4-2}

A `throw` statement is permitted only within a `throws` function or within a `catch` block.

In v1, bare `throws` propagates through the untyped `id<Error>` carrier.
Typed throws forms (for example, `throws(E)`) preserve their source payload in
the frontend/interface contract and use the same hidden error-out propagation
ABI; the payload type remains part of the callable effect signature and is not
erased to bare `throws`.

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

For a known single-payload `throws(E)` source, v1 catch compatibility is
fail-closed:

- `catch (E e)` is the exact typed catch match.
- `catch (id<Error> e)` is admitted only as the explicit bridge-to-`id<Error>`
  policy.
- `catch (Other e)` is rejected unless `Other` is the exact typed payload or the
  explicit bridge carrier.
- unsupported foreign carriers, such as C++ exception carrier spellings, do not
  match and must be diagnosed rather than lowered as catch-all handlers.

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

Typed throws syntax may later restrict throwable error sets more broadly.
ObjC 3.0 v1 intentionally ships only the hidden single-payload error-out ABI plus
catch/bridge policy slice; [§6.3.7](#part-6-3-7) defines the bounded
typed-throws payload contract and the broader future typed-error ABI boundary.

## M267 current implementation closeout note

`M267-E002` closes the current runnable Part 6 slice over the already landed
source closure, semantic model, lowering, runtime helper, live runtime, and
cross-module preservation tranches.

The current truthful implemented slice is:

- reserved and semantically validated `try`, `throw`, and `do/catch`
- Result/error-carrier and NSError/status bridge semantics for the supported
  narrow executable path
- emitted replay/import/link-plan artifacts that preserve the implemented Part 6
  boundary across separate compilation and cross-module builds

The current truthful non-goals remain:

- no generalized foreign exception transport
- no broader typed-throws model
- no additional Part 6 runtime claim beyond the evidenced runnable slice

Canonical closeout evidence:

- `tmp/reports/m267/M267-E002/runnable_throws_result_and_bridge_matrix_summary.json`
- next issue: `M268-A001`
