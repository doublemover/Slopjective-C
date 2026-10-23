# Native executor and actor entry witnesses

These maintained fixtures are authored; execution remains deferred. Compile
`provider.m` with Objective-C 3 C17, blocks, native exceptions and strict
concurrency, then `client.m` with the same settings. Exercise ARC/MRC provider
and client combinations, O0/O2, source declarations and a PCH of `provider.h`.
Link the installed runtime and GNUstep 2.2 Objective-C runtime.

The provider's real compiler-generated frames enter main/global/named
destinations, enter an explicitly associated actor, and assert the original
caller destination after every await. The client pumps only its documented
manual executors and checks exactly-once source argument evaluation, nil
messages, parent cancellation before child entry, lexical cleanup, and retained
object result destruction. The actor binding pins its receiver until the
mailbox is closed, quiescent and explicitly unbound; automatic allocation or
deallocation association is not claimed.

The existing `../isolation_contracts_test.cpp` covers source and IR diagnostics,
ARC/MRC C17/C++20/23 calls, exact named lookup bytes, PCH/import/ODR metadata and
source-free contract preservation. These witnesses add no qualification receipt
or acceptance claim.
