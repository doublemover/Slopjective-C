# Borrowed loans after manual Objective-C release

The System 2.2.0 source span tracks the native instance `release` method family
in mandatory Strict System borrowed-pointer checking under MRC. Releasing an
owner expires loans obtained through known aliases of that object. Conditional
releases retain the hazard at flow joins. A later acquisition or copied raw
alias cannot manufacture a new ownership claim from the consumed claim.

The flow keeps pointer identity separately from the claim's live/unowned state.
Implicit Objective-C `self` parameters now start with their incoming owner
identity, so receiver-derived loans also undergo the mandatory check.
Fresh owner assignment replaces that state. Earlier `withLifetime`/`keepAlive`
pins retain independent claims and continue protecting existing and new loans;
a pin created after the release is rejected. When an earlier pin ends, a
consumed source claim cannot revive its loans. Loan reads, calls and pointer
access retain their existing expiry checks.

Nested Block and C++ lambda checks inherit the claim state, including raw
init-capture aliases. Bodies which first introduce borrowing also trigger the
enclosing capture analysis. ARC release rules are unchanged; this span targets
manual instance-release messages, including lexical super lookup.

The analysis remains conservative about independent manual retains, ambiguous
owner aliases and indirect/interprocedural effects. Raw runtime release calls,
Core Foundation/foreign ownership, consumed arguments/receivers and their
isolation boundaries need their own effects. This span does not claim complete
MRC lifetime analysis or full Strict System conformance.

Maintained witnesses cover C17/C++20/C++23 MRC, source/PCH replay, known aliases,
conditional release, fresh assignment, independent/late/ended pins, new loans,
raw alias copying, pointer reads and nested Block/lambda bodies. Compiler patch
identity requires dependent PCH, objects, interfaces and packages to be rebuilt.
Native extension ABI 21 and runtime ABI layouts are unchanged. SDK 2.33 remains
the accepted installed archive while further implementation is frontloaded.
