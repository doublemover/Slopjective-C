# Borrowed callable results and owner contracts

Current execution checkpoint: the maintained native frontend replay, identity and
consumer controls pass in the ABI 24 development embedding. See the
[2.110 consolidation](linux_publication_and_byte_ranges.md). Source-free package
execution, wider matrices and installed SDK qualification remain pending.

System 2.19 makes a borrowed result an obligation of the
native function type. A separate owner index records which argument supplies its
lifetime. `objc_returns_borrowed(owner_index=N)` applies to function types,
including function pointers, Block pointers, member pointers and aliases. An
unannotated borrowed-result callback retains its nonescape obligation without
claiming an owner relationship.

The contract survives canonical type deduction, C++ template substitution,
partial specialization, overload resolution, AST import and PCH. Canonical call
results recover their borrowed qualifier. Different owner indices remain
different type identities, native ODR hashes, USRs and C++ overload symbols.
An explicit or implicit conversion cannot erase, invent or change an owner
relationship. The physical pointer layout and calling convention stay ordinary.

The mandatory Strict System CFG pass acquires loans from indirect calls using
the actual designated argument and preserves evaluated-owner snapshots. A
matching return contract can forward that result. Unknown-owner callbacks may
feed nonescaping consumers; their results cannot escape or establish a
particular owner's return proof. Dependent forwarding is checked again after
substitution, while known nondependent violations are diagnosed in the template
pattern. Static string data can satisfy a borrowed return because its storage
outlives the designated owner.

Published signatures and bodies require
`objc3.borrowed-callable-owners.v1`. Interface identity records the result and
owner facts independently, including callback contracts used only inside
published bodies. Readers reject absent or invented capability claims, and an
older reader cannot silently admit the new contract. Compiler extension ABI 23
rejects older PCH/AST products before reading the changed type properties.

Maintained controls cover C17/C++20/C++23 under ARC and MRC, direct source and
PCH, generic forwarding and automatic deduction, wrong-owner returns, consumed
owners, Block/member calls, erased/forged casts, canonical native identities and
source-free consumers. All nineteen borrowed compiler regressions pass, including the new eighteen
C17/C++20/C++23 ARC/MRC source/PCH commands and three C++ overload CodeGen
commands. Three native AST identity tests also pass, and the ABI 23 compiler
rejects an ABI 22 PCH before AST loading. Compiler/frontend syntax checks pass;
coordinated frontend/package execution and SDK acceptance remain pending.
The accepted SDK remains 2.62. This span does not infer arbitrary foreign/global
mutation effects or unknown interprocedural lifetime contracts.
