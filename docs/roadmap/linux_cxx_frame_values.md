# C++ values in explicit native frames

The source concurrency module 2.5.0 extends explicit `__objc3_async_frame`
definitions to nontrivial C++ value parameters and results. The installed SDK
remains the qualified [2.4.0 group-scope delivery](../support/linux_cxx_group_scope_acceptance.json)
until this implementation span completes validation and coordinated packaging.
Service ABI 4, frame ABI 1, extension ABI 21 and interface schema 2.1 are unchanged.

```objective-c++
#include <objc3c/runtime/objc3_task.hpp>
#include <memory>
#include <string>

std::string describe(std::unique_ptr<int> value) __objc3_async_frame {
  return std::to_string(*value);
}
```

The factory constructs an independent frame owner from each ABI value parameter
before initial suspension. Move-only parameters are admitted; const parameters
retain ordinary copy construction. Construction failure propagates through the
synchronous factory and unwinds previously constructed frame captures, the
allocation and original ABI parameters. Unstarted disposal and pre-entry
cancellation destroy captures without entering the source body.

The reserved `__objc3_async_frame_capture<T>` and
`__objc3_async_frame_value_ops<T>` templates use native C++ construction and the
bridge's existing typed result operations. Sema instantiates them, preserving
overload resolution, access, deleted-member checks, constraints and implicit
special-member definitions. Code generation resolves those ordinary template
specializations after source parsing, specialization and PCH replay; it does
not synthesize a byte copy for a nontrivial C++ value.

Results require copy construction and nonthrowing destruction, matching the
shared task result contract. Parameters require construction from their value
parameter and nonthrowing destruction. Result copy failure preserves the bridge's
service status, including out-of-memory. Native return initialization, await
copy-out and lexical destruction establish independent owners. Annotated group
scopes retain pending results and frame captures through complete child
quiescence. Over-aligned types keep their native alignment.

Maintained witnesses cover lazy disposal, move-only parameters, const values,
nested native awaits, pre-entry cancellation, throwing partial capture
construction, result-copy failure and scoped finalization. Focused compiler
fixtures cover missing storage hooks, rejected ownership contracts, dependent
definitions and PCH replay.
The rebuilt compiler passes all four focused regressions. The maintained
source/PCH spawn matrix passes 24 configurations / 108 commands in each normal,
ASan/UBSan and TSan mode (72 / 324 total). GNU-backed sanitizer runs reuse the
unchanged qualified service libraries and instrumented GNU runtime variants;
all clients and new frame operations are rebuilt with instrumentation.

Integration also disables ordinary NRVO storage reuse in native frames. The
physical return slot contains the frame carrier, so a logical C++ result local
must retain its own correctly aligned storage, including across an await.
An over-aligned record regression covers this distinction. SDK qualification
and delivery remain pending this coherent source checkpoint.

Reference parameters, implicit C++ `this`, dynamic stack storage, and nontrivial
C++ class publication through the bounded semantic-package projection remain
separate work. The latter currently restricts exported classes to its established
aggregate/out-of-line-method contract. Ordinary source/header C++ ABI use of
these frame values does not promote package or full-profile admission. Public
async/actor and Core/Strict/System conformance predicates remain reserved.
