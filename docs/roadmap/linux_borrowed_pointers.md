# Linux borrowed-pointer implementation

Part 8.7 source development, 2026-10-23. This work targets the canonical native
compiler in `out/toolchains/objc3-development/src`. The root agent owns export of
that source into maintained LLVM patch packets, sealing, and the coordinated
validation batch. No compiler build or test execution has been performed for
this checkpoint.

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
  mutable pointers to proven local storage. Native Optional/Result constructors,
  transfers and expression payloads carry the same obligation.
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
  violate their declared contract.
- Automatic locals, proven noescape/borrowed formals, synchronous block calls,
  noescape block arguments, scalar data copied from the borrowed region, and
  compiler-known data-copy builtins are accepted. Pointer representations copied
  with `memcpy`/`memmove` retain their obligation. Nonescaping block bodies are
  rechecked using captured facts so an ordinary-pointer alias cannot hide a
  later leak in the body. Capture snapshots precede explicit move source
  niling, and owning captures pin the corresponding owner inside the closure.
  Expression snapshots preserve values already evaluated before later operand
  assignments. Null branch refinement accepts proven null uses.

Maintained evidence authored for the later coordinated batch:

- `tests/clang_foundation/borrowed_pointers_test.cpp`, target
  `objc3_borrowed_pointers`: C17/C++20/C++23 under ARC and MRC, required escape
  triggers, kill/join/loop cases, owner forwarding/replacement/end, lexical
  lifetime snapshots, local aliases/records/arrays, synchronous/escaping blocks,
  pointer erasure, data copying, contract validation, compatibility profile,
  late pins, mutable local address aliases, null refinements, moved/owning block
  captures, nested payloads, noescape definition proofs, dynamic globals, typedef
  and variable identity, native PCH, AST import identity, ODR identity, and
  source-free interfaces.
- `tests/clang_foundation/borrowed_pointers/`: separate canonical header,
  accepted source, independently selected escape cases, PCH consumer, and
  source-free import provider/consumer fixtures.

Remaining boundaries to keep explicit while development continues:

- This is authored source, not a validated conformance claim. Compiler/API
  compatibility and runtime lifetime behavior await the coordinated batch.
- Part 8.7 mentions an explicit unsafe escape marker but the catalog/formal
  grammar supplies no spelling. This implementation introduces no speculative
  unsafe syntax. Non-stable owners require explicit lifetime control; automatic
  temporary-owner materialization with a warning is not implemented.
- Arbitrary multi-target pointer aliases, nested aggregate field paths, union
  active-member identity, unknown array indices, bulk aggregate copies,
  and interprocedural owner effects require further precision. Unknown indirect
  stores fail closed; unknown array writes preserve may-alias facts.
- Owner-alias replacement can conservatively expire a loan even if another
  independent strong alias keeps the same object alive. General MRC release
  effects, C++ lambdas, arbitrary constructor/operator effects, dynamic
  allocation provenance, opaque guard/match binding provenance, and callback
  invocation effects need more work.
- Indirect borrowed-result calls preserve their type obligation but cannot
  reconstruct a declaration-only owner-index relationship. No whole-program
  analyzer is required or substituted for the mandatory frontend pass.
