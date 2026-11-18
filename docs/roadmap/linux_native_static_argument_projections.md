# Closed static C++ argument projections

Source 2.211 / System 2.137 admits static C++ methods under the existing closed
native argument-projection proof. Scalar/alternative sources, exact reference
cells, Block values and concrete template projections retain their original
caller contracts. Static views can forward constructed factory results without
substituting a plain formal type or inventing an environment.

Static methods have no hidden receiver parameter. A shared admission check also
requires an object qualifier to have no side effects; opaque qualifier evaluation
cannot hide storage mutation from the projection or factory proof. Nonstatic
methods and static operators remain outside this receiver contract. Class access,
parameter/return ownership, purity, definition, work/depth/cycle and argument-set
rules remain unchanged. Runtime ABI 24, interface schema 2.2 and reader
capabilities remain unchanged.

Controls cover qualified and ordinary-object static lookup, scalar/template
copies, alternative arguments, projected writes through exact reference cells,
constructed work forwarding, unknown callbacks and rejected opaque object
qualifiers. Source-free consumers add an independent static-view work item to
the preceding 31 shapes, with imported resource-copy/write and unknown-environment
negative controls. The four cancellation routes retain exact owner, work, defer,
pause and terminal-outcome checks.

The positive header fails against Source 2.210 and passes with this span.
Thirteen suites pass 936 source/PCH commands across C17/C++20/C++23,
ARC/MRC and all four profiles, including the adjacent scalar/control/alternative
projection lanes. Four optimized native configurations pass 4,096 task scenarios
across two direct and two directed consumers. Fresh patch application and 16
tooling checks pass. Source 2.210's full isolation/codec/122-AST receipt remains
distinct; matching SDK/archive and full-profile acceptance remain pending.
