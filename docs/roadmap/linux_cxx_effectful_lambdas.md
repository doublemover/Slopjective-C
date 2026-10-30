# Effectful C++ lambdas on Linux

Source **2.77** adds canonical native async/frame and typed-error suffixes to C++
lambda declarators, including closed generic local call operators. The native
printer preserves these effects after the ordinary lambda suffix and trailing
return type. Public closure contracts retain checked captures, typed heads,
instantiated bodies, runtime dependencies and task/frame entry semantics.

Published body traversal changes callable context only on entering a lambda body;
capture initializers retain their enclosing callable context. Deduced factory
results preserve authored `auto` spelling; closure result identity excludes physical
locations, and copied class-template captures bind canonical owners/arguments. Native await and
return syntax bind written operations, excluding implicit coroutine machinery.
The capability `objc3.cxx-effectful-lambdas.v1` follows complete published owners;
missing or inconsistent capabilities are rejected with `O3IFC086`.

The [focused source receipt](../support/linux_cxx_effectful_lambdas_progress.json)
records 412 compiler regressions, 26 focused native checks, all 40 native import-family
tests, the emitter suite, 84 tooling tests and 16 source-free configurations /
96 commands passing. Witnesses
cover suspended concrete/generic task and frame lambdas, captured scalar state,
returned concrete closures with deduced results bound to their published factory,
nontrivial C++ record captures with balanced lifetimes, typed
synchronous/asynchronous errors and source-free import. Runtime witnesses use
explicit logical result types for async/frame closures.
Broader validation and SDK delivery remain batched;
[SDK 2.62](../support/linux_source_batch_2_62_acceptance.json) remains accepted.
Remaining public/dependent closure/type projections, custom coroutine promises,
ownership, concurrency, isolation, key-path, language/library/profile work and
native Linux AArch64 remain open.

The [combined 2.74–2.77 source receipt](../support/linux_lambda_address_batch_progress.json)
completes the deferred source checks above: 412 compiler regressions, 226 native
unit tests, all 33 optimized frontend suites and 335 maintained tooling tests pass.
Normal, ASan/UBSan and TSan each pass 168 source/PCH configurations / 1,158 commands.
Source-free C++/key-path packages pass 80 configurations / 384 commands.
SDK delivery remains batched; SDK 2.62 stays accepted.
