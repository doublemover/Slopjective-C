# C++ declaration expressions

The concurrency 2.14.0 source milestone publishes concrete default arguments
and member initializers under required
`objc3.cxx-declaration-expressions.v1` semantics. Functions bind each argument by
parameter position. Fields bind their initializer, and containing records bind
member expressions independently of their child metadata rows.

Expression identity includes canonical spelling, native typed expression nodes,
cast kinds and resolved declaration/native symbol identities. Export, SPI and
resolved dependency checks cover referenced values, callable targets,
constructors and expression types. Local or unresolved dependencies fail closed.
Canonical declarations preserve defaults once across redeclarations and preserve
member initializer syntax. The consumer reproduces expression identities before
native AST import.

Source fixtures cover aggregate defaults, brace/array initializers, references
to earlier members, defaulted construction, ordinary and member call defaults,
and lazy async-frame calls. Runtime counters check evaluation at the call or
construction site after the provider source has been removed.

Lambdas, Blocks and statement expressions in declarations retain an explicit
boundary pending local-declaration/capture identity. Concrete inline/constexpr
bodies, dependent templates, unnamed nested types and broader ownership/profile
contracts remain separate work. The accepted SDK remains
[2.9.0](../support/linux_cxx_class_structure_acceptance.json); broad source,
sanitizer and installed/archive qualification are deferred for the combined batch.
