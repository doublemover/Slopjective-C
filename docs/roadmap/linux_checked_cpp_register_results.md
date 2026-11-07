# Checked native C++ register results and template Blocks

Source 2.133 / System 2.59 completes checked native C++ record transport across
register, mixed-register, ignored-empty and indirect return classifications.
Trivial records, scalar unions and resource-owning `trivial_abi` records now share
success-only result binding through direct/indirect/member/Block calls, ordinary
properties, discarded values, references and `tryOptional`.

Trivial records acquire no destructor cleanup. Indirect failure leaves the
caller's result storage untouched. Direct register failure supplies defined
physical ABI bits after abandoning any constructed return owner; the caller
checks the hidden error output before binding a C++ value. Typed and native
return-cleanup failures preserve exact ownership. Nil checked message/property
receivers retain the explicit service-failure policy and create no result value.
Potentially throwing nontrivial destructors retain their qualification boundary.

Published checked C++ record signatures, nested callable types and bodies require
`objc3.checked-cxx-record-results.v1`. Ordinary `trivial_abi` record attributes are
now modeled and retained by declaration-contract printing, including inherited
attributes. Replay preserves register classification and destructor-for-call
traits. Removing the attribute changes the semantic fingerprint. Four C++20/23
ARC/MRC profiles reject older readers and missing/invented capability claims.

Template instantiation also preserves a Block's own callable effect signature
before transforming captures and its body. Typed error types are substituted and
unqualified consistently; instantiated parameter declarations are retained in the
signature. A checked expression therefore sees the Block's throwing context,
including dependent error types, without inheriting an outer function's effects.
Native typed Blocks, source-free template replay and AST controls defend this
boundary.

All 98 new positive compiler commands and the repaired prior boundary controls
pass. The earlier full 529-case run had three stale expected rejections; all three
now verify the admitted behavior. All 54 AST controls and the new source-free
profiles pass. Twenty-four adjacent frontend groups and 320 exact decoder replays
passed before the template repair; updated frontend/compiler consolidation is
running. Four selected ASan/UBSan configurations / fourteen commands pass with the
complete native record and dependent Block witness, including optimized MRC PCH
without elision and both ownership modes with C++-only exceptions. Scalar,
integer-pair, mixed floating, large, empty, union and owning register records
retain exact values, error identity and owner balance.

Extension ABI 24, native frame 1, services 4 and schema 2.2 retain their layouts.
General C++ carrier runtime encodings, mixed payload calls, broader lifetime and
profile work, final matched analysis and one replacement SDK remain batched.
