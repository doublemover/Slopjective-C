# Ordinary owner scopes and implicit Block cleanup

System source 2.42 makes every native catch handler activate the complete local
exceptional cleanup graph, including catch-all and scalar/reference bindings. An explicit defer, pool, monitor,
lifetime pin or allocation expression is no longer required. Objective-C and C++
catch handlers observe the end of inner owner lifetimes; independent outer owners
remain live. Ordinary source queue ordering still governs ARC release.

Strong ARC Block locals admit implicit by-value scalar, Objective-C owner and
primitive managed-record captures. Capture copy constructors must be implicit or
non-user-provided and nonthrowing; bases, unions, reference captures, nested Block
storage and user capture construction retain their existing proof boundaries.
The CFG registers heap copying as an independent operation before a binding is
initialized. A failed copy unwinds outer owners without destroying an
uninitialized Block slot. Successful Block slots join the native ARC release
queue. Strong slot copies and explicit/implicit zero initialization are included.
Weak or unsafe Block bindings require a separate stack/full-expression capture
cleanup model and remain gated.

Closure discovery now recognizes raw inline ivar addresses/array decay and
receiver-owned native results, even when no expression has a borrowed typedef.
These closures participate in future-owner summaries: MRC captures preserve their
non-owning lifetime obligation, while ARC capture copies hold independent owners.
This fixes a missed analysis entry rather than inventing ownership from a raw
pointer type.

Published bodies carry exact `objc3.automatic-owner-unwind.v1` identity and
capability agreement through emission, replay, codec, artifact/module loading and
imports. Independent AST path controls check local lifetime end before either
kind of catch on every exceptional path. Source/PCH controls cover C17/C++20/23,
ARC/MRC, closure copies, record captures, catch expiry and zero initialization.
Native witnesses check ordinary owner destruction before catch, Block-held owners,
record/slot copies and controlled heap-copy failure with exactly one destruction.
All changes preserve physical extension ABI 24 and the current runtime ABI.

Catch-all replay and standalone handler/finalizer printing also retain their
ellipsis, binding and complete body. All 59 affected compiler cases, six new
source-free profiles, seven adjacent frontend groups, codec/isolation controls,
24 AST controls and twelve instrumented native configurations / 42 commands pass.
The final matched SDK follows consolidated implementation.
