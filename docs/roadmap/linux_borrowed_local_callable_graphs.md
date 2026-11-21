# Closed local graphs of reference callables

System source 2.33 extends scope-bound C++ reference-lambda proofs across local
trivial copies/moves, const and mutable reference aliases, and reference bindings
that extend a literal's temporary lifetime. Direct and explicit `operator()`
invocations, including materialized generic specializations, use the same checked
argument and body contracts. Independent scalar closure state may change; a
captured borrowed address or owner still cannot be rebound by an admitted body.

The bounded complete-use pass follows reverse local binding edges from each
literal. Every transitive reference must be a qualified local binding or stable
call in the enclosing body. One escape, address use, nested capture, unsupported
argument or unmodeled use rejects the entire originating graph. Unconnected
cycles grant no proof; exhausting the existing 8,192-node/edge budget grants no
partial admission. Trivial closure construction excludes arbitrary captured-field
copy effects. Nontrivial copy/move graphs retain their explicit contract boundary.

The ownership solver keeps its ordinary invocation, mutation and expiry checks.
Copies and aliases preserve the originating callable's borrowed dependencies.
A later owner consumption or captured-reference rebinding still diagnoses,
including through lifetime-extended literal references. Named graphs retain their
future-reference hazard; an immediate explicit call retires that hazard after
return just like ordinary immediate-call syntax.

The new compiler fixture supplies twelve C++20/23 ARC/MRC source/PCH commands.
It checks native copies and moves, alias chains, explicit generic operators,
lifetime extension, unused local copies, independent mutable scalar state,
expired/rebound owners and escaping/addressed/nested/nontrivial-copy graphs.
All thirty-seven borrowed compiler regressions pass. Existing copy/alias/operator
expectations were updated only for newly closed valid uses; negative lifetime
and escape obligations remain exercised.

Source-free frontend replay/relocation and valid/invalid consumers publish a
copied/aliased generic helper. All three affected frontend groups pass. The
maintained native capture fixture checks copied/aliased/moved reference closures,
extended temporary references and exact independent owner destruction. Four
ASan/UBSan source/PCH configurations pass fourteen commands.

Open or escaping callables, nontrivial capture-copy effects, state-changing owner
or borrowed-pointer invocation replay, interprocedural/global effects and lazy
async lifetimes remain separate work. Extension ABI 24, runtime ABI 4 and the
accepted SDK 2.62 are unchanged. Matched SDK consolidation follows implementation.
