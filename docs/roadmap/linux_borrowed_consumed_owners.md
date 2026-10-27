# Borrowed loans across annotated object consumption

The System 2.3.0 source span extends MRC borrowed-claim tracking to
`ns_consumed` object parameters and `ns_consumes_self` instance receivers.
Direct function/message calls, C++ constructors and call operators consume the
known ownership claim. Calls through typed function and Block pointers retain
the consumed-parameter contract. A consumed claim expires its known aliases;
earlier independent lifetime pins continue protecting their loans.

The CFG keeps each evaluated expression's owner identity separately from its
borrowed payload. A later argument can replace a source variable without
changing which earlier value the call consumes. Borrowed arguments overlapping
a consumed owner are rejected unless an independent pin protects the loan for
the call. The prior manual-release rules for new acquisitions, raw aliases and
pin expiry remain in use. ARC callers retain their existing independent strong
owners and native consumed-argument initialization.

Objective-C 3 function types now retain consumed-parameter metadata under MRC.
Canonical printing, PCH, AST import and source-free interfaces preserve it.
Implicit function-pointer contract erasure and conflicting unannotated function
redeclarations are rejected. Repeat the ownership contract on definitions;
regenerate dependent objects, PCH, interfaces and packages for the changed
compiler identity. A native IR witness requires MRC direct/indirect calls to
introduce no ARC retains or cleanup releases. Native extension ABI 21 and
runtime ABI layouts remain unchanged.

Consuming lexical super calls currently require side-effect-free arguments;
bind effectful arguments first. Ordinary receiver expressions use their
computed identity. Raw runtime/CF release calls, unknown/interprocedural owner
relationships, explicit function-pointer cast erasure, general MRC ownership
counts and consuming isolation entry remain separate work. This span does not
claim full Strict System conformance.

Maintained witnesses cover C17/C++20/C++23, source/PCH replay, direct and typed
indirect calls, Block callbacks, consumed receivers, C++ constructors/operators,
independent pins, overlapping arguments, evaluated identities, contract erasure
and source-free consumed callback imports. SDK 2.33 remains the accepted
installation while development continues.

The [focused receipt](../support/linux_borrowed_consumed_owners_progress.json)
records 383 compiler regressions, 182 native unit tests, all 33 frontend suites
and 14 tooling tests. Three platform compiler cases remain unsupported. The
combined runtime/sanitizer batch and coordinated SDK delivery remain deferred.
