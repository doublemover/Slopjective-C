# Native checked C++ messages and ordinary properties

Source 2.131 / System 2.57 extends native indirect C++ checked results to ordinary
Objective-C instance, class and super messages, including discarded results,
automatic references and `tryOptional`. Existing success-only result binding,
argument ownership and typed/native failure cleanup apply to each dispatch.

Ordinary property syntax now projects a synchronous checked getter into its
`try` expression while retaining the pseudo-object receiver bindings. The getter
runs exactly once. A failed getter produces no C++ object and does not acquire a
result destructor; a successfully constructed result remains protected through
receiver-temporary cleanup. Source-free replay preserves the written property
syntax and the native checked operation.

A nullable Core receiver is evaluated before its arguments. After argument
construction, nil dispatch throws `objc3::service_failure` with
`OBJC3_SERVICE_INVALID_ARGUMENT`; the C++ result remains uninitialized. Argument
construction faults retain their original identity and cleanup. `tryOptional`
does not swallow this native runtime failure or turn it into a typed object
error. Strict profiles continue to require their existing nonnull receiver proof.
The runtime adds a separate archive member for this bridge and shares the public
exception definition with the host compiler. No C ABI callback throws through
an application callback boundary.

Published bodies using these operations require
`objc3.checked-cxx-messages.v1`; exact identity/capability checks reject older
readers and missing or invented claims. Declaration-only methods do not invent
a body requirement. Four C++20/23 ARC/MRC source-free profiles cover ordinary
messages, properties, discarded values and optional results.

The maintained native witness injects typed and C++ failures across seven
message/property routes, nullable receivers, argument construction and receiver
temporary destruction, with exact owner balance. It also extends composed
carrier-record allocation to runtime-sized arrays, including zero elements and
constructor-body failure at every position. The underlying completed-prefix
cleanup owns only successfully initialized elements.

All 49 dedicated compiler commands, 524 executable compiler cases (527 discovered,
three unsupported), all 52 AST controls, 22 frontend groups, codec/isolation,
280 exact production decoder replays and the complete sixteen-configuration
ASan/UBSan matrix / 56 commands pass. Source/PCH, ARC/MRC, C17/C++20/23,
C++-only exceptions and disabled elision are covered. A standalone host-C++
link/run also verifies the runtime bridge without Objective-C linkage.
Native trivial/register returns, throwing destructors,
mixed carrier runtime encodings and other unqualified shapes remain separate
implementation work. Extension ABI 24, native frame 1, services 4 and schema 2.2
retain their layouts; public runtime header digests change. One matched SDK
follows the implementation batch.
