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

Unmodeled constructs fail closed. C++ construction/destruction and locally handled pool/synchronization
runtime cleanup with defer still need explicit modeling before admission.
The proof does not claim a general theorem about loop termination.

## Current implementation wave

The contextual source parser is now implemented in the tracked patch stack,
including ordered guard lists, single-head if bindings, lexical scopes,
recovery, macro/contextual identifier preservation, templates and PCH fixtures.
Native CFG nullable-receiver analysis covers explicitly nullable references,
aliases, native nil values, source mutation, cleanup effects, constructors and
templates. This development wave has not been built or validated yet; the user
requested implementation first and deferred validation. Unspecified receiver
policy and complete strict-profile enforcement remain open.

Native tooling now includes libclang cursor/type references, source-aware
traversal and parent maps, JSON metadata, AST matchers and clang-query predicates.
The foundation plan includes dedicated matcher tests, dynamic matcher tests and
index/query source regressions. A public source/PCH execution matrix and installed
SDK gate cover guard bindings. These are implementation and test enrollment,
not passing evidence or feature admission.

## Qualified development foundation

The pre-wave [development receipt](../support/linux_guard_ast_acceptance.json) binds
232 passing Clang regressions, 84 AST contract tests, 53 production exit-proof
cases and eight focused tooling tests to the exact sealed inputs. Compiler,
clangd and LLDB rebuilds pass. Native condition nodes pass traversal, printing,
profiling, structural comparison, AST import and save/reload serialization.
Payload aliases and nullable/nonnull annotations survive import and persistence.

The private CodeGen gate exports 404 verified native modules, each generated and
executed with matching O0 or O2 frontend/backend settings. All 404 executions
pass across C17/C++20/23, ARC/MRC modes for integer payloads, nontrivial C++
records in MRC, and managed objects/Blocks in ARC. The runs include 164 expected
C++ exceptions and 312 expected Objective-C exceptions.

Managed cases cross strong/weak/unsafe sources, direct nullable references versus
tagged optionals, immutable/mutable bindings and both branch forms. Clearing the
original owners preserves the bound value until normal or exceptional scope
exit, including Block captures. Zeroed weak references distinguish absent direct
sources from present tagged Some(nil). Ordered C++ cases verify short-circuiting,
earlier-binding access, source mutation, temporary/snapshot lifetime, cleanup
before else, reverse final destruction and partial-failure unwinding.
A separate public defer source/PCH gate passes 24 configurations and 100 commands,
including 24 expected Objective-C and 16 expected C++ terminations. Together the
gates pass 909 commands. This does not qualify public guard parsing or an
installed guard compiler.

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
again after execution. Public guard syntax is still disabled. Defer fixtures and their helper are also
bound by persisted input hashes; source/PCH execution uses the same admitted
compiler and runtime.

## Qualified scope integration

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
Instantiated template runtime cases also verify native owned construction,
source temporary lifetimes and ordered partial-failure cleanup. The receipt above
binds this completed scope and template checkpoint.

## Qualified native CFG

Guard and if-binding now build native branches in source order. Failure skips
later sources and destroys only completed bindings before else. Snapshot and
source temporary cleanup finishes before the next condition; successful bindings
remain active through the native continuation and destroy in reverse order.
Carrier-copy recipes branch on the selected arm, including conditional temporary
cleanup. Explicit selected-payload destructor elements preserve lexical native
exception handlers, including outward goto paths with unresolved synthetic CFG
targets.

The CFG kind tag now has five bits in the same two-word element, with alignment
assertions for supported 64-bit hosts. Every runtime fixture must build a native
CFG before its IR is emitted. Ten C/C++ path cases independently check source
ordering, branch boundaries and C++ prefix/snapshot destruction. Nested exit
proofs and caught carrier-destruction cases exercise the production checker.
A checked-error regression also verifies that initializer failure destroys
previously completed locals while excluding the local still under construction.
The source statement list and public static-analysis gates remain explicit.

## Registered deferred cleanup CFG

