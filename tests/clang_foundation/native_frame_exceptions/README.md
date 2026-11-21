# Native frame exception transport and typed property values

Compile `main.m` as C17 or C++20/23 with Objective-C exceptions; C++ modes also
use C++ exceptions. ARC/MRC, O0/O2 and source/PCH are maintained by optional
`--feature exceptions`. Define `OBJC3_NATIVE_FRAME_EXCEPTIONS_PCH` for PCH use.

The C witness requires a foreign native exception to produce an owned service
error. Objective-C++ preserves the same object through the standard library's
native exception carrier. C++ controls cover direct body throws, local catches,
free/member/message/property isolation entries, typed rethrow into a catching
C++ task, preserved service errors and a distinct cancellation outcome.

Nontrivial copy/move and move-only property writes use exact live/destruction
counts. A separate ordinary property supplies the receiver/construction ordering
oracle. Throwing snapshots, setters, nil setters and constructed nil reads defend
partial cleanup and ownership. A controlled aligned nothrow allocation failure
rejects only owned error-payload storage and requires an allocation-independent
OOM error. Source-free providers and consumers require
`objc3.async-entry-abi.v5`; earlier readers reject the semantic cutover.
