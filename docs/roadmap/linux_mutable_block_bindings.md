# Bounded mutable Block work items

Source 2.190 / System 2.116 admits closed local Block-value graphs at task spawn
and detached boundaries. Initializers, all possible plain writes, conditionals,
loops and later assignment cycles contribute their capture environments. Every
possible environment must satisfy native Sendable and ownership checking.
Captured ordinary Block callbacks receive the same proof before a native async
frame stores them.

The proof uses the completed callable body. It recognizes value reads through
native C and C++ lvalue conditionals, comma expressions and transparent views,
while rejecting addresses, reference bindings, reference captures, shared
`__block` slots, volatile slots, unknown targets and unmodeled mutation. Mutable
global/static slots and unstructured goto/assembly mutation remain outside this
closed local proof. Initializer dependencies must form a DAG; subsequent writes
may form cycles over the admitted values. A value assignment cannot hide an
unproved alternative.

Expression and scan budgets bound each value graph to 256 expressions and 4096
statement visits with depth 64. Nested capture proofs share completed-node
memoization, a 256-visit allowance and a 64-active-Block limit. A 24-level diamond
therefore checks its distinct environments once instead of expanding all capture
paths. Exceeding a bound rejects the proof.

The native package witness also repairs Objective-C ivar-reference import.
Imported expressions own their field declaration, base, type and locations and
retain arrow/free-ivar and bit-field behavior. Typed body identity binds the
selected ivar and access form; dependency checks retain the referenced member's
public/SPI policy. Source-free inline functions and suspended Blocks can now use
those references without an unsupported-AST failure.

All 72 C17/C++20/C++23 source/PCH configurations pass across ARC/MRC and all four
profiles with concurrency checking enabled. They include closed assignment
cycles, unsafe/unknown alternatives, address/reference and lambda-reference
escape, shared/volatile slots, the shared diamond and a rejected over-depth
capture chain. Four optimized source-free C++20/C++23 ARC/MRC configurations
remove the provider before two independent consumers execute. They exercise
128 task/selection/cancellation scenarios, exact selected-owner retirement,
suspension, ordinary nested callbacks and defer cleanup. The production isolation
and codec suites, all 105 AST controls and 70 related tooling/authority checks
pass. Ivar controls cover target ownership after source-AST destruction, inherited
fields, bit-fields and explicit/implicit access forms. Their separate AST test
translation unit keeps future object-import changes out of the large shared suite.

Extension ABI 24, runtime services 4, native frame 1 and interface schema 2.2 keep
their layouts. No complete Strict Concurrency or arbitrary interprocedural value
proof is claimed. Matching SDK/native-host acceptance remains pending; focused
object relinking does not qualify an archive.
