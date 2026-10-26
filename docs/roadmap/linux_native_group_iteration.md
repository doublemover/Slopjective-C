# Native C group-result iteration

The Linux SDK adds completion-order iteration through the existing shared
frame ABI. The [acceptance receipt](../support/linux_native_group_iteration_acceptance.json)
binds that compiler, runtime, installed checks and exact archive. The original
concurrency module version was 2.2.0; the current [scope SDK](linux_cxx_group_scopes.md)
ships 2.4.0. Service ABI 4, frame ABI 1, native extension ABI 21 and
interface schema 2.1 are unchanged.

```objective-c
#include <objc3/concurrency.h>
OBJC_TASK_GROUP_NEXT_DECLARE(next_count, int);

int total(objc3_task_group_t *group) __objc3_async_frame {
  int sum = 0;
  for (;;) {
    Optional(int) item = __objc3_await next_count(group);
    if let value = item { sum += value; } else { break; }
  }
  __objc3_await objc3_task_group_join_async(group);
  return sum;
}
```

The factory retains the group immediately, before returning a lazy frame. Its
first resume closes the group tree and registers a nonblocking next waiter.
Direct tasks are reserved in completion order, with the existing FIFO waiter
policy. Nested groups are not flattened. Even ready values and exhausted groups
queue continuations on the caller's inherited destination. Exhaustion returns
`None`; a selected success returns `Some(value)`. A present nil object or Block
remains `Some(nil)`. `Optional(void)` supplies a tag-only success. Next does not
replace the separate quiescence join.

`OBJC_TASK_GROUP_NEXT_DECLARE_THROWING(name, T, E)` preserves the checked error
domain. A failed or cancelled child forwards its original owned error or
cancellation, rather than constructing an optional value. A canonical type
mismatch leaves the child available for a matching consumer. Before reservation,
cancellation consumes nothing; after reservation, cancellation or copy failure
consumes that item and releases its reserved owner during frame destruction.
Cancelling or disposing this adapter does not explicitly cancel the shared
group; ordinary last-owner rules still apply.

The compiler builtin `__builtin_objc3_async_group_next_plan((Optional(T) *)0)`
returns immutable metadata generated from the canonical AST carrier representation.
The operand is unevaluated. The plan carries the actual element/result descriptors,
complete result copy/destruction operations, payload offset and tag offset. The
runtime validates the plan and copies descriptor identities/operation tables;
it never borrows a caller output slot across suspension or guesses an optional
layout. Factory failure leaves output storage unchanged. Static logical aliases
bind to local physical shims, avoiding an exported reference to an internal
factory. Alias diagnostics compare the shared physical frame return when one
side is a logical frame callable; unrelated argument and logical-result
mismatches remain diagnosed. Checked synchronous aliases retain their hidden
error-output parameter contract; aliases between different logical error
domains are also diagnosed.

Plain scalar, pointer, void, aligned record and nested plain optional values
are admitted in C mode. Owned object/Block and nested managed carriers require
ARC and supported strong storage. Weak/unowned payloads, scalar resources,
nontrivial enclosing record payloads and other unsupported ownership shapes are
rejected before iteration metadata emission. C++ retains its existing typed
`group.next<T>()` API.
These bounded helpers do not promote full async/actor/profile predicates.

Native frame awaits now participate in ARC carrier initialization guards.
Copy-out establishes ownership before child cleanup dispatch, and discarded
managed results receive a full-expression cleanup. This applies the established
carrier lifetime model to awaited values rather than copying an owning carrier
as raw bytes. Qualified enclosing managed records use their private frame
result transport and the same destroy operation for discarded results.
Their ordinary by-value parameter admission remains separate.

Runtime qualification covers descriptor copying, completion ordering, lazy close,
type rejection without consumption, cancellation on both sides of reservation,
allocation/copy failures, retained error identity and releasing original group
owners. The maintained spawn source/PCH fixture adds scalar, aligned record,
nested optional, void, ARC object/Block nil, discarded owned results, checked
error and actor-destination witnesses. Compiler regressions cover metadata
emission, unevaluated operands, unsupported ownership and physical frame aliases.
The frozen source passes all 24 spawn configurations / 108 commands normally,
under ASan/UBSan and under TSan. The instrumented shared C-frame matrix passes
40 configurations / 256 commands; coroutine/group and native C++ async matrices
pass 12 / 48 and 16 / 80 respectively. These sanitizer runs instrument GNU
libobjc, project services and clients together. All five standalone runtime
suites pass in all three modes. Three focused compiler regressions pass,
including ARC debug-info verification and checked alias diagnostics. The
[acceptance record](../support/linux_native_group_iteration_acceptance.json)
binds these source checks. The current rebuilt compiler passes 364 Clang/index/query
regressions and 166 native contracts; all 32 facade suites, nine GNUstep clients,
37 installed consumers, 13 selected integration nodes and all 33 installed SDK
gates pass. That exact archive also passes fresh Debian 13 and Ubuntu 26.04
qualification, including ordinary-user consumers and owned-container cleanup.

Native C-frame and ordinary C++ task-coroutine scope finalization are delivered
by the linked scope SDK. Remaining abstract group-library forms stay open.
