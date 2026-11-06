# Linux borrowed-pointer implementation

Part 8.7 is implemented in the canonical native compiler and exported into the
maintained LLVM patch packets. The coordinated Linux SDK build passes native
regressions, the borrowed-pointer facade suite, PCH/source-free import contracts
and the surrounding lifetime/runtime integration. See the
[integration acceptance receipt](../support/linux_native_feature_integration_acceptance.json)
for the exact source and SDK identities; these results do not promote full
Strict System conformance.

Implemented in source:

- Contextual `borrowed T *`, including typedef/using sugar, borrowed function
  returns, function-pointer result types, and arrays of borrowed pointers. The
  internal spelling-free `ObjC3Borrowed` type attribute preserves ordinary C
  pointer layout/calling convention. The general identifier `borrowed` remains
  usable for names and calls. Pretty printing emits the canonical qualifier.
- Canonical `objc_returns_borrowed(owner_index=N)` and
  `@returns_borrowed(owner: N)`. Validation requires a borrowed pointer result,
  a nonnegative in-range zero-based integer owner index, and an object,
  reference, or non-borrowed pointer owner parameter. Conflicting duplicate
  indices and function/method redeclaration contracts are rejected. Typedef and
  variable redeclarations reject erased markers; C composite-type merging
  restores borrowed positions, including native Optional/Result payloads.
- `clang/AST/ObjC3Borrowed.h` exposes type/return-contract queries and recursive
  signature comparison before ABI canonicalization. AST import, structural
  equivalence, and ODR hashing preserve borrowed type positions and owner
  identity. GNU attribute printing retains the named canonical argument.
- Mandatory strict-system function/method/block checking runs with ordinary
  function-body Sema. A native CFG forward may analysis distinguishes live and
  expired acquisitions, applies assignment kills, joins reachable alternatives,
  and retains provenance across ordinary pointer aliases, casts to `void *` or
  integers, pointer arithmetic, local records, constant array elements, and
  mutable pointers to proven local storage. Interned field/array paths keep
  repeated nested fields distinct. Finite possible-target sets retain local
  address provenance across branch joins and later writes to the pointee.
  Native Optional/Result constructors, transfers and expression payloads carry
  the same obligation. Guard/if-let snapshots and match bindings preserve
  concrete owner identity through their opaque runtime initializers.
  Converged facts drive diagnostics, including loop backedges and EH edges.
- Owner identity flows through mutable local object/pointer aliases so a
  forwarded borrowed return can prove its designated owner parameter. Owner
  replacement or lexical lifetime end expires existing pointer aliases while a
  fresh acquisition remains usable. Strong ARC owners receive precise lexical
  lifetime. `withLifetime`/`keepAlive` snapshot origins remain live after source
  replacement and expire with the native lifetime-release CFG action. Pins
  created after acquisition can protect existing loans; ending a pin falls back
  to the unchanged original owner when that owner remains live. Stable
  conditional-owner expressions retain all possible owner roots.
- Escaping stores to global/static/thread-local, heap/indirect, ivar/property
  storage; unproven calls; escaping block arguments/storage/returns; unmatched
  borrowed returns; and escaping erased representations produce actionable
  errors. Spawn/detached/executor/actor boundaries remain escaping despite a
  superficially nonescaping formal parameter. Objective-C++ dynamic namespace
  and static-member initializers use the same mandatory check. Noescape
  parameters seed loans in definitions so implementations cannot silently
  violate their declared contract. C++ constructor arguments and member/base
  initializers receive the same checks, and borrowed dynamic aggregate
  allocation fails closed. Unavailable CFGs cannot bypass noescape parameter
  obligations. Key-path applications retain erased root/value obligations.
- Automatic locals, proven noescape/borrowed formals, synchronous block calls,
  noescape block arguments, scalar data copied from the borrowed region, and
  compiler-known data-copy builtins are accepted. Pointer representations copied
  with `memcpy`/`memmove` retain their obligation. Nonescaping block bodies are
  rechecked using captured facts so an ordinary-pointer alias cannot hide a
  later leak in the body. Capture snapshots precede explicit move source
  niling, and owning captures pin the corresponding owner inside the closure.
  Expression snapshots preserve values already evaluated before later operand
  assignments. Null branch refinement accepts proven null uses. Native
  aggregate initialization/assignment, trivial C++ copies, and exact-size
  `memcpy`/`memmove` preserve field/array payload paths. Partial representation
  copies weakly update possible destinations; zero-byte copies do not escape
  a pointer representation. Callable signatures containing borrowed results
  remain metadata and do not make the callable address a borrowed value.

Maintained fixtures executed by the coordinated batch:

- `tests/clang_foundation/borrowed_pointers_test.cpp`, target
  `objc3_borrowed_pointers`: C17/C++20/C++23 under ARC and MRC, required escape
  triggers, kill/join/loop cases, owner forwarding/replacement/end, lexical
  lifetime snapshots, local aliases/records/arrays, synchronous/escaping blocks,
  pointer erasure, data copying, contract validation, compatibility profile,
  late pins, mutable local address aliases, null refinements, moved/owning block
  captures, nested payloads, noescape definition proofs, dynamic globals, typedef
  and variable identity, nested sibling fields and arrays, future loans through
  joined address aliases, representation copies, union overlap, guard/match
  owner forwarding, trivial C++ copies, noescape constructors, native PCH, AST
  import identity, ODR identity, and source-free interfaces.
- `tests/clang_foundation/borrowed_pointers/`: separate canonical header,
  accepted source, independently selected escape cases, PCH consumer, and
  source-free import provider/consumer fixtures.

Remaining boundaries to keep explicit while development continues:

