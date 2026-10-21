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
value contract; this does not add move-only task completions.

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

Extension ABI 17 is a hard compiler/header/interface/PCH/provider cutover. Every
artifact must be rebuilt. The runtime task service remains ABI 3 and promise
layout is unchanged; the reserved preflight/resume handshake is the changed
compiler contract. The installed task header explicitly rejects ABI 16.

Source-free packages preserve checked aggregate, complex and member-pointer
result identities through indirect calls, actor hops, provider removal and
relocation. Their C++ declarations remain subject to the
[bounded aggregate interface contract](linux_cxx_interfaces.md). Header/PCH
consumers additionally exercise non-default-constructible records and standard
library values.

Pure Objective-C async lowering, Objective-C async methods and Blocks, compiler
actor isolation and the public async/await capability remain separate compiler
milestones. Native AArch64 support requires qualification on a real native host.

Qualification is in progress; this document does not yet declare a qualified
ABI 17 archive.
