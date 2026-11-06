# Native C++ task result ownership

Concurrency source 2.102 extends initializer, retained Objective-C and plain
opaque Core Foundation result contracts to the native C++ task ABI. It also
constructs typed lazy zero results for eligible task messages to nil.

Initializer receivers keep independent entry, frame and manual-body claims.
Returning self, nil or a replacement preserves the declared retained result.
MRC retained object returns balance the source +1 after the promise has acquired
its own object or Block claim, including exceptional promise initialization.
Awaited `instancetype` results retain their concrete receiver type in both
native-frame and C++ task callers. Runtime descriptor checks still use the
producer's actual logical type. Same-executor calls use a typed task projection;
cross-executor calls keep the native-frame bridge.

A reserved template hook installs declaration-specific CF operations on the
concrete promise after construction. It uses ordinary template resolution and
instantiation; the compiler does not infer promise layout. Retained CF returns
transfer a +1 to the promise. Borrowed returns acquire a claim before replacing
old storage. Publication copies through the same operations and promise teardown
releases its independent claim. Raw pointers without a CF result annotation keep
their ordinary pointer operations. Null values never call CF retain/release.

Native-frame return storage is established before the source full expression
ends. This keeps a CF await's copy-out claim alive while an isolation forwarding
frame acquires its result, including when the producer's original owner has
already disappeared. Source-expression cleanup and Block construction rollback
remain active through this handoff.

Nil task messages build the common typed native frame and adopt it through the
installed task constructor hook. Normal, direct and actor dispatch evaluate
arguments once, dispose consumed object/Block/CF claims, and skip the source
body. Scalar, object, Block, void and trivial record results keep their native
logical descriptors. Allocation, cancellation and destruction use the normal
frame implementation. Nontrivial C++ construction and resource active-arm
semantics remain separate policies.

The source-free contracts cut over to `objc3.async-consuming-ownership.v3` and
`objc3.async-entry-abi.v3`. Signature and published-body requirements cover task
initializers, retained object results and both CF result conventions. Older
reader declarations are rejected. The public physical frame ABI remains 1,
service ABI 4 and extension ABI 22.

The maintained native fixture exercises native-frame, crossing task and
same-executor task callers under separate ARC/MRC compilation. It includes
receiver replacement, retained/borrowed/null results, independent consumed
receiver/CF ownership, nil result kinds, pre-entry and suspended cancellation,
completed-result cancellation and removal of borrowed source owners. Compiler
controls cover C++20/23, ARC/MRC, source/PCH/debug IR and missing SDK hooks.
The twelve source/PCH/debug commands and sixteen missing/malformed-hook controls
pass. Three focused native configurations pass 64 commands and 672 new scenarios
each; the fixture also passes a local ASan/UBSan diagnostic run against the
existing service library. All 71 affected compiler regressions and seventeen
feature-authority/codec controls pass. Frontend sources compile; full matched
frontend/package execution, instrumented services and SDK acceptance remain in
the consolidated verification phase.
