# Linux shared async frame and result ABI

Extension ABI 20 and task service ABI 4 introduce a common lazy frame carrier
and typed completion contract for C17, Objective-C17 and C++20/23. Rebuild
compilers, headers, objects, PCH files, semantic interfaces and packages together.
The task header requires the qualified compiler's type-identity builtin.

This milestone supplies the runtime boundary for native Objective-C async
lowering. Objective-C17 providers in this milestone implement resume callbacks
in C; the reserved async syntax still requires Objective-C++20/23. Public
async/await and actor capability predicates remain reserved.

## Frame ownership

`objc3_async_frame` carries a context pointer, resume/destruction callbacks and
the declared result type. It exposes no C++ promise layout. A C provider can
return this carrier to a C++ caller, and a C++ coroutine can export the same
carrier to C.

```cpp
#include <objc3c/runtime/objc3_task.hpp>

objc3_async_frame frame = make_frame_in_c();
auto pending = objc3::task<int>::from_native(&frame);
auto running = std::move(pending).start(executor);
```

Import consumes the carrier only after validating its type. The inverse
`release_native()` transfers ownership to C. `task_handle<T>::adopt_native()`
and its `release_native()` transfer one owned running-task reference.

`objc3_async_frame_start` and `objc3_async_frame_start_child` clear the carrier
only on success. Failed starts preserve its owner. Disposing an unstarted frame
clears the carrier before invoking destruction, so reentrant disposal is safe.
An all-zero carrier is empty. C source must move these carriers explicitly;
copying their bits does not create a second owner.

The existing task scheduler owns successful starts. It serializes resumes,
preserves actor destinations, wakes cancellation and destroys frames exactly
once. A private child can inherit its parent's destination through the same C
API used by C++ awaits.

## Typed completions

`__builtin_objc3_type_identity((T *)0)` produces a canonical native type identity
without evaluating its operand. C and C++ aliases of the same native type agree.
Top-level qualifiers are omitted; owned object and Block representations supply
their own ownership. Pointer operands for Objective-C retainable types must
spell their ownership explicitly.

A result descriptor combines that identity with its size, alignment and value,
object, Block or void representation. Runtime validation compares identity bytes
and layout rather than descriptor addresses or copy-function addresses.
Providers must agree on complete named-type definitions, as with a native C ABI;
this descriptor does not establish compatibility between conflicting definitions.

Task start copies the declared identity before scheduling work. Successful
completion must match it before any payload copy runs. Completion also owns its
identity independently of the producer's frame. Zero, false and nil successes
retain payload presence; void success has a typed descriptor and no payload.

`objc3_task_get_result_type` makes a declared type available before completion.
`objc3_task_copy_result` checks type and layout before invoking the consumer's
copy operation outside runtime locks. Failed copies leave no newly owned value.
Cancellation and failure return separate statuses and preserve the task's
inspectable completion and declared result type.

Objects and Blocks use a shared owned-pointer payload. Copying retains an object
or heap-copies a Block; destruction releases that independent owner. C copy-out
returns one owned pointer. C++ reads retain their existing autoreleased +0
contract. MRC code must retain a read value before a later suspension drains its
execution-slice pool.

Checked Objective-C errors and native C++ exceptions retain their separate
transport behavior. C can inspect a failed completion without interpreting a
private C++ exception payload. A C++ caller continues to receive native exception
rethrowing through the established task bridge.

## Qualification

Development qualification passes 20 mixed-language configurations and 114
commands, including ten resumed-frame debugger cases. C providers compile as
C17, Objective-C providers as Objective-C17, and consumers as C++20/23 or
Objective-C++20/23 at O0/O2 with all ARC/MRC provider-consumer pairs.

Execution covers moves, abandonment, failed starts, reentrant destruction,
typed handle adoption, zero results, records, complex values, 256-byte-aligned
values, void, objects, Blocks, nil, private child awaits, actor destinations and
worker cancellation. Mismatched types reject before copying. Mutable starting
descriptors do not change stored type identities; failed consumer copies and
reentrant copy callbacks preserve task ownership and avoid runtime locks.

The existing async regression suite passes all 16 configurations and 104
commands. ASan/LSan/UBSan and TSan pass 40 additional bridge configurations with
both services and clients instrumented. Sanitizer libraries come from the pinned
LLVM resource directory and are identified in the receipt; GNU libobjc is loaded
as shipped. Value-only C/C++ bridge clients are checked to link without libobjc.

Full installed SDK, source-free package and fresh-distribution qualification are
the next release gates. Native Objective-C syntax lowering, public await-required
calls, recognized task spawning and compiler isolation remain subsequent work.
