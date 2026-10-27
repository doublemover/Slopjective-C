# Borrowed owners consumed through local closures

System 2.10.0 propagates known captured-owner consumption from concrete local
Block and C++ lambda invocations into the enclosing borrowed CFG. A closure
which borrows before consuming its raw owner can run once; later invocation and
subsequent uses of unprotected loans diagnose. Consumption-only bodies also
invalidate loans acquired by their caller. Existing direct object/foreign and
canonical-family release contracts supply the effects.

Each cached body summary records borrowed owners, consumed owners and invoked
callable captures. Symbolic callable inputs preserve forwarding through nested
closures. Callable effects have separate storage, capture and evaluated-expression
facts: copying a closure or capturing it without invocation does not consume
anything. Local aliases, aggregate/array paths, conditional joins, representation
copies and overwritten callback slots follow the existing storage graph.

A concrete receiver body retains its ordered CFG check, so its first safe call
is accepted. Explicit borrowed arguments overlapping a consumed claim remain
rejected. Unknown noescape callers receive conservative may-consumption effects;
noescape alone does not promise at-most-once invocation. Reference/mutable/generic
precision and byref Block callable invocation retain qualification boundaries.
Independent owning-capture claims and general manual counts need further modeling.

ARC callback copy-in captures its original loan/effect facts before later
arguments can retarget the original strong slot. Out temporaries have no input
facts. Function, message and constructor writebacks weakly preserve tracked
input and intervening stores; opaque output identities and exceptional writeback
precision remain conservative. These facts do not infer arbitrary callee bodies,
heap/global mutation or lazy async result lifetime requirements.

Native witnesses cover C17/C++20/23, ARC/MRC and PCH, direct/repeated/copied/nested
calls, unused callable captures, local records/arrays, overwritten slots, borrowed
argument overlap, throwing before/after consumption, canonical releases and ARC
copy-in/out/writeback. Frontend witnesses repeat key negative and positive cases
and publish a safe concrete consuming closure in a source-free inline body.
Broader native/frontend/package validation remains batched after implementation.
Full Strict System conformance is not claimed. Runtime/extension ABI layouts
remain unchanged; compiler-dependent PCH, interfaces and packages must be rebuilt.
