# Native C17 async definition witnesses

These files are authored development fixtures; execution is intentionally delayed.
Compile `provider.m` as Objective-C 3 C17 with blocks and native Objective-C
exceptions enabled. Link `foreign.cpp` into each client; it exports a C frame
from a C++ coroutine for cross-language await/error transport. Exercise ARC and MRC provider/consumer pairs, O0/O2, source
inclusion and a PCH of `provider.h`.

`client.m` consumes logical async declarations and the C runtime API. It checks
lazy abandonment, failed starts preserving their owner, parent cancellation
while a private child is queued, actor destinations, loop-local continuation
state, exactly-once lexical cleanup, retained object and copied Block results,
checked propagation and suspending typed recovery. It also checks copied async
Block captures and dynamic/protocol/direct/super/class method frames through the
C++ client, including receiver lifetime and nil-message failures. Native Objective-C exceptions
must produce cancellation and bypass checked errors.

`client.cpp` independently consumes physical frame declarations with the public
C++ task bridge. Build it in C++20 and C++23. The provider's private frame layout
and C++ promise representation must remain absent from the consumer interface.

`client.mm` consumes the persistent logical `__objc3_async_frame` declarations
in Objective-C++20/23. Its own async functions use the separate C++ task ABI and
await native functions, function pointers, methods, Blocks, object/record/void
results, checked errors, nil frames and foreign C++ errors through the explicit
frame adapter. A rejected type adoption must dispose a lazy method frame and
release its receiver exactly once. The shared header also supports PCH inclusion.

Use the installed runtime/header owner when later constructing commands. These
fixtures add no qualification receipt or SDK acceptance claim.
