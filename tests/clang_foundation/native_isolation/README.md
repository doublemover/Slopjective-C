# Native executor and actor entry witnesses

All 48 source/PCH execution configurations pass normally and under ASan/UBSan
and TSan, including mixed provider/consumer ownership. The installed SDK feature
gate also passes; see the [integration receipt](../../../docs/support/linux_native_feature_integration_acceptance.json).
Compile
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
source-free contract preservation. The C++ task caller also enters a C provider's
actor and checks destination restoration and independent result ownership.
Public actor/Strict Concurrency conformance remains gated by the capability matrix.

The 2.30 source additions exercise record arguments/results, array fields,
caller restoration, actor entry and cancellation before target execution.
`owned-packet.m` is compiled independently under ARC in every client/provider
combination and proves managed record ownership without granting MRC aggregate
admission. `cxx-packet.mm` supplies move-only parameter and successive throwing
construction witnesses. Both implementation sources are removed before client
and PCH compilation. Their expanded validation is recorded separately from the
earlier 48-configuration acceptance above.

The 2.31 source additions compile `consuming.m` under ARC independently of the
other provider's ownership mode. It checks transferred object/Block parameters,
consuming receivers, retained self results and native-frame targets. Receiver
and argument snapshots survive later operands clearing their original owners;
throwing later arguments unwind the snapshots. Each successful, cancelled or
allocation-failed entry has exact destruction and target-invocation checks.
