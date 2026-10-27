# Published Block and statement expressions

The concurrency 2.24.0 source milestone models native Block literals in published
C++/Objective-C++ expressions under `objc3.cxx-block-expressions.v1` and GNU
statement expressions under `objc3.cxx-statement-expressions.v1`. Containing
records independently advertise expression capabilities from fields, methods
and friend definitions.

Block identity binds native function types, lexical parameters, capture kinds,
byref/nested/resource state, effective capture types and initialization/copy
recipes. Dependency checks retain public signatures and captures while treating
Block parameters and local bindings as local declarations. Native ownership and
transfer admission rules continue to apply.

The native printer now retains Block bodies, explicit results, function effects
and `__block` storage. Canonical source reparses these executable definitions;
AST import verifies their complete typed contracts before backend registration.
Statement expressions retain local initialization styles, copy-elision flags,
typed result construction and referenced declaration identities. Native Block
prototype source locations also retain their imported parameter links.

Source fixtures exercise ordinary/explicit move captures, nested Blocks, shared
byref mutation, default arguments/member initialization and statement-expression
results with nontrivial C++ lifetime. Provider sources are removed before
consumer compilation. The [2.29.0 source milestone](linux_cxx_local_types.md)
subsequently adds concrete named local type definitions. Generic closures
remain separate implementation work; this does not establish full ownership,
async/task or language/profile conformance.

Combined optimized checks additionally repair lexical initialization of
module-owned inline variables and preserve native lexical mangling for closure
and local-class members. Virtual deleting-destructor thunks retain their native
structor variant so virtual-base deletion also deallocates storage; the source
fixture checks the class-specific deallocator independently of destructor counts.

The accepted SDK remains [2.9.0](../support/linux_cxx_class_structure_acceptance.json).
Combined source/sanitizer matrices and installed/archive qualification remain
scheduled for the coordinated source-batch cutover.

The [expression progress receipt](../support/linux_cxx_local_expressions_progress.json)
records the native/frontend batch and C++ language/ownership/optimization matrices.
