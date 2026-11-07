# Scope-bound read-only C++ reference callables

System source 2.32 extends synchronous borrowed-reference capture checking from
immediate literals to local lambda bindings whose complete use graph is known.
Every use must be a direct call in the enclosing body with side-effect-free
primitive/default arguments. Repeated calls, loops, concrete generic
specializations and argument packs share the same bounded body proof as literals.
Unrelated scalar counters may change; captured pointer/record writes, outside
mutable aliases, escaping results and lazy/async work remain guarded.

A bounded expression-graph pass proves that the local callable is never copied,
aliased, passed away, addressed, captured by another closure or invoked through
an unmodeled member expression. Exhausting its budget leaves the proof
unqualified. This proof does not depend on translation-unit parent attachment,
which may not exist during semantic checking. Every materialized generic body
must qualify. Immediate calls still retire their future reference hazard;
named callables retain the hazard, so a later borrowed reassignment diagnoses.
Owner consumption and expiry continue invalidating subsequent invocations.

The maintained compiler regression contains twelve C++20/23 ARC/MRC source/PCH
commands covering repeated/default/generic/pack/looped reads, scalar counters,
record/reference-field and owner references, expired owners, rebinding, escapes/aliases/nesting,
argument effects and specialization-specific mutation. Focused compiler checks
pass. Source-free frontend replay publishes a generic looped reference helper;
all three affected frontend groups pass. Four instrumented native source/PCH
configurations pass fourteen commands, including repeated generic reads with
exact captured-object destruction counts. All thirty-six borrowed compiler regressions pass after updating the older
expectations for newly admitted scope-bound reads; their later expiry and
mutation diagnostics remain enforced.

Open or escaping reference callables, state-changing invocation replay,
interprocedural/global effects, lazy async lifetimes and exceptional ownership
remain separate obligations. Extension ABI 24, runtime ABI 4 and accepted SDK
2.62 remain unchanged; the matched SDK follows consolidated implementation.
