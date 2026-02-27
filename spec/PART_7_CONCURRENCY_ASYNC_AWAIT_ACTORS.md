# Part 7 — Concurrency: async/await, Executors, Cancellation, and Actors {#part-7}

_Working draft v0.11 — last updated 2026-02-27_

_Normative baseline references used in this part: [NR-C18](#part-0-2-1), [NR-LLVM-OBJC](#part-0-2-1), [NR-BLOCKS-ABI](#part-0-2-1), and where applicable [NR-OBJC-RUNTIME](#part-0-2-1)/[NR-ABI-PLATFORM](#part-0-2-1)._

## 7.0 Overview {#part-7-0}

### v0.10 resolved decisions {#part-7-v0-10-resolved-decisions}

- v1 includes standard macro sugar for task spawning as a source-level convenience over library-defined task APIs; sugar expansion is semantically fixed but runtime entrypoint naming remains unfrozen.
- Explicit actor reentrancy controls are deferred from v1; reservation diagnostics and compatibility rules are defined.

### v0.9 resolved decisions {#part-7-v0-9-resolved-decisions}

- `await` is required for any potentially suspending operation, including cross-executor and cross-actor entry ([Decision D-011](#decisions-d-011)).
- Required module metadata for concurrency/isolation is enumerated in D.

### v0.8 resolved decisions {#part-7-v0-8-resolved-decisions}

- Canonical attribute spellings for concurrency/executor features are defined in B.
- Lowering/runtime contracts for `async/await`, executors, and actors are defined in C.

### v0.5 resolved decisions {#part-7-v0-5-resolved-decisions}

- Executor annotations use a canonical Clang-style spelling `__attribute__((objc_executor(...)))`.
- Autorelease pool behavior at suspension points is normative: each task execution slice runs inside an implicit pool drained on suspend and completion.

### v0.4 resolved decisions {#part-7-v0-4-resolved-decisions}

- v1 does **not** add `task {}` keyword syntax. Task spawning is provided by the standard library and recognized by the compiler via standardized attributes.

Objective‑C 3.0 introduces structured concurrency that is composable, auditable, and implementable.

This part defines:

- the `async` effect and `await` expression,
- executor model and annotations,
- structured tasks and cancellation (standard library contract),
- actor isolation,
- sendable-like checking as an opt-in safety ladder.

---

## 7.1 Lexical and grammar additions {#part-7-1}

### 7.1.1 New keywords {#part-7-1-1}

In ObjC 3.0 mode, the following are reserved by this part:

- `async`, `await`, `actor`

### 7.1.2 Effects and isolation {#part-7-1-2}

`async` and `throws` are effects that become part of a function’s type.

Executor affinity (`objc_executor(...)`) and actor isolation are **isolation annotations** that may cause a call to **suspend at the call site** even when the callee is not explicitly `async` ([Decision D-011](#decisions-d-011)).

---

## 7.2 Async declarations {#part-7-2}

### 7.2.1 Grammar (illustrative) {#part-7-2-1}

```text
function-effect-specifier:
    'async'
  | 'throws'
  | 'async' 'throws'
  | 'throws' 'async'
```

### 7.2.2 Static semantics {#part-7-2-2}

- `await` is permitted only in async contexts.
- Any **potentially suspending** operation (defined in [§7.3.2](#part-7-3-2)) shall appear within an `await` expression.

### 7.2.3 Block types {#part-7-2-3}

Async is part of a block’s type:

- `R (^)(Args) async`
- `R (^)(Args)` (sync)

---

## 7.3 `await` expressions {#part-7-3}

### 7.3.1 Grammar {#part-7-3-1}

```text
await-expression:
    'await' expression
```

### 7.3.2 Static semantics (normative) {#part-7-3-2}

A translation unit is ill-formed if it contains a **potentially suspending operation** that is not within the operand of an `await` expression.

The following are potentially suspending operations:

1. A call to a declaration whose type includes the `async` effect.
2. A cross-executor entry into a declaration annotated `objc_executor(X)` when the compiler cannot prove the current executor is `X`.
3. A cross-actor entry to an actor-isolated member when the compiler cannot prove it is already executing on that actor’s executor.
4. A task-join / task-group operation that is specified by the standard library contract to be `async`.

`await` applied to an expression that is not potentially suspending is permitted, but a conforming implementation should warn in strict modes (“unnecessary await”).

### 7.3.3 Dynamic semantics {#part-7-3-3}

`await e` may suspend:

- suspension returns control to the current executor,
- later resumes on an executor consistent with isolation rules.

In particular, `await` may represent:

- suspending to call an `async` function,
- an executor hop to satisfy `objc_executor(...)`,
- or scheduling onto an actor’s serial executor.

---

## 7.4 Executors {#part-7-4}

### 7.4.1 Concept {#part-7-4-1}

An executor schedules task continuations.

### 7.4.2 Default executors {#part-7-4-2}

Runtime shall provide:

- main executor
- global executor

### 7.4.3 Executor annotations (normative) {#part-7-4-3}

Objective‑C 3.0 defines executor affinity using a standardized attribute spelling.

#### 7.4.3.1 Canonical spelling {#part-7-4-3-1}

Executor affinity shall be expressed using a Clang-style attribute:

- `__attribute__((objc_executor(main)))`
- `__attribute__((objc_executor(global)))`
- `__attribute__((objc_executor(named("..."))))`

The attribute may be applied to:

- functions,
- Objective‑C methods,
- Objective‑C types (including actor classes), establishing a default executor for isolated members unless overridden.

#### 7.4.3.2 Static semantics (normative) {#part-7-4-3-2}

If a declaration is annotated `objc_executor(X)`, then entering that declaration from a context not proven to already be on executor `X` is a **potentially suspending operation** ([§7.3.2](#part-7-3-2)) and therefore requires `await`.

In strict concurrency checking mode, the compiler shall reject a call to an executor-annotated declaration unless either:

- the call is within an `await` operand, or
- the compiler can prove the current execution is already on executor `X`.

#### 7.4.3.3 Dynamic semantics (model) {#part-7-4-3-3}

An implementation shall ensure that execution of an executor-annotated declaration occurs on the specified executor.

When a call is type-checked as crossing an executor boundary, the implementation may insert an implicit hop **provided the call site is explicitly marked with `await`**.
The observable effect is that executor-affine code never runs on an incorrect executor, and the call is treated as potentially suspending.

#### 7.4.3.4 Notes {#part-7-4-3-4}

- `main` is intended for UI-thread-affine work.
- `global` is intended for background execution.
- `named("...")` is for custom executors defined by the runtime/standard library.

### 7.4.4 Current executor model (normative) {#part-7-4-4}

Each executing async task shall have a logical `current executor` value in task context metadata.

Rules:

- Entering an executor-annotated declaration (`objc_executor(X)`) sets `current executor := X` for the dynamic extent of that entry.
- Entering actor-isolated code sets `current executor` to the actor instance's serial executor for that entry.
- Calling unannotated synchronous declarations does not change `current executor`.
- Across suspension/resumption (`await`), the task's logical executor context shall be preserved, then updated as required by the resumed continuation's entry isolation/annotation.
- Detached tasks begin on an implementation-selected executor unless the API contract specifies one; once running, executor transitions shall still follow the same hop rules.

### 7.4.5 Proof rules and hop elision (normative) {#part-7-4-5}

The compiler may treat an executor hop as provably unnecessary only when it can establish executor equivalence from language-visible facts.

Accepted proof sources:

- lexical/type-visible annotation identity in the current module (`objc_executor(X)` to `objc_executor(X)`),
- actor-isolated same-instance execution where the current continuation is already bound to that actor executor,
- imported module/interface metadata that preserves executor/isolation semantics per [D Table A](#d-3-1) and compatibility rules in [D.3.4](#d-3-4).

Conservative requirements:

- Missing, incompatible, or unverifiable executor metadata shall be treated as "not proven"; `await` and hop behavior remain required.
- Strict concurrency profiles shall diagnose missing/incompatible required metadata as errors per [D.3.5](#d-3-5), rather than assuming no-hop.
- Implementations may perform additional optimizer-local hop elimination only if they preserve the same observable suspension and isolation correctness required by this section.

### 7.4.6 Fairness and ordering guarantees (normative minimum) {#part-7-4-6}

Objective-C 3.0 does not require a global scheduler policy, but requires the following minimum portability guarantees:

- **Serial exclusion**: a serial executor (including actor executors) shall not run two isolated work items concurrently.
- **Forward progress**: runnable work items enqueued on a live executor shall eventually execute absent permanent external blocking or process termination.
- **No strict global FIFO guarantee**: relative ordering across different executors, priorities, or detached domains is implementation-defined unless explicitly documented by the runtime profile.
- **Intra-actor safety**: regardless of queue policy, actor-isolated mutable state shall observe single-executor access and the reentrancy rules in [§7.7.3](#part-7-7-3).

### 7.4.7 Executor conformance tests (normative minimum) {#part-7-4-7}

Conforming suites shall include at least:

- `EXE-01` required hop: call `objc_executor(main)` from known non-main context; `await` required and execution occurs on main executor.
- `EXE-02` hop elision: call target with proven same executor annotation; no hop required and behavior matches same-executor semantics.
- `EXE-03` cross-module proof: import executor metadata from another module and prove/no-prove hops consistently with in-module behavior.
- `EXE-04` metadata loss guard: remove/mismatch required executor metadata and assert profile-appropriate diagnostics per [D.3.5](#d-3-5).
- `EXE-05` progress smoke test: finite runnable queue on a live executor drains without starvation under documented runtime constraints.

---

## 7.5 Tasks and structured concurrency (standard library contract) {#part-7-5}

### 7.5.1 Design decision (v1) {#part-7-5-1}

Objective‑C 3.0 v1 does not introduce a `task { ... }` keyword expression/statement. Task creation and structured concurrency constructs are provided by the **standard library**, but the compiler shall recognize task-related APIs via attributes so that:

- Sendable-like checking can be enforced for concurrent captures,
- cancellation inheritance and “child vs detached” semantics are predictable,
- diagnostics can be issued for misuse (e.g., discarding a Task handle).

### 7.5.2 Standard attributes (normative) {#part-7-5-2}

#### 7.5.2.1 `__attribute__((objc_task_spawn))` {#part-7-5-2-1}

Applied to a function/method that creates a **child task**.

Requirements:

- The spawned task shall inherit:
  - the current cancellation state,
  - executor context (unless the API explicitly overrides executor),
  - and (optionally) priority metadata.
- The task is considered a child of the current task for cancellation propagation.

#### 7.5.2.2 `__attribute__((objc_task_detached))` {#part-7-5-2-2}

Applied to a function/method that creates a **detached task**.

Requirements:

- Detached tasks do **not** inherit cancellation by default.
- Executor selection is implementation-defined unless explicitly specified.

#### 7.5.2.3 `__attribute__((objc_task_group))` {#part-7-5-2-3}

Applied to a function/method that establishes a structured task group scope.

Requirements:

- All tasks added to the group shall complete (or be cancelled) before the group scope returns.
- If the group scope exits by throwing, the group shall cancel remaining tasks before unwinding completes.

### 7.5.3 Required standard library surface (abstract) {#part-7-5-3}

A conforming implementation shall provide, in a standard concurrency module, APIs equivalent in expressive power to:

- **Spawn child task**
  - input: an `async` block (possibly throwing)
  - output: a task handle representing the child task

- **Spawn detached task**
  - same, but detached semantics

- **Join**
  - `await`-join a task handle to retrieve its result (or throw its error)

- **Task groups**
  - create group, add tasks, iterate results, join implicitly at scope end

The exact naming and Objective‑C surface syntax are implementation-defined, but the compiler shall be able to identify the spawn/group APIs via the attributes above.

### 7.5.4 Compiler checking hooks {#part-7-5-4}

In strict concurrency checking mode:

- The compiler shall enforce Sendable-like requirements for values captured by `__attribute__((objc_task_spawn))` / `__attribute__((objc_task_detached))` async blocks.
- The compiler should warn when a returned task handle is unused (likely “fire and forget”); suggest using the detached API explicitly or awaiting/joining.

### 7.5.5 Standard macro sugar for task spawning (normative, v1) {#part-7-5-5}

Objective‑C 3.0 v1 standardizes optional source sugar for task spawning while keeping task semantics library-defined.

Required public macro interface (standard concurrency headers):

- `OBJC_TASK_SPAWN(block_expr)` where `block_expr` is an async callable expression accepted by the child-spawn API contract.
- `OBJC_TASK_DETACHED(block_expr)` where `block_expr` is an async callable expression accepted by the detached-spawn API contract.

Expansion and semantic requirements:

- `OBJC_TASK_SPAWN(e)` shall be semantically equivalent to a direct call to an API recognized as `__attribute__((objc_task_spawn))` ([§7.5.2.1](#part-7-5-2-1)).
- `OBJC_TASK_DETACHED(e)` shall be semantically equivalent to a direct call to an API recognized as `__attribute__((objc_task_detached))` ([§7.5.2.2](#part-7-5-2-2)).
- The macro layer shall not alter cancellation inheritance, executor behavior, or Sendable-like enforcement relative to the equivalent direct call.
- Conformance does not require fixed runtime symbol names or a fixed token-for-token expansion body; only semantic equivalence to the recognized spawn class is required.

Diagnostics requirements:

- Type/capture diagnostics that would be produced for the equivalent direct spawn call shall also be produced through macro sugar.
- Implementations shall diagnose misuse at or pointing to the user macro invocation/argument site (with expansion notes permitted), not only in synthetic internals.
- The unused-handle warning from [§7.5.4](#part-7-5-4) applies equally to macro spellings.

Metadata/interface emission requirements:

- Use of macro sugar shall not hide spawn classification from semantic analysis or emitted module/interface metadata.
- If exported inline declarations use `OBJC_TASK_SPAWN`/`OBJC_TASK_DETACHED`, importers shall observe equivalent concurrency metadata and diagnostics behavior as for direct recognized API calls.
- No additional v1 required capability is introduced solely for this sugar; it is defined as a source-level front-end convenience over existing spawn metadata contracts.

### 7.5.6 Task macro sugar conformance matrix (normative minimum) {#part-7-5-6}

Conforming suites shall include at least:

- `TSUG-01` child-spawn macro equivalence: `OBJC_TASK_SPAWN(...)` and direct `objc_task_spawn`-recognized API call produce equivalent child cancellation/executor inheritance behavior.
- `TSUG-02` detached-spawn macro equivalence: `OBJC_TASK_DETACHED(...)` and direct `objc_task_detached`-recognized API call produce equivalent detached cancellation behavior.
- `TSUG-03` diagnostics parity: strict-mode Sendable-like and unused-handle diagnostics fire through macro spellings with primary locations at the user invocation/capture sites.
- `TSUG-04` metadata parity: an exported inline declaration using task macros emits/imports concurrency metadata equivalently to a direct recognized call, preserving cross-module checking.

## 7.6 Cancellation {#part-7-6}

### 7.6.1 Cancellation model (normative) {#part-7-6-1}

Cancellation in Objective-C 3.0 concurrency is **cooperative**.

- Requesting cancellation shall not preemptively terminate execution at an arbitrary instruction boundary.
- Each task shall carry a cancellation state (bit/token equivalent) that is:
  - set by a cancellation request,
  - idempotent (multiple requests are equivalent to one),
  - and monotonic (once cancelled, it stays cancelled for the task lifetime).
- A cancellation request by itself does not throw; it changes task state that code may observe.

### 7.6.2 Propagation and inheritance (normative) {#part-7-6-2}

Cancellation propagation shall follow these rules:

- **Parent -> child (`objc_task_spawn`)**
  - A child task shall inherit the parent's cancellation state at spawn time.
  - If the parent is already cancelled, the child starts in the cancelled state.
  - If the parent is cancelled later, cancellation shall propagate to all live structured descendants.
- **Child -> parent/siblings**
  - Cancelling a child task shall not implicitly cancel its parent or sibling tasks.
- **Task groups (`objc_task_group`)**
  - Tasks added to a group are structured children for cancellation purposes.
  - Cancelling the enclosing task or the group scope shall cancel all unfinished tasks in that group.
  - If a group API exits by throwing (including cancellation), remaining unfinished group tasks shall be cancelled before scope unwind completes ([§7.5.2.3](#part-7-5-2-3)).
  - Group operations that propagate one child failure shall cancel unfinished siblings before propagating that failure.
- **Detached tasks (`objc_task_detached`)**
  - Detached tasks do not inherit cancellation unless an API explicitly specifies inherited cancellation.
  - Cancelling a parent task or task group shall not cancel detached tasks by default.
  - Detached tasks may be cancelled only through their own handle/runtime cancellation facility.
- **Across suspension, executor hops, and actor hops**
  - Cancellation state is task-local metadata and shall be preserved across `await` suspension/resume boundaries, executor hops, and actor hops.

### 7.6.3 Observation points and required behavior (normative minimum) {#part-7-6-3}

A conforming standard library/runtime shall expose:

- a non-throwing query equivalent to `TaskIsCancelled()` / `Task.isCancelled`,
- and a throwing checkpoint equivalent to `TaskCheckCancellation()` / `Task.checkCancellation()`.

The following are required cancellation observation points:

1. When entering any standard-library operation documented as a cancellation point.
2. Immediately before a task would suspend in a standard-library wait primitive (for example: join, group wait/next, or sleep).
3. Immediately after resumption from such a wait primitive and before user continuation proceeds.
4. During structured group-scope finalization before waiting for remaining children.

When cancellation is observed:

- A throwing cancellation point shall complete by throwing the cancellation representation defined in [§7.6.4](#part-7-6-4), without beginning additional blocking work.
- A non-throwing cancellation point shall complete promptly using its documented cancelled result/status and shall not block indefinitely.
- Implementations shall not inject asynchronous exceptions into non-observation instruction boundaries.

### 7.6.4 Representation and interaction with `throws` / propagation operators {#part-7-6-4}

Cancellation completion shall be representable as a distinguished error value (for example, `CancellationError` or an implementation-defined equivalent that can be reliably recognized by library predicates/pattern matching).

Rules:

- `await` alone does not transform cancellation into a thrown error.
- `try await` propagates cancellation exactly as other thrown errors when a callee/cancellation point throws the cancellation representation.
- `try? await` (or equivalent optionalizing propagation operator) shall convert cancellation throws into the operator's non-throwing "empty" result, consistent with its behavior for other errors.
- Joining a throwing task that completed due to cancellation shall throw the same cancellation representation.
- Non-throwing join/group APIs shall represent cancellation in their result/status model and shall not silently report successful completion.

### 7.6.5 Cleanup and unwind guarantees under cancellation (normative) {#part-7-6-5}

Cancellation shall not weaken scope cleanup guarantees:

- `defer` handlers shall execute exactly once, in LIFO order, when scope exit occurs due to cancellation-driven unwind.
- Language/runtime cleanup for local resources (including ARC releases and any applicable destructor/finalizer semantics) shall run as on ordinary throw/return paths.
- Async-frame captured objects retained across suspension shall be released when the frame is destroyed, including cancellation unwind ([§7.9.3](#part-7-9-3)).
- Autorelease pool draining requirements remain in force on cancellation unwind ([§7.9.4](#part-7-9-4)).
- Structured group scopes shall not finish returning/unwinding until required child cancellation+completion conditions are satisfied ([§7.5.2.3](#part-7-5-2-3)).

### 7.6.6 Cancellation interaction matrix (normative summary) {#part-7-6-6}

The matrix below is normative and cross-references required behavior:

| Construct                  | If cancellation is observed                                      | Required behavior                                                                                                         | Reference                                        |
| -------------------------- | ---------------------------------------------------------------- | ------------------------------------------------------------------------------------------------------------------------- | ------------------------------------------------ |
| `defer`                    | During unwind from cancellation                                  | Execute all registered defers exactly once (LIFO) before scope exit completes                                             | [§7.6.5](#part-7-6-5), [§7.9.2](#part-7-9-2)     |
| `try await f()`            | `f`/callee cancellation point throws cancellation representation | Propagate as a thrown error via normal `throws` rules                                                                     | [§7.6.4](#part-7-6-4), [§7.9.1](#part-7-9-1)     |
| `try? await f()`           | Same as above                                                    | Convert throw (including cancellation) into the operator's non-throwing empty result                                      | [§7.6.4](#part-7-6-4)                            |
| `await` actor/executor hop | Caller task already cancelled before hop                         | Hop/call remains legal; cancellation state is preserved; no implicit throw unless code hits a throwing cancellation point | [§7.6.2](#part-7-6-2), [§7.7.2.1](#part-7-7-2-1) |
| Task/group wait or join    | Wait primitive observes cancellation                             | Throw cancellation (throwing form) or return cancelled status (non-throwing form); do not block indefinitely              | [§7.6.3](#part-7-6-3), [§7.6.4](#part-7-6-4)     |

### 7.6.7 Cancellation conformance tests (normative minimum) {#part-7-6-7}

Conforming suites shall include at least:

- `CAN-01` parent cancellation propagates to live child tasks spawned with `objc_task_spawn`.
- `CAN-02` detached tasks do not inherit parent/group cancellation unless API explicitly opts in.
- `CAN-03` group failure path cancels unfinished siblings before propagating failure/cancellation.
- `CAN-04` cancellation-driven unwind executes `defer` and required resource cleanups exactly once.
- `CAN-05` `try? await` converts cancellation throw into the operator's non-throwing empty result.
- `CAN-06` cancellation state is preserved across executor and actor hops.
- `CAN-07` non-throwing cancellation points return documented cancelled status and do not report success.

---

## 7.7 Actors {#part-7-7}

### 7.7.1 Actor types {#part-7-7-1}

```objc
actor class Name : NSObject
@end
```

### 7.7.2 Isolation {#part-7-7-2}

Actors provide data-race safety by associating each actor instance with a **serial executor** and treating the actor’s isolated mutable state as accessible only while running on that executor.

Informally: _outside code must hop onto the actor to touch its state._

### 7.7.2.1 Actor-isolated members (normative) {#part-7-7-2-1}

Unless otherwise specified, instance methods and instance properties of an `actor class` are **actor-isolated**.

A reference to an actor-isolated member from outside the actor’s isolation domain is a **potentially suspending operation** ([§7.3.2](#part-7-3-2)) and therefore requires `await`.

Example (illustrative):

```objc
actor class Counter : NSObject
@property (atomic) NSInteger value;
- (void)increment;
@end

async void f(Counter *c) {
  await [c increment];         // may hop to c's executor
  NSInteger v = await c.value; // may hop; read is isolated
}
```

Within actor-isolated code already executing on the actor’s executor, `await` is not required for isolated member access unless the member is explicitly `async`.

### 7.7.3 Reentrancy {#part-7-7-3}

Actor execution is reentrant at suspension points.

Normative rules:

- While an actor-isolated task is running between suspension points, no other actor-isolated task for the same actor may run concurrently.
- If actor-isolated code executes `await` (or another potentially suspending operation), the current task may suspend and other queued actor work may run before the original task resumes.
- Reentrancy occurs only at suspension points; there is no arbitrary mid-expression interleaving inside a non-suspending segment.

#### 7.7.3.1 Reentrancy points and invariants (normative) {#part-7-7-3-1}

- Every potentially suspending operation in actor-isolated code is a reentrancy point boundary.
- Invariants that must hold across actor turns shall be re-established before reaching a reentrancy point.
- Code that requires a non-reentrant critical region shall avoid suspension inside that region (for example, compute local snapshots first, then `await` after region completion).

#### 7.7.3.2 Isolation boundary and atomicity expectations (normative) {#part-7-7-3-2}

- Actor isolation guarantees single-executor access to actor-isolated mutable state; it does not provide multi-operation transactions across separate awaits/calls.
- A single actor-isolated member invocation is atomic only with respect to other actor-isolated entries until it reaches a suspension point.
- Cross-actor property/method access from outside the actor domain is a potentially suspending operation and requires `await` per [§7.7.2.1](#part-7-7-2-1).
- `atomic` property attributes do not weaken actor-isolation requirements and do not imply lock-free progress across actor turns.

### 7.7.4 Nonisolated members (normative) {#part-7-7-4}

A member may be declared **nonisolated** to indicate it is safe to call without actor isolation.

Canonical spelling (attribute):

```objc
actor class Logger : NSObject
- (void)log:(NSString *)message __attribute__((objc_nonisolated));
@end
```

Rules:

- A `nonisolated` member shall not access actor-isolated mutable state without an explicit hop.
- Parameters and return types of `nonisolated` members that may cross concurrency domains shall satisfy Sendable-like checking in strict concurrency mode ([§7.8.3](#part-7-8-3)).

> Note: Toolchains may additionally accept a contextual keyword spelling (`nonisolated`) as sugar, but emitted interfaces should use the canonical attribute spelling ([B.3.4](#b-3-4)).

### 7.7.5 Actor isolation/reentrancy conformance matrix (normative minimum) {#part-7-7-5}

Conforming suites shall include at least:

- `ACT-01` cross-actor property/method access without `await` is rejected in strict concurrency mode.
- `ACT-02` same-actor isolated access without `await` (no suspension boundary crossed) is accepted.
- `ACT-03` actor method with an internal `await` permits interleaving by another queued actor task between pre- and post-await observations.
- `ACT-04` `objc_nonisolated` member that touches isolated mutable state without explicit hop is rejected.
- `ACT-05` `objc_nonisolated` member signatures with non-Sendable boundary types are rejected in strict concurrency mode.
- `ACT-06` cross-module import preserves actor isolation metadata; missing/mismatched metadata triggers profile-appropriate diagnostics.
- `ACT-07` explicit `nonreentrant` actor-member marker use in v1 mode is rejected with a targeted deferred-feature diagnostic.
- `ACT-08` explicit `__attribute__((objc_actor_nonreentrant))` use in v1 mode is rejected with a targeted deferred-feature diagnostic.
- `ACT-09` `nonreentrant` remains usable as an identifier outside reserved actor-member modifier positions.

### 7.7.6 Explicit actor reentrancy controls are deferred in v1 (reserved syntax) {#part-7-7-6}

Objective‑C 3.0 v1 does not include explicit source-level actor nonreentrancy controls. Reentrant behavior at suspension points remains as defined by [§7.7.3](#part-7-7-3).

Reserved future spellings (not enabled in v1):

```text
actor-member-reentrancy-modifier (reserved):
    'nonreentrant'
  | '__attribute__((objc_actor_nonreentrant))'
```

Reservation and compatibility requirements:

- In v1 mode, declarations using either reserved spelling above are ill-formed and shall produce a targeted "explicit actor nonreentrancy is deferred in v1" diagnostic.
- `nonreentrant` is reserved as a contextual keyword only in actor-member reentrancy-modifier positions.
- Outside that slot, `nonreentrant` remains a valid identifier for source compatibility.
- Canonical v1 module/interface emission shall not claim or require explicit actor-reentrancy-control metadata.

Interaction with `await` and cancellation while deferred:

- `await` remains a reentrancy boundary exactly as specified in [§7.7.3.1](#part-7-7-3-1); no extra nonreentrancy exception exists in v1.
- Cancellation semantics and observation points are unchanged by this deferral and remain governed by [§7.6](#part-7-6).

---

## 7.8 Sendable-like checking {#part-7-8}

### 7.8.1 Marker protocol {#part-7-8-1}

```objc
@protocol Sendable
@end
```

### 7.8.2 Default Sendable-like status by category (normative) {#part-7-8-2}

Unless overridden by explicit conformance or attributes, implementations shall classify values as follows:

- **Immutable/value-like data**: Sendable-like by default.
  - This includes C scalar/enumeration values and aggregates whose stored fields are all Sendable-like.
  - Aggregates containing borrowed pointers ([Part 8](#part-8) [§8.7](#part-8-7)) or resource-handle fields requiring cleanup ([Part 8](#part-8) [§8.3](#part-8-3)) are not implicitly Sendable-like.
- **Actor references**: Sendable-like by default.
  - References to `actor class` instances may cross concurrency domains; isolation rules still apply at use sites (`await`/hop requirements are unchanged).
- **Ordinary class instances (non-actor)**: Not Sendable-like by default.
  - A class instance is Sendable-like only if the type conforms to `Sendable` (or is otherwise recognized by toolchain rules as safe), or the type uses the explicit unsafe escape hatch ([§7.8.5](#part-7-8-5)).
  - `const` qualification on an object pointer does not by itself imply Sendable-like status.
- **Blocks/closures**: Not Sendable-like by default.
  - A block value is accepted at a concurrent boundary only when its capture set satisfies [§7.8.4](#part-7-8-4).

### 7.8.3 Enforcement sites (normative) {#part-7-8-3}

In strict concurrency mode, the compiler shall enforce Sendable-like constraints at all of the following boundaries:

1. **Task spawning APIs** recognized by `__attribute__((objc_task_spawn))`, `__attribute__((objc_task_detached))`, and task-group add operations:
   - values captured by the spawned async block,
   - and values explicitly passed into the spawned work item.
2. **Cross-actor entry**:
   - arguments passed to actor-isolated members when a hop may be required,
   - returned values (and thrown values, if any) crossing back to the caller.
3. **`nonisolated` actor members**:
   - parameters and returns of `objc_nonisolated` members that are callable from arbitrary executors.
4. **Concurrent closure/block captures**:
   - implicit and explicit captures (including `self`) in closures known to execute concurrently with the caller.

In permissive modes, implementations may downgrade selected violations to warnings, but strict concurrency mode shall diagnose them as errors unless [§7.8.5](#part-7-8-5) applies.

### 7.8.4 Capture, pointer, and handle interaction rules (normative) {#part-7-8-4}

- **`weak` captures**:
  - `weak` capture does not automatically make a value Sendable-like.
  - The referenced type must still be Sendable-like for the capture to cross a checked boundary.
- **`unowned` / `__unsafe_unretained` captures**:
  - Treated as unsafe by default for concurrent transfer.
  - Strict concurrency mode shall reject these captures unless the captured type/wrapper is explicitly admitted via [§7.8.5](#part-7-8-5).
- **Borrowed pointers** (`borrowed T *`, borrowed returns):
  - Borrowed values are non-Sendable across task/actor/nonisolated boundaries by default.
  - `objc_unsafe_sendable` does not waive borrowed-lifetime non-escaping requirements from [Part 8](#part-8) [§8.7](#part-8-7); violations remain diagnostics.
- **Resource handles / cleanup-managed values**:
  - Values governed by cleanup/resource annotations ([Part 8](#part-8) [§8.3](#part-8-3)) are non-Sendable by default, even when represented as scalar handles.
  - Crossing a concurrency boundary requires an explicit safe transfer abstraction (for example, a wrapper with defined ownership transfer semantics) or explicit unsafe admission per [§7.8.5](#part-7-8-5).

### 7.8.5 Unsafe escape hatch semantics (`objc_unsafe_sendable`) (normative) {#part-7-8-5}

`__attribute__((objc_unsafe_sendable))` (see [B.3.3](#b-3-3)) provides explicit programmer assertion that a type/wrapper may be treated as Sendable-like when proof is unavailable.

Rules:

- The attribute may satisfy Sendable-like checking at the boundaries listed in [§7.8.3](#part-7-8-3).
- It does not relax unrelated concurrency rules:
  - missing required `await`/actor hop remains an error,
  - borrowed-pointer escape/lifetime violations remain diagnostics per [Part 8](#part-8),
  - resource cleanup/move correctness rules remain diagnostics per [Part 8](#part-8).
- Any acceptance that depends on `objc_unsafe_sendable` shall produce an explicit diagnostic note or warning in strict concurrency mode identifying the unchecked transfer site.
- Applying `objc_unsafe_sendable` to APIs that expose borrowed pointers or cleanup-managed resource handles should produce an additional warning that safety is user-asserted and not compiler-proven.

### 7.8.6 Conformance test matrix (normative minimum) {#part-7-8-6}

A conforming implementation's test suite shall include, at minimum, the matrix below.

| Case | Scenario | Expected result in strict concurrency mode | Notes |
| ---- | -------- | ------------------------------------------ | ----- |
| SND-01 | Spawned task captures only scalar/enum/aggregate values that are transitively Sendable-like | Accept | Validates default value-like sendability |
| SND-02 | Spawned task captures actor reference and performs actor-isolated access with `await` | Accept | Actor references are Sendable-like; isolation still enforced |
| SND-03 | Spawned/detached task captures ordinary mutable class instance with no `Sendable` conformance | Error | Non-actor class instances are non-Sendable by default |
| SND-04 | Cross-actor call passes non-Sendable class argument or returns non-Sendable class value | Error | Boundary enforcement for arguments/returns |
| SND-05 | `objc_nonisolated` member signature includes non-Sendable parameter/result type | Error | Enforced even if body is otherwise valid |
| SND-06 | Concurrent closure uses `weak`/`unowned` capture of non-Sendable reference type | Error | `weak`/`unowned` does not bypass checking |
| SND-07 | Borrowed pointer or cleanup-managed resource handle crosses task/actor boundary without safe wrapper | Error | Part 8 interaction rules remain in force |
| SND-08 | Same as SND-03/SND-04 but type is marked `objc_unsafe_sendable` | Accept with required unchecked-transfer warning/note | Escape hatch is explicit and auditable |
| SND-XM-01 | Module `A` exports `Sendable`/`objc_unsafe_sendable` metadata; module `B` imports and uses it at sendability boundaries | Import and checking behavior matches in-module behavior | Cross-module semantic preservation required by [D.3.1](#d-3-1) |
| SND-XM-02 | Module metadata/interface omits or mismatches Sendable/task-spawn metadata, then imported under strict concurrency profile | Hard error on import or use-site validation | Required by [D.3.5](#d-3-5) |

---

## 7.9 Interactions {#part-7-9}

### 7.9.1 `try await` composition {#part-7-9-1}

When calling an `async throws` function, both effects must be handled:

- `await` handles the async effect (may suspend),
- `try` handles the throws effect (may throw).

A conforming implementation shall accept `try await f(...)` as the canonical order.

### 7.9.2 `defer` and scope cleanup across suspension {#part-7-9-2}

Defers registered in an async function scope execute when that scope exits, even if the function has suspended and resumed multiple times.

### 7.9.3 ARC lifetimes across suspension {#part-7-9-3}

Objects referenced across an `await` must remain alive until no longer needed. Implementations shall retain values captured into an async frame and release them when the frame is destroyed (normal completion, throw, or cancellation unwind).

### 7.9.4 Autorelease pools at suspension points (normative on ObjC runtimes) {#part-7-9-4}

On platforms that support Objective‑C autorelease pools:

- Each **task execution slice** (from a resume point until the next suspension or completion) shall execute within an **implicit autorelease pool**.
- If an `await` suspends the current task, the implementation shall drain the implicit autorelease pool **before** the task suspends and control returns to the executor.
- The implementation shall drain the implicit pool at task completion (normal return, thrown error, or cancellation unwind).

If an `await` completes synchronously without suspending, draining is permitted but not required.

Implementations may drain at additional safe points, but shall not permit unbounded autorelease pool growth across long-lived tasks that repeatedly suspend.

---

## 7.10 Diagnostics {#part-7-10}

Minimum diagnostics:

- `await` outside async (error)
- calling async without await (error)
- crossing an executor boundary into `objc_executor(X)` without `await` when a hop may be required (error in strict concurrency mode)
- assuming executor-equivalence/hop elision without required proof metadata in strict concurrency mode (error; reference [§7.4.5](#part-7-4-5), [D.3.5](#d-3-5))
- cross-actor isolated access without `await` when a hop may be required (error in strict)
- Sendable violations in strict concurrency mode (error)
- use of `objc_unsafe_sendable` at a transfer boundary (explicit warning/note in strict concurrency mode)
- attempted use of `objc_unsafe_sendable` to suppress borrowed-pointer/resource correctness violations (diagnose underlying rule violation; do not suppress)
- unused task handles returned from `__attribute__((objc_task_spawn))` APIs (warning in strict concurrency mode)
- invalid use of `OBJC_TASK_SPAWN` / `OBJC_TASK_DETACHED` macro sugar (error/warning parity with equivalent direct spawn call; primary diagnostic on invocation/argument site)
- reserved explicit actor reentrancy-control syntax (`nonreentrant` modifier or `objc_actor_nonreentrant` attribute) used in v1 mode (error; explain deferral)

## 7.11 Open issues {#part-7-11}

None currently tracked in this part.

## 7.12 Lowering, ABI, and runtime hooks (normative for implementations) {#part-7-12}

### 7.12.1 Separate compilation requirements {#part-7-12-1}

Conforming implementations shall satisfy the separate compilation requirements in [C.2](#c-2) and the required metadata set in [D](#d) for all concurrency-relevant declarations, including:

- `async` effects,
- executor annotations (`objc_executor(...)`),
- actor isolation metadata,
- and task-spawn recognition attributes ([B.3.2](#b-3-2)).

### 7.12.2 Coroutine lowering model (informative summary) {#part-7-12-2}

The recommended lowering model is:

- `async` functions lower to coroutine state machines ([C.5](#c-5)),
- `await` lowers to a potential suspension + resumption scheduled by the current executor,
- values that cross suspension are retained/lowered in a way that preserves ARC semantics.

### 7.12.3 Executor hop primitive {#part-7-12-3}

Implementations should provide (directly or via the standard library) a primitive that represents:

- “suspend here and resume on executor X”

so that the compiler can uniformly lower:

- calls into `objc_executor(main)` declarations, and
- actor isolation hops.

### 7.12.4 Actor runtime representation (minimum) {#part-7-12-4}

Each actor instance shall be associated with a serial executor that:

- enqueues actor-isolated work items, and
- ensures mutual exclusion for isolated mutable state.

The concrete queue representation is implementation-defined, but the association must be observable by the runtime/stdlib to implement `await`ed cross-actor calls.

### 7.12.5 Autorelease pools at suspension points {#part-7-12-5}

Autorelease pool boundaries at suspension points are required by [Decision D-006](#decisions-d-006) and defined normatively in [C.7](#c-7) and [§7.9.4](#part-7-9-4).

