# Linux checked async value results

The reserved Objective-C++20/23 Linux x86-64 GNUstep 2.2 async stage admits
checked C++ record, complex and member-pointer results. The canonical callable
still carries its logical result and exact async/throws effects; the physical
call returns `objc3::task<R>`.

A checked await inspects completion before constructing its logical result.
Checked failure enters the enclosing propagation, optionalization or force
operation without calling `await_resume`. No default value, record constructor,
copy or destructor runs for an absent result. Records with a deleted default
constructor use their ordinary native copy/move and destruction rules on
success. Task completion and copied task handles retain the existing copyable
value contract; this does not add move-only task completions. Cross-mode
aggregate signatures must agree on native C++ ownership and special-member
semantics; direct object-pointer normalization does not rewrite aggregate fields.

The compiler serializes the reserved awaiter preflight alongside the ordinary
resume expression. The checked operation owns its awaiter and argument
temporaries until their normal cleanup point. Result temporaries become active
only on the successful edge. Propagation, optional failure, native exceptions,
cancellation and frame abandonment therefore retain distinct cleanup paths.
Checked errors continue to bypass native exception handlers; native exceptions
and cancellation bypass checked handlers and optionalization.

`__objc3_try_optional` can initialize a direct nonvolatile automatic Optional
with a C++ record payload. Its tag becomes engaged only after payload
construction succeeds, and only an engaged payload is destroyed. This reuses
the existing bounded C++ carrier lifetime rules. Nontrivial C++ payload
matching, arbitrary carrier storage and unqualified carrier copying do not gain
new lowering support. Complex and member-pointer payloads use their ordinary
trivial storage operations, including volatile complex transfers.

Checked value results introduced extension ABI 17 and its reserved
preflight/resume handshake. The current [Objective-C method stage](linux_async_objc_methods.md)
advances the compiler/header/interface/PCH/provider contract to ABI 18. Rebuild
every artifact. The runtime task service remains ABI 3 and promise layout is
unchanged. The installed task header explicitly rejects ABI 17.

Source-free packages preserve checked aggregate, complex and member-pointer
result identities through indirect calls, actor hops, provider removal and
relocation. Their C++ declarations remain subject to the
[bounded aggregate interface contract](linux_cxx_interfaces.md). Header/PCH
consumers additionally exercise non-default-constructible records and standard
library values.

[Objective-C async methods](linux_async_objc_methods.md) use the same checked
value and Optional lifetime rules through dynamic, protocol, superclass and direct
messages. Pure Objective-C async lowering, async Blocks, compiler actor isolation
and the public async/await capability remain separate compiler milestones. Native AArch64 support requires qualification on a real native host.

The [installed SDK acceptance receipt](../support/linux_async_objc_methods_acceptance.json)
binds this contract to the exact compiler, runtime, source inputs and sealed
ABI 18 archive. It records 215 compiler tests, 57 AST tests, 318 current tooling
tests, 24 frontend tests, eight GNU runtime tests, 37 installed driver/embedding
checks and all 22 installed package gates.

Sixteen C++20/23, O0/O2 configurations cover every ARC/MRC provider-consumer
pair. Eight LLDB cases inspect resumed record and string values in addition to
the scalar, owned-result, checked-handler and member frames. Real PCH consumers
exercise inline checked record relays, over-alignment, copied handles, checked
failure before result construction, optional lifetimes, native exception boundaries and
cancellation. Source-free packages execute 24 times in eight configurations.

The same archive passes 74 instrumented configurations and 356 commands, four
C++/extension-ABI checks and six post-hop debugger cases. Fresh Debian 13 and
Ubuntu 26.04 consumers run as ordinary users under the explicit
`task-destinations` scope; all 22 package gates run on the publication host.
Sanitizers instrument services and clients; GNU libobjc is loaded as shipped.
The [latency report](../support/linux_async_objc_methods_latency.json) records
five quiet repetitions against the existing release budgets.
