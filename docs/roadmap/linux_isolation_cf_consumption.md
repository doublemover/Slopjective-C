# Consumed CF arguments across native async entries

Concurrency source 2.97 adds `cf_consumed` arguments with plain opaque CF pointer
types to synchronous isolation thunks, native frames and C++ task coroutines.
The contract uses the supplied CF library's standard `CFRetain` and `CFRelease`
ABI. Other foreign ownership families retain their own resource/family contracts.
ARC does not take over the manual CF claim.

Caller operands acquire independent CF snapshots in source order. The entry
ramp adopts the transferred +1, including failure before frame construction.
Lazy frames retain independent pins before suspension or promise construction.
Only a body admitted by entry/cancellation checks receives its manual +1. The
callee balances that claim through its ordinary CF cleanup; cancellation before
entry never calls the callee to dispose it. Generated retain/release operations
skip null. CF ownership stays separate from Objective-C receiver ownership when
both kinds of consumption occur in one message.

Function-type consumption is preserved when a declaration uses a function
typedef. Parameter attributes and function-type facts both participate in
admission, generated signatures, snapshots and frame ownership. Definitions
retain the exact declared consumption contract. This also protects type-carried
`ns_consumed` contracts at their async entry owners.

Published async consuming signatures and bodies require
`objc3.async-consuming-ownership.v1`. Identities bind that requirement for native
function types, method receivers, nested published bodies and awaited isolation
calls. Codec, contract validation, import, artifact loading and resolution reject
missing or invented capability claims. Authored frontend controls cover
signature-only and body-only requirements, relocation/replay, private unpublished
bodies and older-reader rejection. Their native execution remains pending the
coordinated frontend rebuild.

The CF fixture supplies a small implementation of the standard CF ownership ABI
with independent live/destruction counters. ARC and MRC providers exercise
functions, class messages, combined consuming receiver/CF arguments, native
frames, C++ tasks and function typedefs. Controls cover null, pre-entry and
inter-frame cancellation, cancellation during a suspended body, first/second
native-frame allocation failures, unused tasks and throwing parameter copies.
The completion ledger records focused executions; these are compiler ownership
witnesses, rather than qualification of a complete CF framework.

CF result ownership needs distinct result storage and copy-out semantics and
remains rejected at isolated entries. NSObject-backed CF parameter types also
remain gated until their overlapping ARC/CF storage ownership is modeled.
Extension ABI 22 and runtime service/frame layouts are unchanged. SDK 2.62
remains the accepted archive until final consolidated delivery.
