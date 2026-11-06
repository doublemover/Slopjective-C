# NSObject-backed pointer ownership across native async entries

The current implementation span admits explicitly CF-owned `NSObject` pointer
arguments and results across synchronous isolation, native frames and C++ tasks.
Both `cf_consumed` and `ns_consumed` contracts are preserved for NSObject
pointers. Strong, weak and unsafe slots retain their own initialization and
destruction policy. A CF claim remains independent of ARC slot ownership. The entry ramp balances
the transferred CF claim, a lazy frame pins the original argument, and only an
entered body receives its manual claim. An ARC-managed frame slot separately
initializes and destroys its current value, including after reassignment.
Cancellation before entry never manufactures a body claim.

Ordinary typedef chains preserve `NSObject` storage identity. Explicit lifetime
qualifiers are admitted on these pointers, and C++ substitutions retaining a
lifetime qualifier remain recognizable after typedef sugar disappears. Record
flags follow those qualifiers so C++ union/optional special-member decisions
remain consistent. The task bridge keeps physical pointer slots explicitly
unowned and uses the native pointer predicate for ownership-qualified types. Explicit
`cf_returns_retained`/`cf_returns_not_retained` results keep declaration-specific
CF operations. NSObject results without a CF contract instead use Objective-C
reference operations with an independent result owner. Task promises install the
corresponding operations for their physical pointer storage. Pointer layout,
service ABI and task carrier layout remain unchanged.

Published types and bodies bind `objc3.nsobject-storage.v1`. The identity records
NSObject storage before canonical pointer erasure, including aliases, callable
positions and body-local uses. Missing or invented capability claims and older
readers fail admission. Maintained source/PCH, native alias/import and interface
controls cover both ownership modes, changed storage policy, replay/relocation
and capability rejection.

Native runtime fixtures cover consumed values, parameter replacement, retained,
borrowed, null and ordinary NSObject results; native and C++ task callers on the
same and crossing executors; cancellation before entry, inside the body and after
publication; original-owner disappearance and unused entry disposal. The fixture
uses real Objective-C objects behind its standard CF ownership ABI and checks
exact destruction independently of the compiler's ownership callbacks. The
expanded matrix authors 1,084 scenarios per configuration, including strong, weak
and unsafe consumed parameters and unused-entry disposal.

Four focused compiler regressions pass, including eighteen source/PCH commands,
explicit lifetime/union checks and invalid-bridge recovery. An initial native
configuration passes its original 436 scenarios; the expanded synchronous and
retained-result configuration also passes. All 940 initial scenarios pass after repairing weak-slot pin/handoff lowering.
The final 1,084-case expansion also passes at C17/MRC/O0/source; it adds
CF-consumed weak/unsafe slots. C++20/MRC/O2/source and C++23/ARC/O2/PCH also pass: 3,252 new
scenarios across three configurations, with 70 complete isolation-suite commands. Four native AST type/import/identity controls pass. Compiler/frontend syntax checks pass; final
frontend/package and SDK acceptance remain pending. SDK 2.62 is unchanged.
This does not complete unrelated managed-union storage, unmodeled foreign-family
operations, implicit template erasure of arbitrary ownership typedefs or full
Strict Concurrency/System conformance.
