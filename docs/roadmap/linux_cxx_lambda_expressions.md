# C++ local lambda expression contracts

The concurrency 2.21.0 source milestone publishes local non-generic C++ lambdas in
inline bodies, default arguments and member initializers under required
`objc3.cxx-lambda-expressions.v1` semantics. Containing records independently bind
closure-bearing members.

Expression identity binds capture kind/defaults, native typed bodies, copied
closure construction and local member references. Closure types, captures and
members use lexical identities instead of file/line-based USRs. Canonical source
retains the native lambda expressions. Dependency checks distinguish local
parameters/init-captures from referenced public declarations and keep export,
SPI and resolved dependency boundaries.

Native import preserves initialized-capture declaration flags, and declaration
printing retains lambda bodies inside otherwise terse signatures.

Source fixtures exercise init-captures, reference/default captures, copied closures,
call-operator parameters, default invocation and member defaults. Source-free
consumers evaluate these expressions and call a lazy frame with a lambda default.
Capability erasure and changed closure bodies invalidate their contracts.

Generic lambdas and public closure types remain explicit boundaries. The
[2.29.0 source milestone](linux_cxx_local_types.md) adds concrete named local
classes and enums. The [2.24.0 source milestone](linux_cxx_local_expressions.md)
subsequently models published Block/statement expressions and repairs closure
symbol ownership under optimization. This does not establish
complete borrowed/MRC capture or language/profile conformance. The accepted SDK
remains [2.9.0](../support/linux_cxx_class_structure_acceptance.json); broad
source/sanitizer matrices and installed/archive delivery remain deferred for the
combined source batch.

Focused checks and the complete native compiler batch are recorded in the
[lambda progress receipt](../support/linux_cxx_lambda_expressions_progress.json).
