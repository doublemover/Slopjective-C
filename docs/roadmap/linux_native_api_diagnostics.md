# Native API diagnostic contracts

Source 2.176 / System 2.102 extends C++ publication to native availability and
diagnostic contracts throughout public record/template/member/alias/enumerator
graphs and published local declarations. This closes the prior difference
between top-level availability handling and nested declaration admission.

Deprecation messages and replacement names, unavailable messages, complete
platform availability versions/environment/priority/strictness, and native
error/warning messages and severity participate in structural identity. GNU and
standard spellings share their semantic payload; length-delimited strings retain
quotes, backslashes and separators. Canonical publication quotes native
error/warning messages as C string literals. Body-local admission retains the
ordinary declaration's diagnostic behavior.

`objc3.native-declaration-attributes.v4` is a hard reader cutover. Callable ABI
contracts previously covered by v3 remain required under v4. Missing capability
claims and readers limited to v3 reject the new interfaces. The native ABI,
descriptor layouts and runtime service ABI are unchanged.

The dedicated source-free control publishes complete class/template/alias and
platform API graphs, checks metadata mutations, removes provider source, imports
two independent consumers, and checks both ordinary native execution and exact
diagnostic failures for obsolete or unavailable API uses. It covers pure C++,
Objective-C++ ARC/MRC and C++20/23. These are focused development controls;
current-source SDK consolidation and unrelated remaining feature work remain open.

Focused checks pass: all four source-free diagnostic configurations, all four
union-template configurations (including move-only and throwing-copy controls),
12 selected existing template/native policy regression groups, 91 LLVM AST
infrastructure tests, production codec controls and 22 related tooling tests.
The selected regression groups are not a full facade/compiler or SDK acceptance
run. Those complete matrices remain consolidated in CI.
