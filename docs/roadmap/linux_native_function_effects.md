# Native function-effect package contracts

Source 2.188 / System 2.114 preserves Clang's native `nonblocking`,
`nonallocating`, `blocking` and `allocating` function-type effects in source-free
packages. These are separate from Objective-C async/checked-error effects,
C++ exception specifications and the function emission policies delivered in
Source 2.186. A callback cannot lose its effect requirement during AST import.

Native import copies every effect kind and imports dependent boolean predicates
into the target AST. Prototype storage owns the arrays. Structural equivalence
compares kinds and predicates, preventing incompatible callback types from being
unified. Ordinary functions, template/member declarations, Blocks and lambdas
print their native effects through canonical GNU syntax. Function-type printing
continues to retain effects in aliases, parameters, results and nested callbacks.

Publication detects effects in signatures and published bodies/defaults, binds
conditional predicates to typed declaration/parameter/template identities and
checks their public dependencies. Concepts and template/function requires clauses
also contribute their published effect contracts. Concepts, builtin templates and
template-template parameters have no owned callable signature; their own
expression/head identity handles their semantics without dereferencing an absent
templated declaration. `objc3.native-function-effects.v1` is a hard
reader cutover. Declaration markers and capability inventory must agree; forged
markers on unrelated declaration kinds reject. Nonpublished implementation bodies
do not create a package promise.

This contract retains native effect analysis and conversions; it does not make
an allocating/blocking implementation safe by annotation. Normal Clang diagnostics
still apply, including consumer calls and callback initialization. It does not
promote complete Strict Concurrency, general effect polymorphism or a replacement
SDK. Extension ABI 24, interface schema 2.2 and runtime layouts remain unchanged.
Matching SDK and native-host qualification remain pending.

Focused evidence includes nine executed C17/C++20/C++23 × pure/MRC/ARC
source-free native configurations. Providers are removed before two independent
consumers execute direct/record/nested-factory callbacks, conditional templates,
concepts, constraints, stored/factory lambdas and native Blocks. Negative controls
reject incompatible callback conversions/calls, excluded predicate dependencies,
older/missing reader claims and unrelated marker owners. A validly rehashed,
reparsable changed effect is rejected specifically by the type/effect identity
comparison. All 103 AST controls pass, including predicate ownership after source
AST destruction, native/Objective-C effect coexistence and structural inequivalence;
the 17-group import suite, source bridge, production codec and 26 related tooling
checks pass (one optional skip). Maintained patches replay byte-for-byte.
Focused object relinking does not qualify a replacement SDK.
