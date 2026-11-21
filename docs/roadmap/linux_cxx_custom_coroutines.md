# Published C++ custom coroutine promises on Linux

Source **2.88** publishes C++ coroutine definitions with named
custom promise owners. Canonical interfaces retain written `co_await`, `co_yield`
and `co_return` syntax and reconstruct compiler-owned coroutine support through
`<coroutine>`. Generated frame temporaries do not become public declarations.

`objc3.cxx-coroutine-bodies.v1` binds resolved promise and awaiter customization
points to the declaration body contract. Promise construction, allocation,
initial/final suspension, yield/return, await transformation and exception hooks
undergo dependency/export checks. Missing capability or namespace ownership fails
with `O3IFC097`. The native AST importer preserves concrete and dependent yield
nodes alongside existing await and return nodes.

The shared fixture covers namespace templates, concrete member functions and
returned coroutine closures, custom `await_transform`, ADL `operator co_await`,
yielded results, frame-owned locals, move-only task handles, cancellation,
exception cleanup and allocation failure. All 33 frontend suites, 235 native unit tests and 412 compiler regressions
pass (three compiler cases unsupported). Source-free checks pass 32 focused
configurations / 192 commands across normal, eager, ASan/UBSan and TSan, plus
one full async-fixture configuration / six commands per normal/ASan/TSan mode.
The [source receipt](../support/linux_cxx_custom_coroutines_progress.json) records
inputs, reports and limits. Eager and source-directed imports support including
`<coroutine>` before or after the module directive. Exact source mappings join
instantiated nested promise forwards; template friend redeclarations retain
their independent native chains. Public standard coroutine signatures and external
`coroutine_traits` specializations are added in the separately validated
[2.90 source span](linux_cxx_standard_coroutines.md).
This does not establish Objective-C async/profile conformance or SDK delivery.
