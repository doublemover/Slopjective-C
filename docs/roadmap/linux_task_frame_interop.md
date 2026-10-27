# C++ task and native-frame interoperability

The concurrency 2.35.0 source span lets C++ native-frame bodies await logical
C++ task calls. Awaited executor/actor entries can also target supported C++ task
factories. A reserved task-to-frame bridge moves the existing lazy carrier into
the native await scope; it introduces no additional task service or carrier
layout. The moved-from C++ temporary owns no frame. Child registration, result
copying, checked errors, cancellation, cleanup and destination inheritance use
the existing runtime protocol.

```objc
#include <objc3c/runtime/objc3_task.hpp>

int isolatedTask(int value) noexcept __objc3_async
    __attribute__((objc_executor(main)));

int nativeCaller(int value) __objc3_async_frame {
  return __objc3_await isolatedTask(value);
}
```

Isolation retains the existing qualified value/ownership bounds and C++
nonthrowing factory requirement. Instance and named-class messages retain their
native dispatch; instance/class `super` sends retain lexical lookup and captured
`self`. Actor children enter the retained mailbox, and callers recover their
original destination after completion. Qualified record values, object results,
ARC consumed arguments and typed object errors share the preceding frame rules.
Direct nonisolated task calls use ordinary C++ coroutine borrowing rules.

Checked-error lookup strips implicit C++ task temporaries from adapter arguments
before resolving the logical call. Source projection recognizes both adapter
directions. Published bodies select the complete task header when their written
await uses the C++ task ABI, including a native-frame-only public API. Pure frame
bodies do not acquire a task dependency merely because the producer included an
unused task header. Runtime-header provenance and required
`objc3.cxx-async-definitions.v1` semantics continue to bound publication.

Objective-C 3 mode also admits `ns_consumed` on Block parameters. Earlier Block
witnesses exercised captured ownership while Clang ignored that parameter
attribute. The new regression requires the attribute to survive parsing and
checks native Block copying. The existing adopting ramp and frame ownership
then apply to the actual consumed convention.

Maintained witnesses cover direct task calls, record transfer, actor entry,
object results, suspending typed-error recovery, ARC consumed objects/Blocks,
inherited class dispatch and lexical instance/class super calls. Failure cases
cover pre-entry cancellation, cancellation while the C++ task is suspended,
failed entry allocation and exact record cleanup. Compiler replay covers direct
C++20 MRC and C++23 ARC PCH inputs, including message and super entry.

Source qualification is in progress. SDK 2.33 remains the accepted archive.
Service ABI 4, frame ABI 1, extension ABI 21 and interface schema 2.2 remain in
use; regenerate compiler-dependent inputs for the source cutover. MRC/CF
consumption, init-family entry, unqualified dynamic Class receivers, automatic
actor association, runtime/custom awaiter publication and full profile
conformance remain separate work.
