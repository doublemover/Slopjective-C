# Objective C 3.0   Abstract Machine and Semantic Core {#am}

_Working draft v0.11   last updated 2026 02 23_

## AM.0 Purpose and scope {#am 0}

This document defines the unified abstract machine rules that span:

  [Part 3](#part 3) (optionals and optional chaining),
  [Part 5](#part 5) (control flow exits and `defer`),
  [Part 6](#part 6) (throws/try/propagation),
  [Part 7](#part 7) (async/await),
  [Part 8](#part 8) (defer and resource cleanup).

This document is normative for cross part behavior. Part specific rules remain normative for construct local typing and syntax.

If two rules appear to overlap, the more specific construct rule applies, and this document defines how the rules compose.

Current implementation note:

  The live compiler does not yet implement the full Part 3 execution surface.
  Today it freezes protocol `@optional` partitions plus object pointer
  nullability/generic suffix carriers as source level frontend behavior.
  The frontend now also admits parser owned optional binding, optional send,
  nil coalescing, and typed key path source forms.
  Lane B now carries live optional flow semantics for optional bindings,
  nil comparison refinement, nil coalescing, and ordinary vs optional send
  legality.
  Lane C now carries the first lowering owned Part 3 packet for optional
  bindings, optional sends, and nil coalescing.
  Optional sends now lower natively with single evaluation nil short circuit
  behavior; selector arguments are not evaluated on the nil arm.
  Ordinary sends still fail closed for nullable receivers unless they have
  been proven nonnull, and `guard let` / `guard var` `else` bodies must exit
  the current scope.
  The current Part 5 frontend boundary is now explicit: `guard` bindings and
  `switch` / `case` remain the admitted control flow source surface, while
  `defer` and `match` are reserved fail closed keywords until the runnable
  `M266` lowering/runtime work lands.
  Typed key path roots now fail closed unless they resolve to `self`, a known
  class type, or an ObjC reference compatible identifier; class root
  single component paths fail closed unless the component names a readable
  property on that root.
  Generic Objective C method declarations written as `  <T> ...` remain
  reserved in v1 and now diagnose explicitly.
  Optional member access written as `?.` now lowers through the same live
  nil short circuit path as bracketed optional sends.
  Typed key path literals remain truthful source/sema surfaces, and the
  validated single component subset now lowers natively into retained
  descriptor handles.
  The current runtime/helper boundary for that subset still stays narrow:
  optional sends execute through selector lookup plus dispatch, while validated
  single component typed key path handles now feed a private runtime registry
  and probe helpers; full typed key path evaluation remains deferred to later
  runtime work.
  Multi component typed key path member chains, full typed key path
  application/runtime behavior, and the broader Part 3 surface remain future
  work.

### AM.0.1 Normative anchor map {#am 0 1}

This document composes construct local rules from the following sections:

  [Part 3](#part 3): [§3.3.2](#part 3 3 2), [§3.3.4](#part 3 3 4), [§3.4.1](#part 3 4 1), [§3.4.2](#part 3 4 2), [§3.4.2.4](#part 3 4 2 4).
  [Part 5](#part 5): [§5.2](#part 5 2), [§5.2.4](#part 5 2 4), [§5.3.2](#part 5 3 2), [§5.4.3](#part 5 4 3).
  [Part 6](#part 6): [§6.5.3](#part 6 5 3), [§6.5.4](#part 6 5 4), [§6.6](#part 6 6), [§6.6.4](#part 6 6 4).
  [Part 7](#part 7): [§7.3](#part 7 3), [§7.6.5](#part 7 6 5), [§7.9.1](#part 7 9 1), [§7.9.2](#part 7 9 2), [§7.9.3](#part 7 9 3), [§7.9.4](#part 7 9 4).
  [Part 8](#part 8): [§8.1](#part 8 1), [§8.2.3](#part 8 2 3), [§8.3](#part 8 3), [§8.6](#part 8 6), [§8.8.3](#part 8 8 3).

## AM.1 Abstract machine state {#am 1}

For a running function/task, the abstract machine tracks:

  a lexical scope stack;
  for each scope, a scope exit action stack (cleanup stack);
  current evaluation state for the active full expression;
  for `async` functions, an async frame that stores values live across suspension;
  for Objective C runtimes with autorelease semantics, the implicit autorelease pool for the current task execution slice.

### AM.1.1 Full expression boundary {#am 1 1}

A full expression boundary is the point where temporaries that are not lifetime extended may be destroyed.

### AM.1.2 Scope exit action stack {#am 1 2}

A scope exit action stack contains actions registered by:

  executed `defer` statements ([Part 5](#part 5) [§5.2](#part 5 2));
  successful initialization of cleanup/resource locals ([Part 8](#part 8) [§8.3](#part 8 3)).

Actions execute only when the scope exits; mere suspension at `await` is not scope exit.

## AM.2 Expression evaluation order contract {#am 2}

### AM.2.1 Baseline preserved {#am 2 1}

Objective C 3.0 does not globally replace baseline C/Objective C evaluation order rules.

Unless a rule in this specification explicitly adds ordering, ordinary C/Objective C expression ordering remains baseline behavior.

In particular, Objective C 3.0 does not add a universal left to right argument evaluation guarantee for ordinary C calls or ordinary Objective C message sends.

### AM.2.2 Additional ObjC 3.0 guarantees (normative delta) {#am 2 2}

Objective C 3.0 adds the following ordering/single evaluation guarantees relative to the baseline:

| Construct                              | Added guarantee                                                                                                                   | Primary source                              |
|                                        |                                                                                                                                   |                                             |
| `x?.p`                                 | `x` shall be evaluated exactly once; if `x == nil`, result is `nil` and property access is not performed.                         | [Part 3](#part 3) [§3.4.1.2](#part 3 4 1 2) |
| `[receiver? sel:arg1 other:arg2]`      | `receiver` shall be evaluated exactly once; if `receiver == nil`, argument expressions shall not be evaluated and no send occurs. | [Part 3](#part 3) [§3.4.2.4](#part 3 4 2 4) |
| `a ?? b`                               | `a` shall be evaluated first and exactly once; `b` shall be evaluated only if `a` is `nil`.                                       | [Part 3](#part 3) [§3.3.4.2](#part 3 3 4 2) |
| `e?` (postfix propagation)             | `e` shall be evaluated exactly once before deciding unwrap vs early exit.                                                         | [Part 6](#part 6) [§6.6](#part 6 6)         |
| `if let` / `guard let` binding lists   | Binding expressions shall be evaluated left to right.                                                                             | [Part 3](#part 3) [§3.3.2.2](#part 3 3 2 2) |
| `guard` condition lists                | Conditions shall be evaluated left to right.                                                                                      | [Part 5](#part 5) [§5.3.2](#part 5 3 2)     |
| `match (expr)`                         | `expr` shall be evaluated exactly once; case tests are top to bottom.                                                             | [Part 5](#part 5) [§5.4.3](#part 5 4 3)     |
| Block capture list `[cap1, cap2, ...]` | Capture items shall be evaluated left to right at block creation time.                                                            | [Part 8](#part 8) [§8.8.3](#part 8 8 3)     |

No other new global expression order guarantees are introduced in v1.

### AM.2.3 Suspension sequencing boundary {#am 2 3}

For `await e`:

  user visible side effects sequenced before the suspension point shall occur before suspension;
  evaluation that is sequenced after `await` shall not occur until resumption;
  lowering shall not duplicate user subexpression evaluation solely due to suspend/resume transformation.

### AM.2.4 Where ObjC 3.0 changes baseline guarantees {#am 2 4}

Relative to baseline C/Objective C, ObjC 3.0 adds only the explicit guarantees listed in [AM.2.2](#am 2 2), plus cross construct composition guarantees in [AM.4.4](#am 4 4), [AM.5](#am 5), and [AM.6](#am 6).

Outside those sections, evaluation order and sequencing remain baseline/implementation defined as in ordinary C/Objective C.

## AM.3 Temporaries and lifetime rules {#am 3}

### AM.3.1 Base lifetime rule {#am 3 1}

A temporary created during expression evaluation shall remain valid until at least the end of its containing full expression, unless a rule below extends it further.

### AM.3.2 Lifetime extension points {#am 3 2}

A temporary or local lifetime is extended when required by:

  explicit lifetime controls (`withLifetime`, `keepAlive`, precise lifetime annotations in [Part 8](#part 8));
  capture into storage that outlives the full expression (for example block captures);
  async suspension requirements in [AM.3.3](#am 3 3).

### AM.3.3 Values live across `await` {#am 3 3}

Any value needed after a potentially suspending `await` shall be materialized in async frame storage and kept alive across suspension.

For ARC managed values:

  the implementation shall retain frame stored strong values as needed before suspension; and
  shall release them when they become dead (or when the async frame is destroyed by return, throw, or cancellation unwind).

Values proven dead before suspension may be released before suspension unless prohibited by precise lifetime rules.

### AM.3.4 Hidden temporaries in short circuit forms {#am 3 4}

Hidden temporaries used to implement `?.`, optional send, `??`, `try?`, and postfix `?` shall:

  preserve exactly once evaluation guarantees; and
  not outlive the enclosing full expression unless required by [AM.3.3](#am 3 3) or explicit lifetime controls.

## AM.4 Cleanup stack behavior {#am 4}

### AM.4.1 Registration {#am 4 1}

Within a lexical scope, scope exit actions are registered when:

  a `defer` statement executes; or
  a cleanup/resource local completes successful initialization.

Registration is dynamic: code paths not executed do not register actions.

### AM.4.2 Exit triggers {#am 4 2}

Scope exit actions run when leaving the scope via:

  fallthrough to scope end;
  `return`, `break`, `continue`, or `goto` leaving the scope;
  structured error propagation (`throw`, `try` propagation, postfix `?` early exit);
  cancellation unwind paths defined by [Part 7](#part 7);
  exception unwinding paths that run language cleanups.

Non local transfers that bypass cleanups (for example `longjmp` across cleanup scopes) are outside guarantees and are undefined unless the platform ABI explicitly guarantees cleanup execution.

### AM.4.3 Per scope execution order {#am 4 3}

For a scope `S`, registered scope exit actions shall execute in reverse registration order (LIFO).

For exits that leave multiple nested scopes, scopes are processed from innermost to outermost, applying LIFO within each scope.

### AM.4.4 Ordering with implicit ARC releases {#am 4 4}

In each exiting scope:

1. registered scope exit actions execute first (LIFO);
2. implicit ARC releases for strong locals in that scope execute after all scope exit actions of that scope.

Relative order among implicit ARC releases is baseline/implementation defined unless specified elsewhere, but all such releases are sequenced after scope exit actions in the same scope.

## AM.5 Unified scope exit algorithm {#am 5}

When control leaves scope `S`, a conforming implementation shall behave as if by:

1. Pop and execute each registered scope exit action in `S` (LIFO).
2. Perform implicit scope final ARC releases for `S`.
3. Continue unwinding to the next enclosing scope if control is still exiting outward.

This algorithm applies uniformly for normal return, throw paths, postfix propagation early exits, and cancellation unwind.

## AM.6 `await` interaction rules {#am 6}

### AM.6.1 `await` and scope exit actions {#am 6 1}

Encountering `await` is not scope exit.

Therefore, suspension at `await` shall not by itself execute:

  `defer` actions;
  resource cleanup actions;
  scope final implicit ARC releases.

Those actions execute only when their lexical scope actually exits.

### AM.6.2 `await` and ARC lifetime boundaries {#am 6 2}

At a potentially suspending `await`:

  values needed after resumption shall be preserved in the async frame ([AM.3.3](#am 3 3));
  values dead before suspension may be released before suspension;
  precise lifetime rules still prohibit early release when such annotations/constructs apply.

When a scope eventually exits, defer/resource cleanup ordering relative to ARC remains governed by [AM.4.4](#am 4 4).

### AM.6.3 `await` and autorelease pools {#am 6 3}

On Objective C runtimes with autorelease semantics:

  each task execution slice (resume  > next suspension or completion) shall execute inside an implicit autorelease pool;
  if `await` actually suspends, the current slice pool shall be drained before suspension;
  pool drain shall also occur at task completion (normal return, thrown error, or cancellation unwind).

If an `await` completes synchronously without suspension, draining at that point is permitted but not required.

Autorelease pool draining is not scope exit and shall not trigger scope exit actions by itself.

### AM.6.4 `await` with `try`/`throws` {#am 6 4}

`try await e` is the canonical composed form for `async throws` calls.

If the awaited operation throws:

  the throw is observed at the `await` expression;
  propagation follows [Part 6](#part 6) rules; and
  all exited scopes run scope exit actions and ARC releases per [AM.5](#am 5).

For `try? await e`:

  success yields the value;
  throw yields `nil`;
  cleanup ordering remains unchanged.

For `try! await e`:

  success yields the value;
  throw traps as specified in [Part 6](#part 6); no additional post trap ordering guarantees are required.

### AM.6.5 `await` with postfix propagation `?` {#am 6 5}

For forms equivalent to `(await e)?`:

1. evaluate `await e` first (including any suspension/resumption);
2. apply postfix propagation semantics to the resulting carrier value.

If propagation triggers early exit (`return nil`, `return Err(...)`, or `throw ...`), scope exit proceeds under [AM.5](#am 5).

Carrier restrictions from [Part 6](#part 6) remain in force:

  optional propagation is valid only in optional returning functions;
  using optional propagation to implicitly map into `throws` or `Result` contexts is ill formed in v1.

### AM.6.6 `await` with optional chaining and optional send {#am 6 6}

For `await x?.p`:

  `x` is evaluated exactly once;
  if `x == nil`, result is `nil` and no member access occurs, therefore no suspension occurs on that path;
  if `x != nil`, normal member access proceeds; suspension is possible only if that access is potentially suspending.

For `await [receiver? sel:arg1 other:arg2]`:

  `receiver` is evaluated exactly once;
  if `receiver == nil`, arguments are not evaluated, no send occurs, and no suspension occurs on that path;
  if `receiver != nil`, arguments are evaluated using ordinary send ordering, then the send occurs; suspension is possible only on this non `nil` path.

Applying `await` to a path proven non suspending is permitted and may be diagnosed as unnecessary in strict modes.

### AM.6.7 Composed `await`/`try`/propagation ordering {#am 6 7}

For composed forms such as `return (try? await x?.f())?;` in an optional returning `async` function, implementations shall preserve the following abstract order:

1. Evaluate `x` exactly once.
2. Apply optional chaining/send short circuit rules ([AM.2.2](#am 2 2)):
     if `x == nil`, yield `nil` for the chained/send expression, skip chained argument/member evaluation, and do not suspend on that path;
     if `x != nil`, evaluate the non `nil` path normally (including any potentially suspending operation).
3. Apply `await` to the selected non `nil` path evaluation (if any), with possible suspension/resumption.
4. Apply `try`/`try?`/`try!` semantics ([AM.6.4](#am 6 4)).
5. Apply postfix propagation `?` to the resulting carrier value ([AM.6.5](#am 6 5)).
6. If propagation early exits, run scope exit and ARC ordering per [AM.5](#am 5).

Replacing `try?` with `try` preserves steps 1 3; if a throw occurs at step 4, propagation at step 5 is not reached and unwind follows [AM.5](#am 5).

## AM.7 Conformance test matrix (normative minimum) {#am 7}

A conforming implementation shall provide tests equivalent in coverage to the following matrix.

### AM.7.1 Matrix {#am 7 1}

| ID     | Combination under test                                                     | Required outcome                                                                                                                                                    |
|        |                                                                            |                                                                                                                                                                     |
| AM T01 | `defer` + normal return                                                    | Defers execute once, in LIFO order, before scope final ARC releases.                                                                                                |
| AM T02 | `defer` + cleanup/resource local + return                                  | Defer/resource actions follow per scope LIFO registration order; all run before scope final ARC releases.                                                           |
| AM T03 | `defer` + `throw`                                                          | Throw path runs scope exit actions and ARC releases for all exited scopes (inner to outer).                                                                         |
| AM T04 | `defer` + `try await` where awaited call throws                            | Error propagates from `await`; defer/cleanup actions still run exactly once during unwind.                                                                          |
| AM T05 | `try? await`                                                               | Throwing awaited call yields `nil`; no thrown error escapes; cleanup ordering remains per AM.5.                                                                     |
| AM T06 | `try! await` throwing path                                                 | Program traps on thrown error; implementation is not required to provide post trap cleanup guarantees.                                                              |
| AM T07 | `(await optionalProducer())?` in optional returning function               | Operand is evaluated once; on `nil`, function early returns `nil` and runs scope exit actions once.                                                                 |
| AM T08 | `(await optionalProducer())?` in `throws` function                         | Compile time error: optional postfix propagation is not a valid carrier in `throws` context.                                                                        |
| AM T09 | `await x?.p` with `x == nil` path                                          | `x` evaluated once; result `nil`; no member access and no suspension on nil path.                                                                                   |
| AM T10 | `await [r? m:sideEffect()]` with `r == nil` path                           | `r` evaluated once; `sideEffect()` not evaluated; no send and no suspension on nil path.                                                                            |
| AM T11 | `await [r? m:sideEffect()]` with `r != nil` path                           | `sideEffect()` evaluated in ordinary send order; send occurs; suspension permitted only on this path.                                                               |
| AM T12 | `defer` around await cancellation point                                    | If cancellation causes unwind/error at `await`, defer/cleanup actions execute exactly once before task/frame teardown completes.                                    |
| AM T13 | Nested scopes with `defer` + postfix `?` early exit                        | Early exit unwinds innermost to outermost scopes; each scope uses LIFO action order.                                                                                |
| AM T14 | `await` in scope without exit                                              | Suspend/resume alone does not run scope exit actions or scope final ARC releases.                                                                                   |
| AM T15 | `defer` + `return (try? await x?.f())?;` with `x == nil` path              | `x` evaluated once; `f`/arguments not evaluated; no suspension on nil path; postfix propagation early exits and runs cleanup ordering per AM.5.                     |
| AM T16 | `defer` + `return (try? await x?.f())?;` with `x != nil`, `f` throws       | Non `nil` path may suspend; throw is converted to `nil` by `try?`; postfix propagation early exits; defer/cleanup actions run once before ARC scope final releases. |
| AM T17 | `defer` + `return (try await x?.f())?;` with `x != nil`, `f` throws        | Throw propagates from `try await`; postfix propagation is not applied on throwing path; exited scopes still run cleanup ordering per AM.5.                          |
| AM T18 | `defer` + `return (try await x?.f())?;` with `x == nil` path               | Nil short circuit occurs before suspension; no throw occurs on nil path; postfix propagation early exits `nil` and executes scope cleanups once.                    |
| AM T19 | `defer` + `return (try? await [r? m:sideEffect()])?;` with `r == nil` path | `r` evaluated once; `sideEffect()` not evaluated; no send and no suspension on nil path; early exit still executes defer/cleanup in AM.5 order.                     |

### AM.7.2 Static diagnostics required by matrix {#am 7 2}

At minimum, matrix coverage shall include diagnostics for:

  potentially suspending operations used without `await`;
  invalid optional postfix propagation carrier context;
  optional chaining/send restrictions from [Part 3](#part 3) (including scalar/struct restriction).
## M265 cross module optional and key path preservation

Imported runtime surfaces now preserve the live optional/key path boundary
rather than collapsing that information into generic metadata only summaries.
Separate compilation therefore keeps the runnable Part 3 contract truthful: the
consumer module can observe that the provider landed native optional lowering,
retained typed key path descriptors, and the runtime helper boundary required
to materialize those descriptors after startup registration.

## M265 executable type surface gate (E001)

`objc3c type surface executable conformance gate/m265 e001 v1`

The first lane E gate for Part 3 freezes one integrated executable claim over
optional bindings, optional sends, nil coalescing, typed key path artifacts,
and cross module preservation. The gate consumes the current `A002`, `B003`,
`C003`, and `D003` evidence chain and rejects drift if any one of those proofs
stops publishing the currently supported runnable surface.

This executable gate covers optional bindings, optional sends, nil coalescing, typed key path artifacts, and cross module preservation as one synchronized claim.

## M265 runnable type surface closeout matrix (E002)

`objc3c runnable type surface closeout/m265 e002 v1`

The milestone closeout matrix keeps the current Part 3 implementation truthful.
Runnable rows cover optional send short circuiting, optional binding/refinement,
optional member access, and validated typed key path execution. Pragmatic
generic annotations remain represented through preserved metadata and replay
evidence rather than a fabricated runtime behavior claim.

Current Part 5 frontend note:

  `guard` now admits optional binding plus boolean clause lists
  statement form `match` is now parsed as a frontend owned control flow surface
  the currently admitted pattern slice is limited to wildcard, literal,
  binding, and result case patterns
  native lowering now executes literal/default/wildcard/binding match arms
  while result-case payload matching stays fail closed until a runtime Result
  ABI lands
  expression form `match`, guarded patterns, and type test patterns remain fail closed until later `M266` issues land
  source only `defer { ... }` statements are now admitted in the frontend/sema path, while runnable lowering/runtime execution remains deferred to later `M266` work

M266 B001 semantic model note:

  `frontend.pipeline.semantic_surface.objc_part5_control_flow_semantic_model`
  now records the truthful sema boundary for Part 5.
  live today: guard refinement, guard else exit enforcement, statement match
  binding scopes, result case binding scopes, live bool/result case
  exhaustiveness, and `break` / `continue` legality.
  deferred today: `defer` cleanup ordering and `defer` mediated non local
  exit.

M266 B002 implementation note:

  admitted `match` statements now fail closed unless they are exhaustive for
  the supported surface.
  currently supported exhaustive forms are catch all branches, `true` plus
  `false`, and `.Ok(...)` plus `.Err(...)`.



M266-B003 implementation note:

- source-only defer { ... } statements now contribute live sema cleanup-order accounting and deterministic defer-body non-local-exit diagnostics.
- runnable defer lowering/runtime execution remains deferred to later lane-C/lane-D work.

M266-C001 lowering note:

- `frontend.pipeline.semantic_surface.objc_part5_control_flow_safety_lowering_contract` now freezes the truthful lowering boundary for admitted Part 5 control-flow constructs.
- `guard`, statement-form `match`, and source-only `defer` were the original frozen lowering boundary.
- M266-C002 lowering note: native IR now executes `guard` short-circuit control flow and lexical `defer` cleanup insertion directly, while statement-form `match` remains the only Part 5 lowering surface still fail-closed.
- native LLVM lowering still fail-closes these surfaces with deterministic `O3L300` diagnostics until later runnable lowering/runtime work lands.

M266-D001 runtime/toolchain note:

- the current runnable cleanup/unwind boundary does not add a public runtime cleanup ABI
- native `defer` execution is presently proven through:
  - emitted native object/executable artifacts from the existing `M266-C002` lowering slice
  - the emitted linker-response/runtime-archive sidecars used to link those executables
  - the private autoreleasepool push/pop and memory-state snapshot hooks that remain the runtime-owned cleanup carrier
- `M266-D002` now widens that runnable execution surface across ordinary lexical exit, guard-mediated early return, and nested-scope return unwind while keeping the runtime-owned cleanup carrier private

M266-E001 execution gate note:

- `M266-E001` freezes the currently runnable Part 5 claim as one integrated
  executable gate rather than a parser-only or metadata-only assertion
- the gate consumes the emitted manifest/IR/object triplet plus the
  `M266-A002` / `M266-B003` / `M266-C003` / `M266-D002` proof chain
- the runnable slice is intentionally narrow:
  - boolean-clause `guard`
  - supported exhaustive statement-form `match`
  - lexical `defer` cleanup execution on ordinary exit and return unwind
- expression-form `match`, guarded patterns, type-test patterns, public
  cleanup/unwind ABI, and broader result-payload runtime semantics remain out
  of scope until later milestones

M266-E002 closeout note:

- `M266-E002` publishes the runnable Part 5 closeout matrix for the exact
  supported defer/guard/match slice frozen by `M266-E001`
- closeout rows consume the already-proven `M266-D002` executable cleanup
  evidence plus the `M266-E001` integrated guard/match/defer probe
- the closeout matrix is documentary and evidentiary only; it does not widen
  the supported language or runtime boundary
- `M267-A001` is the next issue

M267-A001 source-closure note:

- the current Part 6 frontend boundary is now explicit:
  - `throws` declarations are live source-only frontend surfaces
  - result-like carrier profiling remains live frontend metadata
  - `NSError` bridging profiles remain live frontend metadata
- `try`, `throw`, and `do/catch` are reserved frontend/source constructs in the
  current native implementation; A001 does not claim runnable semantics for them
- runnable error propagation, catch dispatch, and native thrown-error ABI remain
  deferred to later `M267` issues

M267-A002 source-closure note:

- canonical `objc_nserror` / `objc_status_code(...)` declaration markers are now
  admitted as deterministic frontend/source-only Part 6 surfaces
- the frontend summary now counts marker sites and required
  `success` / `error_type` / `mapping` clause sites
- malformed `objc_status_code(...)` payloads fail closed in the parser
- runtime status-to-error execution and thrown-error propagation remain deferred
  to later `M267` issues

M267-B001 semantic-boundary note:

- the semantic boundary is now explicit:
  - throws declarations are carried as deterministic sema state
  - result-like profiles remain carried as deterministic sema state
  - `NSError` bridging and canonical bridge markers remain carried as deterministic sema state
- postfix propagation, status-to-error execution, and native thrown-error ABI remain deferred
- inherited throws/unwind shard summaries are still carried only as placeholder sema packets, not runnable propagation semantics

M267-B002 try/do/catch semantic note:

- `try`, `try?`, `try!`, `throw`, and `do/catch` are now admitted as
  source-only semantic surfaces
- propagating `try` requires a `throws` callable or an enclosing `do/catch`
- `try` operands must resolve to throwing or `NSError`-bridged call surfaces
- `throw` requires a `throws` callable or catch body
- native IR/object/executable behavior, runnable catch transfer, and native
  thrown-error ABI remain deferred to later `M267` issues
- `try`, `throw`, and `do/catch` are reserved fail-closed parser constructs

M267-B003 bridge legality note:

- canonical `objc_nserror` / `objc_status_code(...)` markers are now filtered by
  semantic legality rather than counted blindly
- only semantically valid bridge call surfaces qualify for `try`
- unsupported bridge combinations remain compile-time fail-closed and do not yet
  widen runnable native behavior

M267-C001 lowering-boundary note:

- lane C first froze the current Part 6 lowering packet before the runnable
  error-out implementation tranche landed
- emitted IR now carries:
  - `; part6_throws_abi_propagation_lowering = ...`
  - `!objc3.objc_part6_throws_abi_propagation_lowering = !{!87}`
- `M267-C002` is the next issue

M267-C002 lowering implementation note:

- the native lowering path is now live:
  - hidden error-out ABI is emitted for throwing call targets
  - `throw` stores into the current error slot and exits the current frame
  - `try`, `try?`, and `try!` branch over success and propagated-error paths
  - `do/catch` dispatch selects typed and catch-all handlers from the lowered
    error slot
- emitted IR now carries:
  - `; part6_throws_abi_propagation_lowering = ...`
  - `!objc3.objc_part6_throws_abi_propagation_lowering = !{!87}`
- remaining work moves to `M267-C003`, which is the replay and separate-
  compilation completion tranche rather than the initial runnable lowering step

M267-C003 replay completion note:

- the current Part 6 lowering packet is now preserved through:
  - `frontend.pipeline.semantic_surface.objc_part6_result_and_bridging_artifact_replay`
  - `module.runtime-import-surface.json`
  - `module.part6-error-replay.json`
- emitted IR now carries:
  - `; part6_result_and_bridging_artifact_replay = ...`
  - `!objc3.objc_part6_result_and_bridging_artifact_replay = !{!88}`
- downstream consumers compiled with imported runtime surfaces now republish
  imported provider replay keys without claiming a broader foreign runtime ABI
- remaining work moves to `M267-D001`, which is the runtime helper and helper
  ABI freeze tranche rather than another replay-only step

M267-D001 runtime-helper note:

- the current runnable Part 6 slice now routes thrown-error storage, bridge
  normalization, and catch dispatch through one private runtime helper cluster
- the helper ABI remains bootstrap-internal and still does not claim any public
  error-runtime or foreign-exception header surface
- emitted IR now carries:
  - `; part6_error_runtime_bridge_helper = ...`
  - `!objc3.objc_part6_error_runtime_bridge_helper = !{!89}`
- the canonical runtime testing snapshot is
  `objc3_runtime_copy_error_bridge_state_for_testing`
- remaining work moves to `M267-D002`, which is the first broader live runtime
  execution tranche above this helper boundary

M267-D002 live-runtime note:

- the current runnable Part 6 slice now links and executes through that same
  private helper cluster instead of proving only an IR-level contract
- emitted IR now carries:
  - `; part6_live_error_runtime_integration = ...`
  - `!objc3.objc_part6_live_error_runtime_integration = !{!90}`
- the current live proof stays narrow to the supported status-bridge plus
  `catch (NSError* error)` execution slice
- remaining work moves to `M267-D003`, which is the cross-module/runtime-import
  hardening tranche above this live path

M267-D003 cross-module preservation note:

- imported runtime surfaces now preserve the current runnable Part 6 replay
  contract/source-contract pair plus the readiness and replay-key inventory
- cross-module link-plan construction fail-closes if imported Part 6 replay
  state is incomplete, non-deterministic, or drifted from the canonical Part 6
  replay contracts
- the runtime helper ABI itself does not widen here; D003 hardens the imported
  runtime-surface and cross-image orchestration boundary above `M267-D002`

M267-E002 runnable closeout note:

- `M267-E002` closes the current runnable Part 6 slice over the already-landed
  `M267-A001` through `M267-E001` proof chain
- the closeout does not widen the abstract machine with any new error carrier,
  bridge, or runtime state beyond what earlier M267 issues already proved
- the canonical closeout evidence is
  `tmp/reports/m267/M267-E002/runnable_throws_result_and_bridge_matrix_summary.json`
- the next issue after this closeout is `M268-A001`

M268-A001 source-closure note:

- `async`, `await`, and canonical `objc_executor(...)` callable attributes are
  now parser-owned source constructs
- this does not widen the abstract machine with runnable continuations,
  suspension cleanup, or executor runtime behavior yet

M268-A002 semantic-packet note:

- the frontend now publishes
  `frontend.pipeline.semantic_surface.objc_part7_async_source_closure`
- that packet is the truthful source-owned Part 7 handoff before later `M268`
  lowering and runtime issues widen execution behavior

M268-B001 semantic-model note:

- the frontend now publishes
  `frontend.pipeline.semantic_surface.objc_part7_async_effect_and_suspension_semantic_model`
- that packet freezes the live semantic legality and handoff state for async
  continuations, await suspension, actor isolation, task cancellation, and
  concurrency replay guards
- runnable async frame lowering, suspension cleanup, and executor runtime
  execution remain later `M268` work

M268-B002 semantic-enforcement note:

- the frontend now publishes
  `frontend.pipeline.semantic_surface.objc_part7_await_suspension_and_resume_semantics`
- that packet proves `await` placement is enforced in sema and fails closed
  outside async functions and Objective-C methods
- runnable async frame layout, suspension cleanup, and executor scheduling are
  still later `M268` work

M268-B003 compatibility note:

- the frontend now publishes
  `frontend.pipeline.semantic_surface.objc_part7_async_diagnostics_and_compatibility_completion`
- that packet freezes the current fail-closed async topology boundary around:
  - non-async `objc_executor(...)`
  - async function prototypes
  - async throws functions
- runnable async frame layout, async error propagation, and executor scheduling
  remain later `M268` work

M268-C001 lowering note:

- the frontend now publishes
  `frontend.pipeline.lowering_surface.objc_part7_continuation_abi_and_async_lowering_contract`
- that packet freezes the current continuation and await suspension lowering
  replay keys before runnable async frame execution exists
- emitted IR now carries the same replay-stable continuation and await lowering
  profiles through the frontend metadata surface
- runnable async frame layout, suspension cleanup, and executor scheduling
  remain later `M268` work

M268-C002 lowering note:

- the frontend now publishes
  `frontend.pipeline.semantic_surface.objc_part7_async_function_await_and_continuation_lowering`
- the current runnable Part 7 lowering slice is truthful but narrow:
  - async functions and async Objective-C methods emit runnable IR/object code
  - `await` lowers through the operand direct-call path
  - no continuation allocation, suspend/resume helpers, or async state machine
    is emitted in the supported happy path
- continuation runtime, suspension cleanup, and executor scheduling remain
  later `M268` work

M268-C003 lowering note:

- the frontend now publishes
  `frontend.pipeline.semantic_surface.objc_part7_suspension_autorelease_and_cleanup_integration`
- the current supported async slice composes with existing lowering rather than
  a dedicated suspension runtime:
  - autoreleasepool scopes lower through the live push/pop hooks
  - defer cleanup remains on the existing Part 5 scope-exit path
  - `await` still lowers through the direct-call happy path
- continuation-frame cleanup, suspension resume cleanup, and executor
  scheduling remain later `M268` work

M268-D001 runtime-helper note:

- emitted IR now publishes `; part7_continuation_runtime_helper = ...`
  alongside `!objc3.objc_part7_continuation_runtime_helper = !{!91}`
- the runtime owns one private helper cluster for logical continuation-handle
  allocation, executor handoff, and resume traffic plus one testing snapshot:
  `objc3_runtime_copy_async_continuation_state_for_testing`
- this helper ABI is real and probeable, but compiled async functions still do
  not consume it yet; the current executable async slice remains direct-call
  only
- live suspension-frame materialization and executor scheduling remain later
  `M268` work

M268-D002 runtime integration note:

- emitted IR now publishes `; part7_live_continuation_runtime_integration = ...`
  alongside `!objc3.objc_part7_live_continuation_runtime_integration = !{!92}`
- the supported non-suspending direct-call await path now executes through the
  private continuation helper cluster on live native object code
- runtime probes can observe deterministic allocation, handoff, resume, and
  handle-retirement traffic through
  `objc3_runtime_copy_async_continuation_state_for_testing`
- this issue still does not claim suspension-frame materialization, async state
  machine execution, or general executor scheduling

M268-E001 async executable conformance gate note:

- the lane-E gate freezes one truthful runnable Part 7 proof chain above
  `M268-A002`, `M268-B003`, `M268-C003`, and `M268-D002`
- the gate consumes the same emitted manifest/IR/object triplet already
  published by the native CLI and frontend bridge instead of introducing a
  separate async-only publication surface
- the canonical gate fixture remains the supported non-suspending direct-call
  async slice; this issue does not widen the abstract machine beyond that
  executable boundary

M268-E002 runnable async closeout note:

- the milestone closeout matrix freezes the implemented runnable Part 7 slice by
  replaying the `M268-A002` through `M268-E001` proof chain
- matrix rows remain tied to the current non-suspending direct-call async model
  and do not widen the abstract machine into suspension-frame or executor
  scheduling semantics
- `M269-A001` is the next issue

M269-A001 task/executor/cancellation source-closure note:

- this issue freezes the callable source boundary that later task-runtime work
  must preserve
- no dedicated `task` or `cancel` keyword is claimed yet; source ownership
  remains the existing async/await syntax plus parser-owned identifier profiles
  for task-runtime hooks, cancellation checks, cancellation handlers, and
  suspension-point symbols
- the proof surface is frontend-only and does not widen the abstract machine
  into runnable task allocation, scheduler hops, or live cancellation execution

M269-A002 frontend task-group/cancellation packet note:

- the frontend now publishes one dedicated source packet for task creation,
  supported task-group call sites, and cancellation-oriented call sites
- the packet remains a source-owned handoff surface; it does not claim runnable
  task allocation, scheduler-backed executor hops, or live cancellation
  execution
- later `M269` lanes must consume this packet rather than widening the source
  contract again

M269-B001 task/executor/cancellation semantic-model note:

- the frontend now publishes a dedicated semantic packet at
  `frontend.pipeline.semantic_surface.objc_part7_task_executor_and_cancellation_semantic_model`
- this packet consumes the `M269-A002` source packet and the existing task
  runtime/cancellation sema summaries rather than inventing another source-only
  surface
- runnable lowering, executor runtime behavior, and scheduler runtime behavior
  remain deferred to later `M269` lanes

M269-B002 structured-task/cancellation enforcement note:

- the frontend now publishes a dedicated semantic packet at
  `frontend.pipeline.semantic_surface.objc_part7_structured_task_and_cancellation_semantics`
- non-async task-runtime/task-group/cancellation calls fail closed with
  `O3S227`
- task-group add/wait/cancel calls without scope fail closed with `O3S228`
- detached task creation mixed into a structured task-group callable fails
  closed with `O3S229`
- cancellation handlers and cancel-all calls without a cancellation check fail
  closed with `O3S230`

M269-B003 executor-hop/affinity compatibility note:

- the frontend now publishes a dedicated semantic packet at
  `frontend.pipeline.semantic_surface.objc_part7_executor_hop_and_affinity_compatibility_completion`
- async task callables without `objc_executor(...)` affinity fail closed with
  `O3S231`
- detached task creation under `objc_executor(main)` fails closed with
  `O3S232`

M269-C001 task-runtime lowering note:

- the frontend now publishes
  `frontend.pipeline.semantic_surface.objc_part7_task_runtime_lowering_contract`
- this packet consumes the already-landed `M269-B001`, `M269-B002`, and
  `M269-B003` semantic packets and lowers them into explicit replay-stable
  actor-isolation, task-runtime-interop, and concurrency-replay contracts
- emitted IR now carries replay keys and lowering profiles for executor-hop
  boundaries, task creation, task-group artifacts, cancellation polls, and
  scheduler-visible replay handoff counts
- native task spawn/hop/cancel runtime entrypoints and task-group ABI
  completion remain later `M269` lane-C work

M269-C002 task-runtime lowering implementation note:

- the retained `M269-C001` lowering packet now maps to a live helper-backed IR
  rewrite for the recognized task-runtime symbol family
- supported task creation, task-group, cancellation-poll, and cancel-handler
  calls lower through private runtime helpers with stable executor tags
- awaited `task_group_wait_next` sites also lower through
  `objc3_runtime_executor_hop_i32` before the existing continuation helper
  resume path
- full native end-to-end proof for the issue remains partially blocked by the
  older `O3S260` / `O3L300` front-door gates, so the issue-local runtime probe
  proves the helper-backed execution boundary directly

M269-C003 task-runtime ABI completion note:

- the frontend now publishes
  `frontend.pipeline.semantic_surface.objc_part7_task_group_and_runtime_abi_completion`
- this packet preserves the helper-backed task-runtime ABI surface emitted by
  `M269-C002`, including task-group helper count and the private runtime
  snapshot symbol
- emitted IR now carries `; part7_task_runtime_abi_completion = ...` and
  `!objc3.objc_part7_task_runtime_abi_completion = !{!93}`

M269-D001 scheduler/executor runtime note:

- `M269-D001` freezes the truthful private runtime boundary above the helper
  slice landed in `M269-C002` and `M269-C003`
- the canonical private helper cluster is:
  - `objc3_runtime_spawn_task_i32`
  - `objc3_runtime_enter_task_group_scope_i32`
  - `objc3_runtime_add_task_group_task_i32`
  - `objc3_runtime_wait_task_group_next_i32`
  - `objc3_runtime_cancel_task_group_i32`
  - `objc3_runtime_task_is_cancelled_i32`
  - `objc3_runtime_task_on_cancel_i32`
  - `objc3_runtime_executor_hop_i32`
- the canonical private testing snapshot is
  `objc3_runtime_copy_task_runtime_state_for_testing`
- emitted IR publishes `; part7_scheduler_executor_runtime_contract = ...`
  and `!objc3.objc_part7_scheduler_executor_runtime_contract = !{!94}`
- the public runtime header still does not widen
- broader live scheduler implementation remains the next issue: `M269-D002`

M270-A001 actor/isolation/sendable source-closure note:

- the current repo source boundary freezes the existing parser-owned
  actor/isolation/sendability symbol profiling already carried by the Part 7
  async semantic packet
- no dedicated `actor`, `sendable`, or `nonisolated` keyword is claimed yet
- the admitted source surface covers actor-isolation declaration markers,
  actor-hop markers, sendable markers, and non-sendable crossing markers
- actor-member legality, isolation diagnostics, and runnable actor runtime
  behavior remain later `M270` work

M270-A002 actor-member/isolation-annotation source note:

- the frontend now admits contextual `actor class` declarations and
  `objc_nonisolated` callable annotations without reserving new lexer tokens

M271-A001 system-extension source note:
- the frontend now admits `objc_resource(...)` local annotations, `borrowed`
  callable-signature qualifiers, `objc_returns_borrowed(owner_index=N)`, and
  explicit block capture lists as parser-owned source surfaces without claiming
  cleanup lowering, borrowed escape enforcement, or runtime capture ownership
- this remains a frontend/source-model claim only

M271-A002 frontend completion note:
- the frontend now admits local `cleanup` hook attributes, `@cleanup` sugar,
  and `@resource` sugar as real Part 8 source surfaces
- the emitted frontend packet now preserves all explicit block capture item
  modes: plain, `weak`, `unowned`, and `move`
- cleanup lowering, resource runtime behavior, and borrowed-pointer legality
  remain later `M271` work

M271-A003 retainable-family source note:
- the frontend now admits retainable C-family callable attributes and the
  canonical compatibility aliases on function and method declarations
- the current truthful source slice does not yet claim family legality,
  ARC-family interop, or runnable retainable-family runtime behavior
- the next issue is `M271-B001`

M271-B001 system-extension semantic-model note:

- the semantic pipeline now publishes
  `frontend.pipeline.semantic_surface.objc_part8_system_extension_semantic_model`
- the packet consumes the already-landed `M271-A001`, `M271-A002`, and
  `M271-A003` source surfaces and freezes one truthful sema/accounting boundary
  for cleanup/resource locals, borrowed pointers, capture-list legality, and
  retainable-family declaration metadata
- resource move legality, borrowed escape legality, retainable-family legality,
  lowering, and runtime behavior remain later `M271` work
- actor interfaces now publish deterministic source counts for actor members,
  executor annotations, async actor methods, and actor member metadata sites
- this remains a frontend/source-model claim; actor-member legality,
  cross-actor diagnostics, sendability enforcement, and runnable actor runtime
  behavior remain later `M270` work

M271-B002 resource-move semantic note:
- the semantic pipeline now publishes
  `frontend.pipeline.semantic_surface.objc_part8_resource_move_and_use_after_move_semantics`
- explicit `move` capture now transfers cleanup ownership only for
  cleanup/resource-backed locals
- live sema rejects non-resource `move` capture, later use of a moved
  cleanup/resource-backed local, and duplicate cleanup transfer of the same
  local
- borrowed escape legality, retainable-family legality, lowering, and runtime
  behavior remain later `M271` work

M271-B003 borrowed-escape semantic note:
- the semantic pipeline now publishes
  `frontend.pipeline.semantic_surface.objc_part8_borrowed_pointer_escape_analysis`
- borrowed pointers now cross call boundaries only when the callee parameter is
  explicitly marked `borrowed`
- live sema rejects borrowed pointers passed to parameters not proven
  non-escaping and borrowed returns without a valid
  `objc_returns_borrowed(owner_index=...)` contract
- escaping-block capture diagnostics are wired on the same borrowed binding
  analysis path
- retainable-family legality, lowering, and runtime behavior remain later
  `M271` work

M271-B004 capture-list/family legality note:
- the semantic pipeline now publishes
  `frontend.pipeline.semantic_surface.objc_part8_capture_list_and_retainable_family_legality_completion`
- the packet consumes the already-landed `M271-B003` sema packet and completes
  the remaining legality edge cases for explicit capture lists and
  retainable-family compatibility aliases
- live sema rejects duplicate explicit captures, weak/unowned explicit captures
  on non-object bindings, conflicting retainable-family annotations, and
  compatibility aliases without supporting object-return family surfaces
- lowering and runtime behavior remain later `M271` work

M271-C001 lowering note:
- the frontend pipeline now publishes
  `frontend.pipeline.semantic_surface.objc_part8_system_extension_lowering_contract`
- the packet consumes the already-landed `M271-B001` through `M271-B004`
  semantic summaries and freezes one truthful lowering handoff for cleanup
  locals, resource locals, borrowed boundaries, explicit capture inventories,
  and retainable-family callable inventories
- emitted manifests and IR now carry replay-stable Part 8 lowering metadata
  without claiming live cleanup/runtime carriers, borrowed lifetime runtime
  interop, or runnable retainable-family execution behavior

M271-C002 lowering note:
- the live lowering path now consumes the frozen `M271-C001` Part 8 contract
  directly instead of introducing a second lowering surface
- stack/local cleanup and resource locals now emit real cleanup calls and block
  dispose helpers in native IR/object artifacts
- actual escaping promotion of move-based cleanup/resource captures remains
  fail-closed until later runtime ownership-transfer work lands

M271-C003 Part 8 ABI completion note:
- `frontend.pipeline.semantic_surface.objc_part8_borrowed_pointer_and_retainable_family_abi_completion`
  now publishes one dedicated artifact/replay packet above the frozen Part 8
  lowering contract
- emitted manifests and IR preserve borrowed-return attribute inventory and
  retainable-family operation/compatibility-alias inventories on the supported
  native direct-call path
- borrowed lifetime runtime interop and runnable retainable-family execution
  remain later lane-D work

M271-D001 system-helper/runtime-contract note:
- the current Part 8 runtime proof now freezes one truthful reuse boundary over
  the private ARC/autorelease helper cluster and the paired memory-management /
  ARC-debug snapshots
- cleanup execution and resource invalidation still ride existing cleanup
  lowering plus autoreleasepool state rather than a new cleanup runtime stack
- retainable-family helper integration rides the same private
  retain/release/autorelease entrypoints instead of a separate Part 8 helper ABI
- borrowed lifetime runtime enforcement and escaping cleanup/resource ownership
  transfer remain later lane-D work

M271-D002 live cleanup/runtime integration note:
- the supported Part 8 fixture path now links and executes through the emitted
  cleanup/resource function body and the same private ARC/autorelease helper
  cluster frozen in `M271-D001`
- linked runtime probes now observe retain/release/autorelease helper traffic
  together with direct `CloseFd` / `ReleaseTemp` cleanup execution on the same
  runnable path
- the executable proof remains on the existing runtime archive plus emitted
  module object path rather than a separate resource-runtime package
- borrowed lifetime runtime enforcement and escaping cleanup/resource ownership
  transfer remain later lane-D work

M271-E001 strict system conformance gate note:
- lane-E now freezes the current runnable Part 8 slice on top of
  `M271-A003`, `M271-B004`, `M271-C003`, and `M271-D002`
- the truthful runnable proof remains the linked `M271-D002` `helperSurface`
  runtime probe rather than a widened front-door borrowed-pointer or
  resource-runtime claim
- the gate consumes the same driver, manifest, and frontend publication
  surfaces while leaving deferred borrowed-lifetime runtime enforcement
  explicitly fail-closed

M271-E002 runnable system-extension closeout note:
- the milestone closeout replays the published `M271-A003` through
  `M271-E001` proof chain and freezes one explicit runnable matrix for the
  already-landed Part 8 slice
- supported closeout rows cover retainable-family source completion,
  capture-list legality, borrowed/retainable ABI completion, live
  cleanup/runtime integration, and the lane-E conformance gate
- the next issue is `M272-A001`

M270-B001 actor/sendability semantic-model note:

- the semantic pipeline now publishes
  `frontend.pipeline.semantic_surface.objc_part7_actor_isolation_and_sendable_semantic_model`
- the packet consumes the `M270-A002` actor-member source closure together with
  the already-landed aggregated actor/sendability sema counters
- the current claim is still a deterministic sema/accounting boundary rather
  than a full actor runtime, broad cross-actor legality checker, or live
  strict-concurrency implementation

M270-B002 actor enforcement note:

- sema now fails closed for unsupported actor-method combinations:
  non-actor `objc_nonisolated`, `objc_nonisolated` plus `async`,
  `objc_nonisolated` plus `objc_executor(...)`, non-async actor hops, and
  non-sendable crossings in actor methods
- this is still narrower than a complete strict-concurrency system; runnable
  actor runtime and broader race-hazard closure remain later `M270` work

M270-B003 actor hazard note:

- actor-method task handoff now fails closed without race guard coverage,
  replay proof coverage, and actor isolation coverage
- escaping block literals remain unsupported in that actor-method hazard slice
- runnable actor runtime and broader strict-concurrency closure remain later
  `M270` work

M270-C001 actor lowering note:

- lane C now publishes
  `frontend.pipeline.semantic_surface.objc_part7_actor_lowering_and_metadata_contract`
- the current lowering freeze carries one deterministic contract for actor
  metadata records, actor isolation-thunk planning, and actor hop-artifact
  planning
- live thunk bodies, mailbox runtime entrypoints, and runnable cross-actor
  scheduling remain later `M270-C002` and `M270-C003` work

M270-C002 actor lowering implementation note:

- actor methods now lower the narrow helper-backed actor slice through:
  - `objc3_runtime_actor_enter_isolation_thunk_i32`
  - `objc3_runtime_actor_enter_nonisolated_i32`
  - `objc3_runtime_actor_hop_to_executor_i32`
- this remains a private lowering/runtime boundary rather than a public actor
  runtime ABI

M270-C003 artifact-integration note:

- the runnable actor lowering path now also lowers:
  - `replay_proof_step()` through `objc3_runtime_actor_record_replay_proof_i32`
  - `race_guard_lock()` through `objc3_runtime_actor_record_race_guard_i32`
- emitted IR now carries both actor lowering and concurrency replay/race-guard
  lowering on the same runnable path

M270-D001 runtime-contract note:

- the truthful lane-D actor runtime contract is still private and currently
  consists of the actor helper cluster plus
  `objc3_runtime_actor_runtime_state_snapshot`
- mixed-module actor helper execution remains bound to the packaged runtime
  archive path rather than a separate actor-runtime distribution

M270-D002 live-runtime note:

- the private actor runtime slice now includes live mailbox binding, enqueue,
  and drain helpers
- actor mailbox state still remains private and is published only through
  `objc3_runtime_copy_actor_runtime_state_for_testing`
- mixed-module actor helper execution still remains bound to the packaged
  runtime archive path rather than a separate actor-runtime distribution

M270-D003 cross-module preservation note:

- actor-bearing runtime-import-surface artifacts now preserve mailbox/isolation
  replay facts for mixed-module consumers
- cross-module link-plan construction fail-closes when imported actor mailbox
  runtime contracts or replay keys drift
- mixed-module actor helper execution still remains bound to the packaged
  runtime archive path rather than a separate actor-runtime distribution

M270-E001 strict concurrency conformance gate note:

- the lane-E gate freezes the current runnable Part 7 actor/runtime slice above
  `M270-A002`, `M270-B003`, `M270-C003`, and `M270-D003`
- it does not widen the abstract machine; it republishes the already-landed
  `M270-D002` live mailbox runtime probe and the `M270-D003` cross-module
  preservation artifacts while keeping broader front-door publication truthfully
  fail-closed
- the next issue is `M270-E002`

M270-E002 runnable actor/isolation closeout note:

- the closeout replays the published `M270-A002` through `M270-E001` proof chain
  and freezes one explicit runnable matrix for the already-landed actor/runtime
  slice
- it does not widen the abstract machine; it republishes the already-landed
  actor source, hazard, lowering, runtime, cross-module, and gate evidence as
  one closeout surface
- the next issue is `M271-A001`

M269-D002 live task runtime note:

- the supported Part 7 task slice now executes through the private helper
  cluster already frozen in `M269-D001`
- emitted IR now publishes
  `; part7_live_task_runtime_integration = ...` and
  `!objc3.objc_part7_live_task_runtime_integration = !{!95}`
- linked runtime proof validates live helper traffic through task spawn,
  task-group scope/add/wait/cancel, cancellation polling, executor hops, and
  the private task-runtime snapshot
- the public runtime header still does not widen
- retained `O3S260` / `O3L300` front-door metadata-export gates may still block
  some fixture shapes before object emission, so D002 keeps its claim scoped to
  the helper-backed live runtime boundary

M269-D003 task-runtime hardening note:

- the live Part 7 task slice now publishes one explicit hardening packet for
  cancellation cleanup, autoreleasepool scope interaction, and reset-stable
  replay determinism
- emitted IR now publishes `; part7_task_runtime_hardening = ...` and
  `!objc3.objc_part7_task_runtime_hardening = !{!96}`
- linked runtime proof validates deterministic two-pass replay across reset,
  task snapshot publication, memory-management snapshot publication,
  ARC-debug snapshot publication, and autoreleasepool push/pop boundaries

M269-E001 task and executor conformance gate note:

- the lane-E gate freezes the current runnable Part 7 task/runtime slice above
  `M269-A002`, `M269-B003`, `M269-C003`, and `M269-D003`
- it does not widen the abstract machine; it republishes the already-landed
  hardened runtime proof and keeps the front-door publication path truthfully
  fail-closed where broader metadata export is still blocked
- the next issue is `M269-E002`

M269-E002 runnable task/executor closeout note:

- the closeout replays the published `M269-A002` through `M269-E001` proof
  chain and freezes one explicit runnable matrix for the current task/runtime
  slice
- it does not widen the abstract machine; it republishes the already-landed
  helper-backed task/runtime evidence and hands off to `M270-A001`

M267-E001 error-model conformance gate note:

- the lane-E gate freezes the current runnable Part 6 slice above `M267-A002`,
  `M267-B003`, `M267-C003`, and `M267-D003`
- it does not widen the abstract machine; it proves the same already-landed
  runtime and cross-module behavior through one gate before `M267-E002`


M266-B002 implementation note:

- admitted match statements now fail closed unless they are exhaustive for the supported surface.
- currently supported exhaustive forms are catch-all branches, 	rue plus alse, and .Ok(...) plus .Err(...).

M275-A001 source-inventory note:

- the frontend now publishes
  `frontend.pipeline.semantic_surface.objc_part12_diagnostics_fixit_and_migrator_source_inventory`
- this is a tooling/governance packet, not a new executable language feature
- it aggregates the already-admitted Part 6 through Part 11 advanced source
  packets plus legacy migration-hint counters into one deterministic
  diagnostics/fix-it/migrator planning surface
- it does not yet claim feature-specific fix-it synthesis, migrator rewrite
  application, machine-readable conformance reporting, or release automation

M275-A002 migration/canonicalization completion note:

- the frontend now also publishes
  `frontend.pipeline.semantic_surface.objc_part12_migration_and_canonicalization_source_completion`
- this packet reports the live lexer-owned migration-assist behavior for legacy
  `YES` / `NO` / `NULL` spellings as deterministic canonicalization, fix-it, and
  migrator candidate counts
- it closes the remaining frontend/source-model surface without claiming
  automated rewrite application yet

M275-B001 diagnostic taxonomy/portability freeze note:

- the frontend now also publishes
  `frontend.pipeline.semantic_surface.objc_part12_diagnostic_taxonomy_and_portability_contract`
- this packet freezes the live advanced diagnostic taxonomy over the existing
  sema ARC/fix-it summary and the `M275-A002` migration surface
- portability requirements are explicit completed closeout dependencies rather
  than an implicit release assumption