- Part 8.7 mentions an explicit unsafe escape marker but the catalog/formal
  grammar supplies no spelling. This implementation introduces no speculative
  unsafe syntax. Non-stable owners require explicit lifetime control, one of
  the alternatives expressly allowed by 8.7.1; automatic temporary-owner
  materialization with a warning is not implemented.
- Complete finite local alias writes, including record fields and tracked
  constant offsets, are implemented in System 2.17. Unknown pointer offsets,
  exact union active member changes, partial-copy byte offsets and
  interprocedural owner effects require further precision. Partial may-target
  sets never prove a local store; unknown indirect stores fail closed. Unknown array writes preserve may-alias
  facts, and union field reads conservatively preserve overlapping loans.
- Independent precise strong aliases preserve loans through owner rebinding.
  Explicit known MRC/family retained claims are modeled in System 2.18. Deferred
  release ordering, ambiguous identities and broader foreign count effects,
  mutable/reference/generic lambda precision,
  arbitrary constructor/operator effects, dynamic allocation provenance and
  interprocedural callback mutation effects need more work.
- Indirect borrowed-result calls preserve their type obligation but cannot
  reconstruct a declaration-only owner-index relationship. No whole-program
  analyzer is required or substituted for the mandatory frontend pass.

AST import preserves the owner/type contracts in the target AST. Native CFG
lifetime endpoints follow explicit actions and deferred cleanup for ordinary ARC
strong/weak object slots; nonthrowing release endpoints no longer obscure owner
expiry. The maintained facade regressions exercise both expired owners and
independent lifetime pins. The conservative boundaries above remain deliberate.

The [System 2.1.0 source span](linux_borrowed_cpp_captures.md) extends the
mandatory pass to qualified C++ lambda value captures and their nested bodies.
It rejects hidden alias escapes, future reference/address mutations and
persistent capture storage, with independent ARC capture-owner snapshots.

The [System 2.2.0 source span](linux_borrowed_manual_release.md) tracks manual
instance-release messages through known owner aliases and independent pins.
Other MRC/foreign consumption and indirect/interprocedural effects remain open.

The [System 2.3.0 source span](linux_borrowed_consumed_owners.md) extends known
claim tracking to annotated object consumption and typed callbacks. General
MRC/foreign ownership and unknown/interprocedural effects remain open.

The [System 2.4.0 source span](linux_callable_ownership_casts.md) preserves MRC
retained callback results and checks explicit function/member-pointer ownership
casts, including nested callback contracts and source-free replay.

The [System 2.5.0 source span](linux_borrowed_foreign_consumption.md) adds
annotated foreign consumption under ARC/MRC, consumed Block owner claims and
bound C++ member-call contracts. Ownership-bearing parameter metadata contributes
to native structural equivalence and ODR identity.

The [System 2.6.0 source span](linux_borrowed_block_pins.md) admits ordinary
Block-slot lifetime pins with separate copy-failure cleanup and models ARC
consumed-argument copies before callee entry. Callable contract checks also
follow native arrays and deep pointer containers.

The [System 2.7.0 source span](linux_borrowed_future_captures.md) preserves
requirements for owners first borrowed inside later-invoked concrete closures.
Cached symbolic CFG summaries retain local alias dependencies, independent
capture owners and lifetime pins.

The [System 2.8.0 getter snapshot span](linux_borrowed_getter_snapshots.md)
preserves owner loans across subsequent argument effects.

The [System 2.9.0 canonical family span](linux_retainable_families.md) adds
annotated family-release effects and native/interface family contracts.

The [System 2.10.0 local invocation span](linux_borrowed_closure_consumption.md)
propagates known closure consumption and ARC callback writeback facts.

The [System 2.11.0 mutable capture span](linux_borrowed_mutable_captures.md)
qualifies stable lifetime-sensitive storage alongside mutable scalar state.

System 2.17 carries a separate complete-target proof through CFG joins, local
aliases, aggregate copies, expression evaluation and nested captures. Both arms
must be complete before a conditional/join target set authorizes indirect local
writes. Record fields and tracked constant array offsets preserve exact paths;
branch-specific writes join their resulting facts. Mutable foreign calls and
union representation writes invalidate stale definite-target proofs. Eighteen
C17/C++20/C++23 ARC/MRC source/PCH checks pass; imported getter/record/callback
controls compile and await native frontend consolidation.

## Explicit manual retained claims

System 2.18 tracks a conservative lower bound on explicit MRC instance retains
and annotated retainable-family operations. A retain of one known identity adds
an independent claim; a release or consumed parameter spends one before the
original borrowed lifetime ends. Retain results preserve the argument identity,
so releasing through a returned alias affects the same object. Different
consumed parameters spend distinct claims, including two aliases of one owner.
A family release and its `cf_consumed` parameter describe one handoff.

CFG joins keep the minimum proven claim count. A retain through ambiguous
identities cannot credit every candidate. A source-sized bound keeps loops
finite while preserving arbitrary straight-line sequences. Binding invalidation
clears stale count proofs. Opaque callable consumption retires the proof rather
than assuming one release; deferred/autorelease operations stop using extra
counts until a separate pool-order proof exists. These rules do not reconstruct
unknown interprocedural counts or claim that borrowed owners can be resurrected
by retaining a released pointer.

The source/PCH compiler controls cover all six C17/C++20/C++23 ARC/MRC modes,
retained aliases, branch minima, multiple consumed parameters, overlap,
closures, loops, return contracts, forty explicit claims, deferred effects and
MRC consuming receivers. Native frontend and source-free family fixtures add
balanced published bodies and imported alias/consumption controls. All eighteen new source/PCH commands and all seventeen borrowed compiler
regressions pass. Expanded frontend and source-free regression sources compile;
their execution and full SDK acceptance remain in consolidation.
