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

## Qualified AST foundation

The last qualified [development receipt](../support/linux_guard_ast_acceptance.json) records
229 passing Clang regressions, 62 AST contract tests and 53 production exit-proof
cases. The native condition nodes pass traversal, printing, profiling, structural
comparison, cross-context import and save/reload serialization, including both
binding kinds and an absent if-binding else. Compiler, clangd and LLDB rebuilds
pass. Source parsing and ownership lowering remain under implementation. That receipt
binds the earlier AST revision; the snapshot/continuation revision below is
under fresh development validation. The first full rebuild passed all 229
Clang regressions and exposed a payload-nullability erasure in the new semantic
tests; the revision below addresses it and remains unqualified until rerun.

## Source and lifetime lowering

The current development revision adds semantic construction shared with match
snapshots and normalizes guard continuations without changing label or case
identity. Its expanded tests cover C17/C++20/23, ARC/MRC, nullable references,
tagged nullable payloads and C++ copy recipes. These additions are not yet
qualified by the earlier receipt.

Carrier nodes now retain payload spelling and nullability annotations while
sharing canonical type identity and physical representation. This follows
native composite-type construction and carries annotations through aliases,
AST import and serialization.

A binding owns an implicit native snapshot declaration and a user declaration.
Their initializers retain native ARC/C++ initialization recipes. The snapshot
initializer stays visible to AST traversal even when implicit declarations are
hidden. Source temporaries remain alive through successful binding initialization.

A guard owns the following statements as a transparent continuation. This lets
normal cleanup and debugger scopes span the remaining source block while the
failure branch uses its incoming scope. The continuation adds no source-level
compound scope. Native local initialization is shared with ordinary declarations;
conditional binding cleanup is registered after source temporary cleanup.
Storage lifetime-end/fake-use registrations are deferred with it. An EH-only
construction guard protects a completed binding while source temporaries and
the snapshot finish; it reads the binding address only after initialization.

Private AST lowering now emits ordered tests, snapshots, guard continuations
and if-binding branches. Its new CodeGen gate verifies native modules and can
export those exact modules for execution across optimization levels. C++ cases
include failed snapshot/binding construction and throwing temporary/snapshot
destructors. This gate is under validation; source syntax is still disabled.

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
