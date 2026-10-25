# Linux C++ coroutine lexical autorelease pools

The Linux SDK implements suspension-safe `@autoreleasepool` scopes in
Objective-C 3.0 C++ coroutines, including ordinary C++ coroutine syntax. Source
identity is `llvmorg-23.1.3-0d261d1ca552-e4f1a276e662-00b75dc3`. The
[compiler-milestone receipt](../support/linux_async_lexical_pool_acceptance.json)
binds that archive to its compiler, runtime, sanitizer and fresh-distribution
reports. The [current delivery](../support/linux_native_group_iteration_acceptance.json)
rebuilds the compiler foundation with native C group iteration, awaited carrier
ownership and physical alias checks. It retains native task/group joins and the
Linux native ARC-only runtime policy; its complete compiler and installed SDK
checks pass. Fresh Debian 13 and Ubuntu 26.04 qualification both pass.

Each lexical scope owns a frame-resident pool-token slot. A potentially
suspending await drains active pools from inner to outer before `await_suspend`
can publish the frame. Slots are cleared before object destruction. Suspended
frame destruction therefore never pops a token belonging to another thread.

The ready/resume path reopens only closed slots, outer to inner, on the actual
continuation thread. An already-ready await keeps its existing pools. A vetoed
suspension may perform an extra drain and then recreates the lexical scopes.
An EH cleanup owns a snapshot of the active slot addresses and restores pools
if draining or `await_suspend` throws, including a catch inside the same pool.
Normal and exceptional lexical exits close the current token exactly once.
The C frame implementation retains its existing suspension protocol and shares
the lexical scope emitter.

Focused development checks pass: compiler build, the new C++ pool IR test and
existing C pool IR test, plus C++20 ARC/O0 and C++23 MRC/O2 native async preflights.
The runtime witnesses cover nested pools, ready/vetoed/throwing awaits,
cross-thread resume, suspended destruction on another thread, six nested scopes,
exceptions and existing manual/worker/actor executor hops. The complete compiler run passes 356 regressions and 166 native contracts;
all 16 native C++ async sanitizer configurations and all 33 installed SDK gates
pass. The exact archive also passes fresh Debian/Ubuntu qualification, including
ordinary-user consumers; owned containers are removed.

This changes code generation without adding an AST statement or runtime record
layout. Native extension ABI 21 and the existing task/frame service versions
remain unchanged. Delivery uses a coordinated compiler/SDK cutover; PCH/PCM build guards require
matching source identity.
