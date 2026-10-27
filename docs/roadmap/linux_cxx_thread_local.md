# C++ thread-local package data

The concurrency 2.22.0 source milestone models C++ thread-local globals, class
static data, inline variables and function-local static storage under required
`objc3.cxx-thread-local.v1` semantics. Variable identity preserves the native
thread-storage specifier and TLS kind; containing records independently bind
static-member storage, and published function bodies bind local storage classes.

Canonical declarations retain `thread_local` when provider initializers are
omitted. Inline definitions retain their initializers and enter consumer code
generation through verified backend registration. Native module ownership also
covers TLS initialization/wrapper helpers and inline local-static guards.
Export, SPI and resolved expression dependencies keep their existing checks.

Source fixtures exercise dynamic inline initialization, cross-translation-unit
local-static state, provider-owned and inline class objects, worker-thread value
isolation and exact construction/destruction counts after thread exit. The
provider source is removed before consumer compilation, and both eager and
source-directed imports exercise the complete fixture.

This does not expand capture transfer, actor migration or profile conformance.
The accepted SDK remains [2.9.0](../support/linux_cxx_class_structure_acceptance.json);
combined source/sanitizer matrices and installed/archive qualification remain
scheduled for the coordinated source-batch cutover.
