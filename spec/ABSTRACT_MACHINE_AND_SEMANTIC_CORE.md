# Objective-C 3.0 - Abstract Machine and Semantic Core {#am}

_Working draft v0.11 - last updated 2026-02-23_

## AM.0 Purpose and scope {#am-0}

This document defines the unified abstract-machine rules that span:

- [Part 3](#part-3) (optionals and optional chaining),
- [Part 5](#part-5) (control-flow exits and `defer`),
- [Part 6](#part-6) (throws/try/propagation),
- [Part 7](#part-7) (async/await),
- [Part 8](#part-8) (defer and resource cleanup).

This document is normative for cross-part behavior. Part-specific rules remain normative for construct-local typing and syntax.

If two rules appear to overlap, the more specific construct rule applies, and this document defines how the rules compose.

### AM.0.1 Normative anchor map {#am-0-1}

This document composes construct-local rules from the following sections:

- [Part 3](#part-3): [§3.3.2](#part-3-3-2), [§3.3.4](#part-3-3-4), [§3.4.1](#part-3-4-1), [§3.4.2](#part-3-4-2), [§3.4.2.4](#part-3-4-2-4).
- [Part 5](#part-5): [§5.2](#part-5-2), [§5.2.4](#part-5-2-4), [§5.3.2](#part-5-3-2), [§5.4.3](#part-5-4-3).
- [Part 6](#part-6): [§6.5.3](#part-6-5-3), [§6.5.4](#part-6-5-4), [§6.6](#part-6-6), [§6.6.4](#part-6-6-4).
- [Part 7](#part-7): [§7.3](#part-7-3), [§7.6.5](#part-7-6-5), [§7.9.1](#part-7-9-1), [§7.9.2](#part-7-9-2), [§7.9.3](#part-7-9-3), [§7.9.4](#part-7-9-4).
- [Part 8](#part-8): [§8.1](#part-8-1), [§8.2.3](#part-8-2-3), [§8.3](#part-8-3), [§8.6](#part-8-6), [§8.8.3](#part-8-8-3).

## AM.1 Abstract machine state {#am-1}

For a running function/task, the abstract machine tracks:

- a lexical scope stack;
- for each scope, a scope-exit action stack (cleanup stack);
- current evaluation state for the active full-expression;
- for `async` functions, an async frame that stores values live across suspension;
- for Objective-C runtimes with autorelease semantics, the implicit autorelease pool for the current task execution slice.

### AM.1.1 Full-expression boundary {#am-1-1}

A full-expression boundary is the point where temporaries that are not lifetime-extended may be destroyed.

### AM.1.2 Scope-exit action stack {#am-1-2}

A scope-exit action stack contains actions registered by:

- executed `defer` statements ([Part 5](#part-5) [§5.2](#part-5-2));
- successful initialization of cleanup/resource locals ([Part 8](#part-8) [§8.3](#part-8-3)).

Actions execute only when the scope exits; mere suspension at `await` is not scope exit.

## AM.2 Expression evaluation order contract {#am-2}

### AM.2.1 Baseline preserved {#am-2-1}

Objective-C 3.0 does not globally replace baseline C/Objective-C evaluation-order rules.

Unless a rule in this specification explicitly adds ordering, ordinary C/Objective-C expression ordering remains baseline behavior.

In particular, Objective-C 3.0 does not add a universal left-to-right argument evaluation guarantee for ordinary C calls or ordinary Objective-C message sends.

### AM.2.2 Additional ObjC 3.0 guarantees (normative delta) {#am-2-2}

Objective-C 3.0 adds the following ordering/single-evaluation guarantees relative to the baseline:

| Construct | Added guarantee | Primary source |
| --- | --- | --- |
| `x?.p` | `x` shall be evaluated exactly once; if `x == nil`, result is `nil` and property access is not performed. | [Part 3](#part-3) [§3.4.1.2](#part-3-4-1-2) |
| `[receiver? sel:arg1 other:arg2]` | `receiver` shall be evaluated exactly once; if `receiver == nil`, argument expressions shall not be evaluated and no send occurs. | [Part 3](#part-3) [§3.4.2.4](#part-3-4-2-4) |
| `a ?? b` | `a` shall be evaluated first and exactly once; `b` shall be evaluated only if `a` is `nil`. | [Part 3](#part-3) [§3.3.4.2](#part-3-3-4-2) |
| `e?` (postfix propagation) | `e` shall be evaluated exactly once before deciding unwrap vs early exit. | [Part 6](#part-6) [§6.6](#part-6-6) |
| `if let` / `guard let` binding lists | Binding expressions shall be evaluated left-to-right. | [Part 3](#part-3) [§3.3.2.2](#part-3-3-2-2) |
| `guard` condition lists | Conditions shall be evaluated left-to-right. | [Part 5](#part-5) [§5.3.2](#part-5-3-2) |
| `match (expr)` | `expr` shall be evaluated exactly once; case tests are top-to-bottom. | [Part 5](#part-5) [§5.4.3](#part-5-4-3) |
| Block capture list `[cap1, cap2, ...]` | Capture items shall be evaluated left-to-right at block creation time. | [Part 8](#part-8) [§8.8.3](#part-8-8-3) |

No other new global expression-order guarantees are introduced in v1.

### AM.2.3 Suspension sequencing boundary {#am-2-3}

For `await e`:

- user-visible side effects sequenced before the suspension point shall occur before suspension;
- evaluation that is sequenced after `await` shall not occur until resumption;
- lowering shall not duplicate user subexpression evaluation solely due to suspend/resume transformation.

### AM.2.4 Where ObjC 3.0 changes baseline guarantees {#am-2-4}

Relative to baseline C/Objective-C, ObjC 3.0 adds only the explicit guarantees listed in [AM.2.2](#am-2-2), plus cross-construct composition guarantees in [AM.4.4](#am-4-4), [AM.5](#am-5), and [AM.6](#am-6).

Outside those sections, evaluation order and sequencing remain baseline/implementation-defined as in ordinary C/Objective-C.

## AM.3 Temporaries and lifetime rules {#am-3}

### AM.3.1 Base lifetime rule {#am-3-1}

A temporary created during expression evaluation shall remain valid until at least the end of its containing full-expression, unless a rule below extends it further.

### AM.3.2 Lifetime extension points {#am-3-2}

A temporary or local lifetime is extended when required by:

- explicit lifetime controls (`withLifetime`, `keepAlive`, precise-lifetime annotations in [Part 8](#part-8));
- capture into storage that outlives the full-expression (for example block captures);
- async suspension requirements in [AM.3.3](#am-3-3).

### AM.3.3 Values live across `await` {#am-3-3}

Any value needed after a potentially suspending `await` shall be materialized in async-frame storage and kept alive across suspension.

For ARC-managed values:

- the implementation shall retain frame-stored strong values as needed before suspension; and
- shall release them when they become dead (or when the async frame is destroyed by return, throw, or cancellation unwind).

Values proven dead before suspension may be released before suspension unless prohibited by precise-lifetime rules.

### AM.3.4 Hidden temporaries in short-circuit forms {#am-3-4}

Hidden temporaries used to implement `?.`, optional send, `??`, `try?`, and postfix `?` shall:

- preserve exactly-once evaluation guarantees; and
- not outlive the enclosing full-expression unless required by [AM.3.3](#am-3-3) or explicit lifetime controls.

## AM.4 Cleanup stack behavior {#am-4}

### AM.4.1 Registration {#am-4-1}

Within a lexical scope, scope-exit actions are registered when:

- a `defer` statement executes; or
- a cleanup/resource local completes successful initialization.

Registration is dynamic: code paths not executed do not register actions.

### AM.4.2 Exit triggers {#am-4-2}

Scope-exit actions run when leaving the scope via:

- fallthrough to scope end;
- `return`, `break`, `continue`, or `goto` leaving the scope;
- structured error propagation (`throw`, `try` propagation, postfix `?` early exit);
- cancellation unwind paths defined by [Part 7](#part-7);
- exception unwinding paths that run language cleanups.

Non-local transfers that bypass cleanups (for example `longjmp` across cleanup scopes) are outside guarantees and are undefined unless the platform ABI explicitly guarantees cleanup execution.

### AM.4.3 Per-scope execution order {#am-4-3}

For a scope `S`, registered scope-exit actions shall execute in reverse registration order (LIFO).

For exits that leave multiple nested scopes, scopes are processed from innermost to outermost, applying LIFO within each scope.

### AM.4.4 Ordering with implicit ARC releases {#am-4-4}

In each exiting scope:

1. registered scope-exit actions execute first (LIFO);
2. implicit ARC releases for strong locals in that scope execute after all scope-exit actions of that scope.

Relative order among implicit ARC releases is baseline/implementation-defined unless specified elsewhere, but all such releases are sequenced after scope-exit actions in the same scope.

## AM.5 Unified scope-exit algorithm {#am-5}

When control leaves scope `S`, a conforming implementation shall behave as if by:

1. Pop and execute each registered scope-exit action in `S` (LIFO).
2. Perform implicit scope-final ARC releases for `S`.
3. Continue unwinding to the next enclosing scope if control is still exiting outward.

This algorithm applies uniformly for normal return, throw paths, postfix-propagation early exits, and cancellation unwind.

## AM.6 `await` interaction rules {#am-6}

### AM.6.1 `await` and scope-exit actions {#am-6-1}

Encountering `await` is not scope exit.

Therefore, suspension at `await` shall not by itself execute:

- `defer` actions;
- resource cleanup actions;
- scope-final implicit ARC releases.

Those actions execute only when their lexical scope actually exits.

### AM.6.2 `await` and ARC lifetime boundaries {#am-6-2}

At a potentially suspending `await`:

- values needed after resumption shall be preserved in the async frame ([AM.3.3](#am-3-3));
- values dead before suspension may be released before suspension;
- precise-lifetime rules still prohibit early release when such annotations/constructs apply.

When a scope eventually exits, defer/resource cleanup ordering relative to ARC remains governed by [AM.4.4](#am-4-4).

### AM.6.3 `await` and autorelease pools {#am-6-3}

On Objective-C runtimes with autorelease semantics:

- each task execution slice (resume -> next suspension or completion) shall execute inside an implicit autorelease pool;
- if `await` actually suspends, the current slice pool shall be drained before suspension;
- pool drain shall also occur at task completion (normal return, thrown error, or cancellation unwind).

If an `await` completes synchronously without suspension, draining at that point is permitted but not required.

Autorelease-pool draining is not scope exit and shall not trigger scope-exit actions by itself.

### AM.6.4 `await` with `try`/`throws` {#am-6-4}

`try await e` is the canonical composed form for `async throws` calls.

If the awaited operation throws:

- the throw is observed at the `await` expression;
- propagation follows [Part 6](#part-6) rules; and
- all exited scopes run scope-exit actions and ARC releases per [AM.5](#am-5).

For `try? await e`:

- success yields the value;
- throw yields `nil`;
- cleanup ordering remains unchanged.

For `try! await e`:

- success yields the value;
- throw traps as specified in [Part 6](#part-6); no additional post-trap ordering guarantees are required.

### AM.6.5 `await` with postfix propagation `?` {#am-6-5}

For forms equivalent to `(await e)?`:

1. evaluate `await e` first (including any suspension/resumption);
2. apply postfix propagation semantics to the resulting carrier value.

If propagation triggers early exit (`return nil`, `return Err(...)`, or `throw ...`), scope exit proceeds under [AM.5](#am-5).

Carrier restrictions from [Part 6](#part-6) remain in force:

- optional propagation is valid only in optional-returning functions;
- using optional propagation to implicitly map into `throws` or `Result` contexts is ill-formed in v1.

### AM.6.6 `await` with optional chaining and optional send {#am-6-6}

For `await x?.p`:

- `x` is evaluated exactly once;
- if `x == nil`, result is `nil` and no member access occurs, therefore no suspension occurs on that path;
- if `x != nil`, normal member access proceeds; suspension is possible only if that access is potentially suspending.

For `await [receiver? sel:arg1 other:arg2]`:

- `receiver` is evaluated exactly once;
- if `receiver == nil`, arguments are not evaluated, no send occurs, and no suspension occurs on that path;
- if `receiver != nil`, arguments are evaluated using ordinary-send ordering, then the send occurs; suspension is possible only on this non-`nil` path.

Applying `await` to a path proven non-suspending is permitted and may be diagnosed as unnecessary in strict modes.

### AM.6.7 Composed `await`/`try`/propagation ordering {#am-6-7}

For composed forms such as `return (try? await x?.f())?;` in an optional-returning `async` function, implementations shall preserve the following abstract order:

1. Evaluate `x` exactly once.
2. Apply optional chaining/send short-circuit rules ([AM.2.2](#am-2-2)):
   - if `x == nil`, yield `nil` for the chained/send expression, skip chained argument/member evaluation, and do not suspend on that path;
   - if `x != nil`, evaluate the non-`nil` path normally (including any potentially suspending operation).
3. Apply `await` to the selected non-`nil` path evaluation (if any), with possible suspension/resumption.
4. Apply `try`/`try?`/`try!` semantics ([AM.6.4](#am-6-4)).
5. Apply postfix propagation `?` to the resulting carrier value ([AM.6.5](#am-6-5)).
6. If propagation early-exits, run scope-exit and ARC ordering per [AM.5](#am-5).

Replacing `try?` with `try` preserves steps 1-3; if a throw occurs at step 4, propagation at step 5 is not reached and unwind follows [AM.5](#am-5).

## AM.7 Conformance test matrix (normative minimum) {#am-7}

A conforming implementation shall provide tests equivalent in coverage to the following matrix.

### AM.7.1 Matrix {#am-7-1}

| ID | Combination under test | Required outcome |
| --- | --- | --- |
| AM-T01 | `defer` + normal return | Defers execute once, in LIFO order, before scope-final ARC releases. |
| AM-T02 | `defer` + cleanup/resource local + return | Defer/resource actions follow per-scope LIFO registration order; all run before scope-final ARC releases. |
| AM-T03 | `defer` + `throw` | Throw path runs scope-exit actions and ARC releases for all exited scopes (inner to outer). |
| AM-T04 | `defer` + `try await` where awaited call throws | Error propagates from `await`; defer/cleanup actions still run exactly once during unwind. |
| AM-T05 | `try? await` | Throwing awaited call yields `nil`; no thrown error escapes; cleanup ordering remains per AM.5. |
| AM-T06 | `try! await` throwing path | Program traps on thrown error; implementation is not required to provide post-trap cleanup guarantees. |
| AM-T07 | `(await optionalProducer())?` in optional-returning function | Operand is evaluated once; on `nil`, function early-returns `nil` and runs scope-exit actions once. |
| AM-T08 | `(await optionalProducer())?` in `throws` function | Compile-time error: optional postfix propagation is not a valid carrier in `throws` context. |
| AM-T09 | `await x?.p` with `x == nil` path | `x` evaluated once; result `nil`; no member access and no suspension on nil path. |
| AM-T10 | `await [r? m:sideEffect()]` with `r == nil` path | `r` evaluated once; `sideEffect()` not evaluated; no send and no suspension on nil path. |
| AM-T11 | `await [r? m:sideEffect()]` with `r != nil` path | `sideEffect()` evaluated in ordinary-send order; send occurs; suspension permitted only on this path. |
| AM-T12 | `defer` around await cancellation point | If cancellation causes unwind/error at `await`, defer/cleanup actions execute exactly once before task/frame teardown completes. |
| AM-T13 | Nested scopes with `defer` + postfix `?` early exit | Early exit unwinds innermost-to-outermost scopes; each scope uses LIFO action order. |
| AM-T14 | `await` in scope without exit | Suspend/resume alone does not run scope-exit actions or scope-final ARC releases. |
| AM-T15 | `defer` + `return (try? await x?.f())?;` with `x == nil` path | `x` evaluated once; `f`/arguments not evaluated; no suspension on nil path; postfix propagation early-exits and runs cleanup ordering per AM.5. |
| AM-T16 | `defer` + `return (try? await x?.f())?;` with `x != nil`, `f` throws | Non-`nil` path may suspend; throw is converted to `nil` by `try?`; postfix propagation early-exits; defer/cleanup actions run once before ARC scope-final releases. |
| AM-T17 | `defer` + `return (try await x?.f())?;` with `x != nil`, `f` throws | Throw propagates from `try await`; postfix propagation is not applied on throwing path; exited scopes still run cleanup ordering per AM.5. |
| AM-T18 | `defer` + `return (try await x?.f())?;` with `x == nil` path | Nil short-circuit occurs before suspension; no throw occurs on nil path; postfix propagation early-exits `nil` and executes scope cleanups once. |
| AM-T19 | `defer` + `return (try? await [r? m:sideEffect()])?;` with `r == nil` path | `r` evaluated once; `sideEffect()` not evaluated; no send and no suspension on nil path; early exit still executes defer/cleanup in AM.5 order. |

### AM.7.2 Static diagnostics required by matrix {#am-7-2}

At minimum, matrix coverage shall include diagnostics for:

- potentially suspending operations used without `await`;
- invalid optional postfix propagation carrier context;
- optional chaining/send restrictions from [Part 3](#part-3) (including scalar/struct restriction).

