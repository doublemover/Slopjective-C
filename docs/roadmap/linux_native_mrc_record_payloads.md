# Native MRC C++ carrier records and ordinary value construction

Source 2.153 / System 2.79 admits C++ Optional/Result payload records whose
Objective-C object and Block fields follow the source's MRC lifetime policy.
Native special members perform their declared copy, move and destruction;
raw fields acquire no implicit ARC operations. This includes explicitly written
owning wrappers, trivial unowned views, nested carriers, enclosing records and
explicit union lifetimes. Direct MRC managed scalar carriers and C records keep
their existing admission rules.

The same native construction recipes now apply to local and temporary values,
heap and placement construction, fixed and allocated arrays, and ordinary C++
library storage. Const rvalues select an accessible copy constructor. Functional
casts preserve a replayable type spelling and fresh-value classification; owned
coroutine await results transfer as fresh values, while reference results retain
their alias semantics. Selected special members retain access, deletion,
exception and destructor contracts.

Published declarations and bodies require `objc3.native-mrc-record-payload.v1`.
Owning C++ closures independently require `objc3.native-lambda-argument-abi.v1`:
AST import now preserves their indirect argument ABI instead of manufacturing a
second unconstructed owner through register coercion. Function-template import
also probes different overload heads without issuing spurious ODR errors.
Older readers and missing or invented capability claims fail closed.

Native witnesses remove the provider source before compiling two independent
consumers. Exact object reference counts and C++ lease counters cover function,
message, Block, lambda, byref, union and array storage. Generic controls include
`std::optional`, `std::vector`, temporary lifetime extension, heap and placement
construction, discarded values and full-expression cleanup. Scalar `delete`
now destroys the selected payload before deallocation, sharing the array and
explicit-destruction protocol. Fault controls cover throwing copies/moves, heap Block copy,
byref promotion, aggregate prefixes and partial array construction. Task controls
cover owned await/group results, errors and cancellation before entry.

Interface reparsing preserves sanitizer feature state because native standard
headers can select different declarations and allocator paths under sanitizers.
The header manifest also binds C prerequisites reached first through exact pinned
runtime headers, preserving the same inventory when standard headers include
them later through a guard. Arbitrary lookalike headers remain ordinary package
inputs. Focused tests cover address, undefined, thread and CFI sanitizer configurations.
Syntax-only reparsing retains CFI feature state without requiring a link/LTO
configuration. Constant array bounds retain evaluated type identity when native
printing replaces a written `sizeof`/constexpr expression with its extent.

The [development receipt](../support/linux_native_mrc_record_payloads_progress.json)
records focused scope and the matrices delegated to CI. Local checks target
these changed behaviors. Full compiler/frontend/runtime
matrices, analysis and final SDK delivery belong to the consolidated CI runs;
no new matched SDK is claimed by this development span.
