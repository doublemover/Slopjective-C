# C++ inline and constant definitions

The concurrency 2.15.0 source milestone publishes concrete inline, constexpr and
consteval function definitions, including member bodies and constructor
initializers, under required `objc3.cxx-definitions.v1` semantics. Inline and
constant data retain their initializers, including class static data with native
access and containing-record identity. Ordinary out-of-line implementation
bodies remain absent from public interfaces.

The contract binds native typed statements, reference targets, cast kinds,
constructor initialization and expression spelling. Parameters and ordinary
local variables use lexical indices instead of file/line-based USRs. Containing
records independently bind their inline member bodies. Referenced declarations
must satisfy export, SPI and resolved dependency policy; private implementation
helpers cannot silently become dependencies of an exported body.

Verified inline definitions also register with the native backend after its
initialization. Registration preserves declaration order and deduplicates
reimports; it supplies definitions used exclusively by a consumer.

Canonical source retains executable bodies and definition qualifiers. Consumers
reproduce their contracts before native AST import. Source fixtures exercise
constexpr loops and local variables, consteval calls, constexpr constructors,
member calls, class static data and shared inline variables/local static state
across a provider and a source-free consumer. Consumer-first linking also checks
ordered dynamic initialization. A separate source-free eager-import fixture
checks consumer-only code/data emission without runtime-bearing declarations.

Subsequent source milestones add [defaulted comparisons](linux_cxx_defaulted_comparisons.md),
[out-of-line defaulting](linux_cxx_out_of_line_defaulting.md) and
[local non-generic lambdas](linux_cxx_lambda_expressions.md). Dependent templates,
local classes and generic lambdas remain separate work.
[Published Block/statement expressions](linux_cxx_local_expressions.md) are
modeled at source milestone 2.24.0. [Thread-local data](linux_cxx_thread_local.md) is modeled at source milestone
2.22.0.
The [2.16.0 header coordination milestone](linux_eager_semantic_imports.md)
subsequently enables the full runtime fixture through eager imports as well.
This does not claim broader ownership or profile conformance. The accepted SDK
remains [2.9.0](../support/linux_cxx_class_structure_acceptance.json); broad
source/sanitizer matrices and installed/archive delivery remain deferred for the
combined implementation batch.

Focused source evidence is recorded in the
[definition milestone receipt](../support/linux_cxx_definitions_progress.json).
