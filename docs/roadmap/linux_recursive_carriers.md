# Linux recursive managed carriers

Extension ABI 7 extends active-arm ownership through nested Optional and Result
values. The Linux profile remains x86-64 GNUstep 2.2 with ARC. Rebuild providers,
consumers, interfaces and PCH with the same sealed SDK.

## Ownership and control flow

Construction commits an outer tag after its selected child finishes initializing.
Copy, move, assignment and destruction follow every selected child tag before
touching a leaf. Inactive union storage is never treated as an object or weak
registration. Strong objects and Blocks use native ARC operations; weak leaves
use address-sensitive weak initialization, copy, move and destruction. Const moves
copy their const source. Mutable moves preserve tags and clear owning leaves.
Nested plain arms retain their ordinary value behavior.

Assignment snapshots the complete selected RHS before evaluating the destination
or destroying its old arm. Functions, callbacks, C++ virtual and member-pointer
calls, Objective-C messages, direct methods and Blocks retain the existing
caller-owned argument and fresh-result contract. Nil receivers produce zero empty
carriers. Completed results remain callee-owned until return cleanup finishes.

Managed match statements and scalar match expressions capture one owned snapshot
of their scrutinee. Immutable payload bindings borrow that snapshot, so guards and
bodies can replace the original source without invalidating bindings. The snapshot
cleans up after the match, including return, typed-error propagation and native
Objective-C/C++ exception exits. Weak snapshots remain weak and observe zeroing.

`__objc3_try_optional` can now wrap managed carrier results. Success initializes the
child in the outer Optional's final payload storage; failure publishes none after
the callee's empty-result path. Discarded results and presence queries retain normal
full-expression cleanup. Failed propagating assignments preserve the destination.

## Runtime identity and bounds

Runtime encodings use versioned `__objc3_v7_optional`, `__objc3_v7_result` and
`__objc3_v7_payload` names. Nested arm names contain an `N` marker and SHA-256 of
the child's complete encoding. The body embeds its native boolean/union layout.
This preserves ownership identity while keeping a linear nesting chain linear in
encoding size. The shipped runtime verifies recursive size, alignment, type
skipping and method argument/return reflection.

Recursive lowering limits depth to 64 and payload visits to 256. Runtime encoding
also bounds depth and expanded carrier visits. Unsupported shapes fail compilation;
they cannot fall back to byte copies. AST operation facts reuse shared child types
so repeated `Result<T,T>` types do not cause exponential analysis.

Arrays, enclosing records, ivars/properties, carrier value captures, volatile
managed values and managed MRC transfers remain outside this contract. The next
storage milestone must add element/subobject operations, partial-initialization
cleanup and capture helpers before admitting those forms. Public Optional/Result/
throws predicates remain reserved.

## Qualification

SDK publication requires `qualify-nested-carrier-calls.py`: 65 commands, nine
executions and six forced-failure traps. It compiles separate providers and
consumers at O0/O2/ThinLTO in Objective-C C17 and Objective-C++ C++20, repeats
semantic imports at O0/O2, and consumes C++ PCH. Every executable passes DWARF
verification. The witness covers nested strong/weak/Block and plain arms, Result
transitions, self-assignment, const and mutable moves, nil receivers, reflection,
guard mutation, typed errors, discarded results and native unwinding. The four
earlier installed call suites remain mandatory.

Live clangd probes cover recursive values in both languages. Fresh Debian 13 and
Ubuntu 26.04 archive qualification runs all installed suites as an ordinary user
after relocation. The [acceptance receipt](../support/linux_recursive_carrier_acceptance.json)
binds the source, SDK seal, archive and qualification evidence.
