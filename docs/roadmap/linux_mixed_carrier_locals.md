# Mixed ARC and C++ carrier locals

The production compiler implementation now composes direct `Optional` and
`Result` trees whose alternatives contain ARC pointers and ordinary C++ objects.
The installed recursive carrier qualifier contains the execution matrix for this
work. Its result and the resulting SDK archive must be checked before describing
the change as delivered in an installed release.

```objc
using Managed = Optional(id);
using Native = Optional(Tracked);
using Outcome = Result(Managed, Native);

Outcome owned = Ok(Managed, Native, Some(id, object));
Outcome native = Err(Managed, Native, Some(Tracked, Tracked(7)));
Outcome copied = copyValue(native);
Outcome moved = moveValue(owned);
```

The admitted storage is an initialized automatic local, including an explicitly
constructed nested carrier subobject. Each direct carrier tree selects one
ultimate leaf. Strong objects, strong Blocks, zeroing weak pointers and unsafe
pointers retain their native ARC storage conventions. A C++ leaf uses its actual
copy or move constructor and destructor. The compiler resolves transfer recipes
recursively in Sema, including access, deletion and const move behavior; it does
not synthesize carrier record special members.

Construction commits each discriminant after its selected child succeeds. A
failed leaf constructor does not create a completed carrier. A completed value
is protected while full-expression temporaries are destroyed, so a throwing
temporary also destroys the selected ARC or C++ payload exactly once. Borrowing
`hasValue` or `isOk` reads the existing value's outer tag without transferring
its payload.

C++ destructors remain native cleanup actions in source order. Managed carrier
releases follow the existing source cleanup queue when a surrounding scope has
deferred actions. Immediately after initialization, the compiler snapshots the
active leaf identity by reading only the discriminants along the active path.
The native C++ action and the strong/weak release partitions share that stable
selection and the original payload storage. Later cleanup actions do not cause
the partitions to reclassify a carrier by reading a changed tag. Mutable moves
retain native moved-from storage and preserve source discriminants.

The existing general ARC operation and call ABI predicates remain unchanged.
Mixed values do not acquire implicit assignment, by-value callable transfer,
array storage or enclosing record fields through this local lifetime work.
Explicit and implicit ARC Block captures now compose the selected-arm construction recipes
with independent native/managed cleanup records; their [capture contract](linux_explicit_capture_lists.md)
also covers creation moves, heap copies, rollback and deferred managed releases.
C++ lambda captures remain outside that admission. Mixed
match snapshots retain selected ownership through arm evaluation. Prvalue tag
queries are also outside this slice. Those uses
produce diagnostics before an unqualified native operation is emitted. An
ordinary C++ payload that itself hides ARC storage or another carrier remains
outside the leaf qualification proof. MRC and volatile mixed carriers remain
rejected.

The installed suite exercises public source in C++20 and C++23 at O0 and O2,
with separately compiled reference providers and PCH consumers. It includes
strong, weak, unsafe and Block alternatives; native C++ copy/move/destructor
counts; failed transfers; initializer rollback; native destructor exceptions;
and the ordering of C++ actions and managed releases around `defer`. Rejection
cases cover implicit assignment, callable ABI, arrays, fields,
lambda capture, MRC and volatility. Qualification records bind the actual installed
compiler, frontend, runtime, fixture contents and SDK seal.
