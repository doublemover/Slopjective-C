# Typed nil native-frame messages

Concurrency source 2.101 gives eligible native-frame messages to nil a valid,
typed lazy frame. Awaiting that frame returns the ordinary zero logical result.
The implementation reuses native frame construction, allocation failure,
cancellation, copy-out and destruction. It does not return an invalid all-zero
carrier in place of an async value.

GNUstep message dispatch and direct-method nil prologues use the same owner.
The source method body never runs. Actor nil entries reuse the existing mailbox
lookup bypass. Source arguments still evaluate in caller order. Nil dispatch
balances consumed object/Block and CF claims even when a synchronous method has
no stored async function type. Function-type and parameter attributes both
supply consumption facts. CF operations continue to skip null.

Object, Block, integer, floating, void and trivially copyable record results
retain their logical result descriptors. Native-frame initializers preserve
`instancetype`; C++ task callers retain the typed forwarding bridge. Generated
nil helpers have artificial debug locations, including allocation calls to a
same-module allocator definition. Nontrivial C++ construction and resource
ownership are not invented by zero-initializing such a result.

`objc3.async-entry-abi.v2` is the source-free semantic cutover. The physical frame
ABI remains 1, service ABI remains 4 and extension ABI remains 22. Reader
registries, codec controls, fuzz reader declarations and its dictionary use v2.
A reader advertising only the prior semantic is rejected. Consuming ownership
continues to require its separate v2 semantic.

The maintained CodeGen fixture covers C17/C++20/C++23 under ARC/MRC, source/PCH
replay and debug IR, including a same-module allocator. All eighteen commands
pass. Native controls cover normal/direct/actor and synchronous nil messages,
initializer and scalar/object/Block/record/void results, consumed object/Block/CF
arguments, null consumed operands, cancellation and exact destruction. The
three focused native configurations pass 58 commands and 68 nil scenarios each,
alongside the preceding ownership witnesses. All 45 affected compiler regressions
and seventeen feature-authority/codec policy controls pass. All eight affected
frontend units and expanded emitter/isolation tests compile. Frontend source-free execution, complete
instrumented/package matrices and SDK acceptance remain in consolidation.

[Source 2.102](linux_task_result_ownership.md) adds the task-ABI nil counterparts
and cuts entry semantics over to v3. Focused native execution passes.
Nontrivial/resource result policies and other unqualified value forms retain
their separate implementation work.
