# Native async exports to C

Source 2.240 / System 2.166 implements explicit C completion exports for
native async frames. The SDK header `<objc3c/runtime/objc3_async_c.h>` declares a
typed `f_completion_t` and `f_async_c` from the original parameter list and
result type. Objective-C and Objective-C++ providers use
`<objc3c/runtime/objc3_async_c_export.h>` to define the corresponding symbol.

```objc
#include <objc3c/runtime/objc3_async_c_export.h>

OBJC3_ASYNC_C_DECLARE(increment, int, int value);

static int increment(int value) async { return value + 1; }
OBJC3_ASYNC_C_DEFINE_VALUE(increment, int, increment(value), 0, 0, 0, int value)
```

A C consumer includes the shared declaration and passes the original arguments,
an opaque context and a typed callback. Use the void declaration and definition
forms to omit the result parameter. The object and Block definition forms load
the corresponding owned pointer representation. The provider validates the
native result descriptor before it submits the frame. The explicit checked
argument describes the source effect. Pass zero executor and actor arguments
to select the global executor, or select one destination explicitly.

Successful submission clears the carrier before scheduling. The scheduler holds
the task and its native result through one callback. The callback may run before
the thunk returns. It receives the original opaque context, which the caller
owns. Retain objects and errors and copy Blocks inside the callback to keep
them afterward. Unconsumed MRC source parameters retain their borrowing
contract, so the caller keeps those inputs valid while the frame uses them.

Status values are 0 for success, 1 for error and 2 for cancellation. Successful
callbacks receive no error. Checked Objective-C errors reach the callback as
their original objects. Other checked failure transport maps to a stable
Objective-C error value. Cancellation carries the distinct process-wide
`Error` value recognized by `objc3_async_c_error_is_cancellation`. If nonthrowing
work cannot start or fails inside the scheduler, its callback records
cancellation. A null callback or a mismatched frame result violates the export
contract.

The native test compiles providers as Objective-C C17/C23 and Objective-C++
C++20/23 in ARC and MRC. It removes the provider source before compiling the
separate C caller. The caller checks the aggregate and void callback signatures,
retained object, Block and checked error lifetimes, nil success, cancellation
before entry, a closed actor and 256 global-worker callbacks whose contexts are
freed during delivery. All eight provider/consumer configurations pass locally.

Compiler synthesis from a source declaration, automatic module export
metadata, other async representations and full SDK and native ARM acceptance
remain open.
