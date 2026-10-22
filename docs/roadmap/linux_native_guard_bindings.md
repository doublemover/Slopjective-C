# Native guard and optional binding

This milestone implements the canonical Part 3.3.2 and Part 5.3 surface in
Clang: ordered `guard` conditions, `if let` / `if var`, optional/reference
bindings, lexical ownership and mandatory failure exits. Work starts from the
[checked-error CFG milestone](linux_checked_error_control_flow.md).

## Scope-exit foundation

The production `proveGuardScopeExit` helper analyzes the completed callable's
AST, including forward labels. It recognizes outward return/throw/break/continue
and scope-exiting goto, follows internal labels and rejects same-scope gotos,
reachable fallthrough and closed local cycles. Nested lambda/Block bodies are
separate callables; lambda capture initialization remains in the enclosing flow.

The proof combines native CFG reachability with Sema's exception analysis.
Locally caught exceptions from calls, constructors, allocation, reference casts,
Objective-C messages and cleanups cannot masquerade as scope exits. Checked
errors retain their separate dispatch channel. Destructor/cleanup edges use
lexical handler information without changing the general CFG's cleanup model.
Queries run only on reachable nodes and preserve exceptional paths before
accepting an outward transfer in the same block.

Unmodeled constructs fail closed. Deferred cleanup and locally handled
pool/synchronization runtime cleanup need explicit modeling before admission.
The proof does not claim a general theorem about loop termination.

## Qualified development foundation

The [development receipt](../support/linux_guard_ast_acceptance.json) binds
229 passing Clang regressions, 70 AST contract tests, 53 production exit-proof
cases and eight focused tooling tests to the exact sealed inputs. Compiler,
clangd and LLDB rebuilds pass. Native condition nodes pass traversal, printing,
profiling, structural comparison, AST import and save/reload serialization.
Payload aliases and nullable/nonnull annotations survive import and persistence.

The private CodeGen gate exports 40 verified native modules. All 80 O0/O2
executions pass across C17/C++20/23, ARC/MRC modes for integer payloads and
nontrivial C++ records in MRC. The C++ runs include 92 expected constructor,
temporary-cleanup and snapshot-destructor exceptions. This does not qualify
managed-reference ownership, public parsing or an installed guard compiler.
Ordered cases verify short-circuiting, earlier-binding access in later sources,
source mutation, snapshot/temporary lifetime through binding construction,
cleanup before else, reverse final destruction and partial-failure unwinding.

## Source and lifetime lowering

Semantic construction shares snapshot preparation with match. The current
revision keeps following statements in the native source compound and carries
successful lifetime scopes through a synchronous CodeGen continuation. This
preserves native statement-expression result discovery, source ranges and
ordinary sibling traversal. GNU statement-expression runtime cases verify the
result and cleanup lifetime boundary. Maintained tests
cover C17/C++20/23, ARC/MRC, nullable references, tagged nullable payloads and
C++ copy recipes.

Carrier nodes now retain payload spelling and nullability annotations while
sharing canonical type identity and physical representation. This follows
native composite-type construction and carries annotations through aliases,
AST import and serialization. Initialization permissions compare canonical
type identity while retaining their exact storage-address checks.

A binding owns an implicit native snapshot declaration and a user declaration.
Their initializers retain native ARC/C++ initialization recipes. The snapshot
initializer stays visible to AST traversal even when implicit declarations are
hidden. Source temporaries remain alive through successful binding initialization.

A guard's successful CodeGen continuation spans the following native source
statements. Normal cleanup and debugger scopes stay open through the remaining
source block while failure uses the incoming scope. No extra source compound
or AST-owned suffix is introduced. Native local initialization is shared with ordinary declarations;
conditional binding cleanup is registered after source temporary cleanup.
Storage lifetime-end/fake-use registrations and the binding debug declaration
are deferred with it. Shared source-cleanup blocks retain their incoming debug
scope, including paths where the binding was absent. An EH-only
construction guard protects a completed binding while source temporaries and
the snapshot finish; it reads the binding address only after initialization.

Private AST lowering emits ordered tests, snapshots, scoped guard continuations
and if-binding branches. Its CodeGen gate verifies and executes exact native modules across optimization
levels. C++ cases
include failed snapshot/binding construction and throwing temporary/snapshot
destructors. Run the maintained runtime gate after a passing admitted build:

```sh
python -B -m scripts.objc3c_native_validation.condition_ast
```

It compiles the exported modules at O0/O2, binds exact compiler/test/runtime
hashes, replaces stale receipts before checking admission and reclaims temporary
IR/executables. The gate uses the existing foundation lock and checks inputs
again after execution. Source syntax is still disabled.

## Scope integration under validation

Completed function, method and Block bodies now run mandatory-exit checking
with resolved labels and real source regions, including unbraced control bodies.
Jump analysis protects initialized bindings through subsequent source statements
while keeping failure/sibling branches in the incoming scope. The native bypass
detector follows the same declaration boundaries.

Template reconstruction replays source snapshot and binding initialization
through Sema and maps old local declarations to their concrete replacements.
Source full expressions are rebuilt after the new snapshot, preserving temporary
lifetime through successful binding construction. New maintained tests cover
scope exits, initialization bypass, sibling else isolation and dependent replay.
These changes are under fresh validation; the receipt above binds the preceding
checkpoint.

## Implementation still to complete

- Contextual parser recognition, preserving base-language
  identifiers/macros and recovering cleanly from invalid condition lists.
- Ordered condition typing, independent strong reference/carrier initialization,
  immutable `let`, mutable `var` and correct visibility in later conditions,
  success scopes and failure branches.
- Complete ownership qualification with single source evaluation, native weak loads, C++ copy
  rules, partial-failure cleanup, throwing temporaries and full lexical lifetime.
- Complete guard exit checking, jump protection, nested condition CFGs, deferred
  cleanup, templates, AST import/identity, PCH, source-free interfaces and tooling.
- Runtime, diagnostic, interface and debugger gates across C17/C++20/23,
  ARC/MRC and O0/O2, followed by a rebuilt and activated SDK.

This is development work; public guard syntax and profile conformance are not
claimed by the scope-exit foundation alone. The activated SDK remains the
[qualified match release](../support/linux_match_results_acceptance.json).
