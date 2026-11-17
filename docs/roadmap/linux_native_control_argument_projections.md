# Native argument control graphs

Source 2.199 / System 2.125 extends native resource-provenance inference beyond
single-return helpers. Closed inline/internal value and reference helpers can
select an argument through pure `if` branches, conditional expressions, switch
cases, fallthrough/break paths and immutable local scalar/reference aliases.
Every live return must identify the same caller argument. Exact selected
reference storage remains visible to surrounding reads, writes and escapes;
unused inputs remain reads. Nested closed helpers share the same proof budget.

Known constant branches retain only their live path. C17 generic selections and
choose expressions retain their selected value; C++ `if constexpr` and C++23
`if consteval` retain admitted direct argument returns. This proof does not
invent a target for an unresolved dependent call. Boolean computations keep
their ordinary telemetry contract. Native calls and exception behavior execute
as written; the inference supplies provenance rather than an optimization.

The analysis retains the existing non-owning scalar/reference signature and
effect gates. Weak, multiversioned, non-returning and task-entry functions cannot supply
a transparent view. Runtime mutation, ambiguous returned arguments, owning local
storage, unproved calls, loops, gotos and unsupported control graphs cannot
establish a projection. Variably modified typedefs are rejected because their
bounds can execute and mutate caller storage even though they are type
declarations. A local value alias cannot establish a reference return.

Inference shares 256 steps and a 16-call bound, with 32 arguments, 32 switch
labels and 64 statement/expression levels. Compound bodies are bounded before
copying their statement list. Switch labels must exactly match the switch's
case list, and list traversal is bounded independently. Recursion and exhausted
budgets retain conservative reference-boundary diagnostics.

The source/PCH controls cover C17, C++20/C++23, ARC/MRC and four profiles. They
exercise ordinary transfers, resource copies/captures, selected-reference
writes, immutable aliases, compile-time branch selection, side-effecting
conditions, variably modified typedefs, ambiguous returns, unsupported control
flow, replaceable/runtime-dispatched functions and recursion/depth/label bounds. The source-free native executor witness
removes its provider before two consumers and executes eleven value/reference
paths per consumer. Its Linux descriptor witness requires exact closure and
keeps explicit unsafe admission visible; imported copies and projected writes
have independent rejection controls.

All 72 source/PCH commands, four optimized native configurations, the full
isolation suite, all 116 AST infrastructure controls, production codec and 16
tooling checks pass. The LLVM translation unit compiles as C++17 with warnings
treated as errors. Fresh maintained-patch replay reproduces every authored
production/control file and all manifest digests match.

General interprocedural/global mutation, multiple possible reference-storage
cells, arbitrary dependent targets, wider ownership/FFI/profile conformance and
matching SDK/archive acceptance remain tracked separately. Runtime layouts,
extension ABI 24 and interface schema 2.2 are unchanged.
