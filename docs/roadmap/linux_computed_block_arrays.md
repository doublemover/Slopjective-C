# Computed and nested Block array ownership

System source 2.48 extends member-array registration to bounded constant nested
arrays. Native row-major scalar leaves share one flattened completed-prefix
count, so a failure inside a later row releases exactly earlier initialized
slots. Sparse rows and zero/default fillers retain their native null behavior.

Block values may use sequenced scalar work, conditional selection, stable native
member/index loads and literals with scalar-only captures. Evaluation happens
before the destination copy: a throwing scalar operand releases the prior
prefix; a throwing heap copy still leaves that destination uninitialized.
Strong local bindings and consumed arguments use the same copy boundary. Direct
reads and invocations through stable native member/index storage participate in
the complete lifetime graph. Address/index evaluation keeps ordinary exception
edges and volatile, atomic, weak and unsafe storage retain their own gates.

Managed/byref captures, capture initializers, C++ capture copies, owned Block
producer calls and expressions introducing managed temporaries need independent
cleanup regions. Block writes remain a separate contract. No bound or ownership
predicate is relaxed to treat these operations as primitive copies.

Source-free admission cuts over to `objc3.native-subobject-cleanup.v5`; retired
versions are rejected by the production catalog. Standalone computed-value bodies
also require `objc3.cxx-block-expressions.v2`, including C17 bodies without any
constructor/destructor or Block literal. Reusable constructor printing
also unwraps cleanup expressions before choosing array initializer delimiters,
preserving strict braced syntax instead of emitting a GNU array-parentheses
extension. A print/reparse control and relocated source-free replay defend it.

All 66 affected compiler cases and 44 AST controls pass. Twelve ASan/UBSan native
configurations / 42 commands pass, including copy failures 1–4, scalar evaluation
faults before elements 1 and 4, both conditional branches, body/destructor faults,
sparse rows, copied local bindings and consumed calls. Ten new source-free
profiles, fourteen adjacent frontend groups, codec/isolation controls and forty
exact production decoder replays pass. The print/reparse control also passes.

Extension ABI 24, native frame 1, runtime services 4 and schema 2.2 remain unchanged.
These checks use development source. One matched SDK and installed package
qualification follow the remaining implementation sprint.
