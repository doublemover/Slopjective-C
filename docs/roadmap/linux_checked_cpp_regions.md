# Checked C++ initialization regions

Source 2.125 / System 2.52 extends native checked-result ownership to conditional,
comma, aggregate, base-subobject and constructor initializers. Destination guards
follow checked/await expressions through enclosing initialization without scanning
unexecuted callable bodies. Each destination becomes owned only after its own
construction completes; native prefix cleanup retains partially initialized fields
and arrays.

Lifetime-extended reference temporaries now retain both normal-propagation and
exceptional provisional cleanup. Successful full-expression completion records a
handoff before destroying argument temporaries. Normal completion preserves the
reference's extended lifetime, an earlier typed exit destroys its completed
referents, and a throwing argument destructor retains exceptional destruction.
Permanent and provisional cleanups share one construction flag and clear it
before destruction, preventing duplicate cleanup.

Dynamic construction keeps its matching deallocator active across typed failure,
including conditionally evaluated allocations and partial arrays. Static and TLS
value initialization protects completed objects until initializer temporaries have
finished and the permanent destructor can be registered. Failed initialization
can be retried without retaining an earlier constructed owner.

The maintained native witness covers exact ownership across these paths, including
nested value arrays, multiple reference members, move construction and native-await
aggregate/reference initialization. The source-free and production decoder controls
also include the published conditional, aggregate and reference expressions.
Consolidation covers all 514 executable cases in the 517-case compiler suite
(three unsupported cases); the initial run's three findings pass targeted reruns
following annotation-name ownership and stale expectation repairs. All 48 AST
controls, sixteen ASan/UBSan native configurations / 56 commands, eighteen frontend
groups, codec/isolation controls and 180 exact production decoder replays pass.
The native matrix includes exception-only and elision-disabled execution. The
compiler sanitizer additionally exposed a deferred virtual-thunk annotation name
that outlived its stack mangling buffer; deferred names now use AST-owned storage
and the IR regression requires both thunk annotations. These are development
source receipts, with one matched SDK delivery still deferred until implementation
consolidation. Public extension ABI 24, native frame 1,
runtime services 4 and semantic schema 2.2 remain unchanged. General nullable
synchronous Objective-C record sends, throwing-destructor/register-ABI results,
other construction/lifetime regions and final SDK consolidation remain separate.

Source 2.247 / System 2.173 admits synchronous checked indirect C++ results
whose native destructors can unwind. The callee owns the result until its
return-expression and lexical cleanups finish. An exception from an argument
destructor destroys that result before the native exception reaches the caller.
Result destruction clears its ownership flag before invoking the destructor.
Checked failure before construction transfers the error with no result object.

Four independently linked source-free C++20/23 ARC and MRC consumers verify
the checked error identity, local and discarded result destruction, and
argument-destructor exceptions in both caller and callee. The callee case
caught a leaked constructed result before the return cleanup guard was enabled.
Four compiler controls preserve rejection of throwing-destructor `try?` Optional
payloads in static and thread-local regions. Throwing-destructor register ABI,
Optional payloads and async value-operation transport still require their own
lifetime coverage. The maintained AArch64 patch applies after this change;
native archive qualification remains separate.
