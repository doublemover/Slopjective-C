# Native C group-result iteration

This source span adds completion-order iteration through the existing shared
frame ABI. The accepted SDK in the [handle-join receipt](../support/linux_async_handle_join_acceptance.json)
predates this span. The concurrency module source version is 2.2.0; service ABI 4,
frame ABI 1, native extension ABI 21 and interface schema 2.1 are unchanged.

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
mismatches remain diagnosed.

Plain scalar, pointer, void, aligned record and nested plain optional values
are admitted in C mode. Owned object/Block and nested managed carriers require
ARC and supported strong storage. Weak/unowned payloads, scalar resources,
nontrivial enclosing records and other unsupported ownership shapes are rejected
before metadata emission. C++ retains its existing typed `group.next<T>()` API.
These bounded helpers do not promote full async/actor/profile predicates.

Native frame awaits now participate in ARC carrier initialization guards.
Copy-out establishes ownership before child cleanup dispatch, and discarded
managed results receive a full-expression cleanup. This applies the established
carrier lifetime model to awaited values rather than copying an owning carrier
as raw bytes.

Runtime preflight covers descriptor copying, completion ordering, lazy close,
type rejection without consumption, cancellation on both sides of reservation,
allocation/copy failures, retained error identity and releasing original group
owners. The maintained spawn source/PCH fixture adds scalar, aligned record,
nested optional, void, ARC object/Block nil, discarded owned results, checked
error and actor-destination witnesses. Compiler regressions cover metadata
emission, unevaluated operands, unsupported ownership and physical frame aliases.
The complete validation batch follows the coherent source span; this document
makes no new SDK acceptance claim until that delivery finishes.

Automatic structured-scope finalization, cancellation-shielded caller unwind
and the remaining abstract group-library forms remain separate implementation.
