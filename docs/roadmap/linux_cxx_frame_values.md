# C++ values in explicit native frames

The concurrency module 2.5.0 extends explicit `__objc3_async_frame` definitions
to nontrivial C++ value parameters and results. The
[acceptance receipt](../support/linux_cxx_frame_value_acceptance.json) binds final
source, compiler, runtime matrices, installed SDK and exact archive evidence.
The preceding [2.4.0 scope delivery](../support/linux_cxx_group_scope_acceptance.json)
retains its historical evidence.
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

Private record and enum results also use `__objc3_async_frame_result_type<T>`.
Native frames and typed task adapters share the same descriptor and nominal
identity token within a translation unit. Private types in distinct translation
units remain distinct. These private C++ results require the task header even
when their storage is trivial; the C-only header cannot supply the shared C++
type identity. Private record/enum and PCH witnesses cover this boundary.

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
The rebuilt compiler passes all five focused regressions. The maintained
source/PCH spawn matrix passes 24 configurations / 108 commands in each normal,
ASan/UBSan and TSan mode (72 / 324 total). GNU-backed sanitizer runs reuse the
unchanged qualified service libraries and instrumented GNU runtime variants;
all clients and new frame operations are rebuilt with instrumentation.

Integration also disables ordinary NRVO storage reuse in native frames. The
physical return slot contains the frame carrier, so a logical C++ result local
must retain its own correctly aligned storage, including across an await.
An over-aligned record regression covers this distinction. The rebuilt compiler
passes 367 Clang/index/query regressions and 166 native contracts. All 32 facade
suites, nine GNUstep clients, 37 installed consumers, 13 integration nodes and
33 installed SDK gates pass. The exact archive passes pinned Debian 13 and
Ubuntu 26.04 as an ordinary user, including relocation, source-free packages
and debugger consumers. Both owned containers are removed.

The [2.6.0 delivery](linux_dynamic_frame_storage.md) continues with dynamic
frame storage. Borrowed C++ references and implicit receivers are implemented by the
[2.7.0 source span](linux_cxx_frame_borrows.md). Nontrivial C++ class publication through the bounded semantic-package projection remains
separate work. The latter currently restricts exported classes to its established
aggregate/out-of-line-method contract. Ordinary source/header C++ ABI use of
these frame values does not promote package or full-profile admission. Public
async/actor and Core/Strict/System conformance predicates remain reserved.
