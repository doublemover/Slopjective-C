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

## Implementation still to complete

- Native AST nodes and contextual parser recognition, preserving base-language
  identifiers/macros and recovering cleanly from invalid condition lists.
- Ordered condition typing, independent strong reference/carrier initialization,
  immutable `let`, mutable `var` and correct visibility in later conditions,
  success scopes and failure branches.
- Ownership lowering with single source evaluation, native weak loads, C++ copy
  rules, partial-failure cleanup, throwing temporaries and full lexical lifetime.
- Complete guard exit checking, jump protection, nested condition CFGs, deferred
  cleanup, templates, AST import/identity, PCH, source-free interfaces and tooling.
- Runtime, diagnostic, interface and debugger gates across C17/C++20/23,
  ARC/MRC and O0/O2, followed by a rebuilt and activated SDK.

This is development work; public guard syntax and profile conformance are not
claimed by the scope-exit foundation alone. The activated SDK remains the
[qualified match release](../support/linux_match_results_acceptance.json).
