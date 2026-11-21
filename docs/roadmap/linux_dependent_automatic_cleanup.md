# Dependent and deduced automatic cleanup storage

Source 2.156 / System 2.82 extends native automatic cleanup to dependent C++
template storage, dependent named callbacks and deduced `auto` storage.
Template definitions retain a declarative recipe without guessing whether the
callback takes a value, reference or address. Instantiation substitutes the
callback and rebuilds the complete native call against the actual local type.
Explicit function-template callback arguments retain their selected function.

`auto` and `decltype(auto)` deduction happens after declaration attributes.
Mandatory function checking therefore resolves pending deduced-storage recipes
after their initializers, before ownership/CFG analysis or code generation.
A concrete function cannot retain an unresolved cleanup call. Invalid
initializers do not acquire a callable cleanup action.

Concrete validation still requires complete fixed-size automatic storage,
a synchronous ordinary unary callback and direct value/reference/address
binding without synthesized record copies or ARC writeback. References,
atomic/VLA/address-space storage and async/checked-throwing callbacks retain
their diagnostics. Constructor failure does not call cleanup; normal exits,
exceptions and C++ destructors retain ordinary native ordering.

Publication binds the deferred callback expression and its lexical template
parameters. `objc3.native-storage-lifetime.v3` rejects older readers before
replaying the new bodies. Runtime service, native frame and extension ABI
layouts remain unchanged.

Twelve compiler/PCH commands pass across C++20/23 ARC/MRC with class-trait and
explicit function-template callbacks, value/reference/address bindings, ordinary
and template `auto`, incompatible callbacks, forbidden record-copy cleanup and
deduced-reference rejection. The three preceding cleanup-ownership compiler
controls also pass. Six publication profiles and four source-free native
provider/two-consumer configurations exercise the new forms, exact cleanup and
destructor order, original exceptions and failed initialization. Decoder and
isolation controls pass. All 69 LLVM AST infrastructure controls pass against the
changed production translation units, including callback context/location and
deferred-to-concrete recipe checks. Complete rebuilt compiler/SDK acceptance
remains pending.
