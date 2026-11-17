# Native C overloadable packages

Source 2.184 / System 2.110 completes the source-free C overloadable lane that
remained after the Source 2.94 declaration/predicate work. Marked functions retain
native overload selection in C17 and Objective-C, including one unmarked
fallback, conditionally enabled entries, ordered predicate priority and hidden
object-size parameters. Distinct candidate identities retain distinct native
module-owned symbols and exact canonical source bytes.

Direct-import collision checks and source-directed visibility checks share one
metadata rule: distinct function candidates may coexist in C when the pair
contains a native marked entry. Two incompatible ordinary C entries still
conflict. Marked candidate rows require `objc3.c-overloadable-functions.v1`;
missing capabilities, older readers and forged non-function owners reject.

Native AST import now permits marked candidates and their fallback to retain
separate declarations across provider ASTs. Ordinary C type conflicts remain
conservative. Qualified module lookup forwards the complete legal candidate set
to Sema, including function-address selection, instead of diagnosing C overloads
as ambiguous before resolution. The existing C inline-definition protocol keeps
one physical provider definition per overload.

Six native configurations cover unoptimized/optimized C17, Objective-C MRC and
Objective-C ARC. They remove provider source, import independent providers into
two consumers, and execute direct/indirect calls, fallback selection,
conditional priority, cross-provider predicates, hidden size arguments and
inline overloads. Objective-C cases also compile and execute source-directed,
module-qualified calls and function addresses. Negative controls cover disabled
predicates, incompatible arguments, unavailable entries, plain C conflicts and
reader cutovers. AST controls import independent providers, destroy their ASTs,
then replay the combined overload/fallback graph; a separate control preserves
ordinary C conflicts. All six native configurations, all 97 LLVM AST controls,
all 20 selected publication groups, complete direct-import/source-bridge/codec
tests and 26 tooling checks pass (one optional skip). Maintained patch replay,
feature authority, undefined-name and whitespace checks pass.

Broader declaration/predicate forms and full language/profile conformance remain
open. These checks do not qualify a fresh SDK; SDK 2.62 / ABI 21 remains the last
accepted archive until the current-source replacement passes delivery gates.
