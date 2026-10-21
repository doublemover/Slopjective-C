# Linux Objective-C storage and shared captures

Extension ABI 11 extends qualified ARC carrier lifetimes into Objective-C ivars,
nonatomic properties, whole-record Block captures, and shared `__block` forwarding
cells on Linux x86-64 with GNUstep 2.2. Rebuild all dependent objects and semantic
interfaces for this hard cutover.

Runtime storage accepts bounded, recursively encodable carriers, fixed-size
arrays, and data records with native ARC fields. Runtime record signatures use
fresh indirect return storage and callee-owned incoming snapshots across C and
C++. Objective-C messages and Blocks use that convention for qualified data
records. Nil receivers consume argument snapshots and produce zero-initialized
results, including discarded results.

Synthesized property getters copy into fresh owned storage. Assignment owns the
RHS before evaluating a setter, and destroys that snapshot if a native exception
escapes. Weak reads acquire one stable value; weak registrations follow native
zeroing and object destruction. Matching and tag queries accept property
expressions, including template substitution. Semantic interfaces preserve ivar
access, native strong/weak qualifiers, array declarators, and effective property
attributes. Objective-C++ header PCH and mixed-language object linkage are
qualified; ABI 12 extends semantic interchange through the bounded
[C++ interface contract](linux_cxx_interfaces.md).

A by-value Block owns its carrier or record snapshot. A shared capture moves its
owned state into the runtime forwarding cell once, and all closures follow that
cell until its final release. Cleanup respects each field's ownership, including
weak payloads and nested arrays. Helper identities include structural layout and
ownership. Unsupported atomic managed properties, volatile storage, packed
records, shared arrays, and typed-throws record Block signatures produce source
diagnostics.

Construction establishes ownership of each completed ivar before destroying
initializer temporaries. Failure destroys the completed prefix in reverse order.
The runtime then destroys completed superclass state and frees the allocation.
Its allocation path rejects overflow before adding instance, extra, and reference
count storage. Permanent native tests exercise C++ and Objective-C throws,
multiple superclass levels, destructor order, catch balance, object release, and
oversized allocations.

The installed `qualify-heap-carriers.py` suite runs every C/C++ provider/consumer
pair at O0, O2, and ThinLTO, C semantic imports at O0/O2, and a C++ PCH consumer.
Its 15 executions verify DWARF; twelve rejected cases bound the supported subset.

```sh
python3 -B "$OBJC3C_SDK_ROOT/share/objc3c/examples/native/qualify-heap-carriers.py" \
  --sdk-root "$OBJC3C_SDK_ROOT" --output-dir ./heap-carrier-evidence
```

SDK packaging runs independent installed suites concurrently within the host CPU
and memory limit, waits for all gates, and retains separate logs and receipts.
Every gate must verify the same SDK seal, expected executions, and command
outcomes before the archive can be published.

Public Optional/Result, synchronous throws, async/await, and actors remain reserved
until their public syntax and contracts have complete installed evidence.

The [acceptance receipt](../support/linux_heap_storage_acceptance.json) binds
implementation commit `036c9cabb` to the ABI 11 archive, installed suites,
ASan/LSan, live editor and LLDB checks, and unprivileged Debian 13 and Ubuntu
26.04 qualification.
