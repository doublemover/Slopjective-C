# Ownership

Ownership support is source-derived. The public Objective-C 3.0 memory model is
limited to ownership facts that are present in checked-in sema, lowering,
runtime, fixture, and test contracts; generated reports are evidence only.

The formal issue `#8166` slice is
`objc3c.ownership.memory-model.formal-slice.v1`. It covers strong or owned
retainable values, weak and unowned storage qualifiers, borrowed references,
consumed retainable values, autoreleased results, ARC helper boundaries, and
runtime-owned borrowed public API data.

Consumed retainable values transfer ownership at consumed call boundaries. A
consumed binding is unavailable until explicit reassignment reacquires owned
storage, and use-after-consume remains a deterministic `O3S308` rejection.

Borrowed references are non-owning. They may flow through parameters declared as
borrowed and through returns tied to a valid owner parameter. Escaping through an
unproven parameter, escaping block capture, or untied borrowed return is rejected
by the ownership diagnostics (`O3S298`, `O3S299`, `O3S300`, or `O3S307`).

ARC boundary lowering is the supported helper-backed slice only:
retain/release/autorelease, weak property hooks, autoreleasepool scope lowering,
block capture lifetime cleanup, and block autorelease return handoff lower
through private runtime helpers. Full ARC automation, generalized local lifetime
inference, public ARC runtime ABI widening, cross-module ARC optimization, and
unsupported ownership-qualified signatures remain fail-closed.

Public runtime result and string contracts expose runtime-owned borrowed data.
Callers do not free borrowed result strings or selector spellings, and runtime
ownership diagnostics remain the boundary for unsupported public result
ownership crossings.

## Async ownership and concurrency boundaries

The cross-issue boundary contract is
`objc3c.ownership.concurrency-boundary.v1`. Ownership across `await`,
executor hops, and actor hops is not a public ARC runtime ABI. Strong and
precise-lifetime values that remain live after an `await` stay retained in
async-frame storage until the owning lexical cleanup runs, while autoreleased
temporaries remain bounded to the current task execution slice.

Public executor and actor hops consume the checked-in `objc3.concurrency`
module contract. They do not promote scheduler fairness, distributed actors,
Swift actor ABI compatibility, cross-process mailboxes, or generalized ARC
optimization into public ownership support. Unsupported ownership or
concurrency crossings remain fail-closed through the existing ownership and
actor diagnostics instead of being documented as supported behavior.
