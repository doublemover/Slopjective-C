# Nil async results with construction or ownership obligations

`main.m` runs as Objective-C C17 or Objective-C++20/23 under ARC/MRC.
The optional `--feature nil-results` native feature matrix includes O0/O2
and matching source/PCH transport, with `allocations.c` linked into each case.
For direct PCH use, compile `runtime.h` in the matching header mode and define
`OBJC3_NONZERO_NIL_RESULTS_PCH` for the consumer.

The witnesses check typed lazy INVALID_ARGUMENT failures for scalar, array,
record and base resource leaves, nontrivial/default-initialized/constructor-only
C++ values, reference-bearing records and private nominal result types. Both
native-frame and C++ task entry representations are exercised. No logical
constructor, copier, destructor or resource cleanup may run. Rejecting frame
allocation during construction proves that failure carriers do not allocate.
Consumed CF arguments evaluate/release once, including null arguments; abandoned
and pre-entry-cancelled carriers preserve their distinct cleanup/outcome rules.
Plain scalar and non-owning resource-pointer records retain nil zero results.
Implicit synchronous actor/executor entries fail before a hop, preserve consumed
CF pin counts, and distinguish ordinary free executor calls with nil arguments.

Source-free interfaces require `objc3.async-entry-abi.v4`. Older readers and
missing-capability artifacts are rejected; regenerate dependent interfaces,
PCH, objects and packages. Physical frame/service/extension layouts are unchanged.
