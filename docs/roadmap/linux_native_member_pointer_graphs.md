# Fixed native member-function pointers

Source 2.238 / System 2.164 extends immutable native target discovery to exact
member-function pointers. Const local/global bindings, reference aliases and
pure same-target alternatives retain their canonical method, owning class,
prototype, cv/ref qualifiers, calling convention and effects. Object and pointer
invocation spellings share the existing per-call argument mapping.

Receiver-independent nonvirtual methods and final virtual targets participate in
bounded scalar argument/return, reference-cell, global-write and returned-storage
graphs. An address qualified with a base name does not suppress virtual dispatch
when invoked through a member pointer. Mutable/volatile pointers, weakened
prototypes, owner conversions, differing targets, unknown definitions, receiver
side effects and discovery exhaustion cannot establish a closed target proof.

Known lambda captures and unproved scalar receiver state retain conservative
value sources even when a fixed member target cannot prove a precise projection.
Boolean observations stay independent. This does not implement arbitrary mutable
member-pointer sets, open virtual resolution, receiver mutation summaries or
foreign ownership contracts; it changes no physical ABI.

Maintained source/PCH controls cover native object/pointer invocations, reference
writes, globals, returned references, same/differing targets, exact/weakened
noexcept, final/open virtual dispatch, resource observations, lambda operators,
receiver fields and alias bounds. Source-free public classes and helper bodies
retain member-pointer calls in C++20/23 ARC/MRC consumers. Focused verification
results: all 504 new/selected adjacent source/PCH commands, six expanded
source-free ARC/MRC configurations and 100 focused tooling checks pass. The
preceding compiler misses seven new transfer diagnostics and rejects two ordinary
fixed-target uses. A full isolation regression is running; matching SDK, complete
profiles and native ARM qualification remain pending.
