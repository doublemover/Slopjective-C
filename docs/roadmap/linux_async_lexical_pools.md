# Linux C++ coroutine lexical autorelease pools

The next compiler span implements suspension-safe `@autoreleasepool` scopes in
Objective-C 3.0 C++ coroutines, including ordinary C++ coroutine syntax. This is
source development following the published SDK with source identity
`llvmorg-23.1.3-0d261d1ca552-49f322e67e0d-00b75dc3`; that archive does not include
this later compiler change.

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
exceptions and existing manual/worker/actor executor hops. Broader regression,
sanitizer and SDK qualification for this source span remain postponed.

This changes code generation without adding an AST statement or runtime record
layout. Native extension ABI 21 and the existing task/frame service versions
remain unchanged. Compiler source identity and PCH/PCM build guards still require
a coordinated rebuild when the source span is delivered.
