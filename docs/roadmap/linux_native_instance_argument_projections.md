# Closed instance argument projections

Source 2.213 / System 2.139 extends native argument and constructed-work proofs
to direct nonvirtual C++ instance methods whose definitions never inspect or
capture `this`. Scalar/alternative sources, exact reference cells, concrete
templates, Block forwarding and literal/template/composed factories preserve
original caller ownership and cleanup contracts.

Receiver expressions must have no side effects. A bounded definition walk checks
both ordinary statements and nested Block/lambda environments; its work and
pending frontier are capped at 1,024 nodes. Existing purity, argument/return type,
definition visibility, transfer, work/depth/cycle and MRC copy gates remain active.
Operators, virtual dispatch, member-pointer targets, receiver field reads,
receiver mutation and nested `this` captures cannot establish this proof.
Receiver-dependent lifetime analysis remains separate work.

The positive header fails against Source 2.212 and passes with this span. Seventy-two
source/PCH commands pass across C17/C++20/C++23, ARC/MRC and all four profiles,
including negative resource copies, reference-cell writes, unknown callbacks,
side-effectful receivers, stateful bodies and virtual/nested-receiver paths.

Four optimized C++20/C++23 × ARC/MRC native configurations pass as a complete
configuration union. Thirty-three work shapes execute 4,224 scenarios across two
eager and two source-directed consumers, with provider sources removed. Normal
completion and three cancellation routes retain exact owner, work, defer, pause
and terminal-outcome checks. Each selected case intentionally exits with the
runner's partial-selection code; all four exact expected case receipts are
required by the union driver. This is focused execution evidence, not a raw
full-suite CTest qualification.

Fresh maintained-patch application and 16 adjacent tooling checks pass. Extension
ABI 24, interface schema 2.2 and reader capabilities are retained. Source 2.212's
metaprogramming/transport receipts remain distinct. Matching SDK/archive and full
profile acceptance remain pending.
