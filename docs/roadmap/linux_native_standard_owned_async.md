# Owning standard-library values in source-free native async packages

Source 2.168 / System 2.94 carries native C++ storage and async lowering
instantiations into source-free consumers. This is implementation work against
extension ABI 24; matching compiler/SDK consolidation remains separate.

Sema uses temporary calls to instantiate the SDK's reserved capture, value-ops,
private-result descriptor, CF/NSObject promise-ops and nil-task conversion
helpers. Those temporary calls are discarded and therefore cannot be discovered
by walking the exported function body. The interface importer now imports each
instantiated definition from the canonical interface AST before backend delivery.
It requires the exact qualified runtime task header and checks that the imported
function retains its body and primary template. Unrelated application templates
cannot supply these dependencies. `objc3.native-cxx-async-import.v1` makes this
consumer requirement explicit, including ordinary C++ coroutines and nil async
Objective-C messages in synchronous functions.

The maintained LLVM importer also preserves C++20's special-member eligibility
and prospective-destructor selection. Importing all destructor declarations while
losing the selection flag could make `variant` and `expected` destruction call
an unselected declaration without an instantiated definition. The separate
`objc3.cxx-special-member-selection.v1` capability covers this requirement for
ordinary synchronous templates as well as async payloads. Selection is retained
from the native AST rather than inferred from declaration order.

The native regression publishes the actual runtime header and standard-library
contracts, removes the provider source, imports into two independent consumers
and links and executes them with the provider. It covers ARC/MRC owning leaves in
`std::optional`, `std::variant` and C++23 `std::expected`, caller-owner release
before execution, copies, resets, arm changes, empty/error arms, original native
exceptions and pre-entry cancellation. Private local result types, nil scalar
async messages, retained foreign promise results and custom constrained
destructors exercise the related lowering and selection paths. Object and C++
lifetime counts must return to zero. Production decoder checks reject missing
requirements and readers lacking either capability; namespace claims cannot
fabricate either capability.

The AST infrastructure regression imports both selected and unselected
prospective destructors in C++20/23, into empty and pre-parsed template contexts,
then destroys the source AST and checks the actual selected declaration.
Broader vendor/profile, sanitizer, host and sealed SDK consolidation remain
separate gates.

The focused checkpoint passes all four registered C++20/23 MRC/ARC O0/O2 native
configurations, 84 LLVM AST infrastructure tests, production codec controls and
29 related tooling checks. Maintained patches reproduce the changed compiler
and test sources byte-for-byte and their manifest digests agree. These checks
link the changed LLVM objects with the existing SDK libraries; they do not
constitute a newly sealed SDK acceptance result.
