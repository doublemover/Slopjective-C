# Bounded callback factories in permanent borrowed-result proofs

Source 2.158 / System 2.84 extends permanent borrowed-result proof to conditional
callback target sets and synchronous helper functions returning callbacks.
Immutable higher-order parameters can forward those sets through further
factories and aliases. Conditional factory selection and nested factory calls
retain the actual parameter bindings for each selected definition.

Every reachable callback must prove a permanent result. Every factory must have
a complete bounded return graph: branch, switch, loop, goto and native exception
exits share the existing CFG completeness checks. Known constant conditions may
select one arm; open conditions require both arms. Unknown/mutable/volatile/TLS
bindings, null arms, replaceable definitions, physical ABI casts, missing returns
and recursive or exhausted graphs remain rejected.

AST visits, parameter substitution and CFG blocks share a 4096-unit work budget.
Recursion depth is limited to 32, and each callback collection admits at most 64
unique targets. The collector deduplicates canonical functions without treating
one successful branch as proof of other branches. Existing immutable pointer
argument forwarding and direct helper proofs use the same implementation.

`objc3.borrowed-result-proofs.v5` rejects older readers before replay. Permanent
compiler/PCH controls cover callback choices, returned/forwarded callbacks,
indirect factories, nested factory selection and conservative failure cases.
Publication profiles and source-free native consumers exercise the same bodies.
Complete rebuilt compiler/SDK qualification remains pending.
