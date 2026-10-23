# Native C17 async definition witnesses

These files are authored development fixtures; execution is intentionally delayed.
Compile `provider.m` as Objective-C 3 C17 with blocks and native Objective-C
exceptions enabled. Exercise ARC and MRC provider/consumer pairs, O0/O2, source
inclusion and a PCH of `provider.h`.

`client.m` consumes logical async declarations and the C runtime API. It checks
lazy abandonment, failed starts preserving their owner, parent cancellation
while a private child is queued, actor destinations, loop-local continuation
state, exactly-once lexical cleanup, retained object and copied Block results,
checked propagation and suspending typed recovery. Native Objective-C exceptions
must produce cancellation and bypass checked errors.

`client.cpp` independently consumes physical frame declarations with the public
C++ task bridge. Build it in C++20 and C++23. The provider's private frame layout
and C++ promise representation must remain absent from the consumer interface.

Use the installed runtime/header owner when later constructing commands. These
fixtures add no qualification receipt or SDK acceptance claim.