Normal and checked-error exits now execute registered deferred bodies in LIFO
order. The source registration does not execute its body. Cleanup copies preserve
their lexical checked/native context, isolate local labels and finish local goto
backpatches before restoring another copy. Return, break and continue destinations
remain after the cleanup subgraph. The 66 trace cases cross all three source modes
and native marker options, including early exits, nested scopes, loop transfers,
gotos, copied labels, noreturn calls and matched/unmatched checked handlers.

Bounded native unwinding now routes calls, Objective-C messages and explicit
Objective-C/C++ throws through pending actions to their registration-time lexical
handler. Local catches inside a deferred body remain valid during unwinding; a
second exception escaping that body terminates. Twenty-two production exit-proof
cases and three trace cases qualify this model, including unknown outward goto.
C++ construction/destruction, managed copies, pool/synchronization/finally cleanup
and computed/asm jumps remain gated. The mandatory-exit proof admits only the
bounded model.

Native CodeGen copies now isolate labels, local declarations and VLA state.
EH copies preserve the original exception and selector around body-local catches.
Source/PCH regressions reproduce the former duplicate-label assertion and unwind
crash at O0/O2 across C17/C++20/23 and ARC/MRC. Address-taken labels in deferred
bodies receive an explicit diagnostic; direct local gotos are supported. Return
markers also preserve native fallthrough diagnostics after cleanup.

## ARC scope-exit release ordering

Native source scopes with defer now queue implicit managed releases until their
registered actions finish. Native defer, C++ destructor and cleanup-attribute
ordering is preserved. Normal transfers that retain the source scope drain their
pending releases after the native cleanup chain, including late forward-label
fixups and backward gotos across registrations. Destination identities prevent
local jumps inside a deferred body from draining an enclosing transfer's queue.

Precise strong lifetimes prevent early optimizer releases. Runtime pointer slots
and pending flags exclude uninitialized locals and unrelated cleanup copies.
Lifetime-end/fake-use operations and VLA stack restoration follow destruction;
LLVM lifetime intrinsics retain their required original alloca operand. Native
ARC exception policy and original destruction recipes remain in force.

Source/PCH execution covers objects, Blocks, strong arrays, escaping `__block`
ownership, carriers/arrays, C records, pure C++ aggregates and nested carrier
records. Cases include normal/return/loop/goto, assignment, checked/native errors,
throwing actions, local catches/jumps, native C++ and cleanup-attribute ordering,
VLA storage, methods, Blocks, lambdas and templates. PCH cases enable extended
variable liveness to keep fake-use cleanup coverage in the maintained gate.
Private guard/if modules also verify prefix release before else versus
defer-before-release on success/unwind. Exceptions in a later source unwind
only the completed prefix, and if-body actions remain unregistered until every
condition succeeds.
This qualifies the maintained CodeGen cases; a complete native ARC cleanup CFG
model and public guard source admission remain separate work.

## Implementation still to complete

- Contextual parser recognition, preserving base-language
  identifiers/macros and recovering cleanly from invalid condition lists.
- Ordered condition typing, independent strong reference/carrier initialization,
  immutable `let`, mutable `var` and correct visibility in later conditions,
  success scopes and failure branches.
- Complete ownership qualification with single source evaluation, native weak loads, C++ copy
  rules, partial-failure cleanup, throwing temporaries and full lexical lifetime.
- Complete guard exit checking, jump protection, deferred/runtime
  cleanup and match CFGs, templates, AST import/identity, PCH, source-free interfaces and tooling.
- Runtime, diagnostic, interface and debugger gates across C17/C++20/23,
  ARC/MRC and O0/O2, followed by a rebuilt and activated SDK.

This is development work; public guard syntax and profile conformance are not
claimed by the scope-exit foundation alone. The activated SDK now delivers the deferred cleanup and analysis fixes through
the [qualified scope-release archive](../support/linux_defer_scope_release_acceptance.json).
All 26 installed suites and fresh Debian 13 / Ubuntu 26.04 consumers pass. Public
guard syntax and installed guard qualification remain gated.
