# Published C++ custom coroutine promises on Linux

Source implementation **2.88** publishes C++ coroutine definitions with named
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
exception cleanup and allocation failure. Source-free replay and runtime checks
are pending. Public signatures exposing standard-library coroutine types and
external `coroutine_traits` specialization publication remain separate contracts.
This does not establish Objective-C async/profile conformance or SDK delivery.
